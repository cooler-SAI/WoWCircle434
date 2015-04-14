/*
 * Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "Common.h"
#include "SharedDefines.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "Object.h"
#include "Creature.h"
#include "Player.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "UpdateData.h"
#include "UpdateMask.h"
#include "Util.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Log.h"
#include "Transport.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "VMapFactory.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellAuraEffects.h"
#include "UpdateFieldFlags.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "OutdoorPvPMgr.h"
#include "MovementPacketBuilder.h"
#include "DynamicTree.h"
#include "Unit.h"
#include "Group.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "ScriptSystem.h"

uint32 GuidHigh2TypeId(uint32 guid_hi)
{
    switch (guid_hi)
    {
        case HIGHGUID_ITEM:         return TYPEID_ITEM;
        //case HIGHGUID_CONTAINER:    return TYPEID_CONTAINER; HIGHGUID_CONTAINER == HIGHGUID_ITEM currently
        case HIGHGUID_UNIT:         return TYPEID_UNIT;
        case HIGHGUID_PET:          return TYPEID_UNIT;
        case HIGHGUID_PLAYER:       return TYPEID_PLAYER;
        case HIGHGUID_GAMEOBJECT:   return TYPEID_GAMEOBJECT;
        case HIGHGUID_DYNAMICOBJECT:return TYPEID_DYNAMICOBJECT;
        case HIGHGUID_CORPSE:       return TYPEID_CORPSE;
        case HIGHGUID_MO_TRANSPORT: return TYPEID_GAMEOBJECT;
        case HIGHGUID_VEHICLE:      return TYPEID_UNIT;
    }
    return NUM_CLIENT_OBJECT_TYPES;                         // unknown
}

Object::Object() : m_PackGUID(sizeof(uint64)+1)
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = NULL;
    m_valuesCount       = 0;
    _fieldNotifyFlags   = UF_FLAG_DYNAMIC;

    m_inWorld           = false;
    m_objectUpdated     = false;

    m_PackGUID.appendPackGUID(0);
}

WorldObject::~WorldObject()
{
    // this may happen because there are many !create/delete
    if (IsWorldObject() && m_currMap)
    {
        if (GetTypeId() == TYPEID_CORPSE)
        {
            sLog->outFatal(LOG_FILTER_GENERAL, "Object::~Object Corpse guid="UI64FMTD", type=%d, entry=%u deleted but still in map!!", GetGUID(), ((Corpse*)this)->GetType(), GetEntry());
            ASSERT(false);
        }
        ResetMap();
    }
}

Object::~Object()
{
    if (IsInWorld())
    {
        sLog->outFatal(LOG_FILTER_GENERAL, "Object::~Object - guid="UI64FMTD", typeid=%d, entry=%u deleted but still in world!!", GetGUID(), GetTypeId(), GetEntry());
        if (isType(TYPEMASK_ITEM))
            sLog->outFatal(LOG_FILTER_GENERAL, "Item slot %u", ((Item*)this)->GetSlot());
        ASSERT(false);
        RemoveFromWorld();
    }

    if (m_objectUpdated)
    {
        sLog->outFatal(LOG_FILTER_GENERAL, "Object::~Object - guid="UI64FMTD", typeid=%d, entry=%u deleted but still in update list!!", GetGUID(), GetTypeId(), GetEntry());
        ASSERT(false);
        sObjectAccessor->RemoveUpdateObject(this);
    }

    delete [] m_uint32Values;
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[m_valuesCount];
    memset(m_uint32Values, 0, m_valuesCount*sizeof(uint32));

    _changesMask.SetCount(m_valuesCount);

    m_objectUpdated = false;
}

void Object::_Create(uint32 guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values) _InitValues();

    uint64 guid = MAKE_NEW_GUID(guidlow, entry, guidhigh);
    SetUInt64Value(OBJECT_FIELD_GUID, guid);
    SetUInt16Value(OBJECT_FIELD_TYPE, 0, m_objectType);
    m_PackGUID.clear();
    m_PackGUID.appendPackGUID(GetGUID());
}

std::string Object::_ConcatFields(uint16 startIndex, uint16 size) const
{
    std::ostringstream ss;
    for (uint16 index = 0; index < size; ++index)
        ss << GetUInt32Value(index + startIndex) << ' ';
    return ss.str();
}

void Object::AddToWorld()
{
    if (m_inWorld)
        return;

    ASSERT(m_uint32Values);

    m_inWorld = true;

    // synchronize values mirror with values array (changes will send in updatecreate opcode any way
    ClearUpdateMask(true);
}

void Object::RemoveFromWorld()
{
    if (!m_inWorld)
        return;

    m_inWorld = false;

    // if we remove from world then sending changes not required
    ClearUpdateMask(true);
}

void Object::SendForcedObjectUpdate()
{
    if (!m_inWorld || !ToPlayer())
        return;

    UpdateDataMapType update_players;

    BuildUpdate(update_players);
    sObjectAccessor->RemoveUpdateObject(this);

    for (UpdateDataMapType::iterator iter = update_players.begin(); iter != update_players.end(); ++iter)
    {
        SendUpdateToPlayer(iter->first);
        iter->first->SendUpdateToPlayer(ToPlayer());
    }
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    if (!target)
        return;

    uint8  updateType = UPDATETYPE_CREATE_OBJECT;
    uint16 flags      = m_updateFlag;

    uint32 valCount = m_valuesCount;

    /** lower flag1 **/
    if (target == this)                                      // building packet for yourself
        flags |= UPDATEFLAG_SELF;
    else if (GetTypeId() == TYPEID_PLAYER)
        valCount = PLAYER_END_NOT_SELF;

    switch (GetGUIDHigh())
    {
        case HIGHGUID_PLAYER:
        case HIGHGUID_PET:
        case HIGHGUID_CORPSE:
        case HIGHGUID_DYNAMICOBJECT:
            updateType = UPDATETYPE_CREATE_OBJECT2;
            break;
        case HIGHGUID_UNIT:
            if (ToUnit()->ToTempSummon() && IS_PLAYER_GUID(ToUnit()->ToTempSummon()->GetSummonerGUID()))
                updateType = UPDATETYPE_CREATE_OBJECT2;
            break;
        case HIGHGUID_GAMEOBJECT:
            if (IS_PLAYER_GUID(ToGameObject()->GetOwnerGUID()))
                updateType = UPDATETYPE_CREATE_OBJECT2;
            break;
    }

    if (flags & UPDATEFLAG_STATIONARY_POSITION)
    {
        // UPDATETYPE_CREATE_OBJECT2 for some gameobject types...
        if (isType(TYPEMASK_GAMEOBJECT))
        {
            switch (((GameObject*)this)->GetGoType())
            {
                case GAMEOBJECT_TYPE_TRAP:
                case GAMEOBJECT_TYPE_DUEL_ARBITER:
                case GAMEOBJECT_TYPE_FLAGSTAND:
                case GAMEOBJECT_TYPE_FLAGDROP:
                    updateType = UPDATETYPE_CREATE_OBJECT2;
                    break;
                case GAMEOBJECT_TYPE_TRANSPORT:
                    flags |= UPDATEFLAG_TRANSPORT;
                    break;
                default:
                    break;
            }
        }
    }

    if (ToUnit() && ToUnit()->getVictim())
        flags |= UPDATEFLAG_HAS_TARGET;

    ByteBuffer buf(500);
    buf << uint8(updateType);
    buf.append(GetPackGUID());
    buf << uint8(m_objectTypeId);

    _BuildMovementUpdate(&buf, flags);

    UpdateMask updateMask;
    updateMask.SetCount(valCount);
    _SetCreateBits(&updateMask, target);
    _BuildValuesUpdate(updateType, &buf, &updateMask, target);
    data->AddUpdateBlock(buf);
}

void Object::SendUpdateToPlayer(Player* player)
{
    // send create update to player
    UpdateData upd(player->GetMapId());
    WorldPacket packet;

    BuildCreateUpdateBlockForPlayer(&upd, player);
    upd.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    ByteBuffer buf(500);

    buf << uint8(UPDATETYPE_VALUES);
    buf.append(GetPackGUID());

    UpdateMask updateMask;
    uint32 valCount = m_valuesCount;
    if (GetTypeId() == TYPEID_PLAYER && target != this)
        valCount = PLAYER_END_NOT_SELF;

    updateMask.SetCount(valCount);

    _SetUpdateBits(&updateMask, target);
    _BuildValuesUpdate(UPDATETYPE_VALUES, &buf, &updateMask, target);

    data->AddUpdateBlock(buf);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData* data) const
{
    data->AddOutOfRangeGUID(GetGUID());
}

void Object::DestroyForPlayer(Player* target, bool onDeath) const
{
    ASSERT(target);

    WorldPacket data(SMSG_DESTROY_OBJECT, 8 + 1);
    data << uint64(GetGUID());
    //! If the following bool is true, the client will call "void CGUnit_C::OnDeath()" for this object.
    //! OnDeath() does for eg trigger death animation and interrupts certain spells/missiles/auras/sounds...
    data << uint8(onDeath ? 1 : 0);
    target->GetSession()->SendPacket(&data);
}

void Object::_BuildMovementUpdate(ByteBuffer* data, uint16 flags) const
{
    const Player*        player = ToPlayer();
    const Unit*          unit = ToUnit();
    const GameObject*    go = ToGameObject();
    const DynamicObject* dob = ToDynObject();

    bool unkFlag = false;

    const WorldObject* wo =
        player ? (const WorldObject*)player : (
        unit ? (const WorldObject*)unit : (
        go ? (const WorldObject*)go : dob ? (const WorldObject*)dob : (const WorldObject*)ToCorpse()));

    uint32 transportPause = 0;

    if (unit)
        const_cast<Unit*>(unit)->m_movementInfo.Normalize(GetTypeId() == TYPEID_UNIT);

    data->WriteBit(false); // unkBit1
    data->WriteBit(false); // byte4
    data->WriteBit((flags & UPDATEFLAG_ROTATION) && go);
    data->WriteBit(flags & UPDATEFLAG_ANIMKITS);
    data->WriteBit((flags & UPDATEFLAG_HAS_TARGET) && unit && unit->getVictim());
    data->WriteBit(flags & UPDATEFLAG_SELF); // byte0
    data->WriteBit((flags & UPDATEFLAG_VEHICLE) && unit);
    data->WriteBit((flags & UPDATEFLAG_LIVING) && unit);
        
    std::vector<uint32> transportFrames;
    if (flags & UPDATEFLAG_TRANSPORT_ARR)
    {
        const GameObjectTemplate* goInfo = ToGameObject()->GetGOInfo();
        if (goInfo->type == GAMEOBJECT_TYPE_TRANSPORT)
        {
            if (goInfo->transport.startFrame)
                transportFrames.push_back(goInfo->transport.startFrame);
            if (goInfo->transport.nextFrame1)
                transportFrames.push_back(goInfo->transport.nextFrame1);
            //if (goInfo->transport.nextFrame2)
            //    transportFrames.push_back(goInfo->transport.nextFrame2);
            //if (goInfo->transport.nextFrame3)
            //    transportFrames.push_back(goInfo->transport.nextFrame3);
        }
    }
    data->WriteBits(transportFrames.size(), 24);
    

    data->WriteBit(false); // byte1
    data->WriteBit((flags & UPDATEFLAG_GO_TRANSPORT_POSITION) && wo);
    data->WriteBit((flags & UPDATEFLAG_STATIONARY_POSITION) && wo);
    data->WriteBit(unkFlag);
    data->WriteBit(false); // byte2
    data->WriteBit(flags & (UPDATEFLAG_TRANSPORT | UPDATEFLAG_TRANSPORT_ARR)); // byte198

    if ((flags & UPDATEFLAG_LIVING) && unit)
    {
        ObjectGuid guid = GetGUID();

        (*data)
            .WriteBit(!unit->m_movementInfo.flags)
            .WriteBit(G3D::fuzzyEq(unit->GetOrientation(), 0.0f))
            .WriteByteMask(guid[7])
            .WriteByteMask(guid[3])
            .WriteByteMask(guid[2]);

        if (unit->m_movementInfo.flags)
            data->WriteBits(unit->m_movementInfo.flags, 30);

        (*data)
            .WriteBit(unit->IsSplineEnabled())
            .WriteBit(!unit->m_movementInfo.HavePitch)
            .WriteBit(unit->IsSplineEnabled())
            .WriteBit(unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
            .WriteBit(!unit->m_movementInfo.HaveSplineElevation)
            .WriteByteMask(guid[5])
            .WriteBit(unit->m_movementInfo.HasTransportData())
            .WriteBit(false);

        if (unit->m_movementInfo.HasTransportData())
        {
            ObjectGuid transGuid = unit->m_movementInfo.t_guid;

            (*data)
                .WriteByteMask(transGuid[1])
                .WriteBit(unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                .WriteByteMask(transGuid[4])
                .WriteByteMask(transGuid[0])
                .WriteByteMask(transGuid[6])
                .WriteBit(unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                .WriteByteMask(transGuid[7])
                .WriteByteMask(transGuid[5])
                .WriteByteMask(transGuid[3])
                .WriteByteMask(transGuid[2]);
        }

        data->WriteByteMask(guid[4]);
        if (unit->IsSplineEnabled())
            Movement::PacketBuilder::WriteCreateBits(*unit->movespline, *data);

        data->WriteByteMask(guid[6]);
        if (unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
            data->WriteBit(unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION));

        (*data)
            .WriteByteMask(guid[0])
            .WriteByteMask(guid[1])
            .WriteBit(false) //byte95
            .WriteBit(!unit->m_movementInfo.flags2);

        if (unit->m_movementInfo.flags2)
            data->WriteBits(unit->m_movementInfo.flags2, 12);
    }

    if ((flags & UPDATEFLAG_GO_TRANSPORT_POSITION) && wo)
    {
        ObjectGuid transGuid = wo->m_movementInfo.t_guid;

        (*data)
            .WriteByteMask(transGuid[5])
            .WriteBit(wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
            .WriteByteMask(transGuid[0])
            .WriteByteMask(transGuid[3])
            .WriteByteMask(transGuid[6])
            .WriteByteMask(transGuid[1])
            .WriteByteMask(transGuid[4])
            .WriteByteMask(transGuid[2])
            .WriteBit(wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
            .WriteByteMask(transGuid[7]);
    }

    if ((flags & UPDATEFLAG_HAS_TARGET) && unit && unit->getVictim())
    {
        ObjectGuid victimGuid = unit->getVictim()->GetGUID();

        (*data)
            .WriteByteMask(victimGuid[2])
            .WriteByteMask(victimGuid[7])
            .WriteByteMask(victimGuid[0])
            .WriteByteMask(victimGuid[4])
            .WriteByteMask(victimGuid[5])
            .WriteByteMask(victimGuid[6])
            .WriteByteMask(victimGuid[1])
            .WriteByteMask(victimGuid[3]);
    }

    if (flags & UPDATEFLAG_ANIMKITS)
    {
        (*data)
            .WriteBit(true)                                                      // Missing AnimKit1
            .WriteBit(true)                                                      // Missing AnimKit2
            .WriteBit(true);                                                      // Missing AnimKit3
    }

    for (int i = 0; i < transportFrames.size(); ++i)
        *data << uint32(transportFrames[i]);

    if ((flags & UPDATEFLAG_LIVING) && unit)
    {
        ObjectGuid guid = GetGUID();

        data->WriteByteSeq(guid[4]);
        *data << unit->GetSpeed(MOVE_RUN_BACK);
        if (unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
        {
            if (unit->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
            {
                *data << float(unit->m_movementInfo.j_xyspeed);
                *data << float(unit->m_movementInfo.j_cosAngle);
                *data << float(unit->m_movementInfo.j_sinAngle);
            }

            *data << uint32(unit->m_movementInfo.fallTime);
            *data << float(unit->m_movementInfo.j_zspeed);
        }

        *data << unit->GetSpeed(MOVE_SWIM_BACK);
        if (unit->m_movementInfo.HaveSplineElevation)
            *data << float(unit->m_movementInfo.splineElevation);

        if (unit->IsSplineEnabled())
            Movement::PacketBuilder::WriteCreateData(*unit->movespline, *data);

        *data << float(unit->GetPositionZMinusOffset());
        data->WriteByteSeq(guid[5]);
        if (unit->m_movementInfo.HasTransportData())
        {
            ObjectGuid transGuid = wo->m_movementInfo.t_guid;

            (*data)
                .WriteByteSeq(transGuid[5])
                .WriteByteSeq(transGuid[7])
                .append<uint32>(wo->GetTransTime())
                .append<float>(wo->GetTransOffsetO());

            if (wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                *data << wo->m_movementInfo.t_time2;

            (*data)
                .append<float>(wo->GetTransOffsetY())
                .append<float>(wo->GetTransOffsetX())
                .WriteByteSeq(transGuid[3])
                .append<float>(wo->GetTransOffsetZ())
                .WriteByteSeq(transGuid[0]);

            if (wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                *data << wo->m_movementInfo.t_time3;

            (*data)
                .append<int8>(wo->GetTransSeat())
                .WriteByteSeq(transGuid[1])
                .WriteByteSeq(transGuid[6])
                .WriteByteSeq(transGuid[2])
                .WriteByteSeq(transGuid[4]);
        }

        (*data)
            .append<float>(unit->GetPositionX())
            .append<float>(unit->GetSpeed(MOVE_PITCH_RATE))
            .WriteByteSeq(guid[3])
            .WriteByteSeq(guid[0])
            .append<float>(unit->GetSpeed(MOVE_SWIM))
            .append<float>(unit->GetPositionY())
            .WriteByteSeq(guid[7])
            .WriteByteSeq(guid[1])
            .WriteByteSeq(guid[2])
            .append<float>(unit->GetSpeed(MOVE_WALK));

        if (true)
            *data << uint32(getMSTime());

        (*data)
            .append<float>(unit->GetSpeed(MOVE_FLIGHT_BACK))
            .WriteByteSeq(guid[6])
            .append<float>(unit->GetSpeed(MOVE_TURN_RATE));

        if (!G3D::fuzzyEq(unit->GetOrientation(), 0.0f))
            *data << float(unit->GetOrientation());

        *data << unit->GetSpeed(MOVE_RUN);
        if (unit->m_movementInfo.HavePitch)
            *data << float(unit->m_movementInfo.pitch);

        *data << unit->GetSpeed(MOVE_FLIGHT);
    }

    if ((flags & UPDATEFLAG_VEHICLE) && unit)
    {
        *data << float(unit->GetOrientation());
        *data << uint32(unit->GetVehicleKit()->GetVehicleInfo()->m_ID);
    }

    if ((flags & UPDATEFLAG_GO_TRANSPORT_POSITION) && wo)
    {
        ObjectGuid transGuid = wo->m_movementInfo.t_guid;

        data->WriteByteMask(transGuid[0]);
        data->WriteByteMask(transGuid[5]);
        if (wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
            *data << wo->m_movementInfo.t_time3;

        (*data)
            .WriteByteMask(transGuid[3])
            .append<float>(wo->GetTransOffsetX())
            .WriteByteMask(transGuid[4])
            .WriteByteMask(transGuid[6])
            .WriteByteMask(transGuid[1])
            .append<uint32>(wo->GetTransTime())
            .append<float>(wo->GetTransOffsetY())
            .WriteByteMask(transGuid[2])
            .WriteByteMask(transGuid[7])
            .append<float>(wo->GetTransOffsetZ())
            .append<int8>(wo->GetTransSeat())
            .append<float>(wo->GetTransOffsetO());

        if (wo->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
            *data << wo->m_movementInfo.t_time2;
    }

    if ((flags & UPDATEFLAG_ROTATION) && go)
        *data << uint64(go->GetRotation());

    if (unkFlag)
    {
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << uint8(0);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
        *data << float(0.0f);
    }

    if (flags & UPDATEFLAG_STATIONARY_POSITION)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        *data << float(self->GetOrientation());
        *data << float(self->GetPositionX());
        *data << float(self->GetPositionY());
        if (Unit const* unit = ToUnit())
            *data << float(unit->GetPositionZMinusOffset());
        else
            *data << float(self->GetPositionZ());
    }

    if ((flags & UPDATEFLAG_HAS_TARGET) && unit && unit->getVictim())
    {
        ObjectGuid victimGuid = unit->getVictim()->GetGUID();

        (*data)
            .WriteByteSeq(victimGuid[4])
            .WriteByteSeq(victimGuid[0])
            .WriteByteSeq(victimGuid[3])
            .WriteByteSeq(victimGuid[5])
            .WriteByteSeq(victimGuid[7])
            .WriteByteSeq(victimGuid[6])
            .WriteByteSeq(victimGuid[2])
            .WriteByteSeq(victimGuid[1]);
    }

    //if (flags & UPDATEFLAG_ANIMKITS)
    //{
    //    if (hasAnimKit1)
    //        *data << uint16(animKit1);
    //    if (hasAnimKit2)
    //        *data << uint16(animKit2);
    //    if (hasAnimKit3)
    //        *data << uint16(animKit3);
    //}

    if (flags & UPDATEFLAG_TRANSPORT)
        *data << uint32(getMSTime());                       // Transport path timer - getMSTime is wrong.
    else if (flags & UPDATEFLAG_TRANSPORT_ARR)
        *data << uint32(GetUInt32Value(GAMEOBJECT_LEVEL));
}

void Object::_BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, UpdateMask* updateMask, Player* target) const
{
    if (!target)
        return;

    bool IsActivateToQuest = false;
    if (updatetype == UPDATETYPE_CREATE_OBJECT || updatetype == UPDATETYPE_CREATE_OBJECT2)
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsDynTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->isGameMaster())
                IsActivateToQuest = true;

            if (((GameObject*)this)->GetGoArtKit())
                updateMask->SetBit(GAMEOBJECT_BYTES_1);
        }
        else if (isType(TYPEMASK_UNIT))
        {
            if (((Unit*)this)->HasFlag(UNIT_FIELD_AURASTATE, PER_CASTER_AURA_STATE_MASK))
                updateMask->SetBit(UNIT_FIELD_AURASTATE);
        }
    }
    else                                                    // case UPDATETYPE_VALUES
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->isGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_BYTES_1);

            if (ToGameObject()->GetGoType() == GAMEOBJECT_TYPE_CHEST && ToGameObject()->GetGOInfo()->chest.groupLootRules &&
                ToGameObject()->HasLootRecipient())
                updateMask->SetBit(GAMEOBJECT_FLAGS);
        }
        else if (isType(TYPEMASK_UNIT))
        {
            if (((Unit*)this)->HasFlag(UNIT_FIELD_AURASTATE, PER_CASTER_AURA_STATE_MASK))
                updateMask->SetBit(UNIT_FIELD_AURASTATE);
        }
    }

    uint32 valCount = m_valuesCount;
    if (GetTypeId() == TYPEID_PLAYER && target != this)
        valCount = PLAYER_END_NOT_SELF;

    WPAssert(updateMask && updateMask->GetCount() == valCount);

    // Client block count check
    if (updateMask->GetBlockCount() > 44)
        sLog->outError(LOG_FILTER_GENERAL, "_BuildValuesUpdate: GetBlockCount (%u) > 44 - error! Lead to client visible problems! Object type: %u, Guid " I64FMT ", Name: '%s'", updateMask->GetBlockCount(), GetTypeId(), GetGUID(), ToPlayer() ? ToPlayer()->GetName() : "<none>");

    *data << (uint8)updateMask->GetBlockCount();
    updateMask->AppendToPacket(data);

    // 2 specialized loops for speed optimization in non-unit case
    if (isType(TYPEMASK_UNIT))                               // unit (creature/player) case
    {
        for (uint16 index = 0; index < valCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == UNIT_NPC_FLAGS)
                {
                    // remove custom flag before sending
                    uint32 appendValue = m_uint32Values[index];

                    if (GetTypeId() == TYPEID_UNIT)
                    {
                        if (target->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK) && !target->canSeeSpellClickOn(this->ToCreature()))
                            appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;

                        if (appendValue & UNIT_NPC_FLAG_TRAINER)
                        {
                            if (!this->ToCreature()->isCanTrainingOf(target, false))
                                appendValue &= ~(UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_TRAINER_CLASS | UNIT_NPC_FLAG_TRAINER_PROFESSION);
                        }
                    }

                    *data << uint32(appendValue);
                }
                else if (index == UNIT_FIELD_AURASTATE)
                {
                    // Check per caster aura states to not enable using a pell in client if specified aura is not by target
                    *data << ((Unit*)this)->BuildAuraStateUpdateForTarget(target);
                }
                // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from float to uint32 and send
                    *data << uint32(m_floatValues[index] < 0 ? 0 : m_floatValues[index]);
                }
                // there are some float values which may be negative or can't get negative due to other checks
                else if ((index >= UNIT_FIELD_NEGSTAT0   && index <= UNIT_FIELD_NEGSTAT4) ||
                    (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                    (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                    (index >= UNIT_FIELD_POSSTAT0   && index <= UNIT_FIELD_POSSTAT4))
                {
                    *data << uint32(m_floatValues[index]);
                }
                // Gamemasters should be always able to select units - remove not selectable flag
                else if (index == UNIT_FIELD_FLAGS)
                {
                    if (target->isGameMaster())
                        *data << (m_uint32Values[index] & ~UNIT_FLAG_NOT_SELECTABLE);
                    else
                        *data << m_uint32Values[index];
                }
                // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                else if (index == UNIT_FIELD_DISPLAYID)
                {
                    if (GetTypeId() == TYPEID_UNIT)
                    {
                        CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate();
                        // this also applies for transform auras
                        if (SpellInfo const* transform = sSpellMgr->GetSpellInfo(ToUnit()->getTransForm()))
                            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                if (transform->Effects[i].IsAura(SPELL_AURA_TRANSFORM))
                                    if (CreatureTemplate const* transformInfo = sObjectMgr->GetCreatureTemplate(transform->Effects[i].MiscValue))
                                    {
                                        cinfo = transformInfo;
                                        break;
                                    }

                        if (cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER)
                        {
                            if (target->isGameMaster())
                            {
                                if (cinfo->Modelid1)
                                    *data << cinfo->Modelid1;//Modelid1 is a visible model for gms
                                else
                                    *data << 17519; // world invisible trigger's model
                            }
                            else
                            {
                                if (cinfo->Modelid2)
                                    *data << cinfo->Modelid2;//Modelid2 is an invisible model for players
                                else
                                    *data << 11686; // world invisible trigger's model
                            }
                        }
                        else
                        {
                            // some models must be sent differently when player is unfriendly
                            CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelInfo(cinfo->Modelid1);
                            if (!minfo)
                                minfo = sObjectMgr->GetCreatureModelInfo(cinfo->Modelid2);

                            if (minfo && minfo->negative_modelId && !target->IsFriendlyTo(ToUnit()))
                                *data << minfo->negative_modelId;
                            else
                                *data << m_uint32Values[index];
                        }
                    }
                    else
                        *data << m_uint32Values[index];
                }
                // hide lootable animation for unallowed players
                else if (index == UNIT_DYNAMIC_FLAGS)
                {
                    uint32 dynamicFlags = m_uint32Values[index];

                    if (Creature const* creature = ToCreature())
                    {
                        if (creature->hasLootRecipient())
                        {
                            if (creature->isTappedBy(target))
                            {
                                dynamicFlags |= (UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER);
                            }
                            else
                            {
                                dynamicFlags |= UNIT_DYNFLAG_TAPPED;
                                dynamicFlags &= ~UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                            }
                        }
                        else
                        {
                            dynamicFlags &= ~UNIT_DYNFLAG_TAPPED;
                            dynamicFlags &= ~UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                        }

                        if (!target->isAllowedToLoot(creature))
                            dynamicFlags &= ~UNIT_DYNFLAG_LOOTABLE;
                    }

                    // unit UNIT_DYNFLAG_TRACK_UNIT should only be sent to caster of SPELL_AURA_MOD_STALKED auras
                    if (Unit const* unit = ToUnit())
                        if (dynamicFlags & UNIT_DYNFLAG_TRACK_UNIT)
                            if (!unit->HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, target->GetGUID()))
                                dynamicFlags &= ~UNIT_DYNFLAG_TRACK_UNIT;
                    *data << dynamicFlags;
                }
                // FG: pretend that OTHER players in own group are friendly ("blue")
                else if (index == UNIT_FIELD_BYTES_2 || index == UNIT_FIELD_FACTIONTEMPLATE)
                {
                    Unit const* unit = ToUnit();
                    if (unit->IsControlledByPlayer() && target != this && sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && unit->IsInRaidWith(target))
                    {
                        FactionTemplateEntry const* ft1 = unit->getFactionTemplateEntry();
                        FactionTemplateEntry const* ft2 = target->getFactionTemplateEntry();
                        if (ft1 && ft2 && !ft1->IsFriendlyTo(*ft2))
                        {
                            if (index == UNIT_FIELD_BYTES_2)
                            {
                                // Allow targetting opposite faction in party when enabled in config
                                *data << (m_uint32Values[index] & ((UNIT_BYTE2_FLAG_SANCTUARY /*| UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5*/) << 8)); // this flag is at uint8 offset 1 !!
                            }
                            else
                            {
                                // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                                uint32 faction = target->getFaction();
                                *data << uint32(faction);
                            }
                        }
                        else
                            *data << m_uint32Values[index];
                    }
                    else
                        *data << m_uint32Values[index];
                }
                else
                {
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[index];
                }
            }
        }
    }
    else if (isType(TYPEMASK_GAMEOBJECT))                    // gameobject case
    {
        for (uint16 index = 0; index < valCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                if (index == GAMEOBJECT_DYNAMIC)
                {
                    if (IsActivateToQuest)
                    {
                        switch (ToGameObject()->GetGoType())
                        {
                            case GAMEOBJECT_TYPE_CHEST:
                                if (target->isGameMaster())
                                    *data << uint16(GO_DYNFLAG_LO_ACTIVATE);
                                else
                                    *data << uint16(GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
                                break;
                            case GAMEOBJECT_TYPE_GENERIC:
                                if (target->isGameMaster())
                                    *data << uint16(0);
                                else
                                    *data << uint16(GO_DYNFLAG_LO_SPARKLE);
                                break;
                            case GAMEOBJECT_TYPE_GOOBER:
                                if (target->isGameMaster())
                                    *data << uint16(GO_DYNFLAG_LO_ACTIVATE);
                                else
                                    *data << uint16(GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
                                break;
                            default:
                                *data << uint16(0); // unknown, not happen.
                                break;
                        }
                    }
                    else
                        *data << uint16(0);         // disable quest object

                    *data << uint16(-1);
                }
                else if (index == GAMEOBJECT_FLAGS)
                {
                    uint32 flags = m_uint32Values[index];
                    if (ToGameObject()->GetGoType() == GAMEOBJECT_TYPE_CHEST)
                        if (ToGameObject()->GetGOInfo()->chest.groupLootRules && !ToGameObject()->IsLootAllowedFor(target))
                            flags |= GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE;

                    *data << flags;
                }
                else if (index == GAMEOBJECT_BYTES_1)
                {
                    if (((GameObject*)this)->GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT)
                        *data << uint32(m_uint32Values[index] | GO_STATE_TRANSPORT_SPEC);
                    else
                        *data << uint32(m_uint32Values[index]);
                }
                else
                    *data << m_uint32Values[index];                // other cases
            }
        }
    }
    else if (isType(TYPEMASK_DYNAMICOBJECT))                    // dymamic object case
    {
        for (uint16 index = 0; index < valCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == DYNAMICOBJECT_BYTES)
                {
                    if (!target->IsFriendlyTo(ToDynObject()->GetCaster()))
                        *data << uint32((ToDynObject()->GetSpellInfo()->SpellVisual[1] != 0 ? ToDynObject()->GetSpellInfo()->SpellVisual[1] : ToDynObject()->GetSpellInfo()->SpellVisual[0]) | (ToDynObject()->GetType() << 28));
                    else
                        *data << uint32(ToDynObject()->GetSpellInfo()->SpellVisual[0] | (ToDynObject()->GetType() << 28));
                }
                else
                    *data << m_uint32Values[index];                // other cases
            }
        }
    }
    else                                                    // other objects case (no special index checks)
    {
        for (uint16 index = 0; index < valCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                *data << m_uint32Values[index];
            }
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    _changesMask.Clear();
 
    if (m_objectUpdated)
    {
        if (remove)
            sObjectAccessor->RemoveUpdateObject(this);
        m_objectUpdated = false;
    }
}

void Object::BuildFieldsUpdate(Player* player, UpdateDataMapType& data_map) const
{
    UpdateDataMapType::iterator iter = data_map.find(player);

    if (iter == data_map.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = data_map.insert(UpdateDataMapType::value_type(player, UpdateData(player->GetMapId())));
        ASSERT(p.second);
        iter = p.first;
    }

    BuildValuesUpdateBlockForPlayer(&iter->second, iter->first);
}

void Object::_LoadIntoDataField(char const* data, uint32 startOffset, uint32 count)
{
    if (!data)
        return;

    Tokenizer tokens(data, ' ', count);

    if (tokens.size() != count)
        return;

    for (uint32 index = 0; index < count; ++index)
    {
        m_uint32Values[startOffset + index] = atol(tokens[index]);
        _changesMask.SetBit(startOffset + index);
    }
}

uint32 Object::GetUpdateFieldData(Player const* target, uint32*& flags) const
{
    uint32 visibleFlag = UF_FLAG_PUBLIC;

    if (target == this)
        visibleFlag |= UF_FLAG_PRIVATE;

    switch (GetTypeId())
    {
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
            flags = ItemUpdateFieldFlags;
            if (((Item*)this)->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER | UF_FLAG_ITEM_OWNER;
            break;
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
        {
            Player* plr = ToUnit()->GetCharmerOrOwnerPlayerOrPlayerItself();
            flags = UnitUpdateFieldFlags;
            if (ToUnit()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;

            if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO))	
                if (ToUnit()->HasAuraTypeWithCaster(SPELL_AURA_EMPATHY, target->GetGUID()))
                    visibleFlag |= UF_FLAG_SPECIAL_INFO;

            if (plr && plr->IsInSameRaidWith(target))
                visibleFlag |= UF_FLAG_PARTY_MEMBER;
            break;
        }
        case TYPEID_GAMEOBJECT:
            flags = GameObjectUpdateFieldFlags;
            if (ToGameObject()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_DYNAMICOBJECT:
            flags = DynamicObjectUpdateFieldFlags;
            if (((DynamicObject*)this)->GetCasterGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_CORPSE:
            flags = CorpseUpdateFieldFlags;
            if (ToCorpse()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_OBJECT:
            break;
    }

    return visibleFlag;
}

void Object::_SetUpdateBits(UpdateMask* updateMask, Player* target) const
{
    uint32* flags = NULL;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);

    uint32 valCount = m_valuesCount;
    if (GetTypeId() == TYPEID_PLAYER && target != this)
        valCount = PLAYER_END_NOT_SELF;

    for (uint16 index = 0; index < valCount; ++index)
        if (_fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlag) & UF_FLAG_SPECIAL_INFO) || (_changesMask.GetBit(index) && (flags[index] & visibleFlag)))
            updateMask->SetBit(index);
}

void Object::_SetCreateBits(UpdateMask* updateMask, Player* target) const
{
    uint32* value = m_uint32Values;
    uint32* flags = NULL;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);

    uint32 valCount = m_valuesCount;
    if (GetTypeId() == TYPEID_PLAYER && target != this)
        valCount = PLAYER_END_NOT_SELF;

    for (uint16 index = 0; index < valCount; ++index, ++value)
        if (_fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlag) & UF_FLAG_SPECIAL_INFO) || (*value && (flags[index] & visibleFlag)))
            updateMask->SetBit(index);
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[index] != value)
    {
        m_int32Values[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[index] != value)
    {
        m_uint32Values[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::UpdateUInt32Value(uint16 index, uint32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    m_uint32Values[index] = value;
    _changesMask.SetBit(index);
}

void Object::SetUInt64Value(uint16 index, uint64 value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*)&(m_uint32Values[index])) != value)
    {
        m_uint32Values[index] = PAIR64_LOPART(value);
        m_uint32Values[index + 1] = PAIR64_HIPART(value);
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

bool Object::AddUInt64Value(uint16 index, uint64 value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (value && !*((uint64*)&(m_uint32Values[index])))
    {
        m_uint32Values[index] = PAIR64_LOPART(value);
        m_uint32Values[index + 1] = PAIR64_HIPART(value);
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }

        return true;
    }

    return false;
}

bool Object::RemoveUInt64Value(uint16 index, uint64 value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (value && *((uint64*)&(m_uint32Values[index])) == value)
    {
        m_uint32Values[index] = 0;
        m_uint32Values[index + 1] = 0;
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }

        return true;
    }

    return false;
}

void Object::SetFloatValue(uint16 index, float value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[index] != value)
    {
        m_floatValues[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[index] >> (offset * 16)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    ASSERT(m_uint32Values);

    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[index] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
        {
            sObjectAccessor->AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    sLog->outError(LOG_FILTER_GENERAL, "Attempt %s non-existed value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // ASSERT must fail after function call
    return false;
}

bool Position::HasInLine(WorldObject const* target, float width) const
{
    if (!HasInArc(M_PI, target))
        return false;
    width += target->GetObjectSize();
    float angle = GetRelativeAngle(target);
    return fabs(sin(angle)) * GetExactDist2d(target->GetPositionX(), target->GetPositionY()) < width;
}

std::string Position::ToString() const
{
    std::stringstream sstr;
    sstr << "X: " << m_positionX << " Y: " << m_positionY << " Z: " << m_positionZ << " O: " << m_orientation;
    return sstr.str();
}

ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer)
{
    float x, y, z, o;
    buf >> x >> y >> z >> o;
    streamer.m_pos->Relocate(x, y, z, o);
    return buf;
}
ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer)
{
    float x, y, z;
    streamer.m_pos->GetPosition(x, y, z);
    buf << x << y << z;
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer)
{
    float x, y, z;
    buf >> x >> y >> z;
    streamer.m_pos->Relocate(x, y, z);
    return buf;
}

ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer)
{
    float x, y, z, o;
    streamer.m_pos->GetPosition(x, y, z, o);
    buf << x << y << z << o;
    return buf;
}

void MovementInfo::OutDebug() const
{
    sLog->outError(LOG_FILTER_GENERAL, "==========================");
    sLog->outError(LOG_FILTER_GENERAL, "MOVEMENT INFO");
    sLog->outError(LOG_FILTER_GENERAL, "guid " UI64FMTD, guid);
    sLog->outError(LOG_FILTER_GENERAL, "flags %u", flags);
    sLog->outError(LOG_FILTER_GENERAL, "flags2 %u", flags2);
    sLog->outError(LOG_FILTER_GENERAL, "flags_s %u", flags_s);
    sLog->outError(LOG_FILTER_GENERAL, "time %u current time " UI64FMTD "", time, uint64(::time(NULL)));
    sLog->outError(LOG_FILTER_GENERAL, "current time " UI64FMTD, uint64(::time(NULL)));
    sLog->outError(LOG_FILTER_GENERAL, "position: `%s`", pos.ToString().c_str());
    if (HasTransportData())
    {
        sLog->outError(LOG_FILTER_GENERAL, "TRANSPORT:");
        sLog->outError(LOG_FILTER_GENERAL, "t_guid: " UI64FMTD, t_guid);
        sLog->outError(LOG_FILTER_GENERAL, "t_pos `%s`", t_pos.ToString().c_str());
        sLog->outError(LOG_FILTER_GENERAL, "seat: %i", t_seat);
        sLog->outError(LOG_FILTER_GENERAL, "t_time: %u", t_time);
        if (flags_s & SERVERMOVEFLAG_TRANSPORT_T2)
            sLog->outError(LOG_FILTER_GENERAL, "t_time2: %u", t_time2);
        if (flags_s & SERVERMOVEFLAG_TRANSPORT_T3)
            sLog->outError(LOG_FILTER_GENERAL, "t_time3: %u", t_time3);
    }

    if (HavePitch)
        sLog->outError(LOG_FILTER_GENERAL, "pitch: %f", pitch);

    if (flags_s & SERVERMOVEFLAG_FALLDATA)
    {
        sLog->outError(LOG_FILTER_GENERAL, "fallTime: %u j_zspeed: %f", fallTime, j_zspeed);
        if (flags_s & SERVERMOVEFLAG_FALLDIRECTION)
            sLog->outError(LOG_FILTER_GENERAL, "j_sinAngle: %f j_cosAngle: %f j_xyspeed: %f", j_sinAngle, j_cosAngle, j_xyspeed);
    }

    sLog->outError(LOG_FILTER_GENERAL, "speed: %f", speed);

    if (HaveSplineElevation)
        sLog->outError(LOG_FILTER_GENERAL, "splineElevation: %f", splineElevation);

    sLog->outError(LOG_FILTER_GENERAL, "==========================");
}

bool MovementInfo::Check(Player* target, Opcodes opcode)
{
    //if (opcode == CMSG_FORCE_MOVE_ROOT_ACK || opcode == CMSG_FORCE_MOVE_UNROOT_ACK)
    //    return true; // Skip check for this opcode

    if (opcode == CMSG_MOVE_NOT_ACTIVE_MOVER)
        return true; // Skip check for this opcode

    Unit* mover = target->m_mover;
    ASSERT(mover != NULL);                                  // there must always be a mover

    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;
    Vehicle *vehMover = mover->GetVehicleKit();
    if (vehMover)
        if (mover->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            if (Unit *charmer = mover->GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    plMover = (Player*)charmer;

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (plMover && plMover->IsBeingTeleported())
    {
        /*sLog->outError(LOG_FILTER_GENERAL, "MovementInfo::Check: Opcode %u Session %u Mover is being teleported",
            uint32(opcode), target->GetSession()->GetAccountId()
            );*/
        return false;
    }

    if (!pos.IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "MovementInfo::Check: Opcode %u Session %u Invalid Position: (%f,%f,%f,%f)",
            uint32(opcode), target->GetSession()->GetAccountId(),
            pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()
            );
        return false;
    }

    if (mover->GetGUID() != guid)
    {
        // For this opcode guid = old_mover_guid - always!!!
        bool isGuidLogRequired = opcode == CMSG_FORCE_MOVE_ROOT_ACK || opcode == CMSG_FORCE_MOVE_UNROOT_ACK;
        if (isGuidLogRequired)
            return false;

        sLog->outError(LOG_FILTER_GENERAL, "MovementInfo::Check: Opcode %u Session %u Invalid mover guid: client's 0x" I64FMT " (%s) vs server's 0x" I64FMT " (%s)",
            uint32(opcode), target->GetSession()->GetAccountId(),
            guid, GetLogNameForGuid(guid), mover->GetGUID(), GetLogNameForGuid(mover->GetGUID())
            );
        return false;
    }

    /* handle special cases */
    if (HasTransportData())
    {
        // transports size limited
        // (also received at zeppelin leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (!t_pos.IsPositionValid() ||
            t_pos.GetPositionX() > 50 || t_pos.GetPositionY() > 50 || t_pos.GetPositionZ() > 50)
        {
            return false;
        }

        if (!Trinity::IsValidMapCoord(pos.GetPositionX() + t_pos.GetPositionX(), pos.GetPositionY() + t_pos.GetPositionY(),
            pos.GetPositionZ() + t_pos.GetPositionZ(), pos.GetOrientation() + t_pos.GetOrientation()))
        {
            return false;
        }
    }

    return true;
}

bool MovementInfo::AcceptClientChanges(Player* player, MovementInfo& client, Opcodes opcode)
{
    Unit* mover = player->m_mover;
    ASSERT(mover != NULL);                                  // there must always be a mover

    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;
    Vehicle *vehMover = mover->GetVehicleKit();
    if (vehMover)
        if (mover->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            if (Unit *charmer = mover->GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    plMover = (Player*)charmer;

    if (!client.Check(player, opcode))
        return false;

    /* handle special cases */
    if (client.HasTransportData())
    {
        // if we boarded a transport, add us to it
        if (plMover && !plMover->GetTransport())
        {
            // elevators also cause the client to send HasTransportData - just dismount if the guid can be found in the transport list
            for (MapManager::TransportSet::const_iterator iter = sMapMgr->m_Transports.begin(); iter != sMapMgr->m_Transports.end(); ++iter)
            {
                if ((*iter)->GetGUID() == client.t_guid)
                {
                    plMover->SetTransport(*iter);
                    (*iter)->AddPassenger(plMover);
                    break;
                }
            }
        }

        if (!mover->GetTransport() && !mover->GetVehicle())
        {
            GameObject* go = mover->GetMap()->GetGameObject(client.t_guid);
            if (!go || go->GetGoType() != GAMEOBJECT_TYPE_TRANSPORT)
                client.t_guid = 0;
        }
    }
    else if (plMover && plMover->GetTransport())                // if we were on a transport, leave
    {
        plMover->GetTransport()->RemovePassenger(plMover);
        plMover->SetTransport(NULL);
        client.t_pos.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
        client.t_time = 0;
        client.t_seat = -1;
    }

    if (plMover && ((client.flags & MOVEMENTFLAG_SWIMMING) != 0) != plMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plMover->SetInWater(!plMover->IsInWater() || plMover->GetBaseMap()->IsUnderWater(client.pos.GetPositionX(), client.pos.GetPositionY(), client.pos.GetPositionZ()));
    }

    this->time = getMSTime();
    this->guid = mover->GetGUID();
    //memcpy(this, &client, sizeof(MovementInfo));
    memcpy(&mover->m_movementInfo, &client, sizeof(MovementInfo));

    // this is almost never true (not sure why it is sometimes, but it is), normally use mover->IsVehicle()
    if (mover->GetVehicle())
    {
        mover->SetOrientation(client.pos.GetOrientation());
        return false;
    }

    mover->UpdatePosition(client.pos);

    if (plMover && !vehMover)                                            // nothing is charmed, or player charmed
    {

        if (plMover->GetEmoteState() != 0 && opcode == MSG_MOVE_START_FORWARD)
            plMover->SetEmoteState(0);

        plMover->UpdateFallInformationIfNeed(client);

        // that's too slow
        //AreaTableEntry const* zone = GetAreaEntryByAreaID(plMover->GetAreaId());
        float underMapValueZ = -500.0f;
        switch (plMover->GetMapId())
        {
            case 617: underMapValueZ = 3.0f; break; // Dalaran Sewers
            case 0:                                 // Vajsh'ir
            {
                if (plMover->GetZoneId() == 5144
                    || plMover->GetZoneId() == 5145
                    || plMover->GetZoneId() == 4815
                    || plMover->GetZoneId() == 4816
                    || plMover->GetZoneId() == 5146)
                    underMapValueZ = -3000.0f;
                break;
            }
        }
        if (client.pos.GetPositionZ() < underMapValueZ)
        {
            if (!(plMover->InBattleground()
                && plMover->GetBattleground()
                && plMover->GetBattleground()->HandlePlayerUnderMap(player)))
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                // TODO: discard movement packets after the player is rooted
                if (plMover->isAlive())
                {
                    if (plMover->InBattleground())
                    {
                        plMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, player->GetMaxHealth());
                        // player can be alive if GM/etc
                        // change the death state to CORPSE to prevent the death timer from
                        // starting in the next player update
                        if (!plMover->isAlive())
                            plMover->KillPlayer();
                    }
                    else
                        plMover->RepopAtGraveyard();
                }
            }
        }
    }

    return true;
}

void MovementInfo::Normalize(bool update)
{
    if (HaveSplineElevation)
        AddMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);
    else
        RemoveMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);

    if (HasMovementFlag(MOVEMENTFLAG_FALLING))
        AddServerMovementFlag(SERVERMOVEFLAG_FALLDATA | SERVERMOVEFLAG_FALLDIRECTION);

    // move orientation to range [0, 2*PI)
    pos.m_orientation = Position::NormalizeOrientation(pos.m_orientation);
    t_pos.m_orientation = Position::NormalizeOrientation(t_pos.m_orientation);

    // move pitch to range (-PI, PI)
    pitch = Position::NormalizePitch(pitch);

    if (HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2) && !t_time2)
        RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2);

    if (HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3) && !t_time3)
        RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3);

    if (update)
        flags &= MOVEMENTFLAG_MASK_CREATURE_ALLOWED;
}

WorldObject::WorldObject(bool isWorldObject): WorldLocation(),
m_name(""), m_isActive(false), m_isWorldObject(isWorldObject), m_zoneScript(NULL),
m_transport(NULL), m_currMap(NULL), m_InstanceId(0),
m_phaseMask(PHASEMASK_NORMAL)
{
    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE | GHOST_VISIBILITY_GHOST);
    m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
}

void WorldObject::SetTransport(Transport* transport)
{
    m_transport = transport;
    m_movementInfo.t_guid = transport ? transport->GetGUID() : UI64LIT(0);
}

void WorldObject::SetWorldObject(bool on)
{
    if (!IsInWorld())
        return;

    GetMap()->AddObjectToSwitchList(this, on);
}

bool WorldObject::IsWorldObject() const
{
    if (m_isWorldObject)
        return true;

    if (ToCreature() && ToCreature()->m_isTempWorldObject)
        return true;

    return false;
}

void WorldObject::setActive(bool on)
{
    if (m_isActive == on)
        return;

    if (GetTypeId() == TYPEID_PLAYER)
        return;

    m_isActive = on;

    if (!IsInWorld())
        return;

    Map* map = FindMap();
    if (!map)
        return;

    if (on)
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->AddToActive(this->ToCreature());
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->AddToActive((DynamicObject*)this);
    }
    else
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->RemoveFromActive(this->ToCreature());
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->RemoveFromActive((DynamicObject*)this);
    }
}

void WorldObject::CleanupsBeforeDelete(bool /*finalCleanup*/)
{
    if (IsInWorld())
        RemoveFromWorld();
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh, uint32 phaseMask)
{
    Object::_Create(guidlow, 0, guidhigh);
    m_phaseMask = phaseMask;
}

uint32 WorldObject::GetZoneId() const
{
    return GetBaseMap()->GetZoneId(m_positionX, m_positionY, m_positionZ);
}

uint32 WorldObject::GetAreaId() const
{
    return GetBaseMap()->GetAreaId(m_positionX, m_positionY, m_positionZ);
}

void WorldObject::GetZoneAndAreaId(uint32& zoneid, uint32& areaid) const
{
    GetBaseMap()->GetZoneAndAreaId(zoneid, areaid, m_positionX, m_positionY, m_positionZ);
}

InstanceScript* WorldObject::GetInstanceScript()
{
    Map* map = GetMap();
    return map->IsDungeon() ? ((InstanceMap*)map)->GetInstanceScript() : NULL;
}

float WorldObject::GetDistanceZ(const WorldObject* obj) const
{
    float dz = fabs(GetPositionZ() - obj->GetPositionZ());
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float dist = dz - sizefactor;
    return (dist > 0 ? dist : 0);
}

bool WorldObject::_IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const
{
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float maxdist = dist2compare + sizefactor;

    if (m_transport && obj->GetTransport() &&  obj->GetTransport()->GetGUIDLow() == m_transport->GetGUIDLow())
    {
        float dtx = m_movementInfo.t_pos.m_positionX - obj->m_movementInfo.t_pos.m_positionX;
        float dty = m_movementInfo.t_pos.m_positionY - obj->m_movementInfo.t_pos.m_positionY;
        float disttsq = dtx * dtx + dty * dty;
        if (is3D)
        {
            float dtz = m_movementInfo.t_pos.m_positionZ - obj->m_movementInfo.t_pos.m_positionZ;
            disttsq += dtz * dtz;
        }
        return disttsq < (maxdist * maxdist);
    }

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx*dx + dy*dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz*dz;
    }

    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinLOSInMap(const WorldObject* obj) const
{
    if (!IsInMap(obj))
        return false;

    float ox, oy, oz;
    obj->GetPosition(ox, oy, oz);


    if (obj->GetTypeId() == TYPEID_UNIT)
        switch (obj->GetEntry())
    {
        // Hack fix for Ice Tombs (Sindragosa encounter)
        case 36980:
        case 38320:
        case 38321:
        case 38322:
            // Hack fix for Burning Tendons (Spine of Deathwing)
        case 56341:
        case 56575:
            return true;
        default:
            break;
    }

    // AoE spells
    if (GetTypeId() == TYPEID_UNIT)
        switch (GetEntry())
    {
        // Hack fix for Ice Tombs (Sindragosa encounter)
        case 36980:
        case 38320:
        case 38321:
        case 38322:
            // Hack fix for Burning Tendons (Spine of Deathwing)
        case 56341:
        case 56575:
            return true;
        default:
            break;
    }

    // Hack fix for Alysrazor
    if (GetMapId() == 720 && GetAreaId() == 5766)
        if ((GetTypeId() == TYPEID_PLAYER) || (obj->GetTypeId() == TYPEID_PLAYER))
            return true;

    // AoE spells
    if (GetTypeId() == TYPEID_UNIT)
        if (GetEntry() == 36980 || GetEntry() == 38320 || GetEntry() == 38321 || GetEntry() == 38322)
            return true;

    return IsWithinLOS(ox, oy, oz);
}

bool WorldObject::IsWithinLOS(float ox, float oy, float oz) const
{
    if (IsInWorld())
        return GetMap()->isInLineOfSight(GetPositionX(), GetPositionY(), GetPositionZ()+2.f, ox, oy, oz+2.f, GetPhaseMask());

    return true;
}

bool WorldObject::GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D /* = true */) const
{
    float dx1 = GetPositionX() - obj1->GetPositionX();
    float dy1 = GetPositionY() - obj1->GetPositionY();
    float distsq1 = dx1*dx1 + dy1*dy1;
    if (is3D)
    {
        float dz1 = GetPositionZ() - obj1->GetPositionZ();
        distsq1 += dz1*dz1;
    }

    float dx2 = GetPositionX() - obj2->GetPositionX();
    float dy2 = GetPositionY() - obj2->GetPositionY();
    float distsq2 = dx2*dx2 + dy2*dy2;
    if (is3D)
    {
        float dz2 = GetPositionZ() - obj2->GetPositionZ();
        distsq2 += dz2*dz2;
    }

    return distsq1 < distsq2;
}

bool WorldObject::IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D /* = true */, bool useSizeFactor /* = true */) const
{
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx*dx + dy*dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz*dz;
    }

    float sizefactor = useSizeFactor ? (GetObjectSize() + obj->GetObjectSize()) : 0.0f;

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange2d(float x, float y, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float distsq = dx*dx + dy*dy;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange3d(float x, float y, float z, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float distsq = dx*dx + dy*dy + dz*dz;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

void Position::RelocateOffset(const Position & offset)
{
    m_positionX = GetPositionX() + (offset.GetPositionX() * std::cos(GetOrientation()) + offset.GetPositionY() * std::sin(GetOrientation() + M_PI));
    m_positionY = GetPositionY() + (offset.GetPositionY() * std::cos(GetOrientation()) + offset.GetPositionX() * std::sin(GetOrientation()));
    m_positionZ = GetPositionZ() + offset.GetPositionZ();
    SetOrientation(GetOrientation() + offset.GetOrientation());
}

void Position::GetPositionOffsetTo(const Position & endPos, Position & retOffset) const
{
    float dx = endPos.GetPositionX() - GetPositionX();
    float dy = endPos.GetPositionY() - GetPositionY();

    retOffset.m_positionX = dx * std::cos(GetOrientation()) + dy * std::sin(GetOrientation());
    retOffset.m_positionY = dy * std::cos(GetOrientation()) - dx * std::sin(GetOrientation());
    retOffset.m_positionZ = endPos.GetPositionZ() - GetPositionZ();
    retOffset.SetOrientation(endPos.GetOrientation() - GetOrientation());
}

float Position::GetAngle(const Position* obj) const
{
    if (!obj)
        return 0;

    return GetAngle(obj->GetPositionX(), obj->GetPositionY());
}

// Return angle in range 0..2*pi
float Position::GetAngle(const float x, const float y) const
{
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();

    float ang = atan2(dy, dx);
    ang = (ang >= 0) ? ang : 2 * M_PI + ang;
    return ang;
}

void Position::GetSinCos(const float x, const float y, float &vsin, float &vcos) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;

    if (fabs(dx) < 0.001f && fabs(dy) < 0.001f)
    {
        float angle = (float)rand_norm()*static_cast<float>(2*M_PI);
        vcos = std::cos(angle);
        vsin = std::sin(angle);
    }
    else
    {
        float dist = sqrt((dx*dx) + (dy*dy));
        vcos = dx / dist;
        vsin = dy / dist;
    }
}

bool Position::HasInArc(float arc, const Position* obj) const
{
    // always have self in arc
    if (obj == this)
        return true;

    // move arc to range 0.. 2*pi
    arc = NormalizeOrientation(arc);

    float angle = GetAngle(obj);
    angle -= m_orientation;

    // move angle to range -pi ... +pi
    angle = NormalizePitch(angle);

    float lborder = -1 * (arc/2.0f);                        // in range -pi..0
    float rborder = (arc/2.0f);                             // in range 0..pi
    return ((angle >= lborder) && (angle <= rborder));
}

bool WorldObject::IsInBetween(const WorldObject* obj1, const WorldObject* obj2, float size) const
{
    if (!obj1 || !obj2)
        return false;

    float dist = GetExactDist2d(obj1->GetPositionX(), obj1->GetPositionY());

    // not using sqrt() for performance
    if ((dist * dist) >= obj1->GetExactDist2dSq(obj2->GetPositionX(), obj2->GetPositionY()))
        return false;

    if (!size)
        size = GetObjectSize() / 2;

    float angle = obj1->GetAngle(obj2);

    // not using sqrt() for performance
    return (size * size) >= GetExactDist2dSq(obj1->GetPositionX() + std::cos(angle) * dist, obj1->GetPositionY() + std::sin(angle) * dist);
}

bool WorldObject::IsInBetween(const WorldObject* obj1, float x2, float y2, float size) const
{
    if (!obj1)
        return false;

    float dist = GetExactDist2d(obj1->GetPositionX(), obj1->GetPositionY());

    // not using sqrt() for performance
    if ((dist * dist) >= obj1->GetExactDist2dSq(x2, y2))
        return false;

    if (!size)
        size = GetObjectSize() / 2;

    float angle = obj1->GetAngle(x2, y2);

    // not using sqrt() for performance
    return (size * size) >= GetExactDist2dSq(obj1->GetPositionX() + std::cos(angle) * dist, obj1->GetPositionY() + std::sin(angle) * dist);
}

bool WorldObject::isInFront(WorldObject const* target,  float arc) const
{
    return HasInArc(arc, target);
}

bool WorldObject::isInBack(WorldObject const* target, float arc) const
{
    return !HasInArc(2 * M_PI - arc, target);
}

void WorldObject::GetRandomPoint(const Position &pos, float distance, float &rand_x, float &rand_y, float &rand_z) const
{
    if (!distance)
    {
        pos.GetPosition(rand_x, rand_y, rand_z);
        return;
    }

    // angle to face `obj` to `this`
    float angle = (float)rand_norm()*static_cast<float>(2*M_PI);
    float new_dist = (float)rand_norm()*static_cast<float>(distance);

    rand_x = pos.m_positionX + new_dist * std::cos(angle);
    rand_y = pos.m_positionY + new_dist * std::sin(angle);
    rand_z = pos.m_positionZ;

    Trinity::NormalizeMapCoord(rand_x);
    Trinity::NormalizeMapCoord(rand_y);
    UpdateGroundPositionZ(rand_x, rand_y, rand_z);            // update to LOS height if available
}

void WorldObject::UpdateGroundPositionZ(float x, float y, float &z) const
{
    float new_z = GetBaseMap()->GetHeight(GetPhaseMask(), x, y, z, true);
    if (new_z > INVALID_HEIGHT)
        z = new_z+ 0.05f;                                   // just to be sure that we are not a few pixel under the surface
}

void WorldObject::UpdateAllowedPositionZ(float x, float y, float &z) const
{
    switch (GetTypeId())
    {
        case TYPEID_UNIT:
        {
            // non fly unit don't must be in air
            // non swim unit must be at ground (mostly speedup, because it don't must be in water and water level check less fast
            if (!ToCreature()->CanFly())
            {
                bool canSwim = ToCreature()->canSwim();
                float ground_z = z;
                float max_z = canSwim
                    ? GetBaseMap()->GetWaterOrGroundLevel(x, y, z, &ground_z, !ToUnit()->HasAuraType(SPELL_AURA_WATER_WALK))
                    : ((ground_z = GetBaseMap()->GetHeight(GetPhaseMask(), x, y, z, true)));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            else
            {
                float ground_z = GetBaseMap()->GetHeight(GetPhaseMask(), x, y, z, true);
                if (z < ground_z)
                    z = ground_z;
            }
            break;
        }
        case TYPEID_PLAYER:
        {
            // for server controlled moves playr work same as creature (but it can always swim)
            if (!ToPlayer()->CanFly())
            {
                float ground_z = z;
                float max_z = GetBaseMap()->GetWaterOrGroundLevel(x, y, z, &ground_z, !ToUnit()->HasAuraType(SPELL_AURA_WATER_WALK));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            else
            {
                float ground_z = GetBaseMap()->GetHeight(GetPhaseMask(), x, y, z, true);
                if (z < ground_z)
                    z = ground_z;
            }
            break;
        }
        default:
        {
            float ground_z = GetBaseMap()->GetHeight(GetPhaseMask(), x, y, z, true);
            if (ground_z > INVALID_HEIGHT)
                z = ground_z;
            break;
        }
    }
}

bool Position::IsPositionValid() const
{
    return Trinity::IsValidMapCoord(m_positionX, m_positionY, m_positionZ, m_orientation);
}

float WorldObject::GetGridActivationRange() const
{
    if (ToPlayer())
        return GetMap()->GetVisibilityRange();
    else if (ToCreature())
        return ToCreature()->m_SightDistance;
    else
        return 0.0f;
}

float WorldObject::GetVisibilityRange() const
{
    if (isActiveObject() && !ToPlayer())
        return MAX_VISIBILITY_DISTANCE;
    else
        return GetMap()->GetVisibilityRange();
}

float WorldObject::GetSightRange(const WorldObject* target) const
{
    if (ToUnit())
    {
        if (ToPlayer())
        {
            if (target && target->isActiveObject() && !target->ToPlayer())
                return MAX_VISIBILITY_DISTANCE;
            else
                return GetMap()->GetVisibilityRange();
        }
        else if (ToCreature())
            return ToCreature()->m_SightDistance;
        else
            return SIGHT_RANGE_UNIT;
    }

    return 0.0f;
}

bool WorldObject::canSeeOrDetect(WorldObject const* obj, bool ignoreStealth, bool distanceCheck) const
{
    if (this == obj)
        return true;

    if (obj->IsNeverVisible() || CanNeverSee(obj))
        return false;

    if (obj->IsAlwaysVisibleFor(this))
        return true;

    if (CanAlwaysSee(obj))
        return true;

    bool corpseVisibility = false;
    if (distanceCheck)
    {
        bool corpseCheck = false;
        if (Player const* thisPlayer = ToPlayer())
        {
            if (thisPlayer->isDead() && thisPlayer->GetHealth() > 0 && // Cheap way to check for ghost state
                !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & GHOST_VISIBILITY_GHOST))
            {
                if (Corpse* corpse = thisPlayer->GetCorpse())
                {
                    corpseCheck = true;
                    if (corpse->IsWithinDist(thisPlayer, GetSightRange(obj), false))
                        if (corpse->IsWithinDist(obj, GetSightRange(obj), false))
                            corpseVisibility = true;
                }
            }
        }

        WorldObject const* viewpoint = this;
        if (Player const* player = this->ToPlayer())
            viewpoint = player->GetViewpoint();

        if (!viewpoint)
            viewpoint = this;

        if (!corpseCheck && !viewpoint->IsWithinDist(obj, GetSightRange(obj), false))
            return false;
    }

    // GM visibility off or hidden NPC
    if (!obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM))
    {
        // Stop checking other things for GMs
        if (m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM))
            return true;
    }
    else
        return m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM) >= obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM);

    // Ghost players, Spirit Healers, and some other NPCs
    if (!corpseVisibility && !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GHOST)))
    {
        // Alive players can see dead players in some cases, but other objects can't do that
        if (Player const* thisPlayer = ToPlayer())
        {
            if (thisPlayer->InArena() && thisPlayer->HasAura(8326) && !obj->ToUnit()->HasAura(8326) && CanDetect(obj, ignoreStealth))
                return true;
            if (Player const* objPlayer = obj->ToPlayer())
            {
                if (thisPlayer->GetTeam() != objPlayer->GetTeam() || !thisPlayer->IsGroupVisibleFor(objPlayer))
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (const Player* thisPlayer = ToPlayer())
        if (obj->ToUnit())
            if(thisPlayer->InArena() && thisPlayer->HasAura(8326) && obj->ToUnit()->HasAura(8326) && !thisPlayer->IsInPartyWith(obj->ToUnit()))
                return false;

    if (obj->IsInvisibleDueToDespawn())
        return false;

    if (!CanDetect(obj, ignoreStealth))
        return false;

    return true;
}

bool WorldObject::CanDetect(WorldObject const* obj, bool ignoreStealth) const
{
    const WorldObject* seer = this;

    // Pets don't have detection, they use the detection of their masters
    if (const Unit* thisUnit = ToUnit())
        if (Unit* controller = thisUnit->GetCharmerOrOwner())
            seer = controller;

    if (obj->IsAlwaysDetectableFor(seer))
        return true;

    if (!ignoreStealth && !seer->CanDetectInvisibilityOf(obj))
        return false;

    if (!ignoreStealth && !seer->CanDetectStealthOf(obj))
        return false;

    return true;
}

bool WorldObject::CanDetectInvisibilityOf(WorldObject const* obj) const
{
    // pets are invisible if caster is invisible
    if (obj->ToUnit() && obj->ToUnit()->GetOwner())
        return this->CanDetectInvisibilityOf(obj->ToUnit()->GetOwner());

    uint32 mask = obj->m_invisibility.GetFlags() & m_invisibilityDetect.GetFlags();

    // caster can see targets if has 335 aura
    if (this->ToUnit() && this->ToUnit()->HasAuraType(SPELL_AURA_CAN_SEE_OR_DETECT_STEALTH) && !obj->m_invisibility.GetFlags())
        return true;

    if (obj->ToUnit())
    {
        if (((Unit*)obj)->HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, GetGUID()))
            return true;
    }
    // Check for not detected types
    if (mask != obj->m_invisibility.GetFlags())
        return false;

    // It isn't possible in invisibility to detect something that can't detect the invisible object
    // (it's at least true for spell: 66)
    // It seems like that only Units are affected by this check (couldn't see arena doors with preparation invisibility)
    if (obj->ToUnit())
    {
        if ((m_invisibility.GetFlags() & obj->m_invisibilityDetect.GetFlags()) != m_invisibility.GetFlags())
            return false;
    }

    for (uint32 i = 0; i < TOTAL_INVISIBILITY_TYPES; ++i)
    {
        if (!(mask & (1 << i)))
            continue;

        int32 objInvisibilityValue = obj->m_invisibility.GetValue(InvisibilityType(i));
        int32 ownInvisibilityDetectValue = m_invisibilityDetect.GetValue(InvisibilityType(i));

        // Too low value to detect
        if (ownInvisibilityDetectValue < objInvisibilityValue)
            return false;
    }

    return true;
}

bool WorldObject::CanDetectStealthOf(WorldObject const* obj) const
{
    // Combat reach is the minimal distance (both in front and behind),
    //   and it is also used in the range calculation.
    // One stealth point increases the visibility range by 0.3 yard.

    if (!obj->m_stealth.GetFlags())
        return true;

    float distance = GetExactDist(obj);

    // stealth detection of traps = invisibility detection, calculate from compare detection and stealth values
    if (obj->m_stealth.HasFlag(STEALTH_TRAP))
    {
        if (!HasInArc(M_PI, obj))
            return false;

        // rogue class - detect traps limit to 20 yards
        float maxDetectDistance = 20.0f;
        if (distance > maxDetectDistance)
            return false;

        int32 objTrapStealthValue = obj->m_stealth.GetValue(STEALTH_TRAP);
        int32 ownTrapStealthDetectValue = m_stealthDetect.GetValue(STEALTH_TRAP);

        if (ownTrapStealthDetectValue < objTrapStealthValue)
            // not rogue class - detect traps limit to melee distance
            if (distance > 4.0f)
                return false;
    }
    else
    {
        float combatReach = 0.0f;

        if (isType(TYPEMASK_UNIT))
        {
            combatReach = ((Unit*)this)->GetCombatReach();
            if (distance < combatReach)
                return true;

            if (((Unit*)this)->HasAuraType(SPELL_AURA_DETECT_STEALTH))
                return true;

            if (((Unit*)obj)->HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, GetGUID()))
                return true;
        }

        if (!HasInArc(M_PI, obj))
            return false;

        // Starting points
        int32 detectionValue = 15;
        if (isType(TYPEMASK_PLAYER) && ToPlayer())
        {
            Player const* _plr = ToPlayer();
            uint8 _class = _plr->getClass();
            if (_class == CLASS_ROGUE || (_class == CLASS_DRUID && _plr->GetPrimaryTalentTreeForCurrentSpec() == TALENT_TREE_DRUID_FERAL_COMBAT))
                detectionValue = 50;
        }

        // Level difference: 5 point / level, starting from level 1.
        // There may be spells for this and the starting points too, but
        // not in the DBCs of the client.
        detectionValue += int32(getLevelForTarget(obj) - 1) * 5;

        // Apply modifiers
        detectionValue += m_stealthDetect.GetValue(STEALTH_GENERAL);
        if (obj->isType(TYPEMASK_GAMEOBJECT))
            if (Unit* owner = ((GameObject*)obj)->GetOwner())
                detectionValue -= int32(owner->getLevelForTarget(this) - 1) * 5;

        detectionValue -= obj->m_stealth.GetValue(STEALTH_GENERAL);

        // Calculate max distance
        float visibilityRange = float(detectionValue) * 0.3f + combatReach;

        if (visibilityRange > MAX_PLAYER_STEALTH_DETECT_RANGE)
            visibilityRange = MAX_PLAYER_STEALTH_DETECT_RANGE;

        if (distance > visibilityRange)
            return false;
    }

    return true;
}

void WorldObject::SendPlaySound(uint32 Sound, bool OnlySelf)
{
    if (OnlySelf && GetTypeId() == TYPEID_PLAYER)
    {
        ToPlayer()->SendSound(Sound, GetGUID());
        return;
    }

    WorldPacket data(SMSG_PLAY_SOUND, 12);
    data << uint32(Sound);
    data << uint64(GetGUID());
    SendMessageToSet(&data, true); // ToSelf ignored in this case
}

void Object::ForceValuesUpdateAtIndex(uint32 i)
{
    _changesMask.SetBit(i);
    if (m_inWorld && !m_objectUpdated)
    {
        sObjectAccessor->AddUpdateObject(this);
        m_objectUpdated = true;
    }
}

namespace Trinity
{
    class MonsterChatBuilder
    {
        public:
            MonsterChatBuilder(WorldObject const& obj, ChatMsg msgtype, int32 textId, uint32 language, uint64 targetGUID)
                : i_object(obj), i_msgtype(msgtype), i_textId(textId), i_language(language), i_targetGUID(targetGUID) {}
            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);

                // TODO: i_object.GetName() also must be localized?
                i_object.BuildMonsterChat(&data, i_msgtype, text, i_language, i_object.GetNameForLocaleIdx(loc_idx), i_targetGUID);
            }

        private:
            WorldObject const& i_object;
            ChatMsg i_msgtype;
            int32 i_textId;
            uint32 i_language;
            uint64 i_targetGUID;
    };

    class MonsterCustomChatBuilder
    {
        public:
            MonsterCustomChatBuilder(WorldObject const& obj, ChatMsg msgtype, const char* text, uint32 language, uint64 targetGUID)
                : i_object(obj), i_msgtype(msgtype), i_text(text), i_language(language), i_targetGUID(targetGUID) {}
            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                // TODO: i_object.GetName() also must be localized?
                i_object.BuildMonsterChat(&data, i_msgtype, i_text, i_language, i_object.GetNameForLocaleIdx(loc_idx), i_targetGUID);
            }

        private:
            WorldObject const& i_object;
            ChatMsg i_msgtype;
            const char* i_text;
            uint32 i_language;
            uint64 i_targetGUID;
    };
}                                                           // namespace Trinity

void WorldObject::MonsterSay(const char* text, uint32 language, uint64 TargetGuid) const
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::MonsterCustomChatBuilder say_build(*this, CHAT_MSG_MONSTER_SAY, text, language, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> say_do(say_build);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> > say_worker(this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), say_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY));
}

void WorldObject::DebugMonsterSay(const char* msg, ...) const
{
    va_list ap;
    char text[MAX_QUERY_LEN];
    va_start(ap, msg);
    vsnprintf(text, MAX_QUERY_LEN, msg, ap);
    MonsterSay(text, 0, 0);
}

void WorldObject::DebugMonsterSayTimer( const char* msg, ... ) const
{
    va_list ap;
    char text[MAX_QUERY_LEN];
    va_start(ap, msg);
    vsnprintf(text, MAX_QUERY_LEN, msg, ap);
    sprintf(text, "%s, %u", text, getMSTime());
    MonsterSay(text, 0, 0);

}

void WorldObject::DebugMonsterSayTimerDelta( const char* msg, ... ) const
{
    va_list ap;
    char text[MAX_QUERY_LEN];
    va_start(ap, msg);
    vsnprintf(text, MAX_QUERY_LEN, msg, ap);
    sprintf(text, "%s, %u", text, sWorld->GetMonsterSayDebugTimerAndUpdate());
    MonsterSay(text, 0, 0);

}

void WorldObject::DebugMonsterSayIncCounter(const char* msg, ...) const
{
    va_list ap;
    char text[MAX_QUERY_LEN];
    va_start(ap, msg);
    vsnprintf(text, MAX_QUERY_LEN, msg, ap);
    sprintf(text, "%s, counter %u", text, sWorld->IncMonsterSayDebugCounter());
    MonsterSay(text, 0, 0);
}

void WorldObject::DebugMonsterSayResetCounter(const char* msg, ...) const
{
    sWorld->ResetMonsterSayDebugCounter();
    va_list ap;
    char text[MAX_QUERY_LEN];
    va_start(ap, msg);
    vsnprintf(text, MAX_QUERY_LEN, msg, ap);
    sprintf(text, "%s, counter reseted", text);
    MonsterSay(text, 0, 0);
}

void WorldObject::MonsterSay(int32 textId, uint32 language, uint64 TargetGuid) const
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_SAY, textId, language, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> say_do(say_build);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> > say_worker(this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), say_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY));
}

void WorldObject::MonsterYell(const char* text, uint32 language, uint64 TargetGuid)
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::MonsterCustomChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, text, language, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> say_do(say_build);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> > say_worker(this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL), say_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterCustomChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL));
}

void WorldObject::MonsterYell(int32 textId, uint32 language, uint64 TargetGuid)
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, language, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> say_do(say_build);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> > say_worker(this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL), say_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL));
}

void WorldObject::MonsterYellToZone(int32 textId, uint32 language, uint64 TargetGuid)
{
    Trinity::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, language, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> say_do(say_build);

    uint32 zoneid = GetZoneId();

    Map::PlayerList const& pList = GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (itr->getSource()->GetZoneId() == zoneid)
            say_do(itr->getSource());
}

void WorldObject::MonsterTextEmote(const char* text, uint64 TargetGuid, bool IsBossEmote)
{
    WorldPacket data;
    BuildMonsterChat(&data, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, text, LANG_UNIVERSAL, GetName(), TargetGuid);
    SendMessageToSetInRange(&data, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), true);
}

void WorldObject::MonsterTextEmote(int32 textId, uint64 TargetGuid, bool IsBossEmote)
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::MonsterChatBuilder say_build(*this, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, textId, LANG_UNIVERSAL, TargetGuid);
    Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> say_do(say_build);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> > say_worker(this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), say_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));
}

void WorldObject::MonsterWhisper(const char* text, uint64 receiver, bool IsBossWhisper)
{
    Player* player = ObjectAccessor::FindPlayer(receiver);
    if (!player || !player->GetSession())
        return;

    LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();

    WorldPacket data;
    BuildMonsterChat(&data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, GetNameForLocaleIdx(loc_idx), receiver);

    player->GetSession()->SendPacket(&data);
}

void WorldObject::DoPersonalScriptText(int32 textId, Player* player, bool everywhere)
{
    if ((!player || !player->GetSession() || GetDistance2d(player) > 50.0f) && !everywhere)
        return;

    if (textId >= 0)
    {
        sLog->outError(LOG_FILTER_TSCR, "TSCR: DoPersonalScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process text entry %i, but text entry must be negative.", GetEntry(), GetTypeId(), GetGUIDLow(), textId);
        return;
    }

    const StringTextData* pData = sScriptSystemMgr->GetTextData(textId);

    if (!pData)
    {
        sLog->outError(LOG_FILTER_TSCR, "TSCR: DoPersonalScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", GetEntry(), GetTypeId(), GetGUIDLow(), textId);
        return;
    }

    sLog->outDebug(LOG_FILTER_TSCR, "TSCR: DoPersonalScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u", textId, pData->uiSoundId, pData->uiType, pData->uiLanguage, pData->uiEmote);

    if (pData->uiSoundId)
    {
        if (sSoundEntriesStore.LookupEntry(pData->uiSoundId))
            player->SendPlaySound(pData->uiSoundId, true);
        else
            sLog->outError(LOG_FILTER_TSCR, "TSCR: DoPersonalScriptText entry %i tried to process invalid sound id %u.", textId, pData->uiSoundId);
    }

    if (pData->uiEmote)
    {
        WorldPacket data(SMSG_EMOTE, 4 + 8);
        data << uint32(pData->uiEmote);
        data << uint64(GetGUID());
        player->GetSession()->SendPacket(&data);
    }

    LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
    char const* text = sObjectMgr->GetTrinityString(textId, loc_idx);

    WorldPacket chat_data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&chat_data, pData->uiType, text, LANG_UNIVERSAL, GetNameForLocaleIdx(loc_idx), player->GetGUID());

    player->GetSession()->SendPacket(&chat_data);
}

void WorldObject::MonsterWhisper(int32 textId, uint64 receiver, bool IsBossWhisper)
{
    Player* player = ObjectAccessor::FindPlayer(receiver);
    if (!player || !player->GetSession())
        return;

    LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
    char const* text = sObjectMgr->GetTrinityString(textId, loc_idx);

    WorldPacket data;
    BuildMonsterChat(&data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, GetNameForLocaleIdx(loc_idx), receiver);

    player->GetSession()->SendPacket(&data);
}

/*Packet size:  21 + nlen + 8 + (isNoPlayerGuid ? 5 : 0) + 4 + len + 1*/
void WorldObject::BuildMonsterChat(WorldPacket* data, uint8 msgtype, char const* text, uint32 language, char const* name, uint64 targetGuid) const
{
    bool isNoPlayerGuid = targetGuid && !IS_PLAYER_GUID(targetGuid);
    unsigned int len = (unsigned int)strlen(text)+1;
    unsigned int nlen = (unsigned int)strlen(name)+1;
    {
        size_t newSize = size_t(21 + nlen + 8 + (isNoPlayerGuid ? 5 : 0) + 4 + len + 1 + ((msgtype == CHAT_MSG_RAID_BOSS_EMOTE || msgtype == CHAT_MSG_RAID_BOSS_WHISPER) ? 5 : 0));
        data->Initialize(SMSG_MESSAGECHAT, newSize);
    }

    *data << uint8(msgtype);
    *data << uint32(language);
    *data << uint64(GetGUID());
    *data << uint32(0);                                     // 2.1.0
    *data << nlen;
    *data << name;
    *data << uint64(targetGuid);                            // Unit Target
    if (isNoPlayerGuid)
    {
        *data << uint32(1);                                 // target name length
        *data << uint8(0);                                  // target name
    }
    *data << len;
    *data << text;
    *data << uint8(0);                                      // ChatTag

    if (msgtype == CHAT_MSG_RAID_BOSS_EMOTE || msgtype == CHAT_MSG_RAID_BOSS_WHISPER)
    {
        *data << float(0);
        *data << uint8(0);
    }
}

void WorldObject::SendMessageToSet(WorldPacket* data, bool self)
{
    if (IsInWorld())
        SendMessageToSetInRange(data, GetVisibilityRange(), self);
}

void WorldObject::SendMessageToSetInRange(WorldPacket* data, float dist, bool /*self*/)
{
    Trinity::MessageDistDeliverer notifier(this, data, dist);
    VisitNearbyWorldObject(dist, notifier);
}

void WorldObject::SendMessageToSet(WorldPacket* data, Player const* skipped_rcvr)
{
    Trinity::MessageDistDeliverer notifier(this, data, GetVisibilityRange(), false, skipped_rcvr);
    VisitNearbyWorldObject(GetVisibilityRange(), notifier);
}

void WorldObject::SendObjectDeSpawnAnim(uint64 guid)
{
    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
    data << uint64(guid);
    SendMessageToSet(&data, true);
}

void WorldObject::SetMap(Map* map)
{
    ASSERT(map);
    ASSERT(!IsInWorld() || GetTypeId() == TYPEID_CORPSE);
    if (m_currMap == map) // command add npc: first create, than loadfromdb
        return;
    if (m_currMap)
    {
        sLog->outFatal(LOG_FILTER_GENERAL, "WorldObject::SetMap: obj %u new map %u %u, old map %u %u", (uint32)GetTypeId(), map->GetId(), map->GetInstanceId(), m_currMap->GetId(), m_currMap->GetInstanceId());
        ASSERT(false);
    }
    m_currMap = map;
    m_mapId = map->GetId();
    m_InstanceId = map->GetInstanceId();
    if (IsWorldObject())
        m_currMap->AddWorldObject(this);
}

void WorldObject::ResetMap()
{
    ASSERT(m_currMap);
    ASSERT(!IsInWorld());
    if (IsWorldObject())
        m_currMap->RemoveWorldObject(this);
    m_currMap = NULL;
}

Map const* WorldObject::GetBaseMap() const
{
    ASSERT(m_currMap);
    return m_currMap->GetParent();
}

void WorldObject::AddObjectToRemoveList()
{
    ASSERT(m_uint32Values);

    Map* map = FindMap();
    if (!map)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Object (TypeId: %u Entry: %u GUID: %u) at attempt add to move list not have valid map (Id: %u).", GetTypeId(), GetEntry(), GetGUIDLow(), GetMapId());
        return;
    }

    map->AddObjectToRemoveList(this);
}

TempSummon* Map::SummonCreature(uint32 entry, Position const& pos, SummonPropertiesEntry const* properties /*= NULL*/, uint32 duration /*= 0*/, Unit* summoner /*= NULL*/, uint32 spellId /*= 0*/, uint32 vehId /*= 0*/)
{
    uint32 mask = UNIT_MASK_SUMMON;
    if (properties)
    {
        switch (properties->Category)
        {
            case SUMMON_CATEGORY_PET:
                mask = UNIT_MASK_GUARDIAN;
                break;
            case SUMMON_CATEGORY_PUPPET:
                mask = UNIT_MASK_PUPPET;
                break;
            case SUMMON_CATEGORY_VEHICLE:
                mask = UNIT_MASK_MINION;
                break;
            case SUMMON_CATEGORY_WILD:
            case SUMMON_CATEGORY_ALLY:
            case SUMMON_CATEGORY_UNK:
            {
                switch (properties->Type)
                {
                case SUMMON_TYPE_MINION:
                case SUMMON_TYPE_GUARDIAN:
                case SUMMON_TYPE_GUARDIAN2:
                    mask = UNIT_MASK_GUARDIAN;
                    break;
                case SUMMON_TYPE_TOTEM:
                case SUMMON_TYPE_LIGHTWELL:
                    mask = UNIT_MASK_TOTEM;
                    break;
                case SUMMON_TYPE_VEHICLE:
                case SUMMON_TYPE_VEHICLE2:
                    mask = UNIT_MASK_SUMMON;
                    break;
                case SUMMON_TYPE_MINIPET:
                    mask = UNIT_MASK_MINION;
                    break;
                default:
                    if (properties->Flags & 512) // Mirror Image, Summon Gargoyle
                        mask = UNIT_MASK_GUARDIAN;
                    break;
                }
                break;
            }
            default:
                return NULL;
        }
    }

    uint32 phase = PHASEMASK_NORMAL;
    uint32 team = 0;
    if (summoner)
    {
        phase = summoner->GetPhaseMask();
        if (summoner->GetTypeId() == TYPEID_PLAYER)
            team = summoner->ToPlayer()->GetTeam();
    }

    TempSummon* summon = NULL;
    switch (mask)
    {
        case UNIT_MASK_SUMMON:
            summon = new TempSummon(properties, summoner, false);
            break;
        case UNIT_MASK_GUARDIAN:
            summon = new Guardian(properties, summoner, false);
            break;
        case UNIT_MASK_PUPPET:
            summon = new Puppet(properties, summoner);
            break;
        case UNIT_MASK_TOTEM:
            summon = new Totem(properties, summoner);
            break;
        case UNIT_MASK_MINION:
            summon = new Minion(properties, summoner, false);
            break;
        default:
            return NULL;
    }

    if (!summon->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), this, phase, entry, vehId, team, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
    {
        delete summon;
        return NULL;
    }

    summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, spellId);

    summon->SetHomePosition(pos);

    summon->InitStats(duration);
    AddToMap(summon->ToCreature());
    summon->InitSummon();

    //ObjectAccessor::UpdateObjectVisibility(summon);

    return summon;
}

/**
* Summons group of creatures.
*
* @param group Id of group to summon.
* @param list  List to store pointers to summoned creatures.
*/

void Map::SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list /*= NULL*/)
{
    std::vector<TempSummonData> const* data = sObjectMgr->GetSummonGroup(GetId(), SUMMONER_TYPE_MAP, group);
    if (!data)
        return;

    for (std::vector<TempSummonData>::const_iterator itr = data->begin(); itr != data->end(); ++itr)
        if (TempSummon* summon = SummonCreature(itr->entry, itr->pos, NULL, itr->time))
            if (list)
                list->push_back(summon);
}

void WorldObject::SetZoneScript()
{
    if (Map* map = FindMap())
    {
        if (map->IsDungeon())
            m_zoneScript = (ZoneScript*)((InstanceMap*)map)->GetInstanceScript();
        else if (!map->IsBattlegroundOrArena())
        {
            if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(GetZoneId()))
                m_zoneScript = bf;
            else
            {
                if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(GetZoneId()))
                    m_zoneScript = bf;
                else
                    m_zoneScript = sOutdoorPvPMgr->GetZoneScript(GetZoneId());
            }
        }
    }
}

TempSummon* WorldObject::SummonCreature(uint32 entry, const Position &pos, TempSummonType spwtype, uint32 duration, uint32 /*vehId*/) const
{
    if (Map* map = FindMap())
    {
        Unit* owner = NULL;
        if (isType(TYPEMASK_UNIT))
            owner = (Unit*)this;
        else if (isType(TYPEMASK_GAMEOBJECT))
            owner = ToGameObject()->GetOwner();

        if (TempSummon* summon = map->SummonCreature(entry, pos, NULL, duration, owner))
        {
            summon->SetTempSummonType(spwtype);
            return summon;
        }
    }

    return NULL;
}

Pet* Player::SummonPet(uint32 entry, float x, float y, float z, float ang, PetType petType, uint32 duration, PetSlot slotID)
{
    Pet* pet = new Pet(this, petType);

    bool currentPet = (slotID != PET_SLOT_UNK_SLOT);
    if (pet->GetOwner() && pet->GetOwner()->getClass() != CLASS_HUNTER)
        currentPet = false;

    if (petType == SUMMON_PET && pet->LoadPetFromDB(this, entry, 0, currentPet, slotID))
    {
        // Remove Demonic Sacrifice auras (known pet)
        Unit::AuraEffectList const& auraClassScripts = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraEffectList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetMiscValue() == 2228)
            {
                RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }

        if (pet->IsPetGhoul())
            pet->setPowerType(POWER_ENERGY);

        if (duration > 0)
            pet->SetDuration(duration);

        return NULL;
    }

    // petentry == 0 for hunter "call pet" (current pet summoned if any)
    if (!entry)
    {
        delete pet;
        return NULL;
    }

    pet->Relocate(x, y, z, ang);
    if (!pet->IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "Pet (guidlow %d, entry %d) not summoned. Suggested coordinates isn't valid (X: %f Y: %f)", pet->GetGUIDLow(), pet->GetEntry(), pet->GetPositionX(), pet->GetPositionY());
        delete pet;
        return NULL;
    }

    Map* map = GetMap();
    uint32 pet_number = sObjectMgr->GeneratePetNumber();
    if (!pet->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_PET), map, GetPhaseMask(), entry, pet_number))
    {
        sLog->outError(LOG_FILTER_GENERAL, "no such creature entry %u", entry);
        delete pet;
        return NULL;
    }

    pet->SetCreatorGUID(GetGUID());
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, getFaction());

    pet->setPowerType(POWER_MANA);
    pet->SetUInt32Value(UNIT_NPC_FLAGS, 0);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
    pet->InitStatsForLevel(getLevel());

    SetMinion(pet, true, PET_SLOT_OTHER_PET);

    switch (petType)
    {
        case SUMMON_PET:
            // this enables pet details window (Shift+P)
            pet->GetCharmInfo()->SetPetNumber(pet_number, true);
            pet->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
            pet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
            pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
            pet->SetFullHealth();
            pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
            pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped in this case
            break;
        default:
            break;
    }

    map->AddToMap(pet->ToCreature());

    switch (petType)
    {
        case SUMMON_PET:
            pet->InitPetCreateSpells();
            pet->InitTalentForLevel();
            pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT);
            PetSpellInitialize();
            break;
        default:
            break;
    }

    if (petType == SUMMON_PET)
    {
        // Remove Demonic Sacrifice auras (known pet)
        Unit::AuraEffectList const& auraClassScripts = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraEffectList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetMiscValue() == 2228)
            {
                RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }
    }

    if (duration > 0)
        pet->SetDuration(duration);

    //ObjectAccessor::UpdateObjectVisibility(pet);

    return pet;
}

GameObject* WorldObject::SummonGameObject(uint32 entry, float x, float y, float z, float ang, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime)
{
    if (!IsInWorld())
        return NULL;

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goinfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Gameobject template %u not found in database!", entry);
        return NULL;
    }
    Map* map = GetMap();
    GameObject* go = new GameObject();
    if (!go->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, map, GetPhaseMask(), x, y, z, ang, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        delete go;
        return NULL;
    }
    go->SetRespawnTime(respawnTime);
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT) //not sure how to handle this
        ((Unit*)this)->AddGameObject(go);
    else
        go->SetSpawnedByDefault(false);
    map->AddToMap(go);

    return go;
}

Creature* WorldObject::SummonTrigger(float x, float y, float z, float ang, uint32 duration, CreatureAI* (*GetAI)(Creature*))
{
    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Creature* summon = SummonCreature(WORLD_TRIGGER, x, y, z, ang, summonType, duration);
    if (!summon)
        return NULL;

    //summon->SetName(GetName());
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT)
    {
        summon->setFaction(((Unit*)this)->getFaction());
        summon->SetLevel(((Unit*)this)->getLevel());
    }

    if (GetAI)
        summon->AIM_Initialize(GetAI(summon));
    return summon;
}

/**
* Summons group of creatures. Should be called only by instances of Creature and GameObject classes.
*
* @param group Id of group to summon.
* @param list  List to store pointers to summoned creatures.
*/
void WorldObject::SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list /*= NULL*/)
{
    ASSERT((GetTypeId() == TYPEID_GAMEOBJECT || GetTypeId() == TYPEID_UNIT) && "Only GOs and creatures can summon npc groups!");

    std::vector<TempSummonData> const* data = sObjectMgr->GetSummonGroup(GetEntry(), GetTypeId() == TYPEID_GAMEOBJECT ? SUMMONER_TYPE_GAMEOBJECT : SUMMONER_TYPE_CREATURE, group);
    if (!data)
        return;

    for (std::vector<TempSummonData>::const_iterator itr = data->begin(); itr != data->end(); ++itr)
        if (TempSummon* summon = SummonCreature(itr->entry, itr->pos, itr->type, itr->time))
            if (list)
                list->push_back(summon);
}

Creature* WorldObject::FindNearestCreature(uint32 entry, float range, bool alive) const
{
    Creature* creature = NULL;
    Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*this, entry, alive, range);
    Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(this, creature, checker);
    VisitNearbyObject(range, searcher);
    return creature;
}

GameObject* WorldObject::FindNearestGameObject(uint32 entry, float range) const
{
    GameObject* go = NULL;
    Trinity::NearestGameObjectEntryInObjectRangeCheck checker(*this, entry, range);
    Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectEntryInObjectRangeCheck> searcher(this, go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

GameObject* WorldObject::FindNearestGameObjectOfType(GameobjectTypes type, float range) const
{
    GameObject* go = NULL;
    Trinity::NearestGameObjectTypeInObjectRangeCheck checker(*this, type, range);
    Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectTypeInObjectRangeCheck> searcher(this, go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

Player* WorldObject::FindNearestPlayer(float range, bool alive)
{
  Player* player = NULL;
  Trinity::AnyPlayerInObjectRangeCheck check(this, range);
  Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, player, check);
  VisitNearbyWorldObject(range, searcher);
  return player;
}

void WorldObject::GetGameObjectListWithEntryInGrid(std::list<GameObject*>& gameobjectList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Trinity::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::AllGameObjectsWithEntryInRange check(this, entry, maxSearchRange);
    Trinity::GameObjectListSearcher<Trinity::AllGameObjectsWithEntryInRange> searcher(this, gameobjectList, check);
    TypeContainerVisitor<Trinity::GameObjectListSearcher<Trinity::AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

void WorldObject::GetCreatureListWithEntryInGrid(std::list<Creature*>& creatureList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Trinity::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::AllCreaturesOfEntryInRange check(this, entry, maxSearchRange);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(this, creatureList, check);
    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

//===================================================================================================

void WorldObject::GetNearPoint2D(float &x, float &y, float distance2d, float absAngle) const
{
    x = GetPositionX() + (GetObjectSize() + distance2d) * std::cos(absAngle);
    y = GetPositionY() + (GetObjectSize() + distance2d) * std::sin(absAngle);

    Trinity::NormalizeMapCoord(x);
    Trinity::NormalizeMapCoord(y);
}

void WorldObject::GetNearPoint(WorldObject const* searcher, float &x, float &y, float &z, float searcher_size, float distance2d, float absAngle) const
{
    GetNearPoint2D(x, y, distance2d+searcher_size, absAngle);
    z = GetPositionZ();
    if (!searcher || !searcher->ToCreature() || !searcher->GetMap()->Instanceable())
        UpdateAllowedPositionZ(x, y, z);
}

void WorldObject::MovePosition(Position &pos, float dist, float angle)
{
    angle += GetOrientation();
    float destx, desty, destz, ground, floor;
    destx = pos.m_positionX + dist * std::cos(angle);
    desty = pos.m_positionY + dist * std::sin(angle);

    // Prevent invalid coordinates here, position is unchanged
    if (!Trinity::IsValidMapCoord(destx, desty))
    {
        sLog->outFatal(LOG_FILTER_GENERAL, "WorldObject::MovePosition invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    ground = GetMap()->GetHeight(GetPhaseMask(), destx, desty, MAX_HEIGHT, true);
    floor = GetMap()->GetHeight(GetPhaseMask(), destx, desty, pos.m_positionZ, true);
    destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;

    float step = dist/10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (fabs(pos.m_positionZ - destz) > 6)
        {
            destx -= step * std::cos(angle);
            desty -= step * std::sin(angle);
            ground = GetMap()->GetHeight(GetPhaseMask(), destx, desty, MAX_HEIGHT, true);
            floor = GetMap()->GetHeight(GetPhaseMask(), destx, desty, pos.m_positionZ, true);
            destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;
        }
        // we have correct destz now
        else if (IsWithinLOS(destx, desty, destz))
        {
            pos.Relocate(destx, desty, destz);
            break;
        }
    }

    Trinity::NormalizeMapCoord(pos.m_positionX);
    Trinity::NormalizeMapCoord(pos.m_positionY);
    UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    pos.SetOrientation(GetOrientation());
}

void WorldObject::MovePositionToFirstCollision(Position &pos, float dist, float angle)
{
    angle += GetOrientation();
    float destx, desty, destz, ground, floor;
    pos.m_positionZ += 2.0f;
    destx = pos.m_positionX + dist * std::cos(angle);
    desty = pos.m_positionY + dist * std::sin(angle);

    // Prevent invalid coordinates here, position is unchanged
    if (!Trinity::IsValidMapCoord(destx, desty))
    {
        sLog->outFatal(LOG_FILTER_GENERAL, "WorldObject::MovePositionToFirstCollision invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    ground = GetMap()->GetHeight(GetPhaseMask(), destx, desty, MAX_HEIGHT, true);
    floor = GetMap()->GetHeight(GetPhaseMask(), destx, desty, pos.m_positionZ, true);
    destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;

    bool col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetMapId(), pos.m_positionX, pos.m_positionY, pos.m_positionZ+0.5f, destx, desty, destz+0.5f, destx, desty, destz, -0.5f);

    // collision occured
    if (col)
    {
        // move back a bit
        destx -= CONTACT_DISTANCE * std::cos(angle);
        desty -= CONTACT_DISTANCE * std::sin(angle);
        dist = sqrt((pos.m_positionX - destx)*(pos.m_positionX - destx) + (pos.m_positionY - desty)*(pos.m_positionY - desty));
    }

    // check dynamic collision
    col = GetMap()->getObjectHitPos(GetPhaseMask(), pos.m_positionX, pos.m_positionY, pos.m_positionZ+0.5f, destx, desty, destz+0.5f, destx, desty, destz, -0.5f);

    // Collided with a gameobject
    if (col)
    {
        destx -= CONTACT_DISTANCE * std::cos(angle);
        desty -= CONTACT_DISTANCE * std::sin(angle);
        dist = sqrt((pos.m_positionX - destx)*(pos.m_positionX - destx) + (pos.m_positionY - desty)*(pos.m_positionY - desty));
    }

    float step = dist/10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (fabs(pos.m_positionZ - destz) > 6)
        {
            destx -= step * std::cos(angle);
            desty -= step * std::sin(angle);
            ground = GetMap()->GetHeight(GetPhaseMask(), destx, desty, MAX_HEIGHT, true);
            floor = GetMap()->GetHeight(GetPhaseMask(), destx, desty, pos.m_positionZ, true);
            destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;
        }
        // we have correct destz now
        else
        {
            pos.Relocate(destx, desty, destz);
            break;
        }
    }

    Trinity::NormalizeMapCoord(pos.m_positionX);
    Trinity::NormalizeMapCoord(pos.m_positionY);
    UpdateAllowedPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    pos.SetOrientation(GetOrientation());
}

void WorldObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    m_phaseMask = newPhaseMask;

    if (update && IsInWorld())
        UpdateObjectVisibility();
}

void WorldObject::PlayDistanceSound(uint32 sound_id, Player* target /*= NULL*/)
{
    WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 4+8+8);
    data << uint32(sound_id);
    data << uint64(GetGUID());
    data << uint64(target ? target->GetGUID() : GetGUID());
    if (target)
        target->SendDirectMessage(&data);
    else
        SendMessageToSet(&data, true);
}

void WorldObject::PlayDirectSound(uint32 sound_id, Player* target /*= NULL*/)
{
    if (target)
    {
        target->SendSound(sound_id, GetGUID());
        return;
    }

    WorldPacket data(SMSG_PLAY_SOUND, 12);
    data << uint32(sound_id);
    data << uint64(GetGUID());
    SendMessageToSet(&data, true);
}

void WorldObject::DestroyForNearbyPlayers()
{
    if (!IsInWorld())
        return;

    std::list<Player*> targets;
    Trinity::AnyPlayerInObjectRangeCheck check(this, GetVisibilityRange(), false);
    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, targets, check);
    VisitNearbyWorldObject(GetVisibilityRange(), searcher);
    for (std::list<Player*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        Player* player = (*iter);

        if (player == this)
            continue;

        if (!player->HaveAtClient(this))
            continue;

        if (isType(TYPEMASK_UNIT) && ((Unit*)this)->GetCharmerGUID() == player->GetGUID()) // TODO: this is for puppet
            continue;

        DestroyForPlayer(player);
        player->m_clientGUIDs.erase(GetGUID());
    }
}

void WorldObject::UpdateObjectVisibility(bool /*forced*/)
{
    //updates object's visibility for nearby players
    Trinity::VisibleChangesNotifier notifier(*this);
    VisitNearbyWorldObject(GetVisibilityRange(), notifier);
}

struct WorldObjectChangeAccumulator
{
    UpdateDataMapType& i_updateDatas;
    WorldObject& i_object;
    std::set<uint64> plr_list;
    WorldObjectChangeAccumulator(WorldObject &obj, UpdateDataMapType &d) : i_updateDatas(d), i_object(obj) {}
    void Visit(PlayerMapType &m)
    {
        Player* source = NULL;
        for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->getSource();

            BuildPacket(source);

            if (!source->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = source->GetSharedVisionList().begin();
                for (; it != source->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }

    void Visit(CreatureMapType &m)
    {
        Creature* source = NULL;
        for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->getSource();
            if (!source->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = source->GetSharedVisionList().begin();
                for (; it != source->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }

    void Visit(DynamicObjectMapType &m)
    {
        DynamicObject* source = NULL;
        for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->getSource();
            uint64 guid = source->GetCasterGUID();

            if (IS_PLAYER_GUID(guid))
            {
                //Caster may be NULL if DynObj is in removelist
                if (Player* caster = ObjectAccessor::FindPlayer(guid))
                    if (caster->GetUInt64Value(PLAYER_FARSIGHT) == source->GetGUID())
                        BuildPacket(caster);
            }
        }
    }

    void BuildPacket(Player* player)
    {
        // Only send update once to a player
        if (plr_list.find(player->GetGUID()) == plr_list.end() && player->HaveAtClient(&i_object))
        {
            i_object.BuildFieldsUpdate(player, i_updateDatas);
            plr_list.insert(player->GetGUID());
        }
    }

    template<class SKIP> void Visit(GridRefManager<SKIP> &) {}
};

void WorldObject::BuildUpdate(UpdateDataMapType& data_map)
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());
    Cell cell(p);
    cell.SetNoCreate();
    WorldObjectChangeAccumulator notifier(*this, data_map);
    TypeContainerVisitor<WorldObjectChangeAccumulator, WorldTypeMapContainer > player_notifier(notifier);
    Map& map = *GetMap();
    //we must build packets for all visible players
    cell.Visit(p, player_notifier, map, *this, GetVisibilityRange());

    ClearUpdateMask(false);
}

uint64 WorldObject::GetTransGUID() const
{
    if (GetTransport())
        return GetTransport()->GetGUID();
    return 0;
}

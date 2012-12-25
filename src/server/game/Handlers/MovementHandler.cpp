/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Transport.h"
#include "Battleground.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"
#include "ObjectMgr.h"
#include "MovementStructures.h"

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket& /*recvPacket*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    // get the teleport destination
    WorldLocation const loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        LogoutPlayer(false);
        return;
    }

    uint32 locMapID = loc.GetMapId();

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(locMapID);
    InstanceTemplate const* mInstance = sObjectMgr->GetInstanceTemplate(locMapID);

    // reset instance validity, except if going to an instance inside an instance
    if (!GetPlayer()->m_InstanceValid && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    Map* oldMap = GetPlayer()->GetMap();
    if (GetPlayer()->IsInWorld())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Player (Name %s) is still in world when teleported from map %u to new map %u", GetPlayer()->GetName(), oldMap->GetId(), locMapID);
        oldMap->RemovePlayerFromMap(GetPlayer(), false);
    }

    // relocate the player to the teleport destination
    Map* newMap = sMapMgr->CreateMap(locMapID, GetPlayer());
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || !newMap->CanEnter(GetPlayer()))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Map %d could not be created for player %d, porting player to homebind", locMapID, GetPlayer()->GetGUIDLow());
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }
    else
        GetPlayer()->Relocate(&loc);

    GetPlayer()->ResetMap();
    GetPlayer()->SetMap(newMap);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    if (!GetPlayer()->GetMap()->AddPlayerToMap(GetPlayer()))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WORLD: failed to teleport player %s (%d) to map %d because of unknown reason!", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), locMapID);
        GetPlayer()->ResetMap();
        GetPlayer()->SetMap(oldMap);
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (_player->InBattleground())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattlegroundOrArena())
        {
            // We're not in BG
            _player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
            // reset destination bg team
            _player->SetBGTeam(0);
        }
        // join to bg case
        else if (Battleground* bg = _player->GetBattleground())
        {
            if (_player->IsInvitedForBattlegroundInstance(_player->GetBattlegroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if (!_player->InBattleground())
        {
            // short preparations to continue flight
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Initialize(*GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    Corpse* corpse = GetPlayer()->GetCorpse();
    if (corpse && corpse->GetType() != CORPSE_BONES && corpse->GetMapId() == GetPlayer()->GetMapId())
    {
        if (mEntry->IsDungeon())
        {
            GetPlayer()->ResurrectPlayer(0.5f, false);
            GetPlayer()->SpawnCorpseBones();
        }
    }

    bool allowMount = !mEntry->IsDungeon() || mEntry->IsBattlegroundOrArena();
    if (mInstance)
    {
        Difficulty diff = GetPlayer()->GetDifficulty(mEntry->IsRaid());
        if (MapDifficulty const* mapDiff = GetMapDifficultyData(mEntry->MapID, diff))
        {
            if (mapDiff->resetTime)
            {
                if (time_t timeReset = sInstanceSaveMgr->GetResetTimeFor(mEntry->MapID, diff))
                {
                    uint32 timeleft = uint32(timeReset - time(NULL));
                    GetPlayer()->SendInstanceResetWarning(mEntry->MapID, diff, timeleft);
                }
            }
        }
        allowMount = mInstance->AllowMount;
    }

    // mount allow check
    if (!allowMount)
        _player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // update zone immediately, otherwise leave channel will cause crash in mtmap
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea);
    GetPlayer()->UpdateZone(newzone, newarea);

    for (uint8 i = 0; i < 9; ++i)
        GetPlayer()->UpdateSpeed(UnitMoveType(i), true);

    // honorless target
    if (GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // in friendly area
    else if (GetPlayer()->IsPvP() && !GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
        GetPlayer()->UpdatePvP(false, false);

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recvPacket)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "MSG_MOVE_TELEPORT_ACK");

    ObjectGuid guid;
    uint32 flags, time;
    recvPacket >> flags >> time;

    recvPacket
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[4])

        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[0]);

    sLog->outDebug(LOG_FILTER_NETWORKIO, "MSG_MOVE_TELEPORT_ACK: Guid " UI64FMTD "Flags: %u, Time: %u", uint64(guid), flags, time/IN_MILLISECONDS);

    ASSERT(_player->m_mover);
    Player* _playerMover = _player->m_mover->ToPlayer();

    if (!_playerMover || !_playerMover->IsBeingTeleportedNear())
        return;

    if (guid != _playerMover->GetGUID())
        return;

    _playerMover->SetSemaphoreTeleportNear(false);

    WorldLocation const& dest = _playerMover->GetTeleportDest();
    _playerMover->UpdatePosition(dest, true);

    uint32 newzone, newarea;
    uint32 old_zone = _playerMover->GetZoneId();
    _playerMover->GetZoneAndAreaId(newzone, newarea);
    _playerMover->UpdateZone(newzone, newarea);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (_playerMover->pvpInfo.inHostileArea)
            _playerMover->CastSpell(_playerMover, 2479, true);

        // in friendly area
        else if (_playerMover->IsPvP() && !_playerMover->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
            _playerMover->UpdatePvP(false, false);
    }

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket& recvPacket)
{
    MovementInfo movementInfo;
    ReadMovementInfo(recvPacket, &movementInfo);

    Unit* mover = _player->m_mover;

    ASSERT(mover != NULL);                      // there must always be a mover

    Player* plrMover = mover->ToPlayer();
    bool fall = recvPacket.GetOpcode() == MSG_MOVE_FALL_LAND && plrMover && !plrMover->isInFlight();

    if (fall)
    {
        if (!movementInfo.Check(plrMover, recvPacket.GetOpcode()))
            return;

        plrMover->HandleFall(movementInfo);
    }

    if (!mover->m_movementInfo.AcceptClientChanges(_player, movementInfo, recvPacket.GetOpcode()))
        return;

    BuildMoveUpdatePacket(mover, &movementInfo, recvPacket.size(), _player);

    if (fall)
        plrMover->SetFallInformation(mover->m_movementInfo.fallTime, mover->m_movementInfo.pos.GetPositionZ());
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recvData)
{
    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack", "PitchRate" };

    UnitMoveType move_type;
    switch (recvData.GetOpcode())
    {
        case CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;        break;
        case CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;         break;
        case CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;    break;
        case CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;        break;
        case CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;   break;
        case CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;   break;
        case CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;      break;
        case CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK:          move_type = MOVE_PITCH_RATE;  break;
        default:
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", uint32(recvData.GetOpcode()));
            return;
    }

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);

    if (_player->GetGUID() != movementInfo.guid)
        return;

    float speed = movementInfo.speed;

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[move_type] > 0)
    {
        if (--_player->m_forced_speed_changes[move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - speed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > speed)         // must be greater - just correct
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName(), _player->GetSpeed(move_type), speed);
            _player->SetSpeed(move_type, _player->GetSpeedRate(move_type), true);
        }
        else                                                // must be lesser - cheating
        {
            sLog->outDebug(LOG_FILTER_GENERAL, "Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName(), _player->GetSession()->GetAccountId(), _player->GetSpeed(move_type), speed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket& recvPacket)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    ObjectGuid guid;

    recvPacket
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[3])

        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[7]);

    if (GetPlayer()->IsInWorld())
    {
        if (_player->m_mover->GetGUID() != guid)
            sLog->outError(LOG_FILTER_NETWORKIO, "HandleSetActiveMoverOpcode: incorrect mover guid: mover is " UI64FMTD " (%s - Entry: %u) and should be " UI64FMTD, uint64(guid), GetLogNameForGuid(guid), GUID_ENPART(guid), _player->m_mover->GetGUID());
    }
}

void WorldSession::HandleMoveNotActiveMover(WorldPacket &recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");

    MovementInfo mi;
    ReadMovementInfo(recvData, &mi);

    if (_player->m_mover->GetGUID() == mi.guid)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "HandleMoveNotActiveMover: incorrect mover guid: mover is " UI64FMTD " and should be " UI64FMTD " instead of " UI64FMTD,
            _player->m_mover->GetGUID(),
            _player->GetGUID(),
            mi.guid);
        return;
    }

    _player->m_movementInfo = mi;
}

void WorldSession::BuildMoveUpdatePacket(Unit* mover, MovementInfo* movementInfo, size_t size, Player const* skip) const
{
    ASSERT(mover);
    ASSERT(movementInfo);

    WorldPacket data(SMSG_PLAYER_MOVE, size);
    WriteMovementInfo(data, movementInfo);
    mover->SendMessageToSet(&data, skip);
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvData*/)
{
    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "CMSG_MOVE_KNOCK_BACK_ACK");

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);

    if (!_player->m_movementInfo.AcceptClientChanges(_player, movementInfo, recvData.GetOpcode()))
        return;

    WorldPacket data;
    if (!_player->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA) ||
        !_player->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
        data.Initialize(SMSG_PLAYER_MOVE, recvData.size());
    else
        data.Initialize(SMSG_MOVE_UPDATE_KNOCK_BACK, recvData.size());

    WriteMovementInfo(data, &movementInfo);
    _player->SendMessageToSet(&data, false);
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recvData)
{
    if (!_player->isAlive() || _player->isInCombat())
        return;

    bool agree;
    uint64 summonerGuid;
    recvData >> summonerGuid;
    recvData >> agree;

    _player->SummonIfPossible(agree);
}

void WorldSession::ReadMovementInfo(WorldPacket& data, MovementInfo* mi)
{
    bool hasMovementFlags = false;
    bool hasMovementFlags2 = false;
    bool hasTimestamp = false;
    bool hasOrientation = false;
    bool hasTransportData = false;
    bool hasPitch = false;
    bool hasSplineElevation = false;

    MovementStatusElements* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (sequence == NULL)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::ReadMovementInfo: No movement sequence found for opcode 0x%04X", uint32(data.GetOpcode()));
        return;
    }

    ObjectGuid guid;
    ObjectGuid tguid;

    for (uint32 i = 0; i < MSE_COUNT; ++i)
    {
        MovementStatusElements element = sequence[i];
        if (element == MSEEnd)
            break;

        if (element >= MSEHasGuidByte0 && element <= MSEHasGuidByte7)
        {
            data.ReadByteMask(guid[element - MSEHasGuidByte0]);
            continue;
        }

        if (element >= MSEHasTransportGuidByte0 &&
            element <= MSEHasTransportGuidByte7)
        {
            if (hasTransportData)
                data.ReadByteMask(tguid[element - MSEHasTransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.ReadByteSeq(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (hasTransportData)
                data.ReadByteSeq(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        switch (element)
        {
            case MSEHasMovementFlags:
                hasMovementFlags = !data.ReadBit();
                break;
            case MSEHasMovementFlags2:
                hasMovementFlags2 = !data.ReadBit();
                break;
            case MSEHasTimestamp:
                hasTimestamp = !data.ReadBit();
                break;
            case MSEHasOrientation:
                hasOrientation = !data.ReadBit();
                break;
            case MSEHasTransportData:
                hasTransportData = data.ReadBit();
                break;
            case MSEHasTransportTime2:
                if (hasTransportData)
                {
                    if (data.ReadBit())
                        mi->AddServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2);
                    else
                        mi->RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2);
                }
                break;
            case MSEHasTransportTime3:
                if (hasTransportData)
                {
                    if (data.ReadBit())
                        mi->AddServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3);
                    else
                        mi->RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3);
                }
                break;
            case MSEHasPitch:
                hasPitch = !data.ReadBit();
                break;
            case MSEHasFallData:
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_FALLDATA);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_FALLDATA);
                break;
            case MSEHasFallDirection:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                {
                    if (data.ReadBit())
                        mi->AddServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION);
                    else
                        mi->RemoveServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION);
                }                    
                break;
            case MSEHasSplineElevation:
                hasSplineElevation = !data.ReadBit();
                break;
            case MSEHasSpline:
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_SPLINE1);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_SPLINE1);
                break;
            case MSEHasSpline2:
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_SPLINE2);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_SPLINE2);
                break;
            case MSEMovementFlags:
                if (hasMovementFlags)
                    mi->flags = data.ReadBits(30);
                break;
            case MSEMovementFlags2:
                if (hasMovementFlags2)
                    mi->flags2 = data.ReadBits(12);
                break;
            case MSETimestamp:
                if (hasTimestamp)
                    data >> mi->time;
                break;
            case MSEPositionX:
                data >> mi->pos.m_positionX;
                break;
            case MSEPositionY:
                data >> mi->pos.m_positionY;
                break;
            case MSEPositionZ:
                data >> mi->pos.m_positionZ;
                break;
            case MSEOrientation:
                if (hasOrientation)
                    data >> mi->pos.m_orientation;
                break;
            case MSETransportPositionX:
                if (hasTransportData)
                    data >> mi->t_pos.m_positionX;
                break;
            case MSETransportPositionY:
                if (hasTransportData)
                    data >> mi->t_pos.m_positionY;
                break;
            case MSETransportPositionZ:
                if (hasTransportData)
                    data >> mi->t_pos.m_positionZ;
                break;
            case MSETransportOrientation:
                if (hasTransportData)
                    data >> mi->t_pos.m_orientation;
                break;
            case MSETransportSeat:
                if (hasTransportData)
                    data >> mi->t_seat;
                break;
            case MSETransportTime:
                if (hasTransportData)
                    data >> mi->t_time;
                break;
            case MSETransportTime2:
                if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                    data >> mi->t_time2;
                break;
            case MSETransportTime3:
                if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                    data >> mi->t_time3;
                break;
            case MSEPitch:
                if (hasPitch)
                    data >> mi->pitch;
                break;
            case MSEFallTime:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data >> mi->fallTime;
                break;
            case MSEFallVerticalSpeed:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data >> mi->j_zspeed;
                break;
            case MSEFallCosAngle:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data >> mi->j_cosAngle;
                break;
            case MSEFallSinAngle:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data >> mi->j_sinAngle;
                break;
            case MSEFallHorizontalSpeed:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data >> mi->j_xyspeed;
                break;
            case MSESplineElevation:
                if (hasSplineElevation)
                    data >> mi->splineElevation;
                break;
            case MSEZeroBit:
            case MSEOneBit:
                data.ReadBit();
                break;
            case MSEGenericDword0:
            case MSEGenericDword1:
            case MSEMovementCounter:
                data.read_skip<uint32>();
                break;
            case MSESpeed:
                data >> mi->speed;
                break;
            case MSE2Bits:
                data.ReadBits(2);
                break;
            default:
                ASSERT(false && "Incorrect sequence element detected at ReadMovementInfo");
                break;
        }
    }

    mi->guid = guid;
    mi->t_guid = tguid;

    mi->Normalize();

   if (hasTransportData && mi->pos.m_positionX != mi->t_pos.m_positionX)
       if (GetPlayer()->GetTransport())
           GetPlayer()->GetTransport()->UpdatePosition(mi);

    //! Anti-cheat checks. Please keep them in seperate if() blocks to maintain a clear overview.
    //! Might be subject to latency, so just remove improper flags.
    #ifdef TRINITY_DEBUG
    #define REMOVE_VIOLATING_FLAGS(check, maskToRemove) \
    { \
        if (check) \
        { \
            sLog->outDebug(LOG_FILTER_UNITS, "WorldSession::ReadMovementInfo: Violation of MovementFlags found (%s). " \
                "MovementFlags: %u, MovementFlags2: %u for player GUID: %u. Mask %u will be removed.", \
                STRINGIZE(check), mi->GetMovementFlags(), mi->GetExtraMovementFlags(), GetPlayer()->GetGUIDLow(), maskToRemove); \
            mi->RemoveMovementFlag((maskToRemove)); \
        } \
    }
    #else
    #define REMOVE_VIOLATING_FLAGS(check, maskToRemove) \
        if (check) \
            mi->RemoveMovementFlag((maskToRemove));
    #endif


    /*! This must be a packet spoofing attempt. MOVEMENTFLAG_ROOT sent from the client is not valid
        in conjunction with any of the moving movement flags such as MOVEMENTFLAG_FORWARD.
        It will freeze clients that receive this player's movement info.
    */
    //REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_ROOT),
        //MOVEMENTFLAG_ROOT);

    //! Cannot hover without SPELL_AURA_HOVER
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_HOVER) && !GetPlayer()->HasAuraType(SPELL_AURA_HOVER),
        MOVEMENTFLAG_HOVER);

    //! Cannot ascend and descend at the same time
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_ASCENDING) && mi->HasMovementFlag(MOVEMENTFLAG_DESCENDING),
        MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);

    //! Cannot move left and right at the same time
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_LEFT) && mi->HasMovementFlag(MOVEMENTFLAG_RIGHT),
        MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT);

    //! Cannot strafe left and right at the same time
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_STRAFE_LEFT) && mi->HasMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT),
        MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT);

    //! Cannot pitch up and down at the same time
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_PITCH_UP) && mi->HasMovementFlag(MOVEMENTFLAG_PITCH_DOWN),
        MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);

    //! Cannot move forwards and backwards at the same time
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_FORWARD) && mi->HasMovementFlag(MOVEMENTFLAG_BACKWARD),
        MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD);

    //! Cannot walk on water without SPELL_AURA_WATER_WALK
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_WATERWALKING) && !GetPlayer()->HasAuraType(SPELL_AURA_WATER_WALK),
        MOVEMENTFLAG_WATERWALKING);

    //! Cannot feather fall without SPELL_AURA_FEATHER_FALL
    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_FALLING_SLOW) && !GetPlayer()->HasAuraType(SPELL_AURA_FEATHER_FALL),
        MOVEMENTFLAG_FALLING_SLOW);

    /*! Cannot fly if no fly auras present. Exception is being a GM.
        Note that we check for account level instead of Player::IsGameMaster() because in some
        situations it may be feasable to use .gm fly on as a GM without having .gm on,
        e.g. aerial combat.
    */

    REMOVE_VIOLATING_FLAGS(mi->HasMovementFlag(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_CAN_FLY) && GetSecurity() == SEC_PLAYER &&
        !GetPlayer()->m_mover->HasAuraType(SPELL_AURA_FLY) &&
        !GetPlayer()->m_mover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED),
        MOVEMENTFLAG_FLYING | MOVEMENTFLAG_CAN_FLY);

    #undef REMOVE_VIOLATING_FLAGS
}

void WorldSession::WriteMovementInfo(WorldPacket &data, MovementInfo* mi)
{
    MovementStatusElements* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (!sequence)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::WriteMovementInfo: No movement sequence found for opcode %s, 0x%04X", GetOpcodeNameForLogging(data.GetOpcode()).c_str(), uint32(data.GetOpcode()));
        return;
    }

    mi->Normalize();

    ObjectGuid guid = mi->guid;
    ObjectGuid tguid = mi->t_guid;

    bool hasOrientation = mi->HasOrientation();
    bool hasTransportData = mi->HasTransportData();
    bool hasPitch = mi->HasPitch();
    bool hasSplineElevation = mi->HasSplineElevation();

    for(uint32 i = 0; i < MSE_COUNT; ++i)
    {
        MovementStatusElements element = sequence[i];
        if (element == MSEEnd)
            break;

        if (element >= MSEHasGuidByte0 && element <= MSEHasGuidByte7)
        {
            data.WriteByteMask(guid[element - MSEHasGuidByte0]);
            continue;
        }

        if (element >= MSEHasTransportGuidByte0 &&
            element <= MSEHasTransportGuidByte7)
        {
            if (hasTransportData)
                data.WriteByteMask(tguid[element - MSEHasTransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.WriteByteSeq(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (hasTransportData)
                data.WriteByteSeq(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        switch (element)
        {
            case MSEHasMovementFlags:
                data.WriteBit(mi->GetMovementFlags() == 0);
                break;
            case MSEHasMovementFlags2:
                data.WriteBit(mi->GetExtraMovementFlags() == 0);
                break;
            case MSEHasTimestamp:
                data.WriteBit(mi->time == 0);
                break;
            case MSEHasOrientation:
                data.WriteBit(!hasOrientation);
                break;
            case MSEHasTransportData:
                data.WriteBit(hasTransportData);
                break;
            case MSEHasTransportTime2:
                if (hasTransportData)
                    data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2));
                break;
            case MSEHasTransportTime3:
                if (hasTransportData)
                    data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3));
                break;
            case MSEHasPitch:
                data.WriteBit(!hasPitch);
                break;
            case MSEHasFallData:
                data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA));
                break;
            case MSEHasFallDirection:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION));
                break;
            case MSEHasSplineElevation:
                data.WriteBit(!hasSplineElevation);
                break;
            case MSEHasSpline:
                data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_SPLINE1));
                break;
            case MSEHasSpline2:
                data.WriteBit(mi->HasServerMovementFlag(SERVERMOVEFLAG_SPLINE2));
                break;
            case MSEMovementFlags:
                if (mi->GetMovementFlags() != 0)
                    data.WriteBits(mi->flags, 30);
                break;
            case MSEMovementFlags2:
                if (mi->GetExtraMovementFlags() != 0)
                    data.WriteBits(mi->flags2, 12);
                break;
            case MSETimestamp:
                if (mi->time != 0)
                    data << mi->time;
                break;
            case MSEPositionX:
                data << mi->pos.m_positionX;
                break;
            case MSEPositionY:
                data << mi->pos.m_positionY;
                break;
            case MSEPositionZ:
                data << mi->pos.m_positionZ;
                break;
            case MSEOrientation:
                if (hasOrientation)
                    data << mi->pos.GetOrientation();
                break;
            case MSETransportPositionX:
                if (hasTransportData)
                    data << mi->t_pos.m_positionX;
                break;
            case MSETransportPositionY:
                if (hasTransportData)
                    data << mi->t_pos.m_positionY;
                break;
            case MSETransportPositionZ:
                if (hasTransportData)
                    data << mi->t_pos.m_positionZ;
                break;
            case MSETransportOrientation:
                if (hasTransportData)
                    data << mi->t_pos.GetOrientation();
                break;
            case MSETransportSeat:
                if (hasTransportData)
                    data << mi->t_seat;
                break;
            case MSETransportTime:
                if (hasTransportData)
                    data << mi->t_time;
                break;
            case MSETransportTime2:
                if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                    data << mi->t_time2;
                break;
            case MSETransportTime3:
                if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                    data << mi->t_time3;
                break;
            case MSEPitch:
                if (hasPitch)
                    data << mi->pitch;
                break;
            case MSEFallTime:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data << mi->fallTime;
                break;
            case MSEFallVerticalSpeed:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data << mi->j_zspeed;
                break;
            case MSEFallCosAngle:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mi->j_cosAngle;
                break;
            case MSEFallSinAngle:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mi->j_sinAngle;
                break;
            case MSEFallHorizontalSpeed:
                if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mi->j_xyspeed;
                break;
            case MSESplineElevation:
                if (hasSplineElevation)
                    data << mi->splineElevation;
                break;
            case MSEZeroBit:
            case MSEOneBit:
                data.WriteBit(element == MSEOneBit);
                break;
            case MSEGenericDword0:
            case MSEGenericDword1:
            case MSEMovementCounter:
                data << uint32(0); // counter
                break;
            case MSESpeed:
                data << mi->speed;
                break;
            case MSE2Bits:
                data.WriteBits(0, 2);
                break;
            default:
                ASSERT(false && "Incorrect sequence element detected at ReadMovementInfo");
                break;
        }
    }
}

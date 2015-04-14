/*
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

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket& /*recvPacket*/)
{
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
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[4])

        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[0]);

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
    GetPlayer()->ReadMovementInfo(recvPacket, &movementInfo);

    Unit* mover = _player->m_mover;

    ASSERT(mover != NULL);                      // there must always be a mover

    Player *plrMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;
    Vehicle *vehMover = mover->GetVehicleKit();
    if (vehMover)
        if (mover->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            if (Unit *charmer = mover->GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    plrMover = (Player*)charmer;
    
    bool jump = recvPacket.GetOpcode() == MSG_MOVE_JUMP && plrMover && !plrMover->isInFlight() && !plrMover->IsUnderWater();

    if (jump)
    {
        if (!movementInfo.Check(plrMover, recvPacket.GetOpcode()))
            return;

        plrMover->HandleJump(movementInfo);
    }

    bool fall = recvPacket.GetOpcode() == MSG_MOVE_FALL_LAND && plrMover && !plrMover->isInFlight() && !vehMover;

    if (fall)
    {
        if (!movementInfo.Check(plrMover, recvPacket.GetOpcode()))
            return;

        plrMover->HandleFall(movementInfo);
    }

    if (!mover->m_movementInfo.AcceptClientChanges(_player, movementInfo, recvPacket.GetOpcode()))
        return;

    WorldPacket data(SMSG_PLAYER_MOVE, recvPacket.size());
    _player->WriteMovementInfo(data);
    mover->SendMessageToSet(&data, _player);

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
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo);

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
    MovementInfo mi;
    GetPlayer()->ReadMovementInfo(recvData, &mi);

    if (!_player->m_movementInfo.AcceptClientChanges(_player, mi, CMSG_MOVE_NOT_ACTIVE_MOVER))
        return;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvData*/)
{
    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recvData)
{
    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo);

    if (!_player->m_movementInfo.AcceptClientChanges(_player, movementInfo, recvData.GetOpcode()))
        return;

    WorldPacket data;
    if (!_player->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA) ||
        !_player->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
        data.Initialize(SMSG_PLAYER_MOVE, recvData.size());
    else
        data.Initialize(SMSG_MOVE_UPDATE_KNOCK_BACK, recvData.size());

    _player->WriteMovementInfo(data);
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

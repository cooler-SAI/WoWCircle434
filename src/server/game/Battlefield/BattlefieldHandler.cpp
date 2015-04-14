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
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Object.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Opcodes.h"

void WorldSession::SendBfInvitePlayerToWar(uint64 guid, uint32 zoneId, uint32 pTime)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 12);

    data
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[7])
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[4])
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[0]);

    data.WriteByteSeq(guidBytes[6]);
    data << uint32(zoneId);

    data
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[0]);

    data << uint32(time(NULL) + pTime);
    data
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[5]);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "SendBfInvitePlayerToQueue: battlefieldGuid: " UI64FMTD " ZoneId: %u , zoneId: %u , pTime: %u ",  guidBytes, zoneId, pTime);

    SendPacket(&data);
}

void WorldSession::SendBfInvitePlayerToQueue(uint64 guid)
{
    ObjectGuid guidBytes = guid;
    bool warmup = true;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

    data
        .WriteBit(1)
        .WriteBit(!warmup)
        .WriteBit(1)
        .WriteByteMask(guidBytes[0])
        .WriteBit(1)
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[3])
        .WriteBit(1)
        .WriteBit(0)
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[4])
        .WriteBit(1)
        .WriteByteMask(guidBytes[7])

        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[6]);

    if (warmup)
        data << uint8(2);

    data
        .WriteByteSeq(guidBytes[5])
        .WriteByteSeq(guidBytes[0])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[7]);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "SendBfInvitePlayerToQueue: battlefieldGuid: " UI64FMTD " ZoneId: %u , warmup: %u ",  guidBytes, warmup);

    SendPacket(&data);
}

void WorldSession::SendBfQueueInviteResponse(uint64 guid, uint32 ZoneId, bool CanQueue, bool Full)
{
    const bool hasSecondGuid = false;
    const bool warmup = true;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 11);

    data
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[7])
        .WriteBit(Full ? 0 : 1)
        .WriteByteMask(guidBytes[0])
        .WriteBit(!hasSecondGuid)
        .WriteByteMask(guidBytes[4]);

    if (hasSecondGuid)
        data
        .WriteByteMask(guidBytes[7])
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[4])
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[5]);

    data
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[2]);

    if (hasSecondGuid)
        data
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[4])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[7]);

    data << uint8(CanQueue ? 1 : 0);

    data
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[6])
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[0]);

    data << uint8(warmup);

    data
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[5]);

    data << uint32(ZoneId);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "SendBfLeaveMessage: battlefieldGuid: " UI64FMTD " ZoneId: %u , CanQueue: %u , Full: %u , hasSecondGuid: %u , warmup: %u",  guidBytes, ZoneId, CanQueue, Full, hasSecondGuid, warmup);

    SendPacket(&data);
}

void WorldSession::SendBfEntered(uint64 guid)
{

    uint8 isAFK = _player->isAFK() ? 1 : 0;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 7);

    data
        .WriteBit(0)
        .WriteBit(isAFK)
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[4])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[3])
        .WriteBit(1)
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[7])
        .WriteByteMask(guidBytes[2])

        .WriteByteSeq(guidBytes[5])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[0])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[6]);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "SendBfEntered: battlefieldGuid: " UI64FMTD " isAFK: %u", guidBytes, isAFK);

    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(uint64 guid, BFLeaveReason reason)
{
    ObjectGuid guidBytes = guid;
    uint8 BattleStatus = 2;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 11);
    data
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[6])
        .WriteBit(0)    // Relocated
        .WriteByteMask(guidBytes[7])
        .WriteByteMask(guidBytes[4]);

    data << uint8(BattleStatus);
    data
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[3]);

    data << uint8(reason);  // Reason

    data
        .WriteByteSeq(guidBytes[6])
        .WriteByteSeq(guidBytes[0])
        .WriteByteSeq(guidBytes[5]);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "SendBfLeaveMessage: battlefieldGuid: " UI64FMTD " reason: %u", guidBytes, reason);

    SendPacket(&data);
}

void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    recvData
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[7]);

    accepted = recvData.ReadBit();

    recvData
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[6])

        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5]);

    //sLog->outError(LOG_FILTER_BATTLEFIELD, "HandleBfQueueInviteResponse: battlefieldGuid: " UI64FMTD " accepted: %u", guid, accepted);

    if (!accepted)
        return;

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
    return;
    //if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB))
        bf->PlayerAcceptInviteToQueue(_player);
}

void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    recvData
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[1]);

    accepted = recvData.ReadBit();

    recvData
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[4])

        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[5]);

    //sLog->outError(LOG_FILTER_GENERAL, "HandleBattlefieldInviteResponse: GUID:" UI64FMTD " Accepted:%u", uint64(guid), accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    //Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
    if (accepted)
    {
        bf->PlayerAcceptInviteToWar(_player);
    }
    else
    {
        if (_player->GetZoneId() == bf->GetZoneId())
            bf->KickPlayerFromBattlefield(_player->GetGUID());
    }
}

void WorldSession::HandleBfQueueRequest(WorldPacket& recvData)
{
    ObjectGuid guid;

    recvData
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[5])

        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[0]);

    //sLog->outError(LOG_FILTER_GENERAL, "HandleBfQueueRequest: GUID:" UI64FMTD "", uint64(guid));

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid))

    //if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB))
    {
        if (bf->IsWarTime())
        {
            bf->InvitePlayerToWar(_player);
            //sLog->outError(LOG_FILTER_GENERAL, "HandleBfQueueRequest: GUID:" UI64FMTD " IsWarTime:%u", uint64(guid), bf->IsWarTime());
        }
        else
        {
            uint32 timer = bf->GetTimer() / 1000;
            if (timer < 15 * MINUTE)
            {
                //sLog->outError(LOG_FILTER_GENERAL, "HandleBfQueueRequest: GUID:" UI64FMTD " timer:%u", uint64(guid), uint32(timer));
                bf->InvitePlayerToQueue(_player);
            }
        }
    }
}

void WorldSession::HandleBfExitRequest(WorldPacket& recvData)
{
    ObjectGuid guid;

    recvData
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[1])

        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[6]);

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid))
    //if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB))
    {
        bf->AskToLeaveQueue(_player);
        //sLog->outError(LOG_FILTER_GENERAL, "HandleBfExitRequest: GUID:" UI64FMTD " AskToLeaveQueue", uint64(guid));
    }
}

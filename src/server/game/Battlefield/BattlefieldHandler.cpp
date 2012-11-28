/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
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

#include "gamePCH.h"
#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Object.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Opcodes.h"

//This send to player windows for invite player to join the war
//Param1:(guid) the guid of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(time) Time in second that the player have for accept
void WorldSession::SendBfInvitePlayerToWar(uint64 guid, uint32 zoneId, uint32 pTime)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 16);

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
    data << uint32(zoneId);         // Zone Id

    data
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[0]);

    data << uint32(time(NULL) + pTime); // Invite lasts until
    data
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[5]);

    //Sending the packet to player
    SendPacket(&data);
}

//This send invitation to player to join the queue
void WorldSession::SendBfInvitePlayerToQueue(uint64 guid)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

    data
        .WriteBit(true)               // unk
        .WriteBit(false)               // Has Warmup
        .WriteBit(true)               // unk
        .WriteByteMask(guidBytes[0])
        .WriteBit(true)               // unk
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[3])
        .WriteBit(true)               // unk
        .WriteBit(false)               // unk
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[4])
        .WriteBit(true)               // unk
        .WriteByteMask(guidBytes[7])

        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[3])
        .WriteByteSeq(guidBytes[6]);

    data << uint8(1);               // Warmup

    data
        .WriteByteSeq(guidBytes[5])
        .WriteByteSeq(guidBytes[0])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[7]);

    //Sending packet to player
    SendPacket(&data);
}

//This send packet for inform player that he join queue
//Param1:(guid) the guid of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(CanQueue) if able to queue
//Param4:(Full) on log in is full
void WorldSession::SendBfQueueInviteResponse(uint64 guid, uint32 ZoneId, bool CanQueue, bool Full)
{
    const bool hasSecondGuid = false;
    const bool warmup = true;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 16);

    data
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[6])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[7])
        .WriteBit(Full)  // Logging In, VERIFYME
        .WriteByteMask(guidBytes[0])
        .WriteBit(!hasSecondGuid)
        .WriteByteMask(guidBytes[4])

        // if (hasSecondGuid) 7 3 0 4 2 6 1 5

        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[2]);

    // if (hasSecondGuid) 2 5 3 0 4 6 1 7

    data << uint8(CanQueue);  // Accepted

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

    SendPacket(&data);
}

//This is call when player accept to join war
void WorldSession::SendBfEntered(uint64 guid)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 11);

    ObjectGuid guidBytes = guid;

    data
        .WriteBit(false)              // unk
        .WriteBit(_player->isAFK())
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[4])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[3])
        .WriteBit(false)              // unk
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

    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(uint64 guid, BFLeaveReason reason)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 11);

    ObjectGuid guidBytes = guid;

    data
        .WriteByteMask(guidBytes[2])
        .WriteByteMask(guidBytes[5])
        .WriteByteMask(guidBytes[1])
        .WriteByteMask(guidBytes[0])
        .WriteByteMask(guidBytes[3])
        .WriteByteMask(guidBytes[6])
        .WriteBit(false)               // Relocated
        .WriteByteMask(guidBytes[7])
        .WriteByteMask(guidBytes[4]);

    data << uint8(2);               // BattleStatus
    data
        .WriteByteSeq(guidBytes[1])
        .WriteByteSeq(guidBytes[7])
        .WriteByteSeq(guidBytes[4])
        .WriteByteSeq(guidBytes[2])
        .WriteByteSeq(guidBytes[3]);

    data << uint8(reason);          // Reason

    data
        .WriteByteSeq(guidBytes[6])
        .WriteByteSeq(guidBytes[0])
        .WriteByteSeq(guidBytes[5]);

    SendPacket(&data);
}

//Send by client when he click on accept for queue
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

    sLog->outError(LOG_FILTER_GENERAL, "HandleQueueInviteResponse: GUID:"UI64FMTD" Accepted:%u", (uint64)guid, accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (accepted)
        bf->PlayerAcceptInviteToQueue(_player);
}

//Send by client on clicking in accept or refuse of invitation windows for join game
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

    sLog->outError(LOG_FILTER_GENERAL, "HandleBattlefieldInviteResponse: GUID:"UI64FMTD" Accepted:%u", (uint64)guid, accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (accepted)
        bf->PlayerAcceptInviteToWar(_player);
    else
        if (_player->GetZoneId() == bf->GetZoneId())
            bf->KickPlayerFromBattlefield(_player->GetGUID());
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

    sLog->outError(LOG_FILTER_GENERAL, "HandleBfExitRequest: GUID:"UI64FMTD" ", (uint64)guid);

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid))
        bf->AskToLeaveQueue(_player);
}

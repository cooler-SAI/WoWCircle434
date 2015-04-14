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
#include "DatabaseEnv.h"
#include "Opcodes.h"
#include "Log.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GroupMgr.h"
#include "Player.h"
#include "Group.h"
#include "SocialMgr.h"
#include "Util.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "DB2Structure.h"
#include "DB2Stores.h"
#include "SpellAuraEffects.h"

class Aura;

/* differeces from off:
    -you can uninvite yourself - is is useful
    -you can accept invitation even if leader went offline
*/
/* todo:
    -group_destroyed msg is sent but not shown
    -reduce xp gaining when in raid group
    -quest sharing has to be corrected
    -FIX sending PartyMemberStats
*/

void WorldSession::SendPartyResult(PartyOperation operation, const std::string& member, PartyResult res, uint32 val /* = 0 */)
{
    WorldPacket data(SMSG_PARTY_COMMAND_RESULT, 4 + member.size() + 1 + 4 + 4 + 8);
    data << uint32(operation);
    data << member;
    data << uint32(res);
    data << uint32(val);                                    // LFD cooldown related (used with ERR_PARTY_LFG_BOOT_COOLDOWN_S and ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S)
    data << uint64(0); // player who caused error (in some cases).

    SendPacket(&data);
}

void WorldSession::HandleGroupInviteOpcode(WorldPacket & recvData)
{
    ObjectGuid crossRealmGuid; // unused

    recvData.read_skip<uint32>(); // Non-zero in cross realm invites
    recvData.read_skip<uint32>(); // Always 0

    recvData
        .ReadByteMask(crossRealmGuid[2])
        .ReadByteMask(crossRealmGuid[7]);

    uint8 realmLen = recvData.ReadBits(9);

    recvData.ReadByteMask(crossRealmGuid[3]);

    uint32 nameLen = recvData.ReadBits(10);

    recvData
        .ReadByteMask(crossRealmGuid[5])
        .ReadByteMask(crossRealmGuid[4])
        .ReadByteMask(crossRealmGuid[6])
        .ReadByteMask(crossRealmGuid[0])
        .ReadByteMask(crossRealmGuid[1])

        .ReadByteSeq(crossRealmGuid[4])
        .ReadByteSeq(crossRealmGuid[7])
        .ReadByteSeq(crossRealmGuid[6]);

    std::string memberName, realmName;
    recvData.read(memberName, nameLen);
    recvData.read(realmName, realmLen); // unused

    recvData
        .ReadByteSeq(crossRealmGuid[1])
        .ReadByteSeq(crossRealmGuid[0])
        .ReadByteSeq(crossRealmGuid[5])
        .ReadByteSeq(crossRealmGuid[3])
        .ReadByteSeq(crossRealmGuid[2]);

    // attempt add selected player

    // cheating
    if (!normalizePlayerName(memberName))
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    Player* player = sObjectAccessor->FindPlayerByName(memberName);

    // no player
    if (!player)
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // restrict invite to GMs
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_GM_GROUP) && !GetPlayer()->isGameMaster() && player->isGameMaster())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // can't group with
    if (!GetPlayer()->isGameMaster() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && GetPlayer()->GetTeam() != player->GetTeam())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_PLAYER_WRONG_FACTION);
        return;
    }
    if (GetPlayer()->GetInstanceId() != 0 && player->GetInstanceId() != 0 && GetPlayer()->GetInstanceId() != player->GetInstanceId() && GetPlayer()->GetMapId() == player->GetMapId())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_TARGET_NOT_IN_INSTANCE_S);
        return;
    }
    // just ignore us
    if (player->GetInstanceId() != 0 && player->GetDungeonDifficulty() != GetPlayer()->GetDungeonDifficulty())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_IGNORING_YOU_S);
        return;
    }

    if (player->GetSocial()->HasIgnore(GetPlayer()->GetGUIDLow()))
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_IGNORING_YOU_S);
        return;
    }

    ObjectGuid invitedGuid = player->GetGUID();

    Group* group = GetPlayer()->GetGroup();
    if (group && group->isBGGroup())
        group = GetPlayer()->GetOriginalGroup();

    Group* group2 = player->GetGroup();
    if (group2 && group2->isBGGroup())
        group2 = player->GetOriginalGroup();
    // player already in another group or invited
    if (group2 || player->GetGroupInvite())
    {
        SendPartyResult(PARTY_OP_INVITE, memberName, ERR_ALREADY_IN_GROUP_S);

        if (group2)
        {
            // tell the player that they were invited but it failed as they were already in a group
            WorldPacket data(SMSG_GROUP_INVITE, 45);

            data.WriteBit(false);

            data.WriteByteMask(invitedGuid[0]);
            data.WriteByteMask(invitedGuid[3]);
            data.WriteByteMask(invitedGuid[2]);

            data.WriteBit(false); // Inverse already in group

            data.WriteByteMask(invitedGuid[6]);
            data.WriteByteMask(invitedGuid[5]);

            data.WriteBits(0, 9); // Realm name

            data.WriteByteMask(invitedGuid[4]);

            data.WriteBits(strlen(GetPlayer()->GetName()), 7); // Inviter name length

            data.WriteBits(0, 24); // Count 2

            data.WriteBit(false);

            data.WriteByteMask(invitedGuid[1]);
            data.WriteByteMask(invitedGuid[7]);

            data.WriteByteSeq(invitedGuid[1]);
            data.WriteByteSeq(invitedGuid[4]);

            data << int32(getMSTime());
            data << int32(0);
            data << int32(0);

            data.WriteByteSeq(invitedGuid[6]);
            data.WriteByteSeq(invitedGuid[0]);
            data.WriteByteSeq(invitedGuid[2]);
            data.WriteByteSeq(invitedGuid[3]);

            // for count2 { int32(0) }

            data.WriteByteSeq(invitedGuid[5]);

            // data.append(realm name);

            data.WriteByteSeq(invitedGuid[7]);

            data << WriteBuffer(GetPlayer()->GetName(), strlen(GetPlayer()->GetName()));

            data << int32(0);

            player->GetSession()->SendPacket(&data);
        }

        return;
    }

    if (group && group->isRaidGroup() && !player->GetAllowLowLevelRaid() && (player->getLevel() < sWorld->getIntConfig(CONFIG_MIN_LVL_FOR_RAID)))
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_RAID_DISALLOWED_BY_LEVEL);
        return;
    }

    if (group)
    {
        // not have permissions for invite
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_NOT_LEADER);
            return;
        }
        // not have place
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }
    }

    // ok, but group not exist, start a new group
    // but don't create and save the group to the DB until
    // at least one person joins
    if (!group)
    {
        group = new Group;
        // new group: if can't add then delete
        if (!group->AddLeaderInvite(GetPlayer()))
        {
            delete group;
            return;
        }
        if (!group->AddInvite(player))
        {
            delete group;
            return;
        }
    }
    else
    {
        // already existed group: if can't add then just leave
        if (!group->AddInvite(player))
        {
            return;
        }
    }

    // ok, we do it
    WorldPacket data(SMSG_GROUP_INVITE, 45);

    data.WriteBit(false);

    data.WriteByteMask(invitedGuid[0]);
    data.WriteByteMask(invitedGuid[3]);
    data.WriteByteMask(invitedGuid[2]);

    data.WriteBit(true); // Inverse already in group

    data.WriteByteMask(invitedGuid[6]);
    data.WriteByteMask(invitedGuid[5]);

    data.WriteBits(0, 9); // Realm name

    data.WriteByteMask(invitedGuid[4]);

    data.WriteBits(strlen(GetPlayer()->GetName()), 7); // Inviter name length

    data.WriteBits(0, 24); // Count 2

    data.WriteBit(false);

    data.WriteByteMask(invitedGuid[1]);
    data.WriteByteMask(invitedGuid[7]);

    data.WriteByteSeq(invitedGuid[1]);
    data.WriteByteSeq(invitedGuid[4]);

    data << int32(getMSTime());
    data << int32(0);
    data << int32(0);

    data.WriteByteSeq(invitedGuid[6]);
    data.WriteByteSeq(invitedGuid[0]);
    data.WriteByteSeq(invitedGuid[2]);
    data.WriteByteSeq(invitedGuid[3]);

    // for count2 { int32(0) }

    data.WriteByteSeq(invitedGuid[5]);

    // data.append(realm name);

    data.WriteByteSeq(invitedGuid[7]);

    data << WriteBuffer(GetPlayer()->GetName(), strlen(GetPlayer()->GetName()));

    data << int32(0);

    player->GetSession()->SendPacket(&data);

    SendPartyResult(PARTY_OP_INVITE, memberName, ERR_PARTY_RESULT_OK);
}

void WorldSession::HandleGroupInviteResponseOpcode(WorldPacket& recvData)
{
    recvData.ReadBit(); // unk always 0
    bool accept = recvData.ReadBit();

    Group* group = GetPlayer()->GetGroupInvite();

    if (!group)
        return;

    if (accept)
    {
        // Remove player from invitees in any case
        group->RemoveInvite(GetPlayer());

        if (group->GetLeaderGUID() == GetPlayer()->GetGUID())
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "HandleGroupAcceptOpcode: player %s(%d) tried to accept an invite to his own group", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());
            return;
        }

        // Group is full
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }

        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // Forming a new group, create it
        if (!group->IsCreated())
        {
            // This can happen if the leader is zoning. To be removed once delayed actions for zoning are implemented
            if (!leader)
            {
                group->RemoveAllInvites();
                return;
            }

            // If we're about to create a group there really should be a leader present
            ASSERT(leader);
            group->RemoveInvite(leader);
            group->Create(leader);
            sGroupMgr->AddGroup(group);
        }

        // Everything is fine, do it, PLAYER'S GROUP IS SET IN ADDMEMBER!!!
        if (!group->AddMember(GetPlayer()))
            return;

        group->BroadcastGroupUpdate();
    }
    else
    {
        // Remember leader if online (group pointer will be invalid if group gets disbanded)
        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // uninvite, group can be deleted
        GetPlayer()->UninviteFromGroup();

        if (!leader || !leader->GetSession())
            return;

        // report
        WorldPacket data(SMSG_GROUP_DECLINE, strlen(GetPlayer()->GetName()));
        data << GetPlayer()->GetName();
        leader->GetSession()->SendPacket(&data);
    }
}

void WorldSession::HandleGroupClearMarker(WorldPacket& recv_data)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_CLEAR_RAID_MARKER");

    uint8 marker;
    recv_data >> marker;

    if (Group* group = GetPlayer()->GetGroup())
    {
        group->RemoveGroupMarkerMask(1 << marker);
        group->SendRaidMarkerUpdate();
        group->RemoveMarker();
    }
}

void WorldSession::HandleGroupUninviteGuidOpcode(WorldPacket& recvData)
{
    uint64 guid;
    std::string reason;
    recvData >> guid;
    recvData >> reason;

    //can't uninvite yourself
    if (guid == GetPlayer()->GetGUID())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleGroupUninviteGuidOpcode: leader %s(%d) tried to uninvite himself from the group.", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());
        return;
    }

    PartyResult res = GetPlayer()->CanUninviteFromGroup();
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res);
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (grp->IsLeader(guid))
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", ERR_NOT_LEADER);
        return;
    }

    if (grp->IsMember(guid))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID(), reason.c_str());
        return;
    }

    if (Player* player = grp->GetInvited(guid))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, "", ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupUninviteOpcode(WorldPacket & recvData)
{
    std::string membername;
    recvData >> membername;

    // player not found
    if (!normalizePlayerName(membername))
        return;

    // can't uninvite yourself
    if (GetPlayer()->GetName() == membername)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleGroupUninviteOpcode: leader %s(%d) tried to uninvite himself from the group.", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());
        return;
    }

    PartyResult res = GetPlayer()->CanUninviteFromGroup();
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res);
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (uint64 guid = grp->GetMemberGUID(membername))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID());
        return;
    }

    if (Player* player = grp->GetInvited(membername))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, membername, ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupSetLeaderOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData >> guid;

    Player* player = ObjectAccessor::FindPlayer(guid);
    Group* group = GetPlayer()->GetGroup();

    if (!group || !player)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) || player->GetGroup() != group)
        return;

    // Prevent exploits with instance saves
    for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        if (Player* plr = itr->getSource())
            if (plr->GetMap() && plr->GetMap()->Instanceable())
                return;
        
    // Everything's fine, accepted.
    group->ChangeLeader(guid);
    group->SendUpdate();
}

void WorldSession::HandleGroupSetRolesOpcode(WorldPacket& recvData)
{
    uint32 newRole;
    ObjectGuid guid1;                   // Assigner GUID
    ObjectGuid guid2;                   // Target GUID

    guid1 = GetPlayer()->GetGUID();

    recvData >> newRole;

    recvData
        .ReadByteMask(guid2[2])
        .ReadByteMask(guid2[6])
        .ReadByteMask(guid2[3])
        .ReadByteMask(guid2[7])
        .ReadByteMask(guid2[5])
        .ReadByteMask(guid2[1])
        .ReadByteMask(guid2[0])
        .ReadByteMask(guid2[4])

        .ReadByteSeq(guid2[6])
        .ReadByteSeq(guid2[4])
        .ReadByteSeq(guid2[1])
        .ReadByteSeq(guid2[3])
        .ReadByteSeq(guid2[0])
        .ReadByteSeq(guid2[5])
        .ReadByteSeq(guid2[2])
        .ReadByteSeq(guid2[7]);

    WorldPacket data(SMSG_GROUP_SET_ROLE, 26);

    data.WriteByteMask(guid1[1]);
    data.WriteByteMask(guid2[0]);
    data.WriteByteMask(guid2[2]);
    data.WriteByteMask(guid2[4]);
    data.WriteByteMask(guid2[7]);
    data.WriteByteMask(guid2[3]);
    data.WriteByteMask(guid1[7]);
    data.WriteByteMask(guid2[5]);
    data.WriteByteMask(guid1[5]);
    data.WriteByteMask(guid1[4]);
    data.WriteByteMask(guid1[3]);
    data.WriteByteMask(guid2[6]);
    data.WriteByteMask(guid1[2]);
    data.WriteByteMask(guid1[6]);
    data.WriteByteMask(guid2[1]);
    data.WriteByteMask(guid1[0]);

    data.WriteByteSeq(guid1[7]);
    data.WriteByteSeq(guid2[3]);
    data.WriteByteSeq(guid1[6]);
    data.WriteByteSeq(guid2[4]);
    data.WriteByteSeq(guid2[0]);
    data << uint32(newRole);            // New Role
    data.WriteByteSeq(guid2[6]);
    data.WriteByteSeq(guid2[2]);
    data.WriteByteSeq(guid1[0]);
    data.WriteByteSeq(guid1[4]);
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid1[3]);
    data.WriteByteSeq(guid1[5]);
    data.WriteByteSeq(guid1[2]);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid2[7]);
    data.WriteByteSeq(guid1[1]);
    data << uint32(0);                  // Old Role

    if (GetPlayer()->GetGroup())
        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    else
        SendPacket(&data);
}

void WorldSession::HandleGroupDisbandOpcode(WorldPacket& /*recvData*/)
{
    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (_player->InBattleground())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
        return;
    }

    /** error handling **/
    /********************/

    // everything's fine, do it
    SendPartyResult(PARTY_OP_LEAVE, GetPlayer()->GetName(), ERR_PARTY_RESULT_OK);

    GetPlayer()->RemoveFromGroup(GROUP_REMOVEMETHOD_LEAVE);
}

void WorldSession::HandleLootMethodOpcode(WorldPacket & recvData)
{
    uint32 lootMethod;
    uint64 lootMaster;
    uint32 lootThreshold;
    recvData >> lootMethod >> lootMaster >> lootThreshold;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    /** error handling **/
    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;
    /********************/

    // everything's fine, do it
    group->SetLootMethod((LootMethod)lootMethod);
    group->SetLooterGuid(lootMaster);
    group->SetLootThreshold((ItemQualities)lootThreshold);
    group->SendUpdate();
}

void WorldSession::HandleLootRoll(WorldPacket& recvData)
{
    uint64 guid;
    uint32 itemSlot;
    uint8  rollType;
    recvData >> guid;                  // guid of the item rolled
    recvData >> itemSlot;
    recvData >> rollType;              // 0: pass, 1: need, 2: greed

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    group->CountRollVote(GetPlayer()->GetGUID(), guid, rollType);

    switch (rollType)
    {
        case ROLL_NEED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
            break;
        case ROLL_GREED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
    }
}

void WorldSession::HandleMinimapPingOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->GetGroup())
        return;

    float x, y;
    recvData >> x;
    recvData >> y;

    /** error handling **/
    /********************/

    // everything's fine, do it
    WorldPacket data(MSG_MINIMAP_PING, (8+4+4));
    data << uint64(GetPlayer()->GetGUID());
    data << float(x);
    data << float(y);
    GetPlayer()->GetGroup()->BroadcastPacket(&data, true, -1, GetPlayer()->GetGUID());
}

void WorldSession::HandleRandomRollOpcode(WorldPacket& recvData)
{
    uint32 minimum, maximum, roll;
    recvData >> minimum;
    recvData >> maximum;

    /** error handling **/
    if (minimum > maximum || maximum > 10000)                // < 32768 for urand call
        return;
    /********************/

    // everything's fine, do it
    roll = urand(minimum, maximum);

    WorldPacket data(MSG_RANDOM_ROLL, 4+4+4+8);
    data << uint32(minimum);
    data << uint32(maximum);
    data << uint32(roll);
    data << uint64(GetPlayer()->GetGUID());
    if (GetPlayer()->GetGroup())
        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    else
        SendPacket(&data);
}

void WorldSession::HandleRaidTargetUpdateOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint8 x;
    recvData >> x;

    /** error handling **/
    /********************/

    // everything's fine, do it
    if (x == 0xFF)                                           // target icon request
        group->SendTargetIconList(this);
    else                                                    // target icon update
    {
        if (group->isRaidGroup() && !group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;

        uint64 guid;
        recvData >> guid;
        group->SetTargetIcon(x, _player->GetGUID(), guid);
    }
}

void WorldSession::HandleGroupRaidConvertOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (_player->InBattleground())
        return;

    // error handling
    if (!group->IsLeader(GetPlayer()->GetGUID()) || group->GetMembersCount() < 2)
        return;

    // everything's fine, do it (is it 0 (PARTY_OP_INVITE) correct code)
    SendPartyResult(PARTY_OP_INVITE, "", ERR_PARTY_RESULT_OK);

    // New 4.x: it is now possible to convert a raid to a group if member count is 5 or less

    bool toRaid;
    recvData >> toRaid;

    if (toRaid)
        group->ConvertToRaid();
    else
        group->ConvertToGroup();
}

void WorldSession::HandleGroupChangeSubGroupOpcode(WorldPacket& recvData)
{
    // we will get correct pointer for group here, so we don't have to check if group is BG raid
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    time_t now = time(NULL);
    if (now - timeLastChangeSubGroupCommand < 2)
        return;
    else
       timeLastChangeSubGroupCommand = now;

    std::string name;
    uint8 groupNr;
    recvData >> name;
    recvData >> groupNr;

    if (groupNr >= MAX_RAID_SUBGROUPS)
        return;

    uint64 senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    if (!group->HasFreeSlotSubGroup(groupNr))
        return;

    Player* movedPlayer = sObjectAccessor->FindPlayerByName(name);
    uint64 guid;

    if (movedPlayer)
        guid = movedPlayer->GetGUID();
    else
    {
        CharacterDatabase.EscapeString(name);
        guid = sObjectMgr->GetPlayerGUIDByName(name.c_str());
    }

    group->ChangeMembersGroup(guid, groupNr);
}

void WorldSession::HandleGroupSwapSubGroupOpcode(WorldPacket& recvData)
{
    std::string unk1;
    std::string unk2;

    recvData >> unk1;
    recvData >> unk2;
}

void WorldSession::HandleGroupAssistantLeaderOpcode(WorldPacket & recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    uint64 guid;
    bool apply;
    recvData >> guid;
    recvData >> apply;

    group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_ASSISTANT);

    group->SendUpdate();
}

void WorldSession::HandlePartyAssignmentOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint64 senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    uint8 assignment;
    bool apply;
    uint64 guid;
    recvData >> assignment >> apply;
    recvData >> guid;

    switch (assignment)
    {
        case GROUP_ASSIGN_MAINASSIST:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINASSIST);
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINASSIST);
            break;
        case GROUP_ASSIGN_MAINTANK:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINTANK);           // Remove main assist flag from current if any.
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINTANK);
        default:
            break;
    }

    group->SendUpdate();
}

void WorldSession::HandleRaidReadyCheckOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (recvData.empty())                                   // request
    {
        /** error handling **/
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;
        /********************/

        // everything's fine, do it
        WorldPacket data(MSG_RAID_READY_CHECK, 8);
        data << GetPlayer()->GetGUID();
        group->BroadcastPacket(&data, false, -1);

        group->OfflineReadyCheck();
    }
    else                                                    // answer
    {
        uint8 state;
        recvData >> state;

        // everything's fine, do it
        WorldPacket data(MSG_RAID_READY_CHECK_CONFIRM, 9);
        data << uint64(GetPlayer()->GetGUID());
        data << uint8(state);
        group->BroadcastReadyCheck(&data);
    }
}

void WorldSession::HandleRaidReadyCheckFinishedOpcode(WorldPacket& /*recvData*/)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        return;

    WorldPacket data(MSG_RAID_READY_CHECK_FINISHED, 0);
    group->BroadcastPacket(&data, false, -1); 
}

void _WriteUnitAuras(Unit* unit, WorldPacket* data, uint64 mask)
{
    *data << uint8(0);                                      // unk
    *data << uint64(mask);
    *data << uint32(MAX_AURAS);

    ASSERT(MAX_AURAS <= sizeof(mask)*8 && "Bad MAX_AURAS");

    Unit::VisibleAuraMap const* auras = unit->GetVisibleAuras();
    for (uint32 i = 0; i < MAX_AURAS; ++i)
    {
        if (!(mask & (UI64LIT(1) << i)))
            continue;

        Unit::VisibleAuraMap::const_iterator itr = auras->find(i);
        if (itr != auras->end())
        {
            AuraApplication* aura = itr->second;
            uint16 flags = uint16(aura->GetFlags());

            *data << uint32(aura->GetBase()->GetId());
            *data << uint16(flags);

            if (flags & AFLAG_ANY_EFFECT_AMOUNT_SENT)
            {
                for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                {
                    if (AuraEffect const* effect = aura->GetBase()->GetEffect(j))
                        *data << int32(effect->GetAmount());
                    else
                        *data << int32(0);
                }
            }
        }
        else
        {
            *data << uint32(0);
            *data << uint16(0);
        }
    }

}

void WorldSession::BuildPartyMemberStatsChangedPacket(Player* player, WorldPacket* data, GroupUpdateFlags mask, uint64 guid, bool full /* = false */)
{
    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)                // if update power type, update current/max power also
        mask |= (GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER);

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)            // same for pets
        mask |= (GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER);

    Pet* pet = NULL;
    if (!player)
        mask &= ~GROUP_UPDATE_FULL;
    else if (!(pet = player->GetPet()))
        mask &= ~GROUP_UPDATE_PET ;

    mask |= GROUP_UPDATE_FLAG_STATUS;

    uint32 byteCount = 0;
    for (int i = 1; i < GROUP_UPDATE_FLAGS_COUNT; ++i)
        if (mask & (1 << i))
            byteCount += GroupUpdateLength[i];

    data->Initialize(SMSG_PARTY_MEMBER_STATS, 8 + 4 + byteCount);
    if (player)
        data->append(player->GetPackGUID());
    else
        *data << uint8(0);

    *data << uint32(mask);

    if (mask & GROUP_UPDATE_FLAG_STATUS)
    {
        GroupMemberOnlineStatus status = MEMBER_STATUS_OFFLINE;
        if (player)
        {
            status |= MEMBER_STATUS_ONLINE;

            if (player->IsPvP())
                status |= MEMBER_STATUS_PVP;

            if (player->isDead())
                status |= MEMBER_STATUS_DEAD;

            if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                status |= MEMBER_STATUS_GHOST;

            if (player->isAFK())
                status |= MEMBER_STATUS_AFK;

            if (player->isDND())
                status |= MEMBER_STATUS_DND;
        }
        *data << uint16(status);
    }

    if (mask & GROUP_UPDATE_FLAG_CUR_HP)
        *data << uint32(player->GetHealth());

    if (mask & GROUP_UPDATE_FLAG_MAX_HP)
        *data << uint32(player->GetMaxHealth());

    Powers powerType = POWER_MANA;
    if (player)
        player->getPowerType();

    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)
        *data << uint8(powerType);

    if (mask & GROUP_UPDATE_FLAG_CUR_POWER)
        *data << uint16(player->GetPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_MAX_POWER)
        *data << uint16(player->GetMaxPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_LEVEL)
        *data << uint16(player->getLevel());

    if (mask & GROUP_UPDATE_FLAG_ZONE)
        *data << uint16(player->GetZoneId());

    if (mask & GROUP_UPDATE_FLAG_UNK)
        *data << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_POSITION)
        *data << uint16(player->GetPositionX()) << uint16(player->GetPositionY()) << uint16(player->GetPositionZ());

    if (mask & GROUP_UPDATE_FLAG_AURAS)
        _WriteUnitAuras(player, data, player->GetAuraUpdateMaskForRaid());

    if (mask & GROUP_UPDATE_FLAG_PET_GUID)
        *data << uint64(pet->GetGUID());

    if (mask & GROUP_UPDATE_FLAG_PET_NAME)
        *data << pet->GetName();

    if (mask & GROUP_UPDATE_FLAG_PET_MODEL_ID)
        *data << uint16(pet->GetDisplayId());

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_HP)
        *data << uint32(pet->GetHealth());

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_HP)
        *data << uint32(pet->GetMaxHealth());

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
        *data << uint8(pet->getPowerType());

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_POWER)
        *data << uint16(pet->GetPower(pet->getPowerType()));

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_POWER)
        *data << uint16(pet->GetMaxPower(pet->getPowerType()));

    if (mask & GROUP_UPDATE_FLAG_PET_AURAS)
        _WriteUnitAuras(pet, data, pet->GetAuraUpdateMaskForRaid());

    if (mask & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
    {
        if (Vehicle* veh = player->GetVehicle())
            *data << uint32(veh->GetVehicleInfo()->m_seatID[player->m_movementInfo.t_seat]);
        else
            *data << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PHASE)
    {
        *data << uint32(8); // either 0 or 8, same unk found in SMSG_PHASESHIFT
        *data << uint32(0); // count
        // for (count) *data << uint16(phaseId)
    }
}

/*this procedure handles clients CMSG_REQUEST_PARTY_MEMBER_STATS request*/
void WorldSession::HandleRequestPartyMemberStatsOpcode(WorldPacket& recvData)
{
    uint64 Guid;
    recvData >> Guid;

    if (!Guid)
        return;

    Player* player = HashMapHolder<Player>::Find(Guid);
    if (player && player->GetGroup() != GetPlayer()->GetGroup())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Player %u (%s) sent CMSG_REQUEST_PARTY_MEMBER_STATS for player %u (%s) whos is not in the same group!",
            GetPlayer()->GetGUIDLow(), GetPlayer()->GetName(), player->GetGUIDLow(), player->GetName());
        return;
    }

    GroupUpdateFlags mask = GROUP_UPDATE_FLAG_STATUS;
    if (player)
    {
        mask |= GROUP_UPDATE_PLAYER;

        if (player->GetPet())
            mask |= GROUP_UPDATE_PET;
    }

    WorldPacket data;
    BuildPartyMemberStatsChangedPacket(player, &data, mask, Guid, true);

    SendPacket(&data);
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket& /*recvData*/)
{
    // every time the player checks the character screen
    _player->SendRaidInfo();
}

void WorldSession::HandleOptOutOfLootOpcode(WorldPacket& recvData)
{
    bool passOnLoot;
    recvData >> passOnLoot; // 1 always pass, 0 do not pass

    // ignore if player not loaded
    if (!GetPlayer())                                        // needed because STATUS_AUTHED
    {
        if (passOnLoot)
            sLog->outError(LOG_FILTER_NETWORKIO, "CMSG_OPT_OUT_OF_LOOT value<>0 for not-loaded character!");
        return;
    }

    GetPlayer()->SetPassOnGroupLoot(passOnLoot);
}

void WorldSession::HandleRequestJoinUpdates(WorldPacket & recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GROUP_REQUEST_JOIN_UPDATES ");

    Group* grp = GetPlayer()->GetGroup();

    if(grp)
    {
        WorldPacket data(SMSG_REAL_GROUP_UPDATE, 13);
        data << uint8(grp->GetGroupType());
        data << uint32(grp->GetMembersCount());

        if(grp->GetMembersCount() > 2) //group is already formed, send the latest member guid
        {
            Group::MemberSlotList::const_iterator lastMember = --grp->GetMemberSlots().end();

            data << uint64(lastMember->guid);
        }
        else // group has just formed, send the other player guid
        {
            Group::MemberSlotList::const_iterator member = grp->GetMemberSlots().begin();

            if(member->guid == GetPlayer()->GetGUID())
                ++member;

            data << uint64(member->guid);
        }
        SendPacket(&data);
    }
}

void WorldSession::HandleSetAllowLowLevelRaidOpcode(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received opcode CMSG_LOW_LEVEL_RAID, CMSG_LOW_LEVEL_RAID_USER: %4X", recvData.GetOpcode());

    uint8 allow;
    recvData >> allow;

    GetPlayer()->SetAllowLowLevelRaid(allow);
}

void WorldSession::HandleSetEveryoneIsAssistant(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_SET_EVERYONE_IS_ASSISTANT");

    Group* group = GetPlayer()->GetGroup();

    if (!group || !group->isRaidGroup())    // Only raid groups may have assistant
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    bool active = recvData.ReadBit();   // 0 == inactive, 1 == active

    if (active)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next()) // Loop through all members
            if (Player* player = itr->getSource())
                group->SetGroupMemberFlag(player->GetGUID(), active, MEMBER_FLAG_ASSISTANT);

        group->SendUpdate();
    }
    else
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next()) // Loop through all members
            if (Player* player = itr->getSource())
                group->SetGroupMemberFlag(player->GetGUID(), !active, MEMBER_FLAG_ASSISTANT);

        group->SendUpdate();
    }
}

void WorldSession::SendGroupCancel(std::string unkString)
{
    WorldPacket data(SMSG_GROUP_CANCEL, unkString.length());

    data << unkString;

    SendPacket(&data);
}
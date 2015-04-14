/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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


#include "WorldSession.h"
#include "WorldPacket.h"
#include "Object.h"
#include "SharedDefines.h"
#include "GuildFinderMgr.h"
#include "GuildMgr.h"

void WorldSession::HandleGuildFinderAddRecruit(WorldPacket& recvPacket)
{
    if (sGuildFinderMgr->GetAllMembershipRequestsForPlayer(GetPlayer()->GetGUIDLow()).size() == 10)
        return;

    uint32 classRoles = 0;
    uint32 availability = 0;
    uint32 guildInterests = 0;

    std::string comment;
    std::string playerName;

    recvPacket >> classRoles >> guildInterests >> availability;

    ObjectGuid guid;
    recvPacket
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[1]);

    uint16 commentLength = recvPacket.ReadBits(11);

    recvPacket
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[7]);

    uint8 nameLength = recvPacket.ReadBits(7);

    recvPacket
        .ReadByteMask(guid[2])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[5])
        .read(comment, commentLength)
        .read(playerName, nameLength)
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[3]);

    uint32 guildLowGuid = GUID_LOPART(uint64(guid));

    if (!IS_GUILD_GUID(guid))
        return;
    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & ALL_WEEK) || availability > ALL_WEEK)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;
    if (sObjectMgr->IsPlayerDeleted(guildLowGuid))
        return;

    MembershipRequest request = MembershipRequest(GetPlayer()->GetGUIDLow(), guildLowGuid, availability, classRoles, guildInterests, comment, time(NULL));
    sGuildFinderMgr->AddMembershipRequest(guildLowGuid, request);
}

void WorldSession::HandleGuildFinderBrowse(WorldPacket& recvPacket)
{
	// Temporary
	return;
	
    uint32 classRoles = 0;
    uint32 availability = 0;
    uint32 guildInterests = 0;
    uint32 playerLevel = 0; // Raw player level (1-85), do they use MAX_FINDER_LEVEL when on level 85 ?

    recvPacket >> classRoles >> availability >> guildInterests >> playerLevel;

    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & ALL_WEEK) || availability > ALL_WEEK)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;
    if (playerLevel > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) || playerLevel < 1)
        return;

    Player* player = GetPlayer();

    LFGuildPlayer settings(player->GetGUIDLow(), classRoles, availability, guildInterests, ANY_FINDER_LEVEL);
    LFGuildStore guildList = sGuildFinderMgr->GetGuildsMatchingSetting(settings, player->GetTeamId());
    uint32 guildCount = guildList.size();

    if (guildCount == 0)
    {
        WorldPacket packet(SMSG_LF_GUILD_BROWSE_UPDATED, 0);
        player->SendDirectMessage(&packet);
        return;
    }

    ByteBuffer bufferData(65 * guildCount);
    WorldPacket data(SMSG_LF_GUILD_BROWSE_UPDATED, 3 + guildCount * 65); // Estimated size
    data.WriteBits(guildCount, 19);

    for (LFGuildStore::const_iterator itr = guildList.begin(); itr != guildList.end(); ++itr)
    {
        LFGuildSettings guildSettings = itr->second;
        Guild* guild = sGuildMgr->GetGuildById(itr->first);

        ObjectGuid guildGUID = ObjectGuid(guild->GetGUID());

        data.WriteByteMask(guildGUID[7]);
        data.WriteByteMask(guildGUID[5]);
        data.WriteBits(guild->GetName().size(), 8);
        data.WriteByteMask(guildGUID[0]);
        data.WriteBits(guildSettings.GetComment().size(), 11);
        data.WriteByteMask(guildGUID[4]);
        data.WriteByteMask(guildGUID[1]);
        data.WriteByteMask(guildGUID[2]);
        data.WriteByteMask(guildGUID[6]);
        data.WriteByteMask(guildGUID[3]);

        bufferData << uint32(guild->GetEmblemInfo().GetColor());
        bufferData << uint32(guild->GetEmblemInfo().GetBorderStyle()); // Guessed
        bufferData << uint32(guild->GetEmblemInfo().GetStyle());

        bufferData << WriteBuffer(guildSettings.GetComment().c_str(), guildSettings.GetComment().size());

        bufferData << uint8(0); // Unk

        bufferData.WriteByteSeq(guildGUID[5]);

        bufferData << uint32(guildSettings.GetInterests());

        bufferData.WriteByteSeq(guildGUID[6]);
        bufferData.WriteByteSeq(guildGUID[4]);

        bufferData << uint32(guild->GetLevel());

        bufferData << WriteBuffer(guild->GetName().c_str(), guild->GetName().size());

        bufferData << uint32(guild->GetAchievementMgr().GetAchievementPoints());

        bufferData.WriteByteSeq(guildGUID[7]);

        bufferData << uint8(sGuildFinderMgr->HasRequest(player->GetGUIDLow(), guild->GetGUID())); // Request pending

        bufferData.WriteByteSeq(guildGUID[2]);
        bufferData.WriteByteSeq(guildGUID[0]);

        bufferData << uint32(guildSettings.GetAvailability());

        bufferData.WriteByteSeq(guildGUID[1]);

        bufferData << uint32(guild->GetEmblemInfo().GetBackgroundColor());
        bufferData << uint32(0); // Unk Int 2 (+ 128) // Always 0 or 1
        bufferData << uint32(guild->GetEmblemInfo().GetBorderColor());
        bufferData << uint32(guildSettings.GetClassRoles());

        bufferData.WriteByteSeq(guildGUID[3]);
        bufferData << int32(guild->GetMembersCount());
    }

    data.append(bufferData);

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderDeclineRecruit(WorldPacket& recvPacket)
{
    ObjectGuid playerGuid;

    recvPacket
        .ReadByteMask(playerGuid[1])
        .ReadByteMask(playerGuid[4])
        .ReadByteMask(playerGuid[5])
        .ReadByteMask(playerGuid[2])
        .ReadByteMask(playerGuid[6])
        .ReadByteMask(playerGuid[7])
        .ReadByteMask(playerGuid[0])
        .ReadByteMask(playerGuid[3])

        .ReadByteSeq(playerGuid[5])
        .ReadByteSeq(playerGuid[7])
        .ReadByteSeq(playerGuid[2])
        .ReadByteSeq(playerGuid[3])
        .ReadByteSeq(playerGuid[4])
        .ReadByteSeq(playerGuid[1])
        .ReadByteSeq(playerGuid[0])
        .ReadByteSeq(playerGuid[6]);

    if (!IS_PLAYER_GUID(playerGuid))
        return;

    sGuildFinderMgr->RemoveMembershipRequest(GUID_LOPART(playerGuid), GetPlayer()->GetGuildId());
}

void WorldSession::HandleGuildFinderGetApplications(WorldPacket& /*recvPacket*/)
{
	// Temporary
	return;
	
    std::list<MembershipRequest> applicatedGuilds = sGuildFinderMgr->GetAllMembershipRequestsForPlayer(GetPlayer()->GetGUIDLow());
    uint32 applicationsCount = applicatedGuilds.size();
    WorldPacket data(SMSG_LF_GUILD_MEMBERSHIP_LIST_UPDATED, 7 + 54 * applicationsCount);
    data.WriteBits(applicationsCount, 20);

    if (applicationsCount > 0)
    {
        ByteBuffer bufferData(54 * applicationsCount);
        for (std::list<MembershipRequest>::const_iterator itr = applicatedGuilds.begin(); itr != applicatedGuilds.end(); ++itr)
        {
            Guild* guild = sGuildMgr->GetGuildById(itr->GetGuildId());
            LFGuildSettings guildSettings = sGuildFinderMgr->GetGuildSettings(itr->GetGuildId());
            MembershipRequest request = *itr;

            ObjectGuid guildGuid = ObjectGuid(guild->GetGUID());

            data.WriteByteMask(guildGuid[1]);
            data.WriteByteMask(guildGuid[0]);
            data.WriteByteMask(guildGuid[5]);
            data.WriteBits(request.GetComment().size(), 11);
            data.WriteByteMask(guildGuid[3]);
            data.WriteByteMask(guildGuid[7]);
            data.WriteByteMask(guildGuid[4]);
            data.WriteByteMask(guildGuid[6]);
            data.WriteByteMask(guildGuid[2]);
            data.WriteBits(guild->GetName().size(), 8);

            bufferData.WriteByteSeq(guildGuid[2]);
            bufferData << WriteBuffer(request.GetComment().c_str(), request.GetComment().size());
            bufferData.WriteByteSeq(guildGuid[5]);
            bufferData << WriteBuffer(guild->GetName().c_str(), guild->GetName().size());

            bufferData << uint32(guildSettings.GetAvailability());
            bufferData << uint32(request.GetExpiryTime() - time(NULL)); // Time left to application expiry (seconds)

            bufferData.WriteByteSeq(guildGuid[0]);
            bufferData.WriteByteSeq(guildGuid[6]);
            bufferData.WriteByteSeq(guildGuid[3]);
            bufferData.WriteByteSeq(guildGuid[7]);

            bufferData << uint32(guildSettings.GetClassRoles());

            bufferData.WriteByteSeq(guildGuid[4]);
            bufferData.WriteByteSeq(guildGuid[1]);

            bufferData << uint32(time(NULL) - request.GetSubmitTime()); // Time since application (seconds)
            bufferData << uint32(guildSettings.GetInterests());
        }

        data.append(bufferData);
    }
    data << uint32(10 - sGuildFinderMgr->CountRequestsFromPlayer(GetPlayer()->GetGUIDLow())); // Applications count left

    GetPlayer()->SendDirectMessage(&data);
}

// Lists all recruits for a guild - Misses times
void WorldSession::HandleGuildFinderGetRecruits(WorldPacket& recvPacket)
{
    return;

    uint32 unkTime = 0;
    recvPacket >> unkTime;

    Player* player = GetPlayer();
    if (!player->GetGuildId())
        return;

    std::vector<MembershipRequest> recruitsList = sGuildFinderMgr->GetAllMembershipRequestsForGuild(player->GetGuildId());

    uint32 recruitCount = recruitsList.size();

    ByteBuffer dataBuffer(53 * recruitCount);
    WorldPacket data(SMSG_LF_GUILD_RECRUIT_LIST_UPDATED, 7 + 26 * recruitCount + 53 * recruitCount);
    data.WriteBits(recruitCount, 20);

    for (std::vector<MembershipRequest>::const_iterator itr = recruitsList.begin(); itr != recruitsList.end(); ++itr)
    {
        MembershipRequest request = *itr;
        ObjectGuid playerGuid(MAKE_NEW_GUID(request.GetPlayerGUID(), 0, HIGHGUID_PLAYER));

        data.WriteBits(request.GetComment().size(), 11);
        data.WriteByteMask(playerGuid[2]);
        data.WriteByteMask(playerGuid[4]);
        data.WriteByteMask(playerGuid[3]);
        data.WriteByteMask(playerGuid[7]);
        data.WriteByteMask(playerGuid[0]);
        data.WriteBits(request.GetName().size(), 7);
        data.WriteByteMask(playerGuid[5]);
        data.WriteByteMask(playerGuid[1]);
        data.WriteByteMask(playerGuid[6]);

        dataBuffer.WriteByteSeq(playerGuid[4]);

        dataBuffer << int32(time(NULL) <= request.GetExpiryTime());

        dataBuffer.WriteByteSeq(playerGuid[3]);
        dataBuffer.WriteByteSeq(playerGuid[0]);
        dataBuffer.WriteByteSeq(playerGuid[1]);

        dataBuffer << int32(request.GetLevel());

        dataBuffer.WriteByteSeq(playerGuid[6]);
        dataBuffer.WriteByteSeq(playerGuid[7]);
        dataBuffer.WriteByteSeq(playerGuid[2]);

        dataBuffer << int32(time(NULL) - request.GetSubmitTime()); // Time in seconds since application submitted.
        dataBuffer << int32(request.GetAvailability());
        dataBuffer << int32(request.GetClassRoles());
        dataBuffer << int32(request.GetInterests());
        dataBuffer << int32(request.GetExpiryTime() - time(NULL)); // TIme in seconds until application expires.

        dataBuffer << WriteBuffer(request.GetName().c_str(), request.GetName().size());
        dataBuffer << WriteBuffer(request.GetComment().c_str(), request.GetComment().size());

        dataBuffer << int32(request.GetClass());

        dataBuffer.WriteByteSeq(playerGuid[5]);
    }

    data.append(dataBuffer);
    data << uint32(time(NULL)); // Unk time

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderPostRequest(WorldPacket& /*recvPacket*/)
{
    Player* player = GetPlayer();

    if (!player->GetGuildId()) // Player must be in guild
        return;

    bool isGuildMaster = true;
    if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
        if (guild->GetLeaderGUID() != player->GetGUID())
            isGuildMaster = false;

    LFGuildSettings settings = sGuildFinderMgr->GetGuildSettings(player->GetGuildId());

    WorldPacket data(SMSG_LF_GUILD_POST_UPDATED, 35);
    data.WriteBit(isGuildMaster); // Guessed

    if (isGuildMaster)
    {
        data.WriteBit(settings.IsListed());
        data.WriteBits(settings.GetComment().size(), 11);
        data << uint32(settings.GetLevel());
        data << WriteBuffer(settings.GetComment().c_str(), settings.GetComment().size());
        data << uint32(0); // Unk Int32
        data << uint32(settings.GetAvailability());
        data << uint32(settings.GetClassRoles());
        data << uint32(settings.GetInterests());
    }

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderRemoveRecruit(WorldPacket& recvPacket)
{
    ObjectGuid guildGuid;

    recvPacket
        .ReadByteMask(guildGuid[0])
        .ReadByteMask(guildGuid[4])
        .ReadByteMask(guildGuid[3])
        .ReadByteMask(guildGuid[5])
        .ReadByteMask(guildGuid[7])
        .ReadByteMask(guildGuid[6])
        .ReadByteMask(guildGuid[2])
        .ReadByteMask(guildGuid[1])

        .ReadByteSeq(guildGuid[4])
        .ReadByteSeq(guildGuid[0])
        .ReadByteSeq(guildGuid[3])
        .ReadByteSeq(guildGuid[6])
        .ReadByteSeq(guildGuid[5])
        .ReadByteSeq(guildGuid[1])
        .ReadByteSeq(guildGuid[2])
        .ReadByteSeq(guildGuid[7]);

    if (!IS_GUILD_GUID(guildGuid))
        return;

    sGuildFinderMgr->RemoveMembershipRequest(GetPlayer()->GetGUIDLow(), GUID_LOPART(guildGuid));
}

// Sent any time a guild master sets an option in the interface and when listing / unlisting his guild
void WorldSession::HandleGuildFinderSetGuildPost(WorldPacket& recvPacket)
{
    uint32 classRoles = 0;
    uint32 availability = 0;
    uint32 guildInterests =  0;
    uint32 level = 0;
    std::string comment;

    recvPacket >> level >> availability >> guildInterests >> classRoles;
    // Level sent is zero if untouched, force to any (from interface). Idk why
    if (!level)
        level = ANY_FINDER_LEVEL;

    uint16 length = recvPacket.ReadBits(11);
    bool listed = recvPacket.ReadBit();
    recvPacket.read(comment, length);

    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & ALL_WEEK) || availability > ALL_WEEK)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;
    if (!(level & ALL_GUILDFINDER_LEVELS) || level > ALL_GUILDFINDER_LEVELS)
        return;

    Player* player = GetPlayer();

    if (!player->GetGuildId()) // Player must be in guild
        return;

    if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId())) // Player must be guild master
        if (guild->GetLeaderGUID() != player->GetGUID())
            return;

    LFGuildSettings settings(listed, player->GetTeamId(), player->GetGuildId(), classRoles, availability, guildInterests, level, comment);
    sGuildFinderMgr->SetGuildSettings(player->GetGuildId(), settings);
}

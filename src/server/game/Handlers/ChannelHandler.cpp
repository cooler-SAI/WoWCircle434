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


#include "ObjectMgr.h"                                      // for normalizePlayerName
#include "ChannelMgr.h"
#include "WordFilterMgr.h"

void WorldSession::HandleJoinChannel(WorldPacket& recvPacket)
{
    uint32 channelId;
    std::string channelName, pass;

    recvPacket >> channelId;
    recvPacket.ReadBits(2); // Skip 2 unknown bools
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 passLength = recvPacket.ReadBits(8);
    recvPacket.read(channelName, channelLength);
    recvPacket.read(pass, passLength);

    if (channelId)
    {
        ChatChannelsEntry const* channel = sChatChannelsStore.LookupEntry(channelId);
        if (!channel)
            return;

        AreaTableEntry const* current_zone = GetAreaEntryByAreaID(_player->GetZoneId());
        if (!current_zone)
            return;

        if (!_player->CanJoinConstantChannelInZone(channel, current_zone))
            return;
    }

    if (channelName.empty())
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        cMgr->team = _player->GetTeam();
        if (Channel* chn = cMgr->GetJoinChannel(channelName, channelId))
            chn->Join(_player->GetGUID(), pass.c_str());
    }
}

void WorldSession::HandleLeaveChannel(WorldPacket& recvPacket)
{
    uint32 unk;
    std::string channelname;
    recvPacket >> unk;                                      // channel id?
    uint32 length = recvPacket.ReadBits(8);
    recvPacket.read(channelname, length);

    if (channelname.empty())
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Leave(_player->GetGUID(), true);
        cMgr->LeftChannel(channelname);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    uint32 length = recvPacket.ReadBits(8);
    std::string channelname;
    recvPacket.read(channelname, length);

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->List(_player);
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelPassCommand < 5)
        return;
    else
       timeLastChannelPassCommand = now;

    std::string channelname;
    std::string pass;

    uint32 nameLength = recvPacket.ReadBits(8);
    uint32 passLength = recvPacket.ReadBits(7);

    recvPacket.read(channelname, nameLength);
    recvPacket.read(pass, passLength);

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Password(_player->GetGUID(), pass.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelSetownerCommand < 5)
        return;
    else
       timeLastChannelSetownerCommand = now;

    std::string newp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(newp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(newp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->SetOwner(_player->GetGUID(), newp.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelOwnerCommand < 5)
        return;
    else
       timeLastChannelOwnerCommand = now;

    std::string channelname;
    uint32 length = recvPacket.ReadBits(8);
    recvPacket.read(channelname, length);

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->SendWhoOwner(_player->GetGUID());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelModerCommand < 5)
        return;
    else
       timeLastChannelModerCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->SetModerator(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelUnmoderCommand < 5)
        return;
    else
       timeLastChannelUnmoderCommand = now;

    std::string channelname;
    std::string otp;

    uint32 nameLength = recvPacket.ReadBits(7);
    uint32 channelLength = recvPacket.ReadBits(8);

    recvPacket.read(channelname, channelLength);
    recvPacket.read(otp, nameLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->UnsetModerator(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelMuteCommand < 5)
        return;
    else
       timeLastChannelMuteCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->SetMute(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelUnmuteCommand < 5)
        return;
    else
       timeLastChannelUnmuteCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->UnsetMute(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelInviteCommand < 5)
        return;
    else
       timeLastChannelInviteCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    // check msg to bad word
    if (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE))
    {
        std::string badWord = sWordFilterMgr->FindBadWord(channelname, true);

        if (!badWord.empty())
            return;
    }

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Invite(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelKickCommand < 5)
        return;
    else
       timeLastChannelKickCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Kick(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelBanCommand < 5)
        return;
    else
       timeLastChannelBanCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(8);
    uint32 nameLength = recvPacket.ReadBits(7);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Ban(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelUnbanCommand < 5)
        return;
    else
       timeLastChannelUnbanCommand = now;

    std::string otp;
    std::string channelname;
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8);

    recvPacket.read(otp, nameLength);
    recvPacket.read(channelname, channelLength);

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->UnBan(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelAnnouncements(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastChannelAnnounceCommand < 5)
        return;
    else
       timeLastChannelAnnounceCommand = now;

    std::string channelname;
    uint32 length = recvPacket.ReadBits(8);
    recvPacket.read(channelname, length);

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
            chn->Announce(_player->GetGUID());
}

void WorldSession::HandleChannelDisplayListQuery(WorldPacket &recvPacket)
{
    // this should be OK because the 2 function _were_ the same
    HandleChannelList(recvPacket);
}

void WorldSession::HandleGetChannelMemberCount(WorldPacket &recvPacket)
{
    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel* chn = cMgr->GetChannel(channelname, _player))
        {
            WorldPacket data(SMSG_CHANNEL_MEMBER_COUNT, chn->GetName().size()+1+1+4);
            data << chn->GetName();
            data << uint8(chn->GetFlags());
            data << uint32(chn->GetNumPlayers());
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleSetChannelWatch(WorldPacket& recvPacket)
{
    std::string channelname;
    recvPacket >> channelname;
    /*if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* chn = cMgr->GetChannel(channelName, _player))
            chn->JoinNotify(_player->GetGUID());*/
}


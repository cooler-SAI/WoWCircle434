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
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Guild.h"
#include "GossipDef.h"
#include "SocialMgr.h"
#include "AchievementMgr.h"

// Helper for getting guild object of session's player.
// If guild does not exist, sends error (if necessary).
inline Guild* _GetPlayerGuild(WorldSession* session, bool sendError = false)
{
    if (uint32 guildId = session->GetPlayer()->GetGuildId())    // If guild id = 0, player is not in guild
        if (Guild* guild = sGuildMgr->GetGuildById(guildId))   // Find guild by id
            return guild;
    if (sendError)
        Guild::SendCommandResult(session, GUILD_CREATE_S, ERR_GUILD_PLAYER_NOT_IN_GUILD);
    return NULL;
}

void WorldSession::HandleGuildQueryOpcode(WorldPacket& recvPacket)
{
    uint64 guildGuid, playerGuid;
    recvPacket >> guildGuid >> playerGuid;

    // If guild doesn't exist or player is not part of the guild send error
    if (Guild* guild = sGuildMgr->GetGuildByGuid(guildGuid))
        if (guild->IsMember(playerGuid))
        {
            guild->HandleQuery(this);
            return;
        }

    Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_PLAYER_NOT_IN_GUILD);
}

void WorldSession::HandleGuildInviteOpcode(WorldPacket& recvPacket)
{
    time_t now = time(NULL);
    if (now - timeLastGuildInviteCommand < 5)
        return;
    else
       timeLastGuildInviteCommand = now;

    std::string invitedName;
    uint32 nameLength = recvPacket.ReadBits(7);
    recvPacket.read(invitedName, nameLength);

    if (normalizePlayerName(invitedName))
        if (Guild* guild = _GetPlayerGuild(this, true))
            guild->HandleInviteMember(this, invitedName);
}

void WorldSession::HandleGuildRemoveOpcode(WorldPacket& recvPacket)
{
    ObjectGuid playerGuid;

    recvPacket
        .ReadByteMask(playerGuid[6])
        .ReadByteMask(playerGuid[5])
        .ReadByteMask(playerGuid[4])
        .ReadByteMask(playerGuid[0])
        .ReadByteMask(playerGuid[1])
        .ReadByteMask(playerGuid[3])
        .ReadByteMask(playerGuid[7])
        .ReadByteMask(playerGuid[2])

        .ReadByteSeq(playerGuid[2])
        .ReadByteSeq(playerGuid[6])
        .ReadByteSeq(playerGuid[5])
        .ReadByteSeq(playerGuid[7])
        .ReadByteSeq(playerGuid[1])
        .ReadByteSeq(playerGuid[4])
        .ReadByteSeq(playerGuid[3])
        .ReadByteSeq(playerGuid[0]);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleRemoveMember(this, playerGuid);
}

void WorldSession::HandleGuildAcceptOpcode(WorldPacket& /*recvPacket*/)
{
    // Player cannot be in guild
    if (!GetPlayer()->GetGuildId())
        // Guild where player was invited must exist
        if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildIdInvited()))
            guild->HandleAcceptMember(this);
}

void WorldSession::HandleGuildDeclineOpcode(WorldPacket& /*recvPacket*/)
{
    GetPlayer()->SetGuildIdInvited(0);
    GetPlayer()->SetInGuild(0);
}

void WorldSession::HandleGuildRosterOpcode(WorldPacket& recvPacket)
{
    recvPacket.rfinish();

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleRoster(this);
}

void WorldSession::HandleGuildPromoteOpcode(WorldPacket& recvPacket)
{
    ObjectGuid targetGuid;

    recvPacket
        .ReadByteMask(targetGuid[7])
        .ReadByteMask(targetGuid[2])
        .ReadByteMask(targetGuid[5])
        .ReadByteMask(targetGuid[6])
        .ReadByteMask(targetGuid[1])
        .ReadByteMask(targetGuid[0])
        .ReadByteMask(targetGuid[3])
        .ReadByteMask(targetGuid[4])

        .ReadByteSeq(targetGuid[0])
        .ReadByteSeq(targetGuid[5])
        .ReadByteSeq(targetGuid[2])
        .ReadByteSeq(targetGuid[3])
        .ReadByteSeq(targetGuid[6])
        .ReadByteSeq(targetGuid[4])
        .ReadByteSeq(targetGuid[1])
        .ReadByteSeq(targetGuid[7]);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleUpdateMemberRank(this, targetGuid, false);
}

void WorldSession::HandleGuildDemoteOpcode(WorldPacket& recvPacket)
{
    ObjectGuid targetGuid;

    recvPacket
        .ReadByteMask(targetGuid[7])
        .ReadByteMask(targetGuid[1])
        .ReadByteMask(targetGuid[5])
        .ReadByteMask(targetGuid[6])
        .ReadByteMask(targetGuid[2])
        .ReadByteMask(targetGuid[3])
        .ReadByteMask(targetGuid[0])
        .ReadByteMask(targetGuid[4])

        .ReadByteSeq(targetGuid[1])
        .ReadByteSeq(targetGuid[2])
        .ReadByteSeq(targetGuid[7])
        .ReadByteSeq(targetGuid[5])
        .ReadByteSeq(targetGuid[6])
        .ReadByteSeq(targetGuid[0])
        .ReadByteSeq(targetGuid[4])
        .ReadByteSeq(targetGuid[3]);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleUpdateMemberRank(this, targetGuid, true);
}

void WorldSession::HandleGuildAssignRankOpcode(WorldPacket& recvPacket)
{
    ObjectGuid targetGuid;
    ObjectGuid setterGuid;

    uint32 rankId;
    recvPacket >> rankId;

    recvPacket
        .ReadByteMask(targetGuid[1])
        .ReadByteMask(targetGuid[7])
        .ReadByteMask(setterGuid[4])
        .ReadByteMask(setterGuid[2])
        .ReadByteMask(targetGuid[4])
        .ReadByteMask(targetGuid[5])
        .ReadByteMask(targetGuid[6])
        .ReadByteMask(setterGuid[1])
        .ReadByteMask(setterGuid[7])
        .ReadByteMask(targetGuid[2])
        .ReadByteMask(targetGuid[3])
        .ReadByteMask(targetGuid[0])
        .ReadByteMask(setterGuid[6])
        .ReadByteMask(setterGuid[3])
        .ReadByteMask(setterGuid[0])
        .ReadByteMask(setterGuid[5])

        .ReadByteSeq(targetGuid[0])
        .ReadByteSeq(setterGuid[1])
        .ReadByteSeq(setterGuid[3])
        .ReadByteSeq(setterGuid[5])
        .ReadByteSeq(targetGuid[7])
        .ReadByteSeq(targetGuid[3])
        .ReadByteSeq(setterGuid[0])
        .ReadByteSeq(targetGuid[1])
        .ReadByteSeq(setterGuid[6])
        .ReadByteSeq(targetGuid[2])
        .ReadByteSeq(targetGuid[5])
        .ReadByteSeq(targetGuid[4])
        .ReadByteSeq(setterGuid[2])
        .ReadByteSeq(setterGuid[4])
        .ReadByteSeq(targetGuid[6])
        .ReadByteSeq(setterGuid[7]);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleSetMemberRank(this, targetGuid, setterGuid, rankId);
}

void WorldSession::HandleGuildLeaveOpcode(WorldPacket& /*recvPacket*/)
{
    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleLeaveMember(this);
}

void WorldSession::HandleGuildDisbandOpcode(WorldPacket& /*recvPacket*/)
{
    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleDisband(this);
}

void WorldSession::HandleGuildMOTDOpcode(WorldPacket& recvPacket)
{
    std::string motd;
    uint32 motdLength = recvPacket.ReadBits(11);
    recvPacket.read(motd, motdLength);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleSetMOTD(this, motd);
}

void WorldSession::HandleGuildSetNoteOpcode(WorldPacket& recvPacket)
{
    std::string note;
    ObjectGuid playerGuid;

    recvPacket
        .ReadByteMask(playerGuid[1])
        .ReadByteMask(playerGuid[4])
        .ReadByteMask(playerGuid[5])
        .ReadByteMask(playerGuid[3])
        .ReadByteMask(playerGuid[0])
        .ReadByteMask(playerGuid[7]);

    bool type = recvPacket.ReadBit();      // 0 == Officer, 1 == Public

    recvPacket.ReadByteMask(playerGuid[6]);
    uint32 noteLength = recvPacket.ReadBits(8);

    recvPacket
        .ReadByteMask(playerGuid[2])

        .ReadByteSeq(playerGuid[4])
        .ReadByteSeq(playerGuid[5])
        .ReadByteSeq(playerGuid[0])
        .ReadByteSeq(playerGuid[3])
        .ReadByteSeq(playerGuid[1])
        .ReadByteSeq(playerGuid[6])
        .ReadByteSeq(playerGuid[7])
        .read(note, noteLength)
        .ReadByteSeq(playerGuid[2]);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleSetMemberNote(this, note, playerGuid, type);
}

void WorldSession::HandleGuildQueryRanksOpcode(WorldPacket& recvData)
{
    ObjectGuid guildGuid;

    recvData
        .ReadByteMask(guildGuid[2])
        .ReadByteMask(guildGuid[3])
        .ReadByteMask(guildGuid[0])
        .ReadByteMask(guildGuid[6])
        .ReadByteMask(guildGuid[4])
        .ReadByteMask(guildGuid[7])
        .ReadByteMask(guildGuid[5])
        .ReadByteMask(guildGuid[1])

        .ReadByteSeq(guildGuid[3])
        .ReadByteSeq(guildGuid[4])
        .ReadByteSeq(guildGuid[5])
        .ReadByteSeq(guildGuid[7])
        .ReadByteSeq(guildGuid[1])
        .ReadByteSeq(guildGuid[0])
        .ReadByteSeq(guildGuid[6])
        .ReadByteSeq(guildGuid[2]);

    if (Guild* guild = sGuildMgr->GetGuildByGuid(guildGuid))
        if (guild->IsMember(_player->GetGUID()))
            guild->HandleGuildRanks(this);
}

void WorldSession::HandleGuildAddRankOpcode(WorldPacket& recvPacket)
{
    uint32 rankId;
    std::string rankName;
    recvPacket >> rankId;

    uint32 length = recvPacket.ReadBits(7);
    recvPacket.read(rankName, length);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleAddNewRank(this, rankName); //, rankId);
}

void WorldSession::HandleGuildDelRankOpcode(WorldPacket& recvPacket)
{
    uint32 rankId;
    recvPacket >> rankId;

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleRemoveRank(this, rankId);
}

void WorldSession::HandleGuildChangeInfoTextOpcode(WorldPacket& recvPacket)
{
    std::string info;
    uint32 length = recvPacket.ReadBits(12);
    recvPacket.read(info, length);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleSetInfo(this, info);
}

void WorldSession::HandleSaveGuildEmblemOpcode(WorldPacket& recvPacket)
{
    uint64 vendorGuid;
    recvPacket >> vendorGuid;

    EmblemInfo emblemInfo;
    emblemInfo.ReadPacket(recvPacket);

    if (GetPlayer()->GetNPCIfCanInteractWith(vendorGuid, UNIT_NPC_FLAG_TABARDDESIGNER))
    {
        // Remove fake death
        if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
            GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        if (Guild* guild = _GetPlayerGuild(this))
            guild->HandleSetEmblem(this, emblemInfo);
        else
            // "You are not part of a guild!";
            Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_NOGUILD);
    }
    else
    {
        // "That's not an emblem vendor!"
        Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_INVALIDVENDOR);
    }
}

void WorldSession::HandleGuildEventLogQueryOpcode(WorldPacket& /* recvPacket */)
{
    if (Guild* guild = _GetPlayerGuild(this))
        guild->SendEventLog(this);
}

void WorldSession::HandleGuildBankMoneyWithdrawn(WorldPacket & /* recvData */)
{
    if (Guild* guild = _GetPlayerGuild(this))
        guild->SendMoneyInfo(this);
}

void WorldSession::HandleGuildPermissions(WorldPacket& /* recvData */)
{
    if (Guild* guild = _GetPlayerGuild(this))
        guild->SendPermissions(this);
}

// Called when clicking on Guild bank gameobject
void WorldSession::HandleGuildBankerActivate(WorldPacket& recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    uint8 fullSlotList;
    recvData >> fullSlotList; // 0 = only slots updated in last operation are shown. 1 = all slots updated

    if (GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
    {
        if (Guild* guild = _GetPlayerGuild(this))
            guild->SendBankList(this, 0, true, true);
        else
            Guild::SendCommandResult(this, GUILD_UNK1, ERR_GUILD_PLAYER_NOT_IN_GUILD);
    }
}

// Called when opening guild bank tab only (first one)
void WorldSession::HandleGuildBankQueryTab(WorldPacket & recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    uint8 tabId;
    recvData >> tabId;

    uint8 fullSlotList;
    recvData >> fullSlotList; // 0 = only slots updated in last operation are shown. 1 = all slots updated

    if (GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = _GetPlayerGuild(this))
            guild->SendBankList(this, tabId, true, false);
}

void WorldSession::HandleGuildBankDepositMoney(WorldPacket & recvData)
{
    uint64 goGuid;
    recvData >> goGuid;

    uint64 money;
    recvData >> money;

    if (GetPlayer()->GetGameObjectIfCanInteractWith(goGuid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (money && GetPlayer()->HasEnoughMoney(money))
            if (Guild* guild = _GetPlayerGuild(this))
                guild->HandleMemberDepositMoney(this, money);
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPacket & recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    uint64 money;
    recvData >> money;

    if (money)
        if (GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
            if (Guild* guild = _GetPlayerGuild(this))
                guild->HandleMemberWithdrawMoney(this, money);
}

void WorldSession::HandleGuildBankSwapItems(WorldPacket & recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    if (!GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
    {
        recvData.rfinish();                   // Prevent additional spam at rejected packet
        return;
    }

    Guild* guild = _GetPlayerGuild(this);
    if (!guild)
    {
        recvData.rfinish();                   // Prevent additional spam at rejected packet
        return;
    }

    uint8 bankToBank;
    recvData >> bankToBank;

    uint8 tabId;
    uint8 slotId;
    uint32 itemEntry;
    uint32 splitedAmount = 0;

    if (bankToBank)
    {
        uint8 destTabId;
        recvData >> destTabId;

        uint8 destSlotId;
        recvData >> destSlotId;

        uint32 destItemEntry;
        recvData >> destItemEntry;

        recvData >> tabId;
        recvData >> slotId;
        recvData >> itemEntry;
        recvData.read_skip<uint8>();                       // Always 0
        recvData >> splitedAmount;

        guild->SwapItems(GetPlayer(), tabId, slotId, destTabId, destSlotId, splitedAmount);
    }
    else
    {
        uint8 playerBag = NULL_BAG;
        uint8 playerSlotId = NULL_SLOT;
        uint8 toChar = 1;

        recvData >> tabId;
        recvData >> slotId;
        recvData >> itemEntry;

        uint8 autoStore;
        recvData >> autoStore;
        if (autoStore)
        {
            recvData.read_skip<uint32>();                  // autoStoreCount
            recvData.read_skip<uint8>();                   // ToChar (?), always and expected to be 1 (autostore only triggered in Bank -> Char)
            recvData.read_skip<uint32>();                  // Always 0
        }
        else
        {
            recvData >> playerBag;
            recvData >> playerSlotId;
            recvData >> toChar;
            recvData >> splitedAmount;
        }

        // Player <-> Bank
        // Allow to work with inventory only
        if (!Player::IsInventoryPos(playerBag, playerSlotId) && !(playerBag == NULL_BAG && playerSlotId == NULL_SLOT))
            GetPlayer()->SendEquipError(EQUIP_ERR_INTERNAL_BAG_ERROR, NULL);
        else
            guild->SwapItemsWithInventory(GetPlayer(), toChar, tabId, slotId, playerBag, playerSlotId, splitedAmount);
    }
}

void WorldSession::HandleGuildBankBuyTab(WorldPacket & recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    uint8 tabId;
    recvData >> tabId;

    if (!GoGuid || GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = _GetPlayerGuild(this))
            guild->HandleBuyBankTab(this, tabId);
}

void WorldSession::HandleGuildBankUpdateTab(WorldPacket & recvData)
{
    uint64 GoGuid;
    recvData >> GoGuid;

    uint8 tabId;
    recvData >> tabId;

    std::string name;
    recvData >> name;

    std::string icon;
    recvData >> icon;

    if (!name.empty() && !icon.empty())
        if (GetPlayer()->GetGameObjectIfCanInteractWith(GoGuid, GAMEOBJECT_TYPE_GUILD_BANK))
            if (Guild* guild = _GetPlayerGuild(this))
                guild->HandleSetBankTabInfo(this, tabId, name, icon);
}

void WorldSession::HandleGuildBankLogQuery(WorldPacket & recvData)
{
    uint32 tabId;
    recvData >> tabId;

    if (Guild* guild = _GetPlayerGuild(this))
        guild->SendBankLog(this, tabId);
}

void WorldSession::HandleQueryGuildBankTabText(WorldPacket &recvData)
{
    uint8 tabId;
    recvData >> tabId;

    if (Guild* guild = _GetPlayerGuild(this))
        guild->SendBankTabText(this, tabId);
}

void WorldSession::HandleSetGuildBankTabText(WorldPacket& recvData)
{
    uint32 tabId;
    std::string text;
    recvData >> tabId;

    uint32 textLen = recvData.ReadBits(14);
    recvData.read(text, textLen);

    if (Guild* guild = _GetPlayerGuild(this))
        guild->SetBankTabText(tabId, text);
}

void WorldSession::HandleGuildQueryXPOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guildGuid;

    recvPacket
        .ReadByteMask(guildGuid[2])
        .ReadByteMask(guildGuid[1])
        .ReadByteMask(guildGuid[0])
        .ReadByteMask(guildGuid[5])
        .ReadByteMask(guildGuid[4])
        .ReadByteMask(guildGuid[7])
        .ReadByteMask(guildGuid[6])
        .ReadByteMask(guildGuid[3])

        .ReadByteSeq(guildGuid[7])
        .ReadByteSeq(guildGuid[2])
        .ReadByteSeq(guildGuid[3])
        .ReadByteSeq(guildGuid[6])
        .ReadByteSeq(guildGuid[1])
        .ReadByteSeq(guildGuid[5])
        .ReadByteSeq(guildGuid[0])
        .ReadByteSeq(guildGuid[4]);

    if (Guild* guild = sGuildMgr->GetGuildByGuid(guildGuid))
        if (guild->IsMember(_player->GetGUID()))
            guild->SendGuildXP(this);
}

void WorldSession::HandleGuildSetRankPermissionsOpcode(WorldPacket& recvPacket)
{
    Guild* guild = _GetPlayerGuild(this, true);
    if (!guild)
    {
        recvPacket.rfinish();
        return;
    }

    uint32 unk;
    uint32 rankId;
    uint32 oldRights;
    uint32 newRights;
    uint32 moneyPerDay;

    recvPacket >> unk;
    recvPacket >> oldRights;
    recvPacket >> newRights;

    GuildBankRightsAndSlotsVec rightsAndSlots(GUILD_BANK_MAX_TABS);
    for (uint8 tabId = 0; tabId < GUILD_BANK_MAX_TABS; ++tabId)
    {
        uint32 bankRights;
        uint32 slots;

        recvPacket >> bankRights;
        recvPacket >> slots;

        rightsAndSlots[tabId] = GuildBankRightsAndSlots(uint8(bankRights), slots);
    }

    recvPacket >> moneyPerDay;
    recvPacket >> rankId;
    std::string rankName;
    uint32 nameLength = recvPacket.ReadBits(7);
    recvPacket.read(rankName, nameLength);

    guild->HandleSetRankInfo(this, rankId, rankName, newRights, moneyPerDay, rightsAndSlots);
}

void WorldSession::HandleGuildRequestPartyState(WorldPacket& recvData)
{
    ObjectGuid guildGuid;

    recvData
        .ReadByteMask(guildGuid[0])
        .ReadByteMask(guildGuid[6])
        .ReadByteMask(guildGuid[7])
        .ReadByteMask(guildGuid[3])
        .ReadByteMask(guildGuid[5])
        .ReadByteMask(guildGuid[1])
        .ReadByteMask(guildGuid[2])
        .ReadByteMask(guildGuid[4])

        .ReadByteSeq(guildGuid[6])
        .ReadByteSeq(guildGuid[3])
        .ReadByteSeq(guildGuid[2])
        .ReadByteSeq(guildGuid[1])
        .ReadByteSeq(guildGuid[5])
        .ReadByteSeq(guildGuid[0])
        .ReadByteSeq(guildGuid[7])
        .ReadByteSeq(guildGuid[4]);

    if (Guild* guild = sGuildMgr->GetGuildByGuid(guildGuid))
        guild->HandleGuildPartyRequest(this);
}

void WorldSession::HandleGuildRequestMaxDailyXP(WorldPacket& recvPacket)
{
    ObjectGuid guid;

    recvPacket
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[2])

        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[0]);

    if (Guild* guild = sGuildMgr->GetGuildByGuid(guid))
    {
        if (guild->IsMember(_player->GetGUID()))
        {
            WorldPacket data(SMSG_GUILD_MAX_DAILY_XP, 8);
            data << uint64(sWorld->getIntConfig(CONFIG_GUILD_DAILY_XP_CAP));
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleAutoDeclineGuildInvites(WorldPacket& recvPacket)
{
    uint8 enable;
    recvPacket >> enable;

    GetPlayer()->ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_AUTO_DECLINE_GUILD, enable);
}

void WorldSession::HandleGuildRewardsQueryOpcode(WorldPacket& recvPacket)
{
    recvPacket.read_skip<uint32>(); // Unk

    if (sGuildMgr->GetGuildById(_player->GetGuildId()))
    {
        std::vector<GuildReward> const& rewards = sGuildMgr->GetGuildRewards();

        WorldPacket data(SMSG_GUILD_REWARDS_LIST, 3 + rewards.size() * (4 + 4 + 4 + 8 + 4 + 4));
        data.WriteBits(rewards.size(), 21);

        for (uint32 i = 0; i < rewards.size(); i++)
        {
            data << uint32(rewards[i].Standing);
            data << int32(rewards[i].Racemask);
            data << uint32(rewards[i].Entry);
            data << uint64(rewards[i].Price);
            data << uint32(0); // Unused
            data << uint32(rewards[i].AchievementId);
        }
        data << uint32(time(NULL));
        SendPacket(&data);
    }
}

void WorldSession::HandleGuildQueryNewsOpcode(WorldPacket& recvPacket)
{
    recvPacket.read_skip<uint32>();

    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
    {
        WorldPacket data;
        guild->GetNewsLog().BuildNewsData(data);
        SendPacket(&data);
    }
}

void WorldSession::HandleGuildNewsUpdateStickyOpcode(WorldPacket& recvPacket)
{
    uint32 newsId;
    bool sticky;
    ObjectGuid guid;

    recvPacket >> newsId;

    recvPacket
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[0]);

    sticky = recvPacket.ReadBit();

    recvPacket
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[5])

        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[4]);

    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
    {
        if (GuildNewsEntry* newsEntry = guild->GetNewsLog().GetNewsById(newsId))
        {
            if (sticky)
                newsEntry->Flags |= 1;
            else
                newsEntry->Flags &= ~1;

            WorldPacket data;
            guild->GetNewsLog().BuildNewsData(newsId, *newsEntry, data);
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleGuildSetGuildMaster(WorldPacket& recvPacket)
{
    std::string playerName;
    uint8 nameLength = recvPacket.ReadBits(7);
    recvPacket.read(playerName, nameLength);

    if (Guild* guild = _GetPlayerGuild(this, true))
        guild->HandleSetNewGuildMaster(this, playerName);
}

void WorldSession::HandleGuildRequestChallengeUpdate(WorldPacket& /*recvPacket*/)
{
    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
    {
        GuildChallengeRewardData const& reward = sObjectMgr->GetGuildChallengeRewardData();

        WorldPacket data(SMSG_GUILD_CHALLENGE_UPDATED, 5*4*4);

        for (uint8 i = 0; i < CHALLENGE_MAX; ++i)
            data << uint32(reward[i].Expirience);

        for (uint8 i = 0; i < CHALLENGE_MAX; ++i)
            data << uint32(reward[i].Gold);

        for (uint8 i = 0; i < CHALLENGE_MAX; ++i)
            data << uint32(reward[i].ChallengeCount);

        for (uint8 i = 0; i < CHALLENGE_MAX; ++i)
            data << uint32(reward[i].Gold2);

        for (uint8 i = 0; i < CHALLENGE_MAX; ++i)
            data << uint32(guild->GetGuildChallenge(i));        // Current Count

        SendPacket(&data);
    }
}

void WorldSession::HandleGuildAchievementMembers(WorldPacket& recvPacket)
{
    uint32 achievementId;
    ObjectGuid guildGuid;
    ObjectGuid playerGuid;

    recvPacket >> achievementId;

    guildGuid[0] = recvPacket.ReadBit();
    playerGuid[5] = recvPacket.ReadBit();
    playerGuid[4] = recvPacket.ReadBit();
    playerGuid[7] = recvPacket.ReadBit();
    playerGuid[0] = recvPacket.ReadBit();
    guildGuid[5] = recvPacket.ReadBit();
    guildGuid[7] = recvPacket.ReadBit();
    playerGuid[3] = recvPacket.ReadBit();
    guildGuid[3] = recvPacket.ReadBit();
    playerGuid[2] = recvPacket.ReadBit();
    guildGuid[4] = recvPacket.ReadBit();
    guildGuid[1] = recvPacket.ReadBit();
    guildGuid[6] = recvPacket.ReadBit();
    playerGuid[6] = recvPacket.ReadBit();
    guildGuid[2] = recvPacket.ReadBit();
    playerGuid[1] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guildGuid[0]);
    recvPacket.ReadByteSeq(guildGuid[3]);
    recvPacket.ReadByteSeq(playerGuid[2]);
    recvPacket.ReadByteSeq(guildGuid[7]);
    recvPacket.ReadByteSeq(guildGuid[2]);
    recvPacket.ReadByteSeq(playerGuid[5]);
    recvPacket.ReadByteSeq(playerGuid[0]);
    recvPacket.ReadByteSeq(playerGuid[3]);
    recvPacket.ReadByteSeq(guildGuid[5]);
    recvPacket.ReadByteSeq(guildGuid[1]);
    recvPacket.ReadByteSeq(playerGuid[4]);
    recvPacket.ReadByteSeq(playerGuid[1]);
    recvPacket.ReadByteSeq(playerGuid[6]);
    recvPacket.ReadByteSeq(playerGuid[7]);
    recvPacket.ReadByteSeq(guildGuid[4]);
    recvPacket.ReadByteSeq(guildGuid[6]);

    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_GUILD_ACHIEVEMENT_MEMBERS");
    /*
    if(Guild* pGuild = sGuildMgr->GetGuildByGuid(guildGuid))
    {
        if(pGuild->GetAchievementMgr().HasAchieved(achievementId))
        {
            ObjectGuid gguid = pGuild->GetGUID();
            CompletedAchievementData* achievement = pGuild->GetAchievementMgr().GetCompletedDataForAchievement(achievementId);
            WorldPacket data(SMSG_GUILD_ACHIEVEMENT_MEMBERS);

            data.WriteBit(gguid[3]);
            data.WriteBit(gguid[4]);
            data.WriteBit(gguid[7]);
            data.WriteBit(gguid[0]);

            data.WriteBits(achievement->guids.size(), 26);

            for(std::set<uint64>::iterator itr = achievement->guids.begin(); itr != achievement->guids.end(); ++itr)
            {
                ObjectGuid pguid = (*itr);

                data.WriteBit(pguid[3]);
                data.WriteBit(pguid[1]);
                data.WriteBit(pguid[4]);
                data.WriteBit(pguid[5]);
                data.WriteBit(pguid[7]);
                data.WriteBit(pguid[0]);
                data.WriteBit(pguid[6]);
                data.WriteBit(pguid[2]);
            }

            data.WriteBit(gguid[2]);
            data.WriteBit(gguid[6]);
            data.WriteBit(gguid[5]);
            data.WriteBit(gguid[1]);

            data.WriteByteSeq(gguid[5]);

            for(std::set<uint64>::iterator itr = achievement->guids.begin(); itr != achievement->guids.end(); ++itr)
            {
                ObjectGuid pguid = (*itr);

                data.WriteByteSeq(pguid[1]);
                data.WriteByteSeq(pguid[5]);
                data.WriteByteSeq(pguid[7]);
                data.WriteByteSeq(pguid[0]);
                data.WriteByteSeq(pguid[6]);
                data.WriteByteSeq(pguid[4]);
                data.WriteByteSeq(pguid[3]);
                data.WriteByteSeq(pguid[2]);
            }

            data.WriteByteSeq(gguid[7]);
            data.WriteByteSeq(gguid[2]);
            data.WriteByteSeq(gguid[4]);
            data.WriteByteSeq(gguid[3]);
            data.WriteByteSeq(gguid[6]);
            data.WriteByteSeq(gguid[0]);

            data << achievementId;

            data.WriteByteSeq(gguid[1]);

            SendPacket(&data);
        }
    }
    */
}

void WorldSession::HandleGuildRenameRequest(WorldPacket& recvPacket)
{
    uint32 lenght;
    std::string newName;

    lenght = recvPacket.ReadBits(8);
    newName = recvPacket.ReadString(lenght);

    Guild* pGuild = GetPlayer()->GetGuild();

    if(pGuild)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_NAME);

        _guildRenameCallback.SetParam(newName);

        stmt->setUInt32(1, pGuild->GetId());
        stmt->setString(0, newName);

        _guildRenameCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));

        WorldPacket data(SMSG_GUILD_FLAGGED_FOR_RENAME, 1);

        data.WriteBit(true);    // it may send false also, but we don't know how to handle exeptions by the DB layer

        pGuild->BroadcastPacket(&data);
    }
}

void WorldSession::HandleGuildRenameCallback(std::string newName)
{
    Guild* pGuild = GetPlayer()->GetGuild();
    bool hasRenamed = ((PreparedQueryResult)_guildRenameCallback.GetFutureResult())->GetRowCount() > 0 ? true : false;

    WorldPacket data(SMSG_GUILD_CHANGE_NAME_RESULT, 1);
    data.WriteBit(hasRenamed);

    SendPacket(&data);

    if (pGuild && hasRenamed)
    {
        pGuild->SendGuildRename(newName);
    }
}

void WorldSession::SendGuildCancelInvite(std::string unkString, uint8 unkByte)
{
    WorldPacket data(SMSG_GUILD_INVITE_CANCEL, 1 + unkString.length());

    data << unkString << unkByte;

    SendPacket(&data);
}
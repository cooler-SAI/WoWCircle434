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
#include "Language.h"
#include "WordFilterMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "ArenaTeamMgr.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Guild.h"
#include "ArenaTeam.h"
#include "GossipDef.h"
#include "SocialMgr.h"

#define CHARTER_DISPLAY_ID 16161

/*enum PetitionType // dbc data
{
    PETITION_TYPE_GUILD      = 1,
    PETITION_TYPE_ARENA_TEAM = 3
};*/

// Charters ID in item_template
enum CharterItemIDs
{
    GUILD_CHARTER                                 = 5863,
    ARENA_TEAM_CHARTER_2v2                        = 23560,
    ARENA_TEAM_CHARTER_3v3                        = 23561,
    ARENA_TEAM_CHARTER_5v5                        = 23562
};

enum CharterCosts
{
    GUILD_CHARTER_COST                            = 1000,
    ARENA_TEAM_CHARTER_2v2_COST                   = 800000,
    ARENA_TEAM_CHARTER_3v3_COST                   = 1200000,
    ARENA_TEAM_CHARTER_5v5_COST                   = 2000000
};

void WorldSession::HandlePetitionBuyOpcode(WorldPacket & recvData)
{
    uint64 guidNPC;
    uint32 clientIndex;                                     // 1 for guild and arenaslot+1 for arenas in client
    std::string name;

    recvData >> guidNPC;                                   // NPC GUID
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint64>();                          // 0
    recvData >> name;                                      // name
    recvData.read_skip<std::string>();                     // some string
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint16>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0
    recvData.read_skip<uint32>();                          // 0

    for (int i = 0; i < 10; ++i)
        recvData.read_skip<std::string>();

    recvData >> clientIndex;                               // index
    recvData.read_skip<uint32>();                          // 0

    // prevent cheating
    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guidNPC, UNIT_NPC_FLAG_PETITIONER);
    if (!creature)
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    uint32 charterid = 0;
    uint32 cost = 0;
    // if tabard designer, then trying to buy a guild charter.
    // do not let if already in guild.
    if (_player->GetGuildId())
        return;

    charterid = GUILD_CHARTER;
    cost = GUILD_CHARTER_COST;

    if (sGuildMgr->GetGuildByName(name))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, name);
        return;
    }
    if (sObjectMgr->IsReservedName(name) || !ObjectMgr::IsValidCharterName(name) ||
        (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(name).empty()))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_INVALID, name);
        return;
    }

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(charterid);
    if (!pProto)
    {
        _player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, charterid);
        return;
    }

    if (!_player->HasEnoughMoney(uint64(cost)))
    {                                                       //player hasn't got enough money
        _player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, guidNPC, charterid);
        return;
    }

    ItemPosCountVec dest;
    InventoryResult msg = _player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, charterid, pProto->BuyCount);
    if (msg != EQUIP_ERR_OK)
    {
        _player->SendEquipError(msg, NULL, NULL, charterid);
        return;
    }

    _player->ModifyMoney(-(int32)cost);
    Item* charter = _player->StoreNewItem(dest, charterid, true);
    if (!charter)
        return;

    charter->SetUInt32Value(ITEM_FIELD_ENCHANTMENT_1_1, charter->GetGUIDLow());
    // ITEM_FIELD_ENCHANTMENT_1_1 is guild/arenateam id
    // ITEM_FIELD_ENCHANTMENT_1_1+1 is current signatures count (showed on item)
    charter->SetState(ITEM_CHANGED, _player);
    _player->SendNewItem(charter, 1, true, false);

    // a petition is invalid, if both the owner and the type matches
    // we checked above, if this player is in an arenateam, so this must be
    // datacorruption
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_BY_OWNER);
    stmt->setUInt32(0, _player->GetGUIDLow());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    std::ostringstream ssInvalidPetitionGUIDs;

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            ssInvalidPetitionGUIDs << '\'' << fields[0].GetUInt32() << "', ";
        } while (result->NextRow());
    }

    // delete petitions with the same guid as this one
    ssInvalidPetitionGUIDs << '\'' << charter->GetGUIDLow() << '\'';

    CharacterDatabase.EscapeString(name);
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    trans->PAppend("DELETE FROM petition WHERE petitionguid IN (%s)",  ssInvalidPetitionGUIDs.str().c_str());
    trans->PAppend("DELETE FROM petition_sign WHERE petitionguid IN (%s)", ssInvalidPetitionGUIDs.str().c_str());

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION);
    stmt->setUInt32(0, _player->GetGUIDLow());
    stmt->setUInt32(1, charter->GetGUIDLow());
    stmt->setString(2, name);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void WorldSession::HandlePetitionShowSignOpcode(WorldPacket& recvData)
{
    uint8 signs = 0;
    uint64 petitionguid;
    recvData >> petitionguid;                              // petition guid

    // solve (possible) some strange compile problems with explicit use GUID_LOPART(petitionguid) at some GCC versions (wrong code optimization in compiler?)
    uint32 petitionGuidLow = GUID_LOPART(petitionguid);

    // if guild petition and has guild => error, return;
    if (_player->GetGuildId())
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);

    stmt->setUInt32(0, petitionGuidLow);

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    // result == NULL also correct in case no sign yet
    if (result)
        signs = uint8(result->GetRowCount());

    WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, (8+8+4+1+signs*12));
    data << uint64(petitionguid);                           // petition guid
    data << uint64(_player->GetGUID());                     // owner guid
    data << uint32(petitionGuidLow);                        // guild guid
    data << uint8(signs);                                   // sign's count

    for (uint8 i = 1; i <= signs; ++i)
    {
        Field* fields2 = result->Fetch();
        uint32 lowGuid = fields2[0].GetUInt32();

        data << uint64(MAKE_NEW_GUID(lowGuid, 0, HIGHGUID_PLAYER)); // Player GUID
        data << uint32(0);                                  // there 0 ...

        result->NextRow();
    }
    SendPacket(&data);
}

void WorldSession::HandlePetitionQueryOpcode(WorldPacket & recvData)
{
    uint32 guildguid;
    uint64 petitionguid;
    recvData >> guildguid;                                 // in Trinity always same as GUID_LOPART(petitionguid)
    recvData >> petitionguid;                              // petition guid

    SendPetitionQueryOpcode(petitionguid);
}

void WorldSession::SendPetitionQueryOpcode(uint64 petitionguid)
{
    uint64 ownerguid = 0;
    std::string name = "NO_NAME_FOR_GUID";

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);

    stmt->setUInt32(0, GUID_LOPART(petitionguid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        Field* fields = result->Fetch();
        ownerguid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
        name      = fields[1].GetString();
    }
    else
        return;

    WorldPacket data(SMSG_PETITION_QUERY_RESPONSE, (4+8+name.size()+1+1+4*12+2+10));
    data << uint32(GUID_LOPART(petitionguid));              // guild/team guid (in Trinity always same as GUID_LOPART(petition guid)
    data << uint64(ownerguid);                              // charter owner guid
    data << name;                                           // name (guild/arena team)
    data << uint8(0);                                       // some string
    data << uint32(4);
    data << uint32(4);
    data << uint32(0);                                  // bypass client - side limitation, a different value is needed here for each petition
    data << uint32(0);                                      // 5
    data << uint32(0);                                      // 6
    data << uint32(0);                                      // 7
    data << uint32(0);                                      // 8
    data << uint16(0);                                      // 9 2 bytes field
    data << uint32(0);                                      // 10
    data << uint32(0);                                      // 11
    data << uint32(0);                                      // 13 count of next strings?

    for (int i = 0; i < 10; ++i)
        data << uint8(0);                                   // some string

    data << uint32(0);                                      // 14
    data << uint32(0);                                      // 15 0 - guild, 1 - arena team

    SendPacket(&data);
}

void WorldSession::HandlePetitionRenameOpcode(WorldPacket & recvData)
{
    uint64 petitionGuid;
    std::string newName;

    recvData >> petitionGuid;                              // guid
    recvData >> newName;                                   // new name

    Item* item = _player->GetItemByGuid(petitionGuid);
    if (!item)
        return;

    if (sGuildMgr->GetGuildByName(newName))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, newName);
        return;
    }
    if (sObjectMgr->IsReservedName(newName) || !ObjectMgr::IsValidCharterName(newName) ||
        (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(newName).empty()))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_INVALID, newName);
        return;
    }

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PETITION_NAME);

    stmt->setString(0, newName);
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    CharacterDatabase.Execute(stmt);

    WorldPacket data(MSG_PETITION_RENAME, (8+newName.size()+1));
    data << uint64(petitionGuid);
    data << newName;
    SendPacket(&data);
}

void WorldSession::HandlePetitionSignOpcode(WorldPacket & recvData)
{
    Field* fields;
    uint64 petitionGuid;
    uint8 unk;
    recvData >> petitionGuid;                              // petition guid
    recvData >> unk;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURES);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Petition %u is not found for player %u %s", GUID_LOPART(petitionGuid), GetPlayer()->GetGUIDLow(), GetPlayer()->GetName());
        return;
    }

    fields = result->Fetch();
    uint64 ownerGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
    uint64 signs = fields[1].GetUInt64();

    uint32 playerGuid = _player->GetGUIDLow();
    if (GUID_LOPART(ownerGuid) == playerGuid)
        return;

    // not let enemies sign guild charter
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && GetPlayer()->GetTeam() != sObjectMgr->GetPlayerTeamByGUID(ownerGuid))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NOT_ALLIED);
        return;
    }

    if (_player->GetGuildId())
    {
        Guild::SendCommandResult(this, GUILD_INVITE_S, ERR_ALREADY_IN_GUILD_S, _player->GetName());
        return;
    }
    if (_player->GetGuildIdInvited())
    {
        Guild::SendCommandResult(this, GUILD_INVITE_S, ERR_ALREADY_INVITED_TO_GUILD_S, _player->GetName());
        return;
    }

    if (++signs > 4)                                        // client signs maximum
        return;

    // Client doesn't allow to sign petition two times by one character, but not check sign by another character from same account
    // not allow sign another player from already sign player account
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIG_BY_ACCOUNT);

    stmt->setUInt32(0, GetAccountId());
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    result = CharacterDatabase.Query(stmt);

    if (result)
    {
        // close at signer side
        _player->SendPetitionSignResult(petitionGuid, _player, PETITION_SIGN_ALREADY_SIGNED);
        return;
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION_SIGNATURE);

    stmt->setUInt32(0, GUID_LOPART(ownerGuid));
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));
    stmt->setUInt32(2, playerGuid);
    stmt->setUInt32(3, GetAccountId());

    CharacterDatabase.Execute(stmt);

    _player->SendPetitionSignResult(petitionGuid, _player, PETITION_SIGN_OK);

    // update for owner if online
    if (Player* owner = ObjectAccessor::FindPlayer(ownerGuid))
        owner->SendPetitionSignResult(petitionGuid, _player, PETITION_SIGN_OK);
}

void WorldSession::HandlePetitionDeclineOpcode(WorldPacket & recvData)
{
    uint64 petitionguid;
    uint64 ownerguid;
    recvData >> petitionguid;                              // petition guid

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_OWNER_BY_GUID);

    stmt->setUInt32(0, GUID_LOPART(petitionguid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    Field* fields = result->Fetch();
    ownerguid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);

    Player* owner = ObjectAccessor::FindPlayer(ownerguid);
    if (owner)                                               // petition owner online
    {
        WorldPacket data(MSG_PETITION_DECLINE, 8);
        data << uint64(_player->GetGUID());
        owner->GetSession()->SendPacket(&data);
    }
}

void WorldSession::HandleOfferPetitionOpcode(WorldPacket & recvData)
{
    uint8 signs = 0;
    uint64 petitionguid, plguid;
    uint32 junk;
    Player* player;
    recvData >> junk;                                      // ?
    recvData >> petitionguid;                              // petition guid
    recvData >> plguid;                                    // player guid

    player = ObjectAccessor::FindPlayer(plguid);
    if (!player)
        return;

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && GetPlayer()->GetTeam() != player->GetTeam())
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NOT_ALLIED);
        return;
    }

    if (player->GetGuildId())
    {
        Guild::SendCommandResult(this, GUILD_INVITE_S, ERR_ALREADY_IN_GUILD_S, _player->GetName());
        return;
    }

    if (player->GetGuildIdInvited())
    {
        Guild::SendCommandResult(this, GUILD_INVITE_S, ERR_ALREADY_INVITED_TO_GUILD_S, _player->GetName());
        return;
    }

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);

    stmt->setUInt32(0, GUID_LOPART(petitionguid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    // result == NULL also correct charter without signs
    if (result)
        signs = uint8(result->GetRowCount());

    WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, (8+8+4+signs+signs*12));
    data << uint64(petitionguid);                           // petition guid
    data << uint64(_player->GetGUID());                     // owner guid
    data << uint32(GUID_LOPART(petitionguid));              // guild guid
    data << uint8(signs);                                   // sign's count

    for (uint8 i = 1; i <= signs; ++i)
    {
        Field* fields2 = result->Fetch();
        data << uint64(MAKE_NEW_GUID(fields2[0].GetUInt32(), 0, HIGHGUID_PLAYER)); // Player GUID
        data << uint32(0);                                  // there 0 ...

        result->NextRow();
    }

    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleTurnInPetitionOpcode(WorldPacket & recvData)
{
    // Get petition guid from packet
    uint64 guid;
    recvData >> guid;
    ObjectGuid petitionGuid = guid;

    // Check if player really has the required petition charter
    Item* item = _player->GetItemByGuid(petitionGuid);
    if (!item)
        return;

    // Get petition data from db
    ObjectGuid ownerGuid;
    std::string name;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (result)
    {
        Field* fields = result->Fetch();
        ownerGuid = fields[0].GetUInt32();
        name = fields[1].GetString();
    }
    else
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Player %s (guid: %u) tried to turn in petition (guid: %u) that is not present in the database", _player->GetName(), _player->GetGUIDLow(), GUID_LOPART(petitionGuid));
        return;
    }

    // Only the petition owner can turn in the petition
    if (_player->GetGUIDLow() != ownerGuid)
        return;

    // Check if player is already in a guild
    if (_player->GetGuildId())
    {
        _player->SendPetitionSignResult(petitionGuid, _player, PETITION_TURN_ALREADY_IN_GUILD);
        return;
    }

    // Check if guild name is already taken
    if (sGuildMgr->GetGuildByName(name))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, name);
        return;
    }

    // Get petition signatures from db
    uint8 signatures;

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    result = CharacterDatabase.Query(stmt);

    if (result)
        signatures = uint8(result->GetRowCount());
    else
        signatures = 0;

    uint32 requiredSignatures = sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS);

    // Notify player if signatures are missing
    if (signatures < requiredSignatures)
    {
        _player->SendPetitionTurnInResult(PETITION_TURN_NEED_MORE_SIGNATURES);
        return;
    }

    // Proceed with guild/arena team creation

    // Delete charter item
    _player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

    // Create guild
    Guild* guild = new Guild;

    if (!guild->Create(_player, name))
    {
        delete guild;
        return;
    }

    // Register guild and add guild master
    sGuildMgr->AddGuild(guild);

    // Add members from signatures
    for (uint8 i = 0; i < signatures; ++i)
    {
        Field* fields = result->Fetch();
        guild->AddMember(MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER));
        result->NextRow();
    }

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_BY_GUID);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_SIGNATURE_BY_GUID);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    _player->SendPetitionTurnInResult(PETITION_TURN_OK);
}

void WorldSession::HandlePetitionShowListOpcode(WorldPacket & recvData)
{
    uint64 guid;
    recvData >> guid;

    SendPetitionShowList(guid);
}

void WorldSession::SendPetitionShowList(uint64 guid)
{
    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_PETITIONER);
    if (!creature)
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    WorldPacket data(SMSG_PETITION_SHOWLIST, 8 + 1 + 4 * 6);
    data << guid;                                           // npc guid
    data << uint32(1);                                      // index
    data << uint32(GUILD_CHARTER);                          // charter entry
    data << uint32(CHARTER_DISPLAY_ID);                     // charter display id
    data << uint32(GUILD_CHARTER_COST);                     // charter cost
    data << uint32(0);                                      // unknown
    data << uint32(4);                                      // required signs

    SendPacket(&data);
}

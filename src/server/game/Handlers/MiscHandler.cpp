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
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "Player.h"
#include "GossipDef.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "WorldSession.h"
#include "BigNumber.h"
#include "SHA1.h"
#include "UpdateData.h"
#include "LootMgr.h"
#include "Chat.h"
#include "zlib.h"
#include "ObjectAccessor.h"
#include "Object.h"
#include "Battleground.h"
#include "OutdoorPvP.h"
#include "Pet.h"
#include "SocialMgr.h"
#include "CellImpl.h"
#include "AccountMgr.h"
#include "Vehicle.h"
#include "CreatureAI.h"
#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "MapManager.h"
#include "InstanceScript.h"
#include "GameObjectAI.h"
#include "Group.h"
#include "AccountMgr.h"
#include "Spell.h"
#include "BattlegroundMgr.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "DBCStore.h"

void WorldSession::HandleRepopRequestOpcode(WorldPacket& recvData)
{
    recvData.read_skip<uint8>(); // FromLua ?

    if (GetPlayer()->isAlive() || GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    if (GetPlayer()->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return; // silently return, client should display the error by itself

    // the world update order is sessions, players, creatures
    // the netcode runs in parallel with all of these
    // creatures can kill players
    // so if the server is lagging enough the player can
    // release spirit after he's killed but before he is updated
    if (GetPlayer()->getDeathState() == JUST_DIED)
        GetPlayer()->KillPlayer();

    //this is spirit release confirm?
    GetPlayer()->RemovePet(NULL, PET_SLOT_OTHER_PET, true);
    GetPlayer()->BuildPlayerRepop();
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recvData)
{
    uint32 gossipListId;
    uint32 menuId;
    uint64 guid;
    std::string code = "";

    recvData >> guid >> menuId >> gossipListId;

    if (_player->PlayerTalkClass->IsGossipOptionCoded(gossipListId))
        recvData >> code;

    Creature* unit = NULL;
    GameObject* go = NULL;
    if (IS_CRE_OR_VEH_GUID(guid))
    {
        unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
        if (!unit)
            return;
    }
    else if (IS_GAMEOBJECT_GUID(guid))
    {
        go = _player->GetMap()->GetGameObject(guid);
        if (!go)
            return;
    }
    else
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if ((unit && unit->GetCreatureTemplate()->ScriptID != unit->LastUsedScriptID) || (go && go->GetGOInfo()->ScriptId != go->LastUsedScriptID))
    {
        if (unit)
            unit->LastUsedScriptID = unit->GetCreatureTemplate()->ScriptID;
        if (go)
            go->LastUsedScriptID = go->GetGOInfo()->ScriptId;
        _player->PlayerTalkClass->SendCloseGossip();
        return;
    }
    if (!code.empty())
    {
        if (unit)
        {
            unit->AI()->sGossipSelectCode(_player, menuId, gossipListId, code.c_str());
            if (!sScriptMgr->OnGossipSelectCode(_player, unit, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str()))
                _player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else
        {
            go->AI()->GossipSelectCode(_player, menuId, gossipListId, code.c_str());
            sScriptMgr->OnGossipSelectCode(_player, go, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str());
        }
    }
    else
    {
        if (unit)
        {
            unit->AI()->sGossipSelect(_player, menuId, gossipListId);
            if (!sScriptMgr->OnGossipSelect(_player, unit, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId)))
                _player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else
        {
            go->AI()->GossipSelect(_player, menuId, gossipListId);
            if (!sScriptMgr->OnGossipSelect(_player, go, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId)))
                _player->OnGossipSelect(go, gossipListId, menuId);
        }
    }
}

void WorldSession::HandleWhoOpcode(WorldPacket& recvData)
{
    time_t now = time(NULL);
    if (now - timeLastWhoCommand < 5)
        return;
    else timeLastWhoCommand = now;

    uint32 matchcount = 0;

    uint32 level_min, level_max, racemask, classmask, zones_count, str_count;
    uint32 zoneids[10];                                     // 10 is client limit
    std::string player_name, guild_name;

    recvData >> level_min;                                 // maximal player level, default 0
    recvData >> level_max;                                 // minimal player level, default 100 (MAX_LEVEL)
    recvData >> player_name;                               // player name, case sensitive...

    recvData >> guild_name;                                // guild name, case sensitive...

    recvData >> racemask;                                  // race mask
    recvData >> classmask;                                 // class mask
    recvData >> zones_count;                               // zones count, client limit = 10 (2.0.10)

    if (zones_count > 10)
        return;                                             // can't be received from real client or broken packet

    for (uint32 i = 0; i < zones_count; ++i)
    {
        uint32 temp;
        recvData >> temp;                                  // zone id, 0 if zone is unknown...
        zoneids[i] = temp;
    }

    recvData >> str_count;                                 // user entered strings count, client limit=4 (checked on 2.0.10)

    if (str_count > 4)
        return;                                             // can't be received from real client or broken packet

    std::wstring str[4];                                    // 4 is client limit
    for (uint32 i = 0; i < str_count; ++i)
    {
        std::string temp;
        recvData >> temp;                                  // user entered string, it used as universal search pattern(guild+player name)?

        if (!Utf8toWStr(temp, str[i]))
            continue;

        wstrToLower(str[i]);
    }

    std::wstring wplayer_name;
    std::wstring wguild_name;
    if (!(Utf8toWStr(player_name, wplayer_name) && Utf8toWStr(guild_name, wguild_name)))
        return;
    wstrToLower(wplayer_name);
    wstrToLower(wguild_name);

    // client send in case not set max level value 100 but Trinity supports 255 max level,
    // update it to show GMs with characters after 100 level
    if (level_max >= MAX_LEVEL)
        level_max = STRONG_MAX_LEVEL;

    uint32 team = _player->GetTeam();
    uint32 security = GetSecurity();
    bool allowTwoSideWhoList = sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_WHO_LIST);
    uint32 gmLevelInWhoList  = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);
    uint32 displaycount = 0;

    WorldPacket data(SMSG_WHO, 50);                       // guess size
    data << uint32(matchcount);                           // placeholder, count of players matching criteria
    data << uint32(displaycount);                         // placeholder, count of players displayed

    TRINITY_READ_GUARD(HashMapHolder<Player>::LockType, *HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType const& m = sObjectAccessor->GetPlayers();
    for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (AccountMgr::IsPlayerAccount(security))
        {
            // player can see member of other team only if CONFIG_ALLOW_TWO_SIDE_WHO_LIST
            if (itr->second->GetTeam() != team && !allowTwoSideWhoList)
                continue;

            // player can see MODERATOR, GAME MASTER, ADMINISTRATOR only if CONFIG_GM_IN_WHO_LIST
            if ((itr->second->GetSession()->GetSecurity() > AccountTypes(gmLevelInWhoList)))
                continue;
        }

        //do not process players which are not in world
        if (!(itr->second->IsInWorld()))
            continue;

        // check if target is globally visible for player
        if (!(itr->second->IsVisibleGloballyFor(_player)))
            continue;

        // check if target's level is in level range
        uint8 lvl = itr->second->getLevel();
        if (lvl < level_min || lvl > level_max)
            continue;

        // check if class matches classmask
        uint32 class_ = itr->second->getClass();
        if (!(classmask & (1 << class_)))
            continue;

        // check if race matches racemask
        uint32 race = itr->second->getRace();
        if (!(racemask & (1 << race)))
            continue;

        uint32 pzoneid = itr->second->GetZoneId();
        uint8 gender = itr->second->getGender();

        bool z_show = true;
        for (uint32 i = 0; i < zones_count; ++i)
        {
            if (zoneids[i] == pzoneid)
            {
                z_show = true;
                break;
            }

            z_show = false;
        }
        if (!z_show)
            continue;

        std::string pname = itr->second->GetName();
        std::wstring wpname;
        if (!Utf8toWStr(pname, wpname))
            continue;
        wstrToLower(wpname);

        if (!(wplayer_name.empty() || wpname.find(wplayer_name) != std::wstring::npos))
            continue;

        std::string gname = sGuildMgr->GetGuildNameById(itr->second->GetGuildId());
        std::wstring wgname;
        if (!Utf8toWStr(gname, wgname))
            continue;
        wstrToLower(wgname);

        if (!(wguild_name.empty() || wgname.find(wguild_name) != std::wstring::npos))
            continue;

        std::string aname;
        if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(itr->second->GetZoneId()))
            aname = areaEntry->area_name[GetSessionDbcLocale()];

        bool s_show = true;
        for (uint32 i = 0; i < str_count; ++i)
        {
            if (!str[i].empty())
            {
                if (wgname.find(str[i]) != std::wstring::npos ||
                    wpname.find(str[i]) != std::wstring::npos ||
                    Utf8FitTo(aname, str[i]))
                {
                    s_show = true;
                    break;
                }
                s_show = false;
            }
        }
        if (!s_show)
            continue;

        // 49 is maximum player count sent to client - can be overridden
        // through config, but is unstable
        if ((matchcount++) >= sWorld->getIntConfig(CONFIG_MAX_WHO))
            continue;

        data << pname;                                    // player name
        data << gname;                                    // guild name
        data << uint32(lvl);                              // player level
        data << uint32(class_);                           // player class
        data << uint32(race);                             // player race
        data << uint8(gender);                            // player gender
        data << uint32(pzoneid);                          // player zone id

        ++displaycount;
    }

    uint32 count = 50;
    data.put(0, displaycount);                              // insert right count, count displayed
    data.put( 4, count );        // insert right count, online count

    SendPacket(&data);
}

void WorldSession::HandleLogoutRequestOpcode(WorldPacket& /*recvData*/)
{
    if (uint64 lguid = GetPlayer()->GetLootGUID())
        DoLootRelease(lguid);

    uint32 reason = 0;

    if (GetPlayer()->isInCombat())
        reason = 1;
    else if (GetPlayer()->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR))
        reason = 3;                                         // is jumping or falling
    else if (GetPlayer()->duel || GetPlayer()->HasAura(9454)) // is dueling or frozen by GM via freeze command
        reason = 2;                                         // FIXME - Need the correct value

    if (reason)
    {
        WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
        data << uint32(reason);
        data << uint8(0);
        SendPacket(&data);
        LogoutRequest(0);
        return;
    }

    //instant logout in taverns/cities or on taxi or for admins, gm's, mod's if its enabled in worldserver.conf
    if (GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || GetPlayer()->isInFlight() ||
        GetSecurity() >= AccountTypes(sWorld->getIntConfig(CONFIG_INSTANT_LOGOUT)))
    {
        WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
        data << uint32(reason);
        data << uint8(1);           // instant logout
        SendPacket(&data);
        LogoutPlayer(true);
        return;
    }

    // not set flags if player can't free move to prevent lost state at logout cancel
    if (GetPlayer()->CanFreeMove())
    {
        GetPlayer()->SetStandState(UNIT_STAND_STATE_SIT);
        GetPlayer()->SetRooted(true);
        GetPlayer()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
    data << uint32(0);
    data << uint8(0);
    SendPacket(&data);
    LogoutRequest(time(NULL));
}

void WorldSession::HandlePlayerLogoutOpcode(WorldPacket& /*recvData*/)
{
}

void WorldSession::HandleLogoutCancelOpcode(WorldPacket& /*recvData*/)
{
    // Player have already logged out serverside, too late to cancel
    if (!GetPlayer())
        return;

    LogoutRequest(0);

    WorldPacket data(SMSG_LOGOUT_CANCEL_ACK, 0);
    SendPacket(&data);

    // not remove flags if can't free move - its not set in Logout request code.
    if (GetPlayer()->CanFreeMove())
    {
        //!we can move again
        GetPlayer()->SetRooted(false);

        //! Stand Up
        GetPlayer()->SetStandState(UNIT_STAND_STATE_STAND);

        //! DISABLE_ROTATE
        GetPlayer()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }
}

void WorldSession::HandleTogglePvP(WorldPacket& recvData)
{
    // this opcode can be used in two ways: Either set explicit new status or toggle old status
    if (recvData.size() == 1)
    {
        bool newPvPStatus;
        recvData >> newPvPStatus;
        GetPlayer()->ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP, newPvPStatus);
        GetPlayer()->ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER, !newPvPStatus);
    }
    else
    {
        GetPlayer()->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        GetPlayer()->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER);
    }

    if (GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
    {
        if (!GetPlayer()->IsPvP() || GetPlayer()->pvpInfo.endTimer != 0)
            GetPlayer()->UpdatePvP(true, true);
    }
    else
    {
        if (!GetPlayer()->pvpInfo.inHostileArea && GetPlayer()->IsPvP())
            GetPlayer()->pvpInfo.endTimer = time(NULL);     // start toggle-off
    }

    //if (OutdoorPvP* pvp = _player->GetOutdoorPvP())
    //    pvp->HandlePlayerActivityChanged(_player);
}

void WorldSession::HandleZoneUpdateOpcode(WorldPacket& recvData)
{
    uint32 newZone;
    recvData >> newZone;

    // use server size data
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea);
    GetPlayer()->UpdateZone(newzone, newarea);
    //GetPlayer()->SendInitWorldStates(true, newZone);
}

void WorldSession::HandleReturnToGraveyard(WorldPacket& /*recvPacket*/)
{
    if (GetPlayer()->isAlive() || !GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleSetSelectionOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData >> guid;

    _player->SetSelection(guid);
}

void WorldSession::HandleStandStateChangeOpcode(WorldPacket& recvData)
{
    uint32 animstate;
    recvData >> animstate;

    _player->SetStandState(animstate);
}

void WorldSession::HandleContactListOpcode(WorldPacket& recvData)
{
    recvData.read_skip<uint32>(); // always 1
    _player->GetSocial()->SendSocialList(_player);
}

void WorldSession::HandleAddFriendOpcode(WorldPacket& recvData)
{
    std::string friendName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);
    std::string friendNote;

    recvData >> friendName;

    recvData >> friendNote;

    if (!normalizePlayerName(friendName))
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_RACE_ACC_BY_NAME);

    stmt->setString(0, friendName);

    _addFriendCallback.SetParam(friendNote);
    _addFriendCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));
}

void WorldSession::HandleAddFriendOpcodeCallBack(PreparedQueryResult result, std::string friendNote)
{
    if (!GetPlayer())
        return;

    uint64 friendGuid;
    uint32 friendAccountId;
    uint32 team;
    FriendsResult friendResult;

    friendResult = FRIEND_NOT_FOUND;
    friendGuid = 0;

    if (result)
    {
        Field* fields = result->Fetch();

        friendGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
        team = Player::TeamForRace(fields[1].GetUInt8());
        friendAccountId = fields[2].GetUInt32();

        if (!AccountMgr::IsPlayerAccount(GetSecurity()) || sWorld->getBoolConfig(CONFIG_ALLOW_GM_FRIEND) || AccountMgr::IsPlayerAccount(AccountMgr::GetSecurity(friendAccountId, realmID)))
        {
            if (friendGuid)
            {
                if (friendGuid == GetPlayer()->GetGUID())
                    friendResult = FRIEND_SELF;
                else if (GetPlayer()->GetTeam() != team && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND) && AccountMgr::IsPlayerAccount(GetSecurity()))
                    friendResult = FRIEND_ENEMY;
                else if (GetPlayer()->GetSocial()->HasFriend(GUID_LOPART(friendGuid)))
                    friendResult = FRIEND_ALREADY;
                else
                {
                    Player* pFriend = ObjectAccessor::FindPlayer(friendGuid);
                    if (pFriend && pFriend->IsInWorld() && pFriend->IsVisibleGloballyFor(GetPlayer()))
                        friendResult = FRIEND_ADDED_ONLINE;
                    else
                        friendResult = FRIEND_ADDED_OFFLINE;
                    if (!GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(friendGuid), false))
                    {
                        friendResult = FRIEND_LIST_FULL;
                    }
                }
                GetPlayer()->GetSocial()->SetFriendNote(GUID_LOPART(friendGuid), friendNote);
            }
        }
    }

    sSocialMgr->SendFriendStatus(GetPlayer(), friendResult, GUID_LOPART(friendGuid), false);
}

void WorldSession::HandleDelFriendOpcode(WorldPacket& recvData)
{
    uint64 FriendGUID;

    recvData >> FriendGUID;

    _player->GetSocial()->RemoveFromSocialList(GUID_LOPART(FriendGUID), false);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_REMOVED, GUID_LOPART(FriendGUID), false);
}

void WorldSession::HandleAddIgnoreOpcode(WorldPacket& recvData)
{
    std::string ignoreName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);

    recvData >> ignoreName;

    if (!normalizePlayerName(ignoreName))
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);

    stmt->setString(0, ignoreName);

    _addIgnoreCallback = CharacterDatabase.AsyncQuery(stmt);
}

void WorldSession::HandleAddIgnoreOpcodeCallBack(PreparedQueryResult result)
{
    if (!GetPlayer())
        return;

    uint64 IgnoreGuid;
    FriendsResult ignoreResult;

    ignoreResult = FRIEND_IGNORE_NOT_FOUND;
    IgnoreGuid = 0;

    if (result)
    {
        IgnoreGuid = MAKE_NEW_GUID((*result)[0].GetUInt32(), 0, HIGHGUID_PLAYER);

        if (IgnoreGuid)
        {
            if (IgnoreGuid == GetPlayer()->GetGUID())              //not add yourself
                ignoreResult = FRIEND_IGNORE_SELF;
            else if (GetPlayer()->GetSocial()->HasIgnore(GUID_LOPART(IgnoreGuid)))
                ignoreResult = FRIEND_IGNORE_ALREADY;
            else
            {
                ignoreResult = FRIEND_IGNORE_ADDED;

                // ignore list full
                if (!GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(IgnoreGuid), true))
                    ignoreResult = FRIEND_IGNORE_FULL;
            }
        }
    }

    sSocialMgr->SendFriendStatus(GetPlayer(), ignoreResult, GUID_LOPART(IgnoreGuid), false);
}

void WorldSession::HandleDelIgnoreOpcode(WorldPacket& recvData)
{
    uint64 IgnoreGUID;

    recvData >> IgnoreGUID;

    _player->GetSocial()->RemoveFromSocialList(GUID_LOPART(IgnoreGUID), true);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_IGNORE_REMOVED, GUID_LOPART(IgnoreGUID), false);

}

void WorldSession::HandleSetContactNotesOpcode(WorldPacket& recvData)
{
    uint64 guid;
    std::string note;
    recvData >> guid >> note;
    _player->GetSocial()->SetFriendNote(GUID_LOPART(guid), note);
}

void WorldSession::HandleBugOpcode(WorldPacket& recvData)
{
    uint32 suggestion, contentlen, typelen;
    std::string content, type;

    recvData >> suggestion >> contentlen >> content;

    recvData >> typelen >> type;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BUG_REPORT);

    stmt->setString(0, type);
    stmt->setString(1, content);

    CharacterDatabase.Execute(stmt);
}

void WorldSession::HandleReclaimCorpseOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData >> guid;

    if (GetPlayer()->isAlive())
        return;

    // do not allow corpse reclaim in arena
    if (GetPlayer()->InArena())
        return;

    // body not released yet
    if (!GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    Corpse* corpse = GetPlayer()->GetCorpse();

    if (!corpse)
        return;

    // prevent resurrect before 30-sec delay after body release not finished
    if (time_t(corpse->GetGhostTime() + GetPlayer()->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP)) > time_t(time(NULL)))
        return;

    if (!corpse->IsWithinDistInMap(GetPlayer(), CORPSE_RECLAIM_RADIUS, true))
        return;

    // resurrect
    GetPlayer()->ResurrectPlayer(GetPlayer()->InBattleground() ? 1.0f : 0.5f);

    // spawn bones
    GetPlayer()->SpawnCorpseBones();
}

void WorldSession::HandleResurrectResponseOpcode(WorldPacket& recvData)
{
    uint64 guid;
    uint8 status;
    recvData >> guid;
    recvData >> status;

    if (GetPlayer()->isAlive())
        return;

    if (status == 0)
    {
        GetPlayer()->ClearResurrectRequestData();           // reject
        return;
    }

    if (!GetPlayer()->IsRessurectRequestedBy(guid))
        return;

    GetPlayer()->ResurectUsingRequestData();
}

void WorldSession::SendAreaTriggerMessage(const char* Text, ...)
{
    va_list ap;
    char szStr [1024];
    szStr[0] = '\0';

    va_start(ap, Text);
    vsnprintf(szStr, 1024, Text, ap);
    va_end(ap);

    uint32 length = strlen(szStr)+1;
    WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 4+length);
    data << length;
    data << szStr;
    SendPacket(&data);
}

void WorldSession::HandleAreaTriggerOpcode(WorldPacket& recvData)
{
    uint32 triggerId;
    recvData >> triggerId;

    Player* player = GetPlayer();
    if (player->isInFlight())
        return;

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(triggerId);
    if (!atEntry)
        return;

    if (player->GetMapId() != atEntry->mapid)
        return;

    // delta is safe radius
    const float delta = 5.0f;

    if (atEntry->radius > 0)
    {
        // if we have radius check it
        float dist = player->GetDistance(atEntry->x, atEntry->y, atEntry->z);
        if (dist > atEntry->radius + delta)
            return;
    }
    else
    {
        // we have only extent

        // rotate the players position instead of rotating the whole cube, that way we can make a simplified
        // is-in-cube check and we have to calculate only one point instead of 4

        // 2PI = 360°, keep in mind that ingame orientation is counter-clockwise
        double rotation = 2 * M_PI - atEntry->box_orientation;
        double sinVal = std::sin(rotation);
        double cosVal = std::cos(rotation);

        float playerBoxDistX = player->GetPositionX() - atEntry->x;
        float playerBoxDistY = player->GetPositionY() - atEntry->y;

        float rotPlayerX = float(atEntry->x + playerBoxDistX * cosVal - playerBoxDistY*sinVal);
        float rotPlayerY = float(atEntry->y + playerBoxDistY * cosVal + playerBoxDistX*sinVal);

        // box edges are parallel to coordiante axis, so we can treat every dimension independently :D
        float dz = player->GetPositionZ() - atEntry->z;
        float dx = rotPlayerX - atEntry->x;
        float dy = rotPlayerY - atEntry->y;
        if ((fabs(dx) > atEntry->box_x / 2 + delta) ||
            (fabs(dy) > atEntry->box_y / 2 + delta) ||
            (fabs(dz) > atEntry->box_z / 2 + delta))
            return;
    }

    if (player->isDebugAreaTriggers)
        ChatHandler(player).PSendSysMessage(LANG_DEBUG_AREATRIGGER_REACHED, triggerId);

    if (sScriptMgr->OnAreaTrigger(player, atEntry))
        return;

    if (player->isAlive())
        if (uint32 questId = sObjectMgr->GetQuestForAreaTrigger(triggerId))
            if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(questId);

    if (sObjectMgr->IsTavernAreaTrigger(triggerId))
    {
        // set resting flag we are in the inn
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING);
        player->InnEnter(time(NULL), atEntry->mapid, atEntry->x, atEntry->y, atEntry->z);
        player->SetRestType(REST_TYPE_IN_TAVERN);

        if (sWorld->IsFFAPvPRealm())
            player->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);

        return;
    }

    if (Battleground* bg = player->GetBattleground())
        if (bg->GetStatus() == STATUS_IN_PROGRESS)
        {
            bg->HandleAreaTrigger(player, triggerId);
            return;
        }

    if (OutdoorPvP* pvp = player->GetOutdoorPvP())
        if (pvp->HandleAreaTrigger(_player, triggerId))
            return;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
        return;

    bool teleported = false;
    if (player->GetMapId() != at->target_mapId)
    {
        if (!sMapMgr->CanPlayerEnter(at->target_mapId, player, false))
            return;

        if (Group* group = player->GetGroup())
            if (group->isLFGGroup() && player->GetMap()->IsDungeon())
                teleported = player->TeleportToBGEntryPoint();
    }

    if (!teleported)
        player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
}

void WorldSession::HandleUpdateAccountData(WorldPacket& recvData)
{
    uint32 type, timestamp, decompressedSize;
    recvData >> type >> timestamp >> decompressedSize;

    if (type > NUM_ACCOUNT_DATA_TYPES)
        return;

    if (decompressedSize == 0)                               // erase
    {
        SetAccountData(AccountDataType(type), 0, "");

        WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, 4+4);
        data << uint32(type);
        data << uint32(0);
        SendPacket(&data);

        return;
    }

    if (decompressedSize > 0xFFFF)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        sLog->outError(LOG_FILTER_NETWORKIO, "UAD: Account data packet too big, size %u", decompressedSize);
        return;
    }

    ByteBuffer dest;
    dest.resize(decompressedSize);

    uLongf realSize = decompressedSize;
    if (uncompress(const_cast<uint8*>(dest.contents()), &realSize, const_cast<uint8*>(recvData.contents() + recvData.rpos()), recvData.size() - recvData.rpos()) != Z_OK)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        sLog->outError(LOG_FILTER_NETWORKIO, "UAD: Failed to decompress account data");
        return;
    }

    recvData.rfinish();                       // uncompress read (recvData.size() - recvData.rpos())

    std::string adata;
    dest >> adata;

    SetAccountData(AccountDataType(type), timestamp, adata);

    WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, 4+4);
    data << uint32(type);
    data << uint32(0);
    SendPacket(&data);
}

void WorldSession::HandleRequestAccountData(WorldPacket& recvData)
{
    uint32 type;
    recvData >> type;

    if (type > NUM_ACCOUNT_DATA_TYPES)
        return;

    AccountData* adata = GetAccountData(AccountDataType(type));

    uint32 size = adata->Data.size();

    uLongf destSize = compressBound(size);

    ByteBuffer dest;
    dest.resize(destSize);

    if (size && compress(const_cast<uint8*>(dest.contents()), &destSize, (uint8*)adata->Data.c_str(), size) != Z_OK)
        return;

    dest.resize(destSize);

    WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA, 8+4+4+4+destSize);
    data << uint64(_player ? _player->GetGUID() : 0);       // player guid
    data << uint32(type);                                   // type (0-7)
    data << uint32(adata->Time);                            // unix time
    data << uint32(size);                                   // decompressed length
    data.append(dest);                                      // compressed data
    SendPacket(&data);
}

int32 WorldSession::HandleEnableNagleAlgorithm()
{
    // Instructs the server we wish to receive few amounts of large packets (SMSG_MULTIPLE_PACKETS?)
    // instead of large amount of small packets
    return 0;
}

void WorldSession::HandleSetActionButtonOpcode(WorldPacket& recvData)
{
    uint8 button;
    uint32 packetData;
    recvData >> button >> packetData;

    uint32 action = ACTION_BUTTON_ACTION(packetData);
    uint8  type   = ACTION_BUTTON_TYPE(packetData);

    if (!packetData)
        GetPlayer()->removeActionButton(button);
    else
        GetPlayer()->addActionButton(button, action, type);
}

void WorldSession::HandleCompleteCinematic(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_COMPLETE_CINEMATIC");
}

void WorldSession::HandleNextCinematicCamera(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_NEXT_CINEMATIC_CAMERA");
}

void WorldSession::HandleCompleteMovie(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_COMPLETE_MOVIE");
    _player->ModExtraFlags(PLAYER_EXTRA_WATCHING_MOVIE, false);
    sScriptMgr->OnPlayerEndWatchingMovie(_player);
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint32 time;
    recvData >> time;

    recvData
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[2])

        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5]);

    //TODO!

    /*
        uint64 guid;
        uint32 time_skipped;
        recvData >> guid;
        recvData >> time_skipped;
        sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: CMSG_MOVE_TIME_SKIPPED");

        /// TODO
        must be need use in Trinity
        We substract server Lags to move time (AntiLags)
        for exmaple
        GetPlayer()->ModifyLastMoveTime(-int32(time_skipped));
    */
}

void WorldSession::HandleSetActionBarToggles(WorldPacket& recvData)
{
    uint8 actionBar;

    recvData >> actionBar;

    if (!GetPlayer())                                        // ignore until not logged (check needed because STATUS_AUTHED)
    {
        if (actionBar != 0)
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetActionBarToggles in not logged state with value: %u, ignored", uint32(actionBar));
        return;
    }

    GetPlayer()->SetByteValue(PLAYER_FIELD_BYTES, 2, actionBar);
}

void WorldSession::HandlePlayedTime(WorldPacket& recvData)
{
    uint8 unk1;
    recvData >> unk1;                                      // 0 or 1 expected

    WorldPacket data(SMSG_PLAYED_TIME, 4 + 4 + 1);
    data << uint32(_player->GetTotalPlayedTime());
    data << uint32(_player->GetLevelPlayedTime());
    data << uint8(unk1);                                    // 0 - will not show in chat frame
    SendPacket(&data);
}

void WorldSession::HandleInspectOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData >> guid;

    _player->SetSelection(guid);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
        return;

    uint32 talent_points = 41;
    WorldPacket data(SMSG_INSPECT_TALENT, 8 + 4 + 1 + 1 + talent_points + 8 + 4 + 8 + 4);
    data << player->GetGUID();

    if (sWorld->getBoolConfig(CONFIG_TALENTS_INSPECTING) || _player->isGameMaster())
        player->BuildPlayerTalentsInfoData(&data);
    else
    {
        data << uint32(0);                                  // unspentTalentPoints
        data << uint8(0);                                   // talentGroupCount
        data << uint8(0);                                   // talentGroupIndex
    }

    player->BuildEnchantmentsInfoData(&data);
    if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
    {
        data << uint64(guild->GetGUID());
        data << uint32(guild->GetLevel());
        data << uint64(guild->GetExperience());
        data << uint32(guild->GetMembersCount());
    }
    SendPacket(&data);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    
    recvData
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[6])

        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[3]);

    Player* player = ObjectAccessor::FindPlayer(guid);

    if (!player)
        return;

    ObjectGuid playerGuid = player->GetGUID();
    WorldPacket data(SMSG_INSPECT_HONOR_STATS, 8+1+4+4);
    data.WriteByteMask(playerGuid[4]);
    data.WriteByteMask(playerGuid[3]);
    data.WriteByteMask(playerGuid[6]);
    data.WriteByteMask(playerGuid[2]);
    data.WriteByteMask(playerGuid[5]);
    data.WriteByteMask(playerGuid[0]);
    data.WriteByteMask(playerGuid[7]);
    data.WriteByteMask(playerGuid[1]);
    data << uint8(0);                                               // rank
    data << uint16(player->GetUInt16Value(PLAYER_FIELD_KILLS, 1));  // yesterday kills
    data << uint16(player->GetUInt16Value(PLAYER_FIELD_KILLS, 0));  // today kills
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[0]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(playerGuid[1]);
    data.WriteByteSeq(playerGuid[5]);
    data << uint32(player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS));
    data.WriteByteSeq(playerGuid[7]);
    SendPacket(&data);
}

void WorldSession::HandleWorldTeleportOpcode(WorldPacket& recvData)
{
    uint32 mapid;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Orientation;

    recvData >> mapid;
    recvData >> Orientation;
    recvData >> PositionY;
    recvData >> PositionX;
    recvData >> PositionZ;

    recvData.rfinish(); // Do not read guid data

    if (GetPlayer()->isInFlight())
        return;

    if (AccountMgr::IsAdminAccount(GetSecurity()))
        GetPlayer()->TeleportTo(mapid, PositionX, PositionY, PositionZ, Orientation);
    else
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
}

void WorldSession::HandleWhoisOpcode(WorldPacket& recvData)
{
    std::string charname;
    recvData >> charname;

    if (!AccountMgr::IsAdminAccount(GetSecurity()))
    {
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
        return;
    }

    if (charname.empty() || !normalizePlayerName (charname))
    {
        SendNotification(LANG_NEED_CHARACTER_NAME);
        return;
    }

    Player* player = sObjectAccessor->FindPlayerByName(charname);

    if (!player)
    {
        SendNotification(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, charname.c_str());
        return;
    }

    uint32 accid = player->GetSession()->GetAccountId();

    QueryResult result = LoginDatabase.PQuery("SELECT username, email, last_ip FROM account WHERE id=%u", accid);
    if (!result)
    {
        SendNotification(LANG_ACCOUNT_FOR_PLAYER_NOT_FOUND, charname.c_str());
        return;
    }

    Field* fields = result->Fetch();
    std::string acc = fields[0].GetString();
    if (acc.empty())
        acc = "Unknown";
    std::string email = fields[1].GetString();
    if (email.empty())
        email = "Unknown";
    std::string lastip = fields[2].GetString();
    if (lastip.empty())
        lastip = "Unknown";

    std::string msg = charname + "'s " + "account is " + acc + ", e-mail: " + email + ", last ip: " + lastip;

    WorldPacket data(SMSG_WHOIS, msg.size()+1);
    data << msg;
    SendPacket(&data);
}

void WorldSession::HandleComplainOpcode(WorldPacket& recvData)
{
    uint8 spam_type;                                        // 0 - mail, 1 - chat
    uint64 spammer_guid;
    uint32 unk1 = 0;
    uint32 unk2 = 0;
    uint32 unk3 = 0;
    uint32 unk4 = 0;
    std::string description = "";
    recvData >> spam_type;                                 // unk 0x01 const, may be spam type (mail/chat)
    recvData >> spammer_guid;                              // player guid
    switch (spam_type)
    {
        case 0:
            recvData >> unk1;                              // const 0
            recvData >> unk2;                              // probably mail id
            recvData >> unk3;                              // const 0
            break;
        case 1:
            recvData >> unk1;                              // probably language
            recvData >> unk2;                              // message type?
            recvData >> unk3;                              // probably channel id
            recvData >> unk4;                              // time
            recvData >> description;                       // spam description string (messagetype, channel name, player name, message)
            break;
    }

    // NOTE: all chat messages from this spammer automatically ignored by spam reporter until logout in case chat spam.
    // if it's mail spam - ALL mails from this spammer automatically removed by client

    // Complaint Received message
    WorldPacket data(SMSG_COMPLAIN_RESULT, 2);
    data << uint8(0); // value 1 resets CGChat::m_complaintsSystemStatus in client. (unused?)
    data << uint8(0); // value 0xC generates a "CalendarError" in client.
    SendPacket(&data);
}

void WorldSession::HandleRealmSplitOpcode(WorldPacket& recvData)
{
    uint32 unk;
    std::string split_date = "01/01/01";
    recvData >> unk;

    WorldPacket data(SMSG_REALM_SPLIT, 4+4+split_date.size()+1);
    data << unk;
    data << uint32(0x00000000);                             // realm split state
    // split states:
    // 0x0 realm normal
    // 0x1 realm split
    // 0x2 realm split pending
    data << split_date;
    SendPacket(&data);
    //sLog->outDebug("response sent %u", unk);
}

void WorldSession::HandleFarSightOpcode(WorldPacket& recvData)
{
    uint8 apply;
    recvData >> apply;

    switch (apply)
    {
        case 0:
            _player->SetSeer(_player);
            break;
        case 1:
            if (WorldObject* target = _player->GetViewpoint())
                _player->SetSeer(target);
            else
                sLog->outError(LOG_FILTER_NETWORKIO, "Player %s requests non-existing seer " UI64FMTD, _player->GetName(), _player->GetUInt64Value(PLAYER_FARSIGHT));
            break;
        default:
            return;
    }

    GetPlayer()->UpdateVisibilityForPlayer();
}

void WorldSession::HandleSetTitleOpcode(WorldPacket& recvData)
{
    int32 title;
    recvData >> title;

    // -1 at none
    if (title > 0 && title < MAX_TITLE_INDEX)
    {
       if (!GetPlayer()->HasTitle(title))
            return;
    }
    else
        title = 0;

    GetPlayer()->SetUInt32Value(PLAYER_CHOSEN_TITLE, title);
}

void WorldSession::HandleTimeSyncResp(WorldPacket& recvData)
{
    uint32 counter, clientTicks;
    recvData >> counter >> clientTicks;

    uint32 ourTicks = clientTicks + (getMSTime() - _player->m_timeSyncServer);

    _player->m_timeSyncClient = clientTicks;
}

void WorldSession::HandleResetInstancesOpcode(WorldPacket& /*recvData*/)
{
    if (Group* group = _player->GetGroup())
    {
        if (group->IsLeader(_player->GetGUID()))
            group->ResetInstances(INSTANCE_RESET_ALL, false, _player);
    }
    else
        _player->ResetInstances(INSTANCE_RESET_ALL, false);
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPacket& recvData)
{
    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_DUNGEON_DIFFICULTY)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUIDLow(), mode);
        return;
    }

    if (Difficulty(mode) == _player->GetDungeonDifficulty())
        return;

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: player (Name: %s, GUID: %u) tried to reset the instance while player is inside!", _player->GetName(), _player->GetGUIDLow());
        return;
    }

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* groupGuy = itr->getSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsNonRaidDungeon())
                {
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: player %d tried to reset the instance while group member (Name: %s, GUID: %u) is inside!", _player->GetGUIDLow(), groupGuy->GetName(), groupGuy->GetGUIDLow());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            //_player->SendDungeonDifficulty(true);
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, _player);
            group->SetDungeonDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false);
        _player->SetDungeonDifficulty(Difficulty(mode));
    }
}

void WorldSession::HandleSetRaidDifficultyOpcode(WorldPacket& recvData)
{
    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_RAID_DIFFICULTY)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetRaidDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUIDLow(), mode);
        return;
    }

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUIDLow());
        return;
    }

    if (Difficulty(mode) == _player->GetRaidDifficulty())
        return;

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* groupGuy = itr->getSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsRaid())
                {
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUIDLow());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            //_player->SendDungeonDifficulty(true);
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, _player);
            group->SetRaidDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true);
        _player->SetRaidDifficulty(Difficulty(mode));
    }
}

void WorldSession::HandleCancelMountAuraOpcode(WorldPacket& /*recvData*/)
{
    //If player is not mounted, so go out :)
    if (!_player->IsMounted())                              // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_CHAR_NON_MOUNTED);
        return;
    }

    if (_player->isInFlight())                               // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_YOU_IN_FLIGHT);
        return;
    }

    _player->Dismount();
    _player->RemoveAurasByType(SPELL_AURA_MOUNTED);
}

void WorldSession::HandleRequestPetInfoOpcode(WorldPacket& /*recvData */)
{
    /*
        sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: CMSG_REQUEST_PET_INFO");
        recvData.hexlike();
    */
}

void WorldSession::HandleSetTaxiBenchmarkOpcode(WorldPacket& recvData)
{
    uint8 mode;
    recvData >> mode;
}

void WorldSession::HandleQueryInspectAchievements(WorldPacket& recvData)
{
    uint64 guid;
    recvData.readPackGUID(guid);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
        return;

    player->SendRespondInspectAchievements(_player);
}

void WorldSession::HandleGuildAchievementProgressQuery(WorldPacket& recvData)
{
    uint32 achievementId;
    recvData >> achievementId;

    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
        guild->GetAchievementMgr().SendAchievementInfo(_player, achievementId);
}

void WorldSession::HandleWorldStateUITimerUpdate(WorldPacket& /*recvData*/)
{
    WorldPacket data(SMSG_WORLD_STATE_UI_TIMER_UPDATE, 4);
    data << uint32(time(NULL));
    SendPacket(&data);
}

void WorldSession::HandleReadyForAccountDataTimes(WorldPacket& /*recvData*/)
{
    SendAccountDataTimes(GLOBAL_CACHE_MASK);
}

void WorldSession::SendSetPhaseShift(std::set<uint32> const& phaseIds, std::set<uint32> const& terrainswaps)
{
    ObjectGuid guid = _player->GetGUID();

    WorldPacket data(SMSG_SET_PHASE_SHIFT, 1 + 8 + 4 + 4 + 4 + 4 + 2 * phaseIds.size() + 4 + terrainswaps.size() * 2);
    data.WriteByteMask(guid[2]);
    data.WriteByteMask(guid[3]);
    data.WriteByteMask(guid[1]);
    data.WriteByteMask(guid[6]);
    data.WriteByteMask(guid[4]);
    data.WriteByteMask(guid[5]);
    data.WriteByteMask(guid[0]);
    data.WriteByteMask(guid[7]);

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);

    data << uint32(0);
    //for (uint8 i = 0; i < worldMapAreaCount; ++i)
    //    data << uint16(0);                    // WorldMapArea.dbc id (controls map display)

    data.WriteByteSeq(guid[1]);

    uint32 flag = 8;

    for (std::set<uint32>::const_iterator itr = phaseIds.begin(); itr != phaseIds.end(); ++itr)
        if (PhaseEntry const* phaseEntry = sPhaseStore.LookupEntry(*itr))
             flag |= phaseEntry->Flag;

    data << uint32(flag);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);

    data << uint32(0);                          // Inactive terrain swaps
    //for (uint8 i = 0; i < inactiveSwapsCount; ++i)
    //    data << uint16(0);

    data << uint32(phaseIds.size()) * 2;        // Phase.dbc ids
    for (std::set<uint32>::const_iterator itr = phaseIds.begin(); itr != phaseIds.end(); ++itr)
        data << uint16(*itr);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[0]);

    data << uint32(terrainswaps.size()) * 2;    // Active terrain swaps
    for (std::set<uint32>::const_iterator itr = terrainswaps.begin(); itr != terrainswaps.end(); ++itr)
        data << uint16(*itr);

    data.WriteByteSeq(guid[5]);

    SendPacket(&data);
}

// Battlefield and Battleground
void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& recv_data)
{
    Battleground* bg = _player->GetBattleground();

    uint64 guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    if (bg)
        sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, guid);

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->SendAreaSpiritHealerQueryOpcode(_player,guid);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& recv_data)
{
    Battleground* bg = _player->GetBattleground();

    uint64 guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    if (bg)
        bg->AddPlayerToResurrectQueue(guid, _player->GetGUID());

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->AddPlayerToResurrectQueue(guid, _player->GetGUID());
}

void WorldSession::HandleHearthAndResurrect(WorldPacket& /*recvData*/)
{
    if (_player->isInFlight())
        return;

    if (/*Battlefield* bf = */sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
    {
        // bf->PlayerAskToLeave(_player); FIXME
        return;
    }

    AreaTableEntry const* atEntry = GetAreaEntryByAreaID(_player->GetAreaId());
    if (!atEntry || !(atEntry->flags & AREA_FLAG_WINTERGRASP_2))
        return;

    _player->BuildPlayerRepop();
    _player->ResurrectPlayer(100);
    _player->TeleportTo(_player->m_homebindMapId, _player->m_homebindX, _player->m_homebindY, _player->m_homebindZ, _player->GetOrientation());
}

void WorldSession::HandleInstanceLockResponse(WorldPacket& recvPacket)
{
    uint8 accept;
    recvPacket >> accept;

    if (!_player->HasPendingBind())
        return;

    if (accept)
        _player->BindToInstance();
    else
        _player->RepopAtGraveyard();

    _player->SetPendingBind(0, 0);
}

void WorldSession::HandleRequestHotfix(WorldPacket& recvPacket)
{
    uint32 type, count;
    recvPacket >> type;

    count = recvPacket.ReadBits(23);

    ObjectGuid* guids = new ObjectGuid[count];
    for (uint32 i = 0; i < count; ++i)
    {
        recvPacket
            .ReadByteMask(guids[i][0])
            .ReadByteMask(guids[i][4])
            .ReadByteMask(guids[i][7])
            .ReadByteMask(guids[i][2])
            .ReadByteMask(guids[i][5])
            .ReadByteMask(guids[i][3])
            .ReadByteMask(guids[i][6])
            .ReadByteMask(guids[i][1]);
    }

    uint32 entry;
    for (uint32 i = 0; i < count; ++i)
    {
        recvPacket
            .ReadByteSeq(guids[i][5])
            .ReadByteSeq(guids[i][6])
            .ReadByteSeq(guids[i][7])
            .ReadByteSeq(guids[i][0])
            .ReadByteSeq(guids[i][1])
            .ReadByteSeq(guids[i][3])
            .ReadByteSeq(guids[i][4]);
        recvPacket >> entry;
        recvPacket.ReadByteSeq(guids[i][2]);

        switch (type)
        {
            case DB2_REPLY_ITEM:
                SendItemDb2Reply(entry);
                break;
            case DB2_REPLY_SPARSE:
                SendItemSparseDb2Reply(entry);
                break;
            default:
                sLog->outError(LOG_FILTER_NETWORKIO, "CMSG_REQUEST_HOTFIX: Received unknown hotfix type: %u", type);
                recvPacket.rfinish();
                break;
        }
    }

    delete[] guids;
}

void WorldSession::HandleUpdateMissileTrajectory(WorldPacket& recvPacket)
{
    uint64 guid;
    uint32 spellId;
    float elevation, speed;
    float curX, curY, curZ;
    float targetX, targetY, targetZ;
    uint8 moveStop;

    recvPacket >> guid >> spellId >> elevation >> speed;
    recvPacket >> curX >> curY >> curZ;
    recvPacket >> targetX >> targetY >> targetZ;
    recvPacket >> moveStop;

    Unit* caster = ObjectAccessor::GetUnit(*_player, guid);
    Spell* spell = caster ? caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) : NULL;
    if (!spell || spell->m_spellInfo->Id != spellId || !spell->m_targets.HasDst() || !spell->m_targets.HasSrc())
    {
        recvPacket.rfinish();
        return;
    }

    Position pos = *spell->m_targets.GetSrcPos();
    pos.Relocate(curX, curY, curZ);
    spell->m_targets.ModSrc(pos);

    pos = *spell->m_targets.GetDstPos();
    pos.Relocate(targetX, targetY, targetZ);
    spell->m_targets.ModDst(pos);

    spell->m_targets.SetElevation(elevation);
    spell->m_targets.SetSpeed(speed);

    if (moveStop)
    {
        uint32 opcode;
        recvPacket >> opcode;
        recvPacket.SetOpcode(MSG_MOVE_STOP); // always set to MSG_MOVE_STOP in client SetOpcode
        HandleMovementOpcodes(recvPacket);
    }
}

void WorldSession::HandleViolenceLevel(WorldPacket& recvPacket)
{
    uint8 violenceLevel;
    recvPacket >> violenceLevel;

    // do something?
}

void WorldSession::HandleObjectUpdateFailedOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    recvPacket
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[1])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[2])

        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[7])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[5]);

    Player* player = GetPlayer();
    WorldObject* object = ObjectAccessor::GetWorldObject(*player, guid);

    // workaround to fix visibility objects bug
    if (object && object->IsInWorld() && object->ToPlayer())
        object->SendForcedObjectUpdate();
}

void WorldSession::HandleObjectUpdateRescuedOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    recvPacket
        .ReadByteMask(guid[0])
        .ReadByteMask(guid[5])
        .ReadByteMask(guid[6])
        .ReadByteMask(guid[2])
        .ReadByteMask(guid[4])
        .ReadByteMask(guid[3])
        .ReadByteMask(guid[7])
        .ReadByteMask(guid[1])

        .ReadByteSeq(guid[0])
        .ReadByteSeq(guid[3])
        .ReadByteSeq(guid[6])
        .ReadByteSeq(guid[1])
        .ReadByteSeq(guid[2])
        .ReadByteSeq(guid[5])
        .ReadByteSeq(guid[4])
        .ReadByteSeq(guid[7]);
        
    Player* player = GetPlayer();
    WorldObject* object = ObjectAccessor::GetWorldObject(*player, guid);
    sLog->outError(LOG_FILTER_NETWORKIO, 
        "CMSG_OBJECT_UPDATE_RESCUED: Object Guid=[" UI64FMTD "] Object Name: [%s] Local Player=[%u, %s] "
        "Position=(%f, %f, %f) Map=%u", uint64(guid),
        object ? object->GetName() : "NULL", player->GetGUIDLow(), player->GetName(),
        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
        player->GetMapId());
    }

void WorldSession::HandleSaveCUFProfiles(WorldPacket& recvPacket)
{
    uint8 count = (uint8)recvPacket.ReadBits(20);

    if (count > MAX_CUF_PROFILES)
    {
        sLog->outError(LOG_FILTER_PLAYER, "HandleSaveCUFProfiles - %s tried to save more than %i CUF profiles. Hacking attempt?", GetPlayerName().c_str(), MAX_CUF_PROFILES);
        recvPacket.rfinish();
        return;
    }

    CUFProfile* profiles[MAX_CUF_PROFILES];
    uint8 strlens[MAX_CUF_PROFILES];

    for (uint8 i = 0; i < count; ++i)
    {
        profiles[i] = new CUFProfile;
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_SPEC_2            , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_10_PLAYERS        , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_157                         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_HEAL_PREDICTION         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_SPEC_1            , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_PVP               , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_POWER_BAR               , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_15_PLAYERS        , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_40_PLAYERS        , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_PETS                    , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_5_PLAYERS         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_ONLY_DISPELLABLE_DEBUFFS, recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_2_PLAYERS         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_156                         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_NON_BOSS_DEBUFFS        , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_MAIN_TANK_AND_ASSIST    , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_AGGRO_HIGHLIGHT         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_3_PLAYERS         , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_BORDER                  , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_USE_CLASS_COLORS                , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_145                         , recvPacket.ReadBit());
        strlens[i] = (uint8)recvPacket.ReadBits(8);
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_PVE               , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_HORIZONTAL_GROUPS       , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_25_PLAYERS        , recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_KEEP_GROUPS_TOGETHER            , recvPacket.ReadBit());
    }

    for (uint8 i = 0; i < count; ++i)
    {
        recvPacket >> profiles[i]->Unk146;
        recvPacket.read(profiles[i]->ProfileName, strlens[i]);
        recvPacket >> profiles[i]->Unk152;
        recvPacket >> profiles[i]->FrameHeight;
        recvPacket >> profiles[i]->FrameWidth;
        recvPacket >> profiles[i]->Unk150;
        recvPacket >> profiles[i]->HealthText;
        recvPacket >> profiles[i]->Unk147;
        recvPacket >> profiles[i]->SortBy;
        recvPacket >> profiles[i]->Unk154;
        recvPacket >> profiles[i]->Unk148;

        GetPlayer()->SaveCUFProfile(i, profiles[i]);
    }

    for (uint8 i = count; i < MAX_CUF_PROFILES; ++i)
        GetPlayer()->SaveCUFProfile(i, NULL);
}

void WorldSession::SendLoadCUFProfiles()
{
    Player* player = GetPlayer();

    uint8 count = player->GetCUFProfilesCount();

    ByteBuffer byteBuffer(25 * count);
    WorldPacket data(SMSG_LOAD_CUF_PROFILES, 5 * count + 25 * count);

    data.WriteBits(count, 20);
    for (uint8 i = 0; i < MAX_CUF_PROFILES; ++i)
    {
        CUFProfile* profile = player->GetCUFProfile(i);
        if (!profile)
            continue;

        uint32 nameSize = uint32(profile->ProfileName.size());
        data.WriteBit(profile->BoolOptions[CUF_UNK_157]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_10_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_5_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_25_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_HEAL_PREDICTION]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_PVE]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_HORIZONTAL_GROUPS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_40_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_3_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_AGGRO_HIGHLIGHT]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_BORDER]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_2_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_NON_BOSS_DEBUFFS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_MAIN_TANK_AND_ASSIST]);
        data.WriteBit(profile->BoolOptions[CUF_UNK_156]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_SPEC_2]);
        data.WriteBit(profile->BoolOptions[CUF_USE_CLASS_COLORS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_POWER_BAR]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_SPEC_1]);
        data.WriteBits(nameSize, 8);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_ONLY_DISPELLABLE_DEBUFFS]);
        data.WriteBit(profile->BoolOptions[CUF_KEEP_GROUPS_TOGETHER]);
        data.WriteBit(profile->BoolOptions[CUF_UNK_145]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_15_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_PETS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_PVP]);

        byteBuffer << uint16(profile->Unk154);
        byteBuffer << uint16(profile->FrameHeight);
        byteBuffer << uint16(profile->Unk152);
        byteBuffer << uint8(profile->Unk147);
        byteBuffer << uint16(profile->Unk150);
        byteBuffer << uint8(profile->Unk146);
        byteBuffer << uint8(profile->HealthText);
        byteBuffer << uint8(profile->SortBy);
        byteBuffer << uint16(profile->FrameWidth);
        byteBuffer << uint8(profile->Unk148);
        byteBuffer << WriteBuffer(profile->ProfileName.c_str(), nameSize);
    }

    data.append(byteBuffer);
    SendPacket(&data);
}

void WorldSession::HandleChangeCurrencyFlags(WorldPacket& recvPacket)
{
    uint32 currencyId, flags;
    recvPacket >> flags >> currencyId;

    if (GetPlayer())
        GetPlayer()->ModifyCurrencyFlag(currencyId, uint8(flags));
}
void WorldSession::HandlerCategoryCooldownOpocde(WorldPacket& recvPacket)
{
    Unit::AuraEffectList const& list = GetPlayer()->GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN);

    WorldPacket data(SMSG_SPELL_CATEGORY_COOLDOWN, 4 + (int(list.size()) * 8));
    data.WriteBits<int>(list.size(), 23);
    for (Unit::AuraEffectList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
    {
        AuraEffect* effect = *itr;
        data << uint32(effect->GetMiscValue());
        data << int32(-effect->GetAmount());
    }

    SendPacket(&data);
}

void WorldSession::HandleChangePlayerDifficulty(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Received CMSG_CHANGEPLAYER_DIFFICULTY");

    uint32 difficulty;

    recvData >> difficulty;

    uint32 result = 0;

    switch(result)
    {
    case ERR_DIFFICULTY_CHANGE_COOLDOWN_S:
    case ERR_DIFFICULTY_CHANGE_UPDATE_TIME:
    case ERR_DIFFICULTY_CHANGE_UPDATE_MAP_DIFFICULTY_ENTRY:
        {
            uint32 time = 0;
            uint32 difficultyMapId = 0;
            uint32 cooldownTime = 0;

            WorldPacket data(SMSG_PLAYER_DIFFICULTY_CHANGE, 8);
            data << result;

            if (result == ERR_DIFFICULTY_CHANGE_UPDATE_TIME)
                data << time;
            else if (result == ERR_DIFFICULTY_CHANGE_COOLDOWN_S)
                data << cooldownTime;
            else
                data << difficultyMapId;

            SendPacket(&data);
            break;
        }
    case ERR_DIFFICULTY_CHANGE_OTHER_HEROIC_S:
        {
            uint64 guid = 0;

            WorldPacket data(SMSG_PLAYER_DIFFICULTY_CHANGE);
            data << result;
            data.appendPackGUID(guid); //guid of the player which is locked

            SendPacket(&data);
            break;
        }
    default:
        {
            WorldPacket data(SMSG_PLAYER_DIFFICULTY_CHANGE,4);
            data << result;

            SendPacket(&data);
            break;
        }
    }
}

void WorldSession::HandleSetPreferedCemetery(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Received CMSG_SET_PREFERED_CEMETERY");

    uint32 CemeteryId;
    recvData >> CemeteryId;
}

void WorldSession::HandleCemeteryListRequest(WorldPacket& recvPacket)
{
    GetPlayer()->SendCemeteryList(false);
}

void WorldSession::SendStreamingMovie()
{
    uint8 count = 0;

    WorldPacket data(SMSG_STREAMING_MOVIE, 4 + (2 * count));
    data.WriteBits(count, 25);

    for (uint8 i = 0; i < 0; ++i)
    {
        data << uint16(0);  //File Data ID
    }

    SendPacket(&data);
}

void WorldSession::SendServerWorldInfo()
{
    WorldPacket data(SMSG_WORLD_SERVER_INFO);
    data.WriteBit(IsTrialAccount());                                      //has restriction on level
    data.WriteBit(IsTrialAccount());                                      //has money restriction                   
    data.WriteBit(IsTrialAccount());                                      //is ineligible for loot
    data.ResetBits();
    if(IsTrialAccount()) 
        data << uint32(0);
    data << uint8(sWorld->getBoolConfig(CONFIG_IS_TOURNAMENT_REALM));
    if(IsTrialAccount()) 
        data << uint32(sWorld->getIntConfig(CONFIG_TRIAL_MAX_MONEY));   //max amount of money allowed
    if(IsTrialAccount()) 
        data << uint32(sWorld->getIntConfig(CONFIG_TRIAL_MAX_LEVEL));   //max level allowed
    data << uint32(540882000);                                          //UTC Time + 1 week in seconds
    data << uint32(GetPlayer()->GetMap()->GetDifficulty());             //realm Difficulty
    SendPacket(&data);
}

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

#ifndef BATTLEFIELD_MGR_H_
#define BATTLEFIELD_MGR_H_

#include "Battlefield.h"
#include "ace/Singleton.h"

class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;

class BattlefieldMgr
{
public:
    BattlefieldMgr();
    ~BattlefieldMgr();

    void InitBattlefield();
    void HandlePlayerEnterZone(Player * player, uint32 areaflag);
    void HandlePlayerLeaveZone(Player * player, uint32 areaflag);
    void HandlePlayerResurrects(Player * player, uint32 areaflag);
    Battlefield* GetBattlefieldToZoneId(uint32 zoneid);
    Battlefield* GetBattlefieldByBattleId(uint32 battleid);
    Battlefield* GetBattlefieldByGUID(uint64 guid);
    ZoneScript* GetZoneScript(uint32 zoneId);
    void AddZone(uint32 zoneid, Battlefield * handle);
    void Update(uint32 diff);
    void HandleGossipOption(Player * player, uint64 guid, uint32 gossipid);
    bool CanTalkTo(Player * player, Creature * creature, GossipMenuItems gso);
    void HandleDropFlag(Player * player, uint32 spellId);
    typedef std::vector < Battlefield * >BattlefieldSet;
    typedef std::map < uint32 /* zoneid */ , Battlefield* >BattlefieldMap;
private:
    BattlefieldSet m_BattlefieldSet;
    BattlefieldMap m_BattlefieldMap;
    uint32 m_UpdateTimer;
};

#define sBattlefieldMgr ACE_Singleton<BattlefieldMgr, ACE_Null_Mutex>::instance()

#endif

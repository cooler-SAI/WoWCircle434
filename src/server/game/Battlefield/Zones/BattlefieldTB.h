/*
* Copyright (C) 2008-2012 Trinity Core <http://www.trinitycore.org/>
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

#ifndef BATTLEFIELD_TB_
#define BATTLEFIELD_TB_

#include "ObjectAccessor.h"
#include "WorldPacket.h"
#include "World.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Battlefield.h"

//const uint32 TBClockWorldState[2] = {TB_WS_TIME_TO_END, TB_WS_TIME_TO_NEXT_BATTLE};
const uint32 TolBaradFaction[3] = {2354, 2355, 35};

class BattlefieldTB;

enum eTBpell
{
    SPELL_SPIRITUAL_IMMUNITY_TB                     = 95332,
    SPELL_ALLIANCE_FLAG_TB                          = 14268,
    SPELL_HORDE_FLAG_TB                             = 14267,

    // Reward spells
    SPELL_VICTORY_REWARD_ALLIANCE_TB                = 89789,
    SPELL_VICTORY_REWARD_HORDE_TB                   = 89791,
    SPELL_DEFEAT_REWARD_TB                          = 89793,
    SPELL_DAMAGED_TOWER_TB                          = 89795,
    SPELL_DESTROYED_TOWER_TB                        = 89796,
    SPELL_DEFENDED_TOWER_TB                         = 89794,

    SPELL_TOWER_BONUS                               = 82629,
    SPELL_VETERAN                                   = 84655,
    SPELL_SLOW_FALL_TB                              = 88473,

    SPELL_TOL_BARAD_VICTORY_ALLIANCE                = 89789,
    SPELL_TOL_BARAD_VICTORY_HORDE                   = 89791,
    SPELL_TOL_BARAD_DEFEAT                          = 89793,
    SPELL_TOL_BARAD_TOWER_DEFENDED                  = 89794,
    SPELL_TOL_BARAD_TOWER_DAMAGED                   = 89795,
    SPELL_TOL_BARAD_TOWER_DESTROYED                 = 89796,

    SPELL_TOL_BARAD_QUEST_ALLIANCE                  = 94665,
    SPELL_TOL_BARAD_QUEST_HORDE                     = 94763,
};

class BattlefieldTB: public Battlefield
{
public:
    bool SetupBattlefield();

protected:
};

enum TBWorldStates
{
    TB_WS_TIME_TO_NEXT_BATTLE                   = 5332,
    TB_WS_TIME_TO_END                           = 5333,
    TB_WS_TOWERS_DESTROYED                      = 5347, // ?
    TB_WS_BUILDINGS_CAPTURED                    = 5348, // ?

    TB_WS_HORDE_CONTROLS_SHOW                   = 5384,
    TB_WS_ALLIANCE_CONTROLS_SHOW                = 5385,
    TB_WS_ALLIANCE_ATTACKING_SHOW               = 5546,
    TB_WS_HORDE_ATTACKING_SHOW                  = 5547,

    TB_WS_NEXT_BATTLE_TIMER_SHOW                = 5387,
    TB_WS_TOWERS_DESTROYED_SHOW                 = 5350, // ?
    TB_WS_BUILDINGS_CAPTURED_SHOW               = 5349,
    TB_WS_TIME_REMAINING_SHOW                   = 5346,

    TB_WS_GARRISON_HORDE_CONTROLLED             = 5418, // ?
    TB_WS_GARRISON_HORDE_CAPTURING              = 5419, // ?
    TB_WS_GARRISON_ALLIANCE_CAPTURING           = 5421, // ?
    TB_WS_GARRISON_ALLIANCE_CONTROLLED          = 5422, // ?

    TB_WS_VIGIL_HORDE_CONTROLLED                = 5423, // ?
    TB_WS_VIGIL_HORDE_CAPTURING                 = 5424, // ?
    TB_WS_VIGIL_ALLIANCE_CAPTURING              = 5426, // ?
    TB_WS_VIGIL_ALLIANCE_CONTROLLED             = 5427, // ?

    TB_WS_SLAGWORKS_HORDE_CONTROLLED            = 5428, // ?
    TB_WS_SLAGWORKS_HORDE_CAPTURING             = 5429, // ?
    TB_WS_SLAGWORKS_ALLIANCE_CAPTURING          = 5431, // ?
    TB_WS_SLAGWORKS_ALLIANCE_CONTROLLED         = 5432, // ?

    TB_WS_WEST_INTACT_HORDE                     = 5433, // ?
    TB_WS_WEST_DAMAGED_HORDE                    = 5434, // ?
    TB_WS_WEST_DESTROYED_NEUTRAL                = 5435, // ?
    TB_WS_WEST_INTACT_ALLIANCE                  = 5436, // ?
    TB_WS_WEST_DAMAGED_ALLIANCE                 = 5437, // ?
    TB_WS_WEST_INTACT_NEUTRAL                   = 5453, // ?
    TB_WS_WEST_DAMAGED_NEUTRAL                  = 5454, // ?

    TB_WS_SOUTH_INTACT_HORDE                    = 5438, // ?
    TB_WS_SOUTH_DAMAGED_HORDE                   = 5439, // ?
    TB_WS_SOUTH_DESTROYED_NEUTRAL               = 5440, // ?
    TB_WS_SOUTH_INTACT_ALLIANCE                 = 5441, // ?
    TB_WS_SOUTH_DAMAGED_ALLIANCE                = 5442, // ?
    TB_WS_SOUTH_INTACT_NEUTRAL                  = 5455, // ?
    TB_WS_SOUTH_DAMAGED_NEUTRAL                 = 5456, // ?

    TB_WS_EAST_INTACT_HORDE                     = 5443, // ?
    TB_WS_EAST_DAMAGED_HORDE                    = 5444, // ?
    TB_WS_EAST_DESTROYED_NEUTRAL                = 5445, // ?
    TB_WS_EAST_INTACT_ALLIANCE                  = 5446, // ?
    TB_WS_EAST_DAMAGED_ALLIANCE                 = 5447, // ?
    TB_WS_EAST_INTACT_NEUTRAL                   = 5451, // ?
    TB_WS_EAST_DAMAGED_NEUTRAL                  = 5452, // ?
};

#endif

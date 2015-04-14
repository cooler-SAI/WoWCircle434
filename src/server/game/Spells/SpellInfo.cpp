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


#include "SpellAuraDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Spell.h"
#include "DBCStores.h"
#include "ConditionMgr.h"
#include "Common.h"
#include "Vehicle.h"
#include "InstanceScript.h"

uint32 GetTargetFlagMask(SpellTargetObjectTypes objType)
{
    switch (objType)
    {
        case TARGET_OBJECT_TYPE_DEST:
            return TARGET_FLAG_DEST_LOCATION;
        case TARGET_OBJECT_TYPE_UNIT_AND_DEST:
            return TARGET_FLAG_DEST_LOCATION | TARGET_FLAG_UNIT;
        case TARGET_OBJECT_TYPE_CORPSE_ALLY:
            return TARGET_FLAG_CORPSE_ALLY;
        case TARGET_OBJECT_TYPE_CORPSE_ENEMY:
            return TARGET_FLAG_CORPSE_ENEMY;
        case TARGET_OBJECT_TYPE_CORPSE:
            return TARGET_FLAG_CORPSE_ALLY | TARGET_FLAG_CORPSE_ENEMY;
        case TARGET_OBJECT_TYPE_UNIT:
            return TARGET_FLAG_UNIT;
        case TARGET_OBJECT_TYPE_GOBJ:
            return TARGET_FLAG_GAMEOBJECT;
        case TARGET_OBJECT_TYPE_GOBJ_ITEM:
            return TARGET_FLAG_GAMEOBJECT_ITEM;
        case TARGET_OBJECT_TYPE_ITEM:
            return TARGET_FLAG_ITEM;
        case TARGET_OBJECT_TYPE_SRC:
            return TARGET_FLAG_SOURCE_LOCATION;
        default:
            return TARGET_FLAG_NONE;
    }
}

SpellImplicitTargetInfo::SpellImplicitTargetInfo(uint32 target)
{
    _target = Targets(target);
}

bool SpellImplicitTargetInfo::IsArea() const
{
    return GetSelectionCategory() == TARGET_SELECT_CATEGORY_AREA || GetSelectionCategory() == TARGET_SELECT_CATEGORY_CONE;
}

SpellTargetSelectionCategories SpellImplicitTargetInfo::GetSelectionCategory() const
{
    return _data[_target].SelectionCategory;
}

SpellTargetReferenceTypes SpellImplicitTargetInfo::GetReferenceType() const
{
    return _data[_target].ReferenceType;
}

SpellTargetObjectTypes SpellImplicitTargetInfo::GetObjectType() const
{
    return _data[_target].ObjectType;
}

SpellTargetCheckTypes SpellImplicitTargetInfo::GetCheckType() const
{
    return _data[_target].SelectionCheckType;
}

SpellTargetDirectionTypes SpellImplicitTargetInfo::GetDirectionType() const
{
    return _data[_target].DirectionType;
}

float SpellImplicitTargetInfo::CalcDirectionAngle() const
{
    switch (GetDirectionType())
    {
        case TARGET_DIR_FRONT:
            return 0.0f;
        case TARGET_DIR_BACK:
            return static_cast<float>(M_PI);
        case TARGET_DIR_RIGHT:
            return static_cast<float>(-M_PI/2);
        case TARGET_DIR_LEFT:
            return static_cast<float>(M_PI/2);
        case TARGET_DIR_FRONT_RIGHT:
            return static_cast<float>(-M_PI/4);
        case TARGET_DIR_BACK_RIGHT:
            return static_cast<float>(-3*M_PI/4);
        case TARGET_DIR_BACK_LEFT:
            return static_cast<float>(3*M_PI/4);
        case TARGET_DIR_FRONT_LEFT:
            return static_cast<float>(M_PI/4);
        case TARGET_DIR_RANDOM:
            return float(rand_norm())*static_cast<float>(2*M_PI);
        default:
            return 0.0f;
    }
}

Targets SpellImplicitTargetInfo::GetTarget() const
{
    return _target;
}

uint32 SpellImplicitTargetInfo::GetExplicitTargetMask(bool& srcSet, bool& dstSet) const
{
    uint32 targetMask = 0;
    if (GetTarget() == TARGET_DEST_TRAJ)
    {
        if (!srcSet)
            targetMask = TARGET_FLAG_SOURCE_LOCATION;
        if (!dstSet)
            targetMask |= TARGET_FLAG_DEST_LOCATION;
    }
    else
    {
        switch (GetReferenceType())
        {
            case TARGET_REFERENCE_TYPE_SRC:
                if (srcSet)
                    break;
                targetMask = TARGET_FLAG_SOURCE_LOCATION;
                break;
            case TARGET_REFERENCE_TYPE_DEST:
                if (dstSet)
                    break;
                targetMask = TARGET_FLAG_DEST_LOCATION;
                break;
            case TARGET_REFERENCE_TYPE_TARGET:
                switch (GetObjectType())
                {
                    case TARGET_OBJECT_TYPE_GOBJ:
                        targetMask = TARGET_FLAG_GAMEOBJECT;
                        break;
                    case TARGET_OBJECT_TYPE_GOBJ_ITEM:
                        targetMask = TARGET_FLAG_GAMEOBJECT_ITEM;
                        break;
                    case TARGET_OBJECT_TYPE_UNIT_AND_DEST:
                    case TARGET_OBJECT_TYPE_UNIT:
                    case TARGET_OBJECT_TYPE_DEST:
                        switch (GetCheckType())
                        {
                            case TARGET_CHECK_ENEMY:
                                targetMask = TARGET_FLAG_UNIT_ENEMY;
                                break;
                            case TARGET_CHECK_ALLY:
                                targetMask = TARGET_FLAG_UNIT_ALLY;
                                break;
                            case TARGET_CHECK_PARTY:
                                targetMask = TARGET_FLAG_UNIT_PARTY;
                                break;
                            case TARGET_CHECK_RAID:
                                targetMask = TARGET_FLAG_UNIT_RAID;
                                break;
                            case TARGET_CHECK_PASSENGER:
                                targetMask = TARGET_FLAG_UNIT_PASSENGER;
                                break;
                            case TARGET_CHECK_RAID_CLASS:
                                // nobreak;
                            default:
                                targetMask = TARGET_FLAG_UNIT;
                                break;
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    switch (GetObjectType())
    {
        case TARGET_OBJECT_TYPE_SRC:
            srcSet = true;
            break;
        case TARGET_OBJECT_TYPE_DEST:
        case TARGET_OBJECT_TYPE_UNIT_AND_DEST:
            dstSet = true;
            break;
        default:
            break;
    }
    return targetMask;
}

SpellImplicitTargetInfo::StaticData  SpellImplicitTargetInfo::_data[TOTAL_SPELL_TARGETS] =
{
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        //
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 1 TARGET_UNIT_CASTER
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 2 TARGET_UNIT_NEARBY_ENEMY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 3 TARGET_UNIT_NEARBY_PARTY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 4 TARGET_UNIT_NEARBY_ALLY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 5 TARGET_UNIT_PET
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 6 TARGET_UNIT_TARGET_ENEMY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ENTRY,    TARGET_DIR_NONE},        // 7 TARGET_UNIT_SRC_AREA_ENTRY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ENTRY,    TARGET_DIR_NONE},        // 8 TARGET_UNIT_DEST_AREA_ENTRY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 9 TARGET_DEST_HOME
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 10
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 11 TARGET_UNIT_SRC_AREA_UNK_11
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 12
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 13
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 14
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 15 TARGET_UNIT_SRC_AREA_ENEMY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 16 TARGET_UNIT_DEST_AREA_ENEMY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 17 TARGET_DEST_DB
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 18 TARGET_DEST_CASTER
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 19
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 20 TARGET_UNIT_CASTER_AREA_PARTY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 21 TARGET_UNIT_TARGET_ALLY
    {TARGET_OBJECT_TYPE_SRC,  TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 22 TARGET_SRC_CASTER
    {TARGET_OBJECT_TYPE_GOBJ, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 23 TARGET_GAMEOBJECT_TARGET
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_ENEMY,    TARGET_DIR_FRONT},       // 24 TARGET_UNIT_CONE_ENEMY_24
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 25 TARGET_UNIT_TARGET_ANY
    {TARGET_OBJECT_TYPE_GOBJ_ITEM, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT, TARGET_DIR_NONE},        // 26 TARGET_GAMEOBJECT_ITEM_TARGET
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 27 TARGET_UNIT_MASTER
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 28 TARGET_DEST_DYNOBJ_ENEMY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 29 TARGET_DEST_DYNOBJ_ALLY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 30 TARGET_UNIT_SRC_AREA_ALLY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 31 TARGET_UNIT_DEST_AREA_ALLY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_LEFT},  // 32 TARGET_DEST_CASTER_SUMMON
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 33 TARGET_UNIT_SRC_AREA_PARTY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 34 TARGET_UNIT_DEST_AREA_PARTY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 35 TARGET_UNIT_TARGET_PARTY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 36 TARGET_DEST_CASTER_UNK_36
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_LAST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_PARTY,    TARGET_DIR_NONE},        // 37 TARGET_UNIT_LASTTARGET_AREA_PARTY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_ENTRY,    TARGET_DIR_NONE},        // 38 TARGET_UNIT_NEARBY_ENTRY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 39 TARGET_DEST_CASTER_FISHING
    {TARGET_OBJECT_TYPE_GOBJ, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_ENTRY,    TARGET_DIR_NONE},        // 40 TARGET_GAMEOBJECT_NEARBY_ENTRY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_RIGHT}, // 41 TARGET_DEST_CASTER_FRONT_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_RIGHT},  // 42 TARGET_DEST_CASTER_BACK_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_LEFT},   // 43 TARGET_DEST_CASTER_BACK_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_LEFT},  // 44 TARGET_DEST_CASTER_FRONT_LEFT
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ALLY,     TARGET_DIR_NONE},        // 45 TARGET_UNIT_TARGET_CHAINHEAL_ALLY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_ENTRY,    TARGET_DIR_NONE},        // 46 TARGET_DEST_NEARBY_ENTRY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT},       // 47 TARGET_DEST_CASTER_FRONT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK},        // 48 TARGET_DEST_CASTER_BACK
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RIGHT},       // 49 TARGET_DEST_CASTER_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_LEFT},        // 50 TARGET_DEST_CASTER_LEFT
    {TARGET_OBJECT_TYPE_GOBJ, TARGET_REFERENCE_TYPE_SRC,    TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 51 TARGET_GAMEOBJECT_SRC_AREA
    {TARGET_OBJECT_TYPE_GOBJ, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 52 TARGET_GAMEOBJECT_DEST_AREA
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 53 TARGET_DEST_TARGET_ENEMY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_ENEMY,    TARGET_DIR_FRONT},       // 54 TARGET_UNIT_CONE_ENEMY_54
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 55 TARGET_DEST_CASTER_FRONT_LEAP
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_RAID,     TARGET_DIR_NONE},        // 56 TARGET_UNIT_CASTER_AREA_RAID
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_RAID,     TARGET_DIR_NONE},        // 57 TARGET_UNIT_TARGET_RAID
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_NEARBY,  TARGET_CHECK_RAID,     TARGET_DIR_NONE},        // 58 TARGET_UNIT_NEARBY_RAID
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_RAID,     TARGET_DIR_FRONT},       // 59 TARGET_UNIT_CONE_ALLY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_ENTRY,    TARGET_DIR_FRONT},       // 60 TARGET_UNIT_CONE_ENTRY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_RAID_CLASS,TARGET_DIR_NONE},       // 61 TARGET_UNIT_TARGET_AREA_RAID_CLASS
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 62 TARGET_UNK_62
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 63 TARGET_DEST_TARGET_ANY
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT},       // 64 TARGET_DEST_TARGET_FRONT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK},        // 65 TARGET_DEST_TARGET_BACK
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RIGHT},       // 66 TARGET_DEST_TARGET_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_LEFT},        // 67 TARGET_DEST_TARGET_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_RIGHT}, // 68 TARGET_DEST_TARGET_FRONT_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_RIGHT},  // 69 TARGET_DEST_TARGET_BACK_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_LEFT},   // 70 TARGET_DEST_TARGET_BACK_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_LEFT},  // 71 TARGET_DEST_TARGET_FRONT_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 72 TARGET_DEST_CASTER_RANDOM
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 73 TARGET_DEST_CASTER_RADIUS
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 74 TARGET_DEST_TARGET_RANDOM
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 75 TARGET_DEST_TARGET_RADIUS
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CHANNEL, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 76 TARGET_DEST_CHANNEL_TARGET
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CHANNEL, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 77 TARGET_UNIT_CHANNEL_TARGET
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT},       // 78 TARGET_DEST_DEST_FRONT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK},        // 79 TARGET_DEST_DEST_BACK
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RIGHT},       // 80 TARGET_DEST_DEST_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_LEFT},        // 81 TARGET_DEST_DEST_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_RIGHT}, // 82 TARGET_DEST_DEST_FRONT_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_RIGHT},  // 83 TARGET_DEST_DEST_BACK_RIGHT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_BACK_LEFT},   // 84 TARGET_DEST_DEST_BACK_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT_LEFT},  // 85 TARGET_DEST_DEST_FRONT_LEFT
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 86 TARGET_DEST_DEST_RANDOM
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 87 TARGET_DEST_DEST
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 88 TARGET_DEST_DYNOBJ_NONE
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 89 TARGET_DEST_TRAJ
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 90 TARGET_UNIT_TARGET_MINIPET
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_RANDOM},      // 91 TARGET_DEST_DEST_RADIUS
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 92 TARGET_UNIT_SUMMONER
    {TARGET_OBJECT_TYPE_CORPSE, TARGET_REFERENCE_TYPE_SRC,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_ENEMY,    TARGET_DIR_NONE},        // 93 TARGET_CORPSE_SRC_AREA_ENEMY
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 94 TARGET_UNIT_VEHICLE
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_PASSENGER, TARGET_DIR_NONE},        // 95 TARGET_UNIT_TARGET_PASSENGER
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 96 TARGET_UNIT_PASSENGER_0
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 97 TARGET_UNIT_PASSENGER_1
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 98 TARGET_UNIT_PASSENGER_2
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 99 TARGET_UNIT_PASSENGER_3
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 100 TARGET_UNIT_PASSENGER_4
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 101 TARGET_UNIT_PASSENGER_5
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 102 TARGET_UNIT_PASSENGER_6
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 103 TARGET_UNIT_PASSENGER_7
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_ENEMY,    TARGET_DIR_FRONT},       // 104 TARGET_UNIT_CONE_ENEMY_104
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 105 TARGET_UNIT_UNK_105
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CHANNEL, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 106 TARGET_DEST_CHANNEL_CASTER
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_DEST,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 107 TARGET_UNK_DEST_AREA_UNK_107
    {TARGET_OBJECT_TYPE_GOBJ, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT},       // 108 TARGET_GAMEOBJECT_CONE
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 109
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_CONE,    TARGET_CHECK_DEFAULT,  TARGET_DIR_FRONT},       // 110 TARGET_DEST_UNK_110
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 111
    {TARGET_OBJECT_TYPE_DEST, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 112
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 113
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 114
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 115
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_CASTER, TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_RAID,     TARGET_DIR_NONE},        // 116 TARGET_RAID_MEMBERS
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 117
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 118
    {TARGET_OBJECT_TYPE_UNIT, TARGET_REFERENCE_TYPE_TARGET, TARGET_SELECT_CATEGORY_AREA,    TARGET_CHECK_RAID,     TARGET_DIR_NONE},        // 119
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 120
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 121
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 122
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 123
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 124
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 125
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 126
    {TARGET_OBJECT_TYPE_NONE, TARGET_REFERENCE_TYPE_NONE,   TARGET_SELECT_CATEGORY_NYI,     TARGET_CHECK_DEFAULT,  TARGET_DIR_NONE},        // 127
};

SpellEffectInfo::SpellEffectInfo(SpellEntry const* /*spellEntry*/, SpellInfo const* spellInfo, uint8 effIndex, SpellEffectEntry const* _effect)
{
    SpellScalingEntry const* scaling = spellInfo->GetSpellScaling();

    _spellInfo = spellInfo;
    _effIndex = _effect ? _effect->EffectIndex : effIndex;
    Effect = _effect ? _effect->Effect : 0;
    ApplyAuraName = _effect ? _effect->EffectApplyAuraName : 0;
    Amplitude = _effect ? _effect->EffectAmplitude : 0;
    DieSides = _effect ? _effect->EffectDieSides : 0;
    RealPointsPerLevel = _effect ? _effect->EffectRealPointsPerLevel : 0.0f;
    BasePoints = _effect ? _effect->EffectBasePoints : 0;
    PointsPerComboPoint = _effect ? _effect->EffectPointsPerComboPoint : 0.0f;
    ValueMultiplier = _effect ? _effect->EffectValueMultiplier : 0.0f;
    DamageMultiplier = _effect ? _effect->EffectDamageMultiplier : 0.0f;
    BonusMultiplier = _effect ? _effect->EffectBonusMultiplier : 0.0f;
    MiscValue = _effect ? _effect->EffectMiscValue : 0;
    MiscValueB = _effect ? _effect->EffectMiscValueB : 0;
    Mechanic = Mechanics(_effect ? _effect->EffectMechanic : 0);
    TargetA = SpellImplicitTargetInfo(_effect ? _effect->EffectImplicitTargetA : 0);
    TargetB = SpellImplicitTargetInfo(_effect ? _effect->EffectImplicitTargetB : 0);
    RadiusEntry = _effect && _effect->EffectRadiusIndex ? sSpellRadiusStore.LookupEntry(_effect->EffectRadiusIndex) : NULL;
    MaxRadiusEntry = _effect && _effect->EffectRadiusMaxIndex ? sSpellRadiusStore.LookupEntry(_effect->EffectRadiusMaxIndex) : NULL;
    ChainTarget = _effect ? _effect->EffectChainTarget : 0;
    ItemType = _effect ? _effect->EffectItemType : 0;
    TriggerSpell = _effect ? _effect->EffectTriggerSpell : 0;
    SpellClassMask = _effect ? _effect->EffectSpellClassMask : flag96(0);
    ImplicitTargetConditions = NULL;
    ScalingMultiplier = scaling ? scaling->Multiplier[_effIndex] : 0.0f;
    DeltaScalingMultiplier = scaling ? scaling->RandomMultiplier[_effIndex] : 0.0f;
    ComboScalingMultiplier = scaling ? scaling->OtherMultiplier[_effIndex] : 0.0f;
}

bool SpellEffectInfo::IsEffect() const
{
    return Effect != 0;
}

bool SpellEffectInfo::IsEffect(SpellEffects effectName) const
{
    return Effect == uint32(effectName);
}

bool SpellEffectInfo::IsAura() const
{
    return (IsUnitOwnedAuraEffect() || Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA) && ApplyAuraName != 0;
}

bool SpellEffectInfo::IsPersistenAura() const
{
    return Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA && ApplyAuraName != 0;
}

bool SpellEffectInfo::IsAura(AuraType aura) const
{
    return IsAura() && AuraType(ApplyAuraName) == uint32(aura);
}

bool SpellEffectInfo::IsTargetingArea() const
{
    return TargetA.IsArea() || TargetB.IsArea();
}

bool SpellEffectInfo::IsAreaAuraEffect() const
{
    if (Effect == SPELL_EFFECT_APPLY_AREA_AURA_PARTY    ||
        Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID     ||
        Effect == SPELL_EFFECT_APPLY_AREA_AURA_FRIEND   ||
        Effect == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY    ||
        Effect == SPELL_EFFECT_APPLY_AREA_AURA_PET      ||
        Effect == SPELL_EFFECT_APPLY_AREA_AURA_OWNER)
        return true;
    return false;
}

bool SpellEffectInfo::IsAreaAuraRaidEffect() const
{
    if (Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID)
        return true;
    return false;
}

bool SpellEffectInfo::IsFarUnitTargetEffect() const
{
    return (Effect == SPELL_EFFECT_SUMMON_PLAYER)
        || (Effect == SPELL_EFFECT_SUMMON_RAF_FRIEND)
        || (Effect == SPELL_EFFECT_RESURRECT)
        || (Effect == SPELL_EFFECT_RESURRECT_NEW)
        || (Effect == SPELL_EFFECT_SKIN_PLAYER_CORPSE);
}

bool SpellEffectInfo::IsFarDestTargetEffect() const
{
    return Effect == SPELL_EFFECT_TELEPORT_UNITS;
}

bool SpellEffectInfo::IsUnitOwnedAuraEffect() const
{
    return IsAreaAuraEffect() || Effect == SPELL_EFFECT_APPLY_AURA;
}

bool SpellEffectInfo::IsPeriodicEffect() const
{
    if (!ApplyAuraName)
        return false;

    switch (ApplyAuraName)
    {
        case SPELL_AURA_PERIODIC_DUMMY:
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
        case SPELL_AURA_PERIODIC_MANA_LEECH:
        case SPELL_AURA_OBS_MOD_POWER:
        case SPELL_AURA_PERIODIC_ENERGIZE:
        case SPELL_AURA_POWER_BURN:
            return true;
    }

    return false;
}

bool SpellEffectInfo::IsRessurrectionEffect() const
{
    switch (Effect)
    {
        case SPELL_EFFECT_RESURRECT:
        case SPELL_EFFECT_RESURRECT_WITH_AURA:
            return true;
    }

    return false;
}

int32 SpellEffectInfo::CalcValue(Unit const* caster, int32 const* bp, Unit const* target) const
{
    float basePointsPerLevel = RealPointsPerLevel;
    int32 basePoints = bp ? *bp : BasePoints;
    float comboDamage = PointsPerComboPoint;

    // base amount modification based on spell lvl vs caster lvl
    if (ScalingMultiplier != 0.0f)
    {
        if (caster)
        {
            int32 level = caster->getLevel();
            if (target && _spellInfo->IsPositiveEffect(_effIndex) && (Effect == SPELL_EFFECT_APPLY_AURA))
	if (target)
                level = target->getLevel();

            if (GtSpellScalingEntry const* gtScaling = sGtSpellScalingStore.LookupEntry((_spellInfo->ScalingClass != -1 ? _spellInfo->ScalingClass - 1 : MAX_CLASSES - 1) * 100 + level - 1))
            {
                float multiplier = gtScaling->value;
                if (_spellInfo->CastTimeMax > 0 && _spellInfo->CastTimeMaxLevel > level)
                    multiplier *= float(_spellInfo->CastTimeMin + (level - 1) * (_spellInfo->CastTimeMax - _spellInfo->CastTimeMin) / (_spellInfo->CastTimeMaxLevel - 1)) / float(_spellInfo->CastTimeMax);
                if (_spellInfo->CoefLevelBase > level)
                    multiplier *= (1.0f - _spellInfo->CoefBase) * (float)(level - 1) / (float)(_spellInfo->CoefLevelBase - 1) + _spellInfo->CoefBase;

                float preciseBasePoints = ScalingMultiplier * multiplier;
                if (DeltaScalingMultiplier/* && target*/)
                {
                   // float delta = DeltaScalingMultiplier * ScalingMultiplier * multiplier * 0.5f;
                    float delta = fabs(DeltaScalingMultiplier * ScalingMultiplier * multiplier * 0.5f); // TODO: if all be correct need uncomment assert again
                    preciseBasePoints += frand(-delta, delta);
                }

                basePoints = int32(preciseBasePoints);

                if (ComboScalingMultiplier)
                    comboDamage = ComboScalingMultiplier * multiplier;
            }
        }
    }
    else
    {
        if (caster)
        {
            int32 level = int32(caster->getLevel());
            if (level > int32(_spellInfo->MaxLevel) && _spellInfo->MaxLevel > 0)
                level = int32(_spellInfo->MaxLevel);
            else if (level < int32(_spellInfo->BaseLevel))
                level = int32(_spellInfo->BaseLevel);
            level -= int32(_spellInfo->SpellLevel);
            basePoints += int32(level * basePointsPerLevel);
        }

        // roll in a range <1;EffectDieSides> as of patch 3.3.3
        int32 randomPoints = int32(DieSides);
        switch (randomPoints)
        {
            case 0: break;
            case 1: basePoints += 1; break;                     // range 1..1
            default:
            {
                // range can have positive (1..rand) and negative (rand..1) values, so order its for irand
                int32 randvalue = (randomPoints >= 1)
                    ? irand(1, randomPoints)
                    : irand(randomPoints, 1);

                basePoints += randvalue;
                break;
            }
        }
    }

    float value = float(basePoints);

    // random damage
    if (caster)
    {
        // bonus amount from combo points
        if (caster->m_movedPlayer && comboDamage)
            if (uint8 comboPoints = caster->m_movedPlayer->GetComboPoints())
                value += comboDamage * comboPoints;

        value = caster->ApplyEffectModifiers(_spellInfo, _effIndex, value);

        // amount multiplication based on caster's level
        if (!_spellInfo->GetSpellScaling() && !basePointsPerLevel && (_spellInfo->Attributes & SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION && _spellInfo->SpellLevel) &&
                Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE &&
                Effect != SPELL_EFFECT_KNOCK_BACK &&
                Effect != SPELL_EFFECT_ADD_EXTRA_ATTACKS &&
                Effect != SPELL_EFFECT_GAMEOBJECT_DAMAGE &&
                ApplyAuraName != SPELL_AURA_MOD_SPEED_ALWAYS &&
                ApplyAuraName != SPELL_AURA_MOD_SPEED_NOT_STACK &&
                ApplyAuraName != SPELL_AURA_MOD_INCREASE_SPEED &&
                ApplyAuraName != SPELL_AURA_MOD_DECREASE_SPEED)
                //there are many more: slow speed, -healing pct
            value *= 0.25f * exp(caster->getLevel() * (70 - _spellInfo->SpellLevel) / 1000.0f);
            //value = int32(value * (int32)getLevel() / (int32)(_spellInfo->spellLevel ? _spellInfo->spellLevel : 1));
    }

    return int32(value);
}

int32 SpellEffectInfo::CalcBaseValue(int32 value) const
{
    if (DieSides == 0)
        return value;
    else
        return value - 1;
}

float SpellEffectInfo::CalcValueMultiplier(Unit* caster, Spell* spell) const
{
    float multiplier = ValueMultiplier;
    if (Player* modOwner = (caster ? caster->GetSpellModOwner() : NULL))
        modOwner->ApplySpellMod(_spellInfo->Id, SPELLMOD_VALUE_MULTIPLIER, multiplier, spell);
    return multiplier;
}

float SpellEffectInfo::CalcDamageMultiplier(Unit* caster, Spell* spell) const
{
    float multiplier = DamageMultiplier;
    if (Player* modOwner = (caster ? caster->GetSpellModOwner() : NULL))
        modOwner->ApplySpellMod(_spellInfo->Id, SPELLMOD_DAMAGE_MULTIPLIER, multiplier, spell);
    return multiplier;
}

bool SpellEffectInfo::HasRadius() const
{
    return RadiusEntry != NULL;
}

bool SpellEffectInfo::HasMaxRadius() const	
{
    return MaxRadiusEntry != NULL;	
}

float SpellEffectInfo::CalcRadius(Unit* caster, Spell* spell, bool positive) const
{
    if (!HasRadius())
    {	
        if (HasMaxRadius())	
        {	
            //! Still not sure which to pick. Anyway at the current time (Patch 4.3.4) most of the spell effects
            //! have no radius mod per level, and RadiusMin is equal to RadiusMax.	
            return MaxRadiusEntry->RadiusMin;	
        }
        return 0.0f;
    }

    float radius = RadiusEntry->RadiusMin;	
    if (caster)	
    {
        radius += RadiusEntry->RadiusPerLevel * caster->getLevel();	
        radius = std::min(radius, RadiusEntry->RadiusMax);	
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(_spellInfo->Id, SPELLMOD_RADIUS, radius, spell);	
    }

    return radius;
}

uint32 SpellEffectInfo::GetProvidedTargetMask() const
{
    return GetTargetFlagMask(TargetA.GetObjectType()) | GetTargetFlagMask(TargetB.GetObjectType());
}

void SpellEffectInfo::SetRadiusIndex(uint32 index)
{
    SpellRadiusEntry const* radiusIndex = sSpellRadiusStore.LookupEntry(index);
    if (!radiusIndex)
        return;

    RadiusEntry = radiusIndex;
}

uint32 SpellEffectInfo::GetMissingTargetMask(bool srcSet /*= false*/, bool dstSet /*= false*/, uint32 mask /*=0*/) const
{
    uint32 effImplicitTargetMask = GetTargetFlagMask(GetUsedTargetObjectType());
    uint32 providedTargetMask = GetTargetFlagMask(TargetA.GetObjectType()) | GetTargetFlagMask(TargetB.GetObjectType()) | mask;

    // remove all flags covered by effect target mask
    if (providedTargetMask & TARGET_FLAG_UNIT_MASK)
        effImplicitTargetMask &= ~(TARGET_FLAG_UNIT_MASK);
    if (providedTargetMask & TARGET_FLAG_CORPSE_MASK)
        effImplicitTargetMask &= ~(TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK);
    if (providedTargetMask & TARGET_FLAG_GAMEOBJECT_ITEM)
        effImplicitTargetMask &= ~(TARGET_FLAG_GAMEOBJECT_ITEM | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_ITEM);
    if (providedTargetMask & TARGET_FLAG_GAMEOBJECT)
        effImplicitTargetMask &= ~(TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_GAMEOBJECT_ITEM);
    if (providedTargetMask & TARGET_FLAG_ITEM)
        effImplicitTargetMask &= ~(TARGET_FLAG_ITEM | TARGET_FLAG_GAMEOBJECT_ITEM);
    if (dstSet || providedTargetMask & TARGET_FLAG_DEST_LOCATION)
        effImplicitTargetMask &= ~(TARGET_FLAG_DEST_LOCATION);
    if (srcSet || providedTargetMask & TARGET_FLAG_SOURCE_LOCATION)
        effImplicitTargetMask &= ~(TARGET_FLAG_SOURCE_LOCATION);

    return effImplicitTargetMask;
}

SpellEffectImplicitTargetTypes SpellEffectInfo::GetImplicitTargetType() const
{
    return _data[Effect].ImplicitTargetType;
}

SpellTargetObjectTypes SpellEffectInfo::GetUsedTargetObjectType() const
{
    return _data[Effect].UsedTargetObjectType;
}

SpellEffectInfo::StaticData  SpellEffectInfo::_data[TOTAL_SPELL_EFFECTS] =
{
    // implicit target type           used target object type
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 0
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 1 SPELL_EFFECT_INSTAKILL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 2 SPELL_EFFECT_SCHOOL_DAMAGE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 3 SPELL_EFFECT_DUMMY
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 4 SPELL_EFFECT_PORTAL_TELEPORT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 5 SPELL_EFFECT_TELEPORT_UNITS
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 6 SPELL_EFFECT_APPLY_AURA
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 8 SPELL_EFFECT_POWER_DRAIN
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 9 SPELL_EFFECT_HEALTH_LEECH
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 10 SPELL_EFFECT_HEAL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 11 SPELL_EFFECT_BIND
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 12 SPELL_EFFECT_PORTAL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 13 SPELL_EFFECT_RITUAL_BASE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 14 SPELL_EFFECT_RITUAL_SPECIALIZE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 16 SPELL_EFFECT_QUEST_COMPLETE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_CORPSE_ALLY}, // 18 SPELL_EFFECT_RESURRECT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 20 SPELL_EFFECT_DODGE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 21 SPELL_EFFECT_EVADE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 22 SPELL_EFFECT_PARRY
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 23 SPELL_EFFECT_BLOCK
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 24 SPELL_EFFECT_CREATE_ITEM
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 25 SPELL_EFFECT_WEAPON
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 26 SPELL_EFFECT_DEFENSE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 28 SPELL_EFFECT_SUMMON
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 29 SPELL_EFFECT_LEAP
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 30 SPELL_EFFECT_ENERGIZE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 32 SPELL_EFFECT_TRIGGER_MISSILE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_GOBJ_ITEM}, // 33 SPELL_EFFECT_OPEN_LOCK
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 36 SPELL_EFFECT_LEARN_SPELL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 37 SPELL_EFFECT_SPELL_DEFENSE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 38 SPELL_EFFECT_DISPEL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 39 SPELL_EFFECT_LANGUAGE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 40 SPELL_EFFECT_DUAL_WIELD
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 41 SPELL_EFFECT_JUMP
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_DEST}, // 42 SPELL_EFFECT_JUMP_DEST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 44 SPELL_EFFECT_SKILL_STEP
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 45 SPELL_EFFECT_PLAY_MOVIE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 46 SPELL_EFFECT_SPAWN
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 47 SPELL_EFFECT_TRADE_SKILL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 48 SPELL_EFFECT_STEALTH
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 49 SPELL_EFFECT_DETECT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 50 SPELL_EFFECT_TRANS_DOOR
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 52 SPELL_EFFECT_GUARANTEE_HIT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 53 SPELL_EFFECT_ENCHANT_ITEM
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 55 SPELL_EFFECT_TAMECREATURE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 56 SPELL_EFFECT_SUMMON_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 57 SPELL_EFFECT_LEARN_PET_SPELL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 58 SPELL_EFFECT_WEAPON_DAMAGE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 59 SPELL_EFFECT_CREATE_RANDOM_ITEM
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 60 SPELL_EFFECT_PROFICIENCY
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 61 SPELL_EFFECT_SEND_EVENT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 62 SPELL_EFFECT_POWER_BURN
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 63 SPELL_EFFECT_THREAT
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 64 SPELL_EFFECT_TRIGGER_SPELL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 65 SPELL_EFFECT_APPLY_AREA_AURA_RAID
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 66 SPELL_EFFECT_CREATE_MANA_GEM
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 68 SPELL_EFFECT_INTERRUPT_CAST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 69 SPELL_EFFECT_DISTRACT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 70 SPELL_EFFECT_PULL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 71 SPELL_EFFECT_PICKPOCKET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 72 SPELL_EFFECT_ADD_FARSIGHT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 73 SPELL_EFFECT_UNTRAIN_TALENTS
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 74 SPELL_EFFECT_APPLY_GLYPH
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 75 SPELL_EFFECT_HEAL_MECHANICAL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 77 SPELL_EFFECT_SCRIPT_EFFECT
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 78 SPELL_EFFECT_ATTACK
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 79 SPELL_EFFECT_SANCTUARY
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 81 SPELL_EFFECT_CREATE_HOUSE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 82 SPELL_EFFECT_BIND_SIGHT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 83 SPELL_EFFECT_DUEL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 84 SPELL_EFFECT_STUCK
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 85 SPELL_EFFECT_SUMMON_PLAYER
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_GOBJ}, // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_GOBJ}, // 87 SPELL_EFFECT_GAMEOBJECT_DAMAGE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_GOBJ}, // 88 SPELL_EFFECT_GAMEOBJECT_REPAIR
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_GOBJ}, // 89 SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 90 SPELL_EFFECT_KILL_CREDIT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 91 SPELL_EFFECT_THREAT_ALL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 93 SPELL_EFFECT_FORCE_DESELECT
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 94 SPELL_EFFECT_SELF_RESURRECT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 95 SPELL_EFFECT_SKINNING
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 96 SPELL_EFFECT_CHARGE
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 97 SPELL_EFFECT_CAST_BUTTON
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 98 SPELL_EFFECT_KNOCK_BACK
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 99 SPELL_EFFECT_DISENCHANT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 100 SPELL_EFFECT_INEBRIATE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 101 SPELL_EFFECT_FEED_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 102 SPELL_EFFECT_DISMISS_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 103 SPELL_EFFECT_REPUTATION
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 108 SPELL_EFFECT_DISPEL_MECHANIC
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 109 SPELL_EFFECT_RESURRECT_PET
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 111 SPELL_EFFECT_DURABILITY_DAMAGE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 112 SPELL_EFFECT_112
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_CORPSE_ALLY}, // 113 SPELL_EFFECT_RESURRECT_NEW
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 114 SPELL_EFFECT_ATTACK_ME
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_CORPSE_ENEMY}, // 116 SPELL_EFFECT_SKIN_PLAYER_CORPSE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 117 SPELL_EFFECT_SPIRIT_HEAL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 118 SPELL_EFFECT_SKILL
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 120 SPELL_EFFECT_TELEPORT_GRAVEYARD
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 122 SPELL_EFFECT_122
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 123 SPELL_EFFECT_SEND_TAXI
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 124 SPELL_EFFECT_PULL_TOWARDS
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 127 SPELL_EFFECT_PROSPECTING
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 130 SPELL_EFFECT_REDIRECT_THREAT
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 131 SPELL_EFFECT_PLAY_SOUND
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 132 SPELL_EFFECT_PLAY_MUSIC
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 133 SPELL_EFFECT_UNLEARN_SPECIALIZATION
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 134 SPELL_EFFECT_KILL_CREDIT2
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 135 SPELL_EFFECT_CALL_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 136 SPELL_EFFECT_HEAL_PCT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 137 SPELL_EFFECT_ENERGIZE_PCT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 138 SPELL_EFFECT_LEAP_BACK
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 139 SPELL_EFFECT_CLEAR_QUEST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 140 SPELL_EFFECT_FORCE_CAST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 141 SPELL_EFFECT_FORCE_CAST_WITH_VALUE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 144 SPELL_EFFECT_KNOCK_BACK_DEST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT_AND_DEST}, // 145 SPELL_EFFECT_PULL_TOWARDS_DEST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 146 SPELL_EFFECT_ACTIVATE_RUNE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 147 SPELL_EFFECT_QUEST_FAIL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 148 SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_DEST}, // 149 SPELL_EFFECT_CHARGE_DEST
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 150 SPELL_EFFECT_QUEST_START
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 151 SPELL_EFFECT_TRIGGER_SPELL_2
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 152 SPELL_EFFECT_SUMMON_RAF_FRIEND
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 153 SPELL_EFFECT_CREATE_TAMED_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 154 SPELL_EFFECT_DISCOVER_TAXI
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 155 SPELL_EFFECT_TITAN_GRIP
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 156 SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 157 SPELL_EFFECT_CREATE_ITEM_2
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_ITEM}, // 158 SPELL_EFFECT_MILLING
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 159 SPELL_EFFECT_ALLOW_RENAME_PET
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 160 SPELL_EFFECT_160
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 161 SPELL_EFFECT_TALENT_SPEC_COUNT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 162 SPELL_EFFECT_TALENT_SPEC_SELECT
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 163 SPELL_EFFECT_163
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 164 SPELL_EFFECT_REMOVE_AURA
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 165 SPELL_EFFECT_DAMAGE_SELF_PCT
    {EFFECT_IMPLICIT_TARGET_CASTER,   TARGET_OBJECT_TYPE_UNIT}, // 166 SPELL_EFFECT_REWARD_CURRENCY
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 167 SPELL_EFFECT_167
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 168 SPELL_EFFECT_PET_CONTROL
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_ITEM}, // 169 SPELL_EFFECT_DESTROY_ITEM
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 170 SPELL_EFFECT_170
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_DEST}, // 171 SPELL_EFFECT_171
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 172 SPELL_EFFECT_172
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 173 SPELL_EFFECT_UNLOCK_GUILD_VAULT_TAB
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 174 SPELL_EFFECT_174
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 175 SPELL_EFFECT_175
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 176 SPELL_EFFECT_SANCTUARY_2
    {EFFECT_IMPLICIT_TARGET_EXPLICIT, TARGET_OBJECT_TYPE_UNIT}, // 177 SPELL_EFFECT_177
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 178 SPELL_EFFECT_178
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_DEST}, // 179 SPELL_EFFECT_CREATE_AREATRIGGER
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 180 SPELL_EFFECT_180
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_NONE}, // 181 SPELL_EFFECT_181
    {EFFECT_IMPLICIT_TARGET_NONE,     TARGET_OBJECT_TYPE_UNIT}, // 182 SPELL_EFFECT_182
};

SpellInfo::SpellInfo(SpellEntry const* spellEntry, SpellEffectEntry const** effects)
{
    Id = spellEntry->Id;
    Attributes = spellEntry->Attributes;
    AttributesEx = spellEntry->AttributesEx;
    AttributesEx2 = spellEntry->AttributesEx2;
    AttributesEx3 = spellEntry->AttributesEx3;
    AttributesEx4 = spellEntry->AttributesEx4;
    AttributesEx5 = spellEntry->AttributesEx5;
    AttributesEx6 = spellEntry->AttributesEx6;
    AttributesEx7 = spellEntry->AttributesEx7;
    AttributesEx8 = spellEntry->AttributesEx8;
    AttributesEx9 = spellEntry->AttributesEx9;
    AttributesEx10 = spellEntry->AttributesEx10;
    AttributesCu = 0;
    CastTimeEntry = spellEntry->CastingTimeIndex ? sSpellCastTimesStore.LookupEntry(spellEntry->CastingTimeIndex) : NULL;
    DurationEntry = spellEntry->DurationIndex ? sSpellDurationStore.LookupEntry(spellEntry->DurationIndex) : NULL;
    PowerType = spellEntry->powerType;
    RangeEntry = spellEntry->rangeIndex ? sSpellRangeStore.LookupEntry(spellEntry->rangeIndex) : NULL;
    Speed = spellEntry->speed;
    for (uint8 i = 0; i < 2; ++i)
        SpellVisual[i] = spellEntry->SpellVisual[i];
    SpellIconID = spellEntry->SpellIconID;
    ActiveIconID = spellEntry->activeIconID;
    SpellName = spellEntry->SpellName;
    Rank = spellEntry->Rank;
    SchoolMask = spellEntry->SchoolMask;
    RuneCostID = spellEntry->runeCostID;
    SpellDifficultyId = spellEntry->SpellDifficultyId;
    SpellScalingId = spellEntry->SpellScalingId;
    SpellAuraOptionsId = spellEntry->SpellAuraOptionsId;
    SpellAuraRestrictionsId = spellEntry->SpellAuraRestrictionsId;
    SpellCastingRequirementsId = spellEntry->SpellCastingRequirementsId;
    SpellCategoriesId = spellEntry->SpellCategoriesId;
    SpellClassOptionsId = spellEntry->SpellClassOptionsId;
    SpellCooldownsId = spellEntry->SpellCooldownsId;
    SpellEquippedItemsId = spellEntry->SpellEquippedItemsId;
    SpellInterruptsId = spellEntry->SpellInterruptsId;
    SpellLevelsId = spellEntry->SpellLevelsId;
    SpellPowerId = spellEntry->SpellPowerId;
    SpellReagentsId = spellEntry->SpellReagentsId;
    SpellShapeshiftId = spellEntry->SpellShapeshiftId;
    SpellTargetRestrictionsId = spellEntry->SpellTargetRestrictionsId;
    SpellTotemsId = spellEntry->SpellTotemsId;

    // SpellDifficultyEntry
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        Effects[i] = SpellEffectInfo(spellEntry, this, i, effects[i]);

    // SpellScalingEntry
    SpellScalingEntry const* _scaling = GetSpellScaling();
    CastTimeMin = _scaling ? _scaling->CastTimeMin : 0;
    CastTimeMax = _scaling ?_scaling->CastTimeMax : 0;
    CastTimeMaxLevel = _scaling ? _scaling->CastTimeMaxLevel : 0;
    ScalingClass = _scaling ? _scaling->ScalingClass : 0;
    CoefBase = _scaling ? _scaling->CoefBase : 0;
    CoefLevelBase = _scaling ? _scaling->CoefLevelBase : 0;

    // SpellAuraOptionsEntry
    SpellAuraOptionsEntry const* _options = GetSpellAuraOptions();
    ProcFlags = _options ? _options->procFlags : 0;
    ProcChance = _options ? _options->procChance : 0;
    ProcCharges = _options ? _options->procCharges : 0;
    StackAmount = _options ? _options->StackAmount : 0;

    // SpellAuraRestrictionsEntry
    SpellAuraRestrictionsEntry const* _aura = GetSpellAuraRestrictions();
    CasterAuraState = _aura ? _aura->CasterAuraState : 0;
    TargetAuraState = _aura ? _aura->TargetAuraState : 0;
    CasterAuraStateNot = _aura ? _aura->CasterAuraStateNot : 0;
    TargetAuraStateNot = _aura ? _aura->TargetAuraStateNot : 0;
    CasterAuraSpell = _aura ? _aura->casterAuraSpell : 0;
    TargetAuraSpell = _aura ? _aura->targetAuraSpell : 0;
    ExcludeCasterAuraSpell = _aura ? _aura->excludeCasterAuraSpell : 0;
    ExcludeTargetAuraSpell = _aura ? _aura->excludeTargetAuraSpell : 0;

    // SpellCastingRequirementsEntry
    SpellCastingRequirementsEntry const* _castreq = GetSpellCastingRequirements();
    RequiresSpellFocus = _castreq ? _castreq->RequiresSpellFocus : 0;
    FacingCasterFlags = _castreq ? _castreq->FacingCasterFlags : 0;
    AreaGroupId = _castreq ? _castreq->AreaGroupId : -1;

    // SpellCategoriesEntry
    SpellCategoriesEntry const* _categorie = GetSpellCategories();
    Category = _categorie ? _categorie->Category : 0;
    Dispel = _categorie ? _categorie->Dispel : 0;
    Mechanic = _categorie ? _categorie->Mechanic : 0;
    StartRecoveryCategory = _categorie ? _categorie->StartRecoveryCategory : 0;
    DmgClass = _categorie ? _categorie->DmgClass : 0;
    PreventionType = _categorie ? _categorie->PreventionType : 0;

    // SpellClassOptionsEntry
    SpellClassOptionsEntry const* _class = GetSpellClassOptions();
    SpellFamilyName = _class ? _class->SpellFamilyName : 0;
    SpellFamilyFlags = _class ? _class->SpellFamilyFlags : flag96(0);

    // SpellCooldownsEntry
    SpellCooldownsEntry const* _cooldowns = GetSpellCooldowns();
    RecoveryTime = _cooldowns ? _cooldowns->RecoveryTime : 0;
    CategoryRecoveryTime = _cooldowns ? _cooldowns->CategoryRecoveryTime : 0;
    StartRecoveryTime = _cooldowns ? _cooldowns->StartRecoveryTime : 0;

    // SpellEquippedItemsEntry
    SpellEquippedItemsEntry const* _equipped = GetSpellEquippedItems();
    EquippedItemClass = _equipped ? _equipped->EquippedItemClass : -1;
    EquippedItemSubClassMask = _equipped ?_equipped->EquippedItemSubClassMask : -1;
    EquippedItemInventoryTypeMask = _equipped ? _equipped->EquippedItemInventoryTypeMask : -1;

    // SpellInterruptsEntry
    SpellInterruptsEntry const* _interrupt = GetSpellInterrupts();
    InterruptFlags = _interrupt ? _interrupt->InterruptFlags : 0;
    AuraInterruptFlags = _interrupt ? _interrupt->AuraInterruptFlags : 0;
    ChannelInterruptFlags = _interrupt ? _interrupt->ChannelInterruptFlags : 0;

    // SpellLevelsEntry
    SpellLevelsEntry const* _levels = GetSpellLevels();
    MaxLevel = _levels ? _levels->maxLevel : 0;
    BaseLevel = _levels ? _levels->baseLevel : 0;
    SpellLevel = _levels ? _levels->spellLevel : 0;

    // SpellPowerEntry
    SpellPowerEntry const* _power = GetSpellPower();
    ManaCost = _power ? _power->manaCost : 0;
    ManaCostPerlevel = _power ? _power->manaCostPerlevel : 0;
    ManaCostPercentage = _power ? _power->ManaCostPercentage : 0;
    ManaPerSecond = _power ? _power->manaPerSecond : 0;

    // SpellReagentsEntry
    SpellReagentsEntry const* _reagents = GetSpellReagents();
    for (uint8 i = 0; i < MAX_SPELL_REAGENTS; ++i)
        Reagent[i] = _reagents ? _reagents->Reagent[i] : 0;
    for (uint8 i = 0; i < MAX_SPELL_REAGENTS; ++i)
        ReagentCount[i] = _reagents ? _reagents->ReagentCount[i] : 0;

    // SpellShapeshiftEntry
    SpellShapeshiftEntry const* _shapeshift = GetSpellShapeshift();
    Stances = _shapeshift ? _shapeshift->Stances : 0;
    StancesNot = _shapeshift ? _shapeshift->StancesNot : 0;

    // SpellTargetRestrictionsEntry
    SpellTargetRestrictionsEntry const* _target = GetSpellTargetRestrictions();
    Targets = _target ? _target->Targets : 0;
    TargetCreatureType = _target ? _target->TargetCreatureType : 0;
    MaxAffectedTargets = _target ? _target->MaxAffectedTargets : 0;

    // SpellTotemsEntry
    SpellTotemsEntry const* _totem = GetSpellTotems();
    for (uint8 i = 0; i < 2; ++i)
        TotemCategory[i] = _totem ? _totem->TotemCategory[i] : 0;
    for (uint8 i = 0; i < 2; ++i)
        Totem[i] = _totem ? _totem->Totem[i] : 0;

    ChainEntry = NULL;

    ResearchProject =  spellEntry->ResearchProject;
}

SpellInfo::~SpellInfo()
{
    _UnloadImplicitTargetConditionLists();
}

void SpellInfo::LoadSpellTargetMask()
{
    ExplicitTargetMask = _GetExplicitTargetMask();
}

bool SpellInfo::HasEffect(SpellEffects effect) const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsEffect(effect))
            return true;
    return false;
}

bool SpellInfo::HasAura(AuraType aura) const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsAura(aura))
            return true;
    return false;
}

bool SpellInfo::HasAreaAuraEffect() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsAreaAuraEffect())
            return true;
    return false;
}

bool SpellInfo::HasAreaAuraRaidEffect() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsAreaAuraRaidEffect())
            return true;
    return false;
}

bool SpellInfo::HasPersistenAura() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsPersistenAura())
            return true;
    return false;
}

bool SpellInfo::IsExplicitDiscovery() const
{
    return ((Effects[0].Effect == SPELL_EFFECT_CREATE_RANDOM_ITEM
        || Effects[0].Effect == SPELL_EFFECT_CREATE_ITEM_2)
        && Effects[1].Effect == SPELL_EFFECT_SCRIPT_EFFECT)
        || Id == 64323 || Id == 101805;
}

bool SpellInfo::IsLootCrafting() const
{
    return (Effects[0].Effect == SPELL_EFFECT_CREATE_RANDOM_ITEM ||
        // different random cards from Inscription (121==Virtuoso Inking Set category) r without explicit item
        (Effects[0].Effect == SPELL_EFFECT_CREATE_ITEM_2 &&
        (TotemCategory[0] != 0 || Effects[0].ItemType == 0)));
}

bool SpellInfo::IsQuestTame() const
{
    return Effects[0].Effect == SPELL_EFFECT_THREAT && Effects[1].Effect == SPELL_EFFECT_APPLY_AURA && Effects[1].ApplyAuraName == SPELL_AURA_DUMMY;
}

bool SpellInfo::IsProfessionOrRiding() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].Effect == SPELL_EFFECT_SKILL)
        {
            uint32 skill = Effects[i].MiscValue;

            if (IsProfessionOrRidingSkill(skill))
                return true;
        }
    }
    return false;
}

bool SpellInfo::IsProfession() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].Effect == SPELL_EFFECT_SKILL)
        {
            uint32 skill = Effects[i].MiscValue;

            if (IsProfessionSkill(skill))
                return true;
        }
    }
    return false;
}

bool SpellInfo::IsPrimaryProfession() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].Effect == SPELL_EFFECT_SKILL)
        {
            uint32 skill = Effects[i].MiscValue;

            if (IsPrimaryProfessionSkill(skill))
                return true;
        }
    }
    return false;
}

bool SpellInfo::IsPrimaryProfessionFirstRank() const
{
    return IsPrimaryProfession() && GetRank() == 1;
}

bool SpellInfo::IsAbilityLearnedWithProfession() const
{
    SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(Id);

    for (SkillLineAbilityMap::const_iterator _spell_idx = bounds.first; _spell_idx != bounds.second; ++_spell_idx)
    {
        SkillLineAbilityEntry const* pAbility = _spell_idx->second;
        if (!pAbility || pAbility->learnOnGetSkill != ABILITY_LEARNED_ON_GET_PROFESSION_SKILL)
            continue;

        if (pAbility->req_skill_value > 0)
            return true;
    }

    return false;
}

bool SpellInfo::IsAbilityOfSkillType(uint32 skillType) const
{
    SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(Id);

    for (SkillLineAbilityMap::const_iterator _spell_idx = bounds.first; _spell_idx != bounds.second; ++_spell_idx)
        if (_spell_idx->second->skillId == uint32(skillType))
            return true;

    return false;
}

bool SpellInfo::IsAffectingArea() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsEffect() && (Effects[i].IsTargetingArea() || Effects[i].IsEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA) || Effects[i].IsAreaAuraEffect()))
            return true;
    return false;
}

// checks if spell targets are selected from area, doesn't include spell effects in check (like area wide auras for example)
bool SpellInfo::IsTargetingArea() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsEffect() && Effects[i].IsTargetingArea())
            return true;
    return false;
}

bool SpellInfo::NeedsExplicitUnitTarget() const
{
    return GetExplicitTargetMask() & TARGET_FLAG_UNIT_MASK;
}

bool SpellInfo::NeedsToBeTriggeredByCaster() const
{
    if (NeedsExplicitUnitTarget())
        return true;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].IsEffect())
        {
            if (Effects[i].TargetA.GetSelectionCategory() == TARGET_SELECT_CATEGORY_CHANNEL
                || Effects[i].TargetB.GetSelectionCategory() == TARGET_SELECT_CATEGORY_CHANNEL)
                return true;
        }
    }
    return false;
}

bool SpellInfo::IsPassive() const
{
    return Attributes & SPELL_ATTR0_PASSIVE;
}

bool SpellInfo::IsRaidMarker() const
{
    return AttributesEx8 & SPELL_ATTR8_RAID_MARKER;
}

bool SpellInfo::IsAutocastable() const
{
    if (Attributes & SPELL_ATTR0_PASSIVE)
        return false;
    if (AttributesEx & SPELL_ATTR1_UNAUTOCASTABLE_BY_PET)
        return false;
    return true;
}

bool SpellInfo::IsStackableWithRanks() const
{
    if (IsPassive())
        return false;
    if (PowerType != POWER_MANA && PowerType != POWER_HEALTH)
        return false;
    if (IsProfessionOrRiding())
        return false;

    if (IsAbilityLearnedWithProfession())
        return false;

    // All stance spells. if any better way, change it.
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (SpellFamilyName)
        {
            case SPELLFAMILY_PALADIN:
                // Paladin aura Spell
                if (Effects[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID)
                    return false;
                break;
            case SPELLFAMILY_DRUID:
                // Druid form Spell
                if (Effects[i].Effect == SPELL_EFFECT_APPLY_AURA &&
                    Effects[i].ApplyAuraName == SPELL_AURA_MOD_SHAPESHIFT)
                    return false;
                break;
        }
    }
    return true;
}

bool SpellInfo::IsPassiveStackableWithRanks() const
{
    return IsPassive() && !HasEffect(SPELL_EFFECT_APPLY_AURA);
}

bool SpellInfo::IsMultiSlotAura() const
{
    return IsPassive() || Id == 55849 || Id == 40075 || Id == 44413; // Power Spark, Fel Flak Fire, Incanter's Absorption
}

bool SpellInfo::IsDeathPersistent() const
{
    return AttributesEx3 & SPELL_ATTR3_DEATH_PERSISTENT;
}

bool SpellInfo::IsRequiringDeadTarget() const
{
    return AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_GHOSTS;
}

bool SpellInfo::IsAllowingDeadTarget() const
{
    return AttributesEx2 & SPELL_ATTR2_CAN_TARGET_DEAD || Targets & (TARGET_FLAG_CORPSE_ALLY | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_UNIT_DEAD) || IsRessurrect();
}

bool SpellInfo::CanBeUsedInCombat() const
{
    return !(Attributes & SPELL_ATTR0_CANT_USED_IN_COMBAT);
}

bool SpellInfo::IsPositive() const
{
    return !(AttributesCu & SPELL_ATTR0_CU_NEGATIVE);
}

bool SpellInfo::IsPositiveEffect(uint8 effIndex) const
{
    switch (effIndex)
    {
        default:
        case 0:
            return !(AttributesCu & SPELL_ATTR0_CU_NEGATIVE_EFF0);
        case 1:
            return !(AttributesCu & SPELL_ATTR0_CU_NEGATIVE_EFF1);
        case 2:
            return !(AttributesCu & SPELL_ATTR0_CU_NEGATIVE_EFF2);
    }
}

bool SpellInfo::IsChanneled() const
{
    return (AttributesEx & (SPELL_ATTR1_CHANNELED_1 | SPELL_ATTR1_CHANNELED_2));
}

bool SpellInfo::NeedsComboPoints() const
{
    return (AttributesEx & (SPELL_ATTR1_REQ_COMBO_POINTS1 | SPELL_ATTR1_REQ_COMBO_POINTS2));
}

bool SpellInfo::IsRangedWeaponSpell() const
{
    return (SpellFamilyName == SPELLFAMILY_HUNTER && !(SpellFamilyFlags[1] & 0x10000000)) // for 53352, cannot find better way
        || (EquippedItemSubClassMask & ITEM_SUBCLASS_MASK_WEAPON_RANGED);
}

bool SpellInfo::IsAutoRepeatRangedSpell() const
{
    return AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG;
}

bool SpellInfo::IsPeriodic() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
       if (Effects[i].IsPeriodicEffect())
           return true;
    }
    return false;
}

bool SpellInfo::IsRessurrect() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsRessurrectionEffect())
            return true;

    return false;
}

bool SpellInfo::CanTriggerPoisonAdditional() const
{
    if (SpellFamilyName == SPELLFAMILY_ROGUE)
    {
        switch (Id)
        {
            case 1766:  // Kick
            case 1943:  // Rupture
            case 51722: // Dismantle
            case 703:   // Garrote
                return true;
            default:
                break;
        }
    }
    return false;
}

bool SpellInfo::IsAffectedBySpellMods() const
{
    return !(AttributesEx3 & SPELL_ATTR3_NO_DONE_BONUS);
}

bool SpellInfo::IsAffectedBySpellMod(SpellModifier const* mod) const
{
    if (mod->op != SPELLMOD_CRIT_DAMAGE_BONUS && !IsAffectedBySpellMods())
        return false;

    SpellInfo const* affectSpell = sSpellMgr->GetSpellInfo(mod->spellId);
    // False if affect_spell == NULL or spellFamily not equal
    if (!affectSpell || affectSpell->SpellFamilyName != SpellFamilyName)
        return false;

    // true
    if (mod->mask & SpellFamilyFlags)
        return true;

    return false;
}

bool SpellInfo::CanPierceImmuneAura(SpellInfo const* aura) const
{
    // these spells pierce all avalible spells (Resurrection Sickness for example)
    if (Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return true;

    if (aura && aura->Id == 33786)
    {
        // Re-cyclone & Fiery Fire
        if (Id == aura->Id || SpellIconID == 109)
            return false;
    }

    if (HasEffect(SPELL_EFFECT_INTERRUPT_CAST) && aura && aura->Id == 102915)
        return true;

    // these spells (Cyclone for example) can pierce all...
    if ((AttributesEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE)
        // ...but not these (Divine shield for example)
        && !(aura && (aura->Mechanic == MECHANIC_IMMUNE_SHIELD || aura->Mechanic == MECHANIC_INVULNERABILITY || aura->Id == 48707)))
        return true;

    return false;
}

bool SpellInfo::CanDispelAura(SpellInfo const* aura) const
{
    // Faerie Fire
    if (aura->SpellIconID == 109)
        return true;

    // These spells (like Mass Dispel) can dispell all auras
    if (Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return true;

    // These auras (like Divine Shield) can't be dispelled
    if (aura->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    // These auras (Cyclone for example) are not dispelable
    if (aura->AttributesEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE)
        return false;

    switch (aura->Id)
    {
        case 94528: // Flare
        case 76577: // Smoke Bomb
            return false;
        default:
            break;
    }

    return true;
}

bool SpellInfo::CanCritDamageClassNone() const
{
    switch (Id)
    {
        case 379:   // Shaman - Earth Shield
        case 73685: // Shaman - Unleash Elements - Unleash Life 

        case 86958: // Shaman - Cleansing Waters
        case 86961:

        case 33778: // Druid - Lifebloom Final Bloom
        case 22845: // Druid - Frenzied Regeneration

        case 64844: // Priest - Divine Hymn

        case 85222: // Paladin - Light of Dawn

        case 94286: // Paladin - Protector of the Innocent proc
        case 94288:
        case 94289:

        case 71607: // Item - Bauble of True Blood 10m
        case 71646: // Item - Bauble of True Blood 25m

        case 109825: // Item - Windward Heart heroic
        case 108000: // Item - Windward Heart lfr
        case 109822: // Item - Windward Heart normal
            return true;
    }
    return false;
}

bool SpellInfo::IsSingleTarget(Unit* caster) const
{
    // all other single target spells have if it has AttributesEx5
    if (AttributesEx5 & SPELL_ATTR5_SINGLE_TARGET_SPELL)
        return true;

    switch (GetSpellSpecific())
    {
        case SPELL_SPECIFIC_JUDGEMENT:
            return true;
        default:
            break;
    }

    // Life Bloom and Tree of Life
    if (Id == 33763 && caster && !caster->HasAura(33891))
        return true;

    return false;
}

bool SpellInfo::IsSingleTargetWith(SpellInfo const* spellInfo) const
{
    // TODO - need better check
    // Equal icon and spellfamily
    if (SpellFamilyName == spellInfo->SpellFamilyName &&
        SpellIconID == spellInfo->SpellIconID)
        return true;

    SpellSpecificType spec = GetSpellSpecific();
    // spell with single target specific types
    switch (spec)
    {
        case SPELL_SPECIFIC_JUDGEMENT:
        case SPELL_SPECIFIC_MAGE_POLYMORPH:
            if (spellInfo->GetSpellSpecific() == spec)
                return true;
            break;
        default:
            break;
    }

    return false;
}

bool SpellInfo::IsAuraExclusiveBySpecificWith(SpellInfo const* spellInfo) const
{
    SpellSpecificType spellSpec1 = GetSpellSpecific();
    SpellSpecificType spellSpec2 = spellInfo->GetSpellSpecific();
    switch (spellSpec1)
    {
        case SPELL_SPECIFIC_WARLOCK_ARMOR:
        case SPELL_SPECIFIC_WELL_FED:
        case SPELL_SPECIFIC_MAGE_ARMOR:
        case SPELL_SPECIFIC_ELEMENTAL_SHIELD:
        case SPELL_SPECIFIC_MAGE_POLYMORPH:
        case SPELL_SPECIFIC_PRESENCE:
        case SPELL_SPECIFIC_CHARM:
        case SPELL_SPECIFIC_SCROLL:
        case SPELL_SPECIFIC_WARRIOR_ENRAGE:
        case SPELL_SPECIFIC_MAGE_ARCANE_BRILLANCE:
        case SPELL_SPECIFIC_PRIEST_DIVINE_SPIRIT:
        case SPELL_SPECIFIC_PRIEST_SANCTUM:
        case SPELL_SPECIFIC_CHAKRA:
            return spellSpec1 == spellSpec2;
        case SPELL_SPECIFIC_FOOD:
            return spellSpec2 == SPELL_SPECIFIC_FOOD
                || spellSpec2 == SPELL_SPECIFIC_FOOD_AND_DRINK;
        case SPELL_SPECIFIC_DRINK:
            return spellSpec2 == SPELL_SPECIFIC_DRINK
                || spellSpec2 == SPELL_SPECIFIC_FOOD_AND_DRINK;
        case SPELL_SPECIFIC_FOOD_AND_DRINK:
            return spellSpec2 == SPELL_SPECIFIC_FOOD
                || spellSpec2 == SPELL_SPECIFIC_DRINK
                || spellSpec2 == SPELL_SPECIFIC_FOOD_AND_DRINK;
        default:
            return false;
    }
}

bool SpellInfo::IsAuraExclusiveBySpecificPerCasterWith(SpellInfo const* spellInfo) const
{
    SpellSpecificType spellSpec = GetSpellSpecific();
    switch (spellSpec)
    {
        case SPELL_SPECIFIC_BLESSING:
        case SPELL_SPECIFIC_SEAL:
        case SPELL_SPECIFIC_HAND:
        case SPELL_SPECIFIC_AURA:
        case SPELL_SPECIFIC_STING:
        case SPELL_SPECIFIC_CURSE:
        case SPELL_SPECIFIC_BANE:
        case SPELL_SPECIFIC_ASPECT:
        case SPELL_SPECIFIC_JUDGEMENT:
        case SPELL_SPECIFIC_WARLOCK_CORRUPTION:
            return spellSpec == spellInfo->GetSpellSpecific();
        default:
            return false;
    }
}

SpellCastResult SpellInfo::CheckShapeshift(uint32 form) const
{
    // talents that learn spells can have stance requirements that need ignore
    // (this requirement only for client-side stance show in talent description)
    if (GetTalentSpellCost(Id) > 0 &&
        (Effects[0].Effect == SPELL_EFFECT_LEARN_SPELL || Effects[1].Effect == SPELL_EFFECT_LEARN_SPELL || Effects[2].Effect == SPELL_EFFECT_LEARN_SPELL))
        return SPELL_CAST_OK;

    uint32 stanceMask = (form ? 1 << (form - 1) : 0);

    if (stanceMask & StancesNot)                 // can explicitly not be casted in this stance
        return SPELL_FAILED_NOT_SHAPESHIFT;

    if (stanceMask & Stances)                    // can explicitly be casted in this stance
        return SPELL_CAST_OK;

    bool actAsShifted = false;
    SpellShapeshiftFormEntry const* shapeInfo = NULL;
    if (form > 0)
    {
        shapeInfo = sSpellShapeshiftFormStore.LookupEntry(form);
        if (!shapeInfo)
            return SPELL_CAST_OK;

        actAsShifted = !(shapeInfo->flags1 & 1);            // shapeshift acts as normal form for spells
    }

    if (actAsShifted)
    {
        if (Attributes & SPELL_ATTR0_NOT_SHAPESHIFT) // not while shapeshifted
            return SPELL_FAILED_NOT_SHAPESHIFT;
        else if (Stances != 0)                   // needs other shapeshift
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }
    else
    {
        // needs shapeshift
        if (!(AttributesEx2 & SPELL_ATTR2_NOT_NEED_SHAPESHIFT) && Stances != 0)
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }

    // Check if stance disables cast of not-stance spells
    // Example: cannot cast any other spells in zombie or ghoul form
    // TODO: Find a way to disable use of these spells clientside
    if (shapeInfo && shapeInfo->flags1 & 0x400)
    {
        if (!(stanceMask & Stances))
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }

    return SPELL_CAST_OK;
}

SpellCastResult SpellInfo::CheckLocation(uint32 map_id, uint32 zone_id, uint32 area_id, Player* player) const
{
    // normal case
    if (AreaGroupId > 0)
    {
        bool found = false;
        AreaGroupEntry const* groupEntry = sAreaGroupStore.LookupEntry(AreaGroupId);
        while (groupEntry)
        {
            for (uint8 i = 0; i < MAX_GROUP_AREA_IDS; ++i)
                if (groupEntry->AreaId[i] == zone_id || groupEntry->AreaId[i] == area_id)
                    found = true;
            if (found || !groupEntry->nextGroup)
                break;
            // Try search in next group
            groupEntry = sAreaGroupStore.LookupEntry(groupEntry->nextGroup);
        }

        if (!found)
            return SPELL_FAILED_INCORRECT_AREA;
    }

    // continent limitation (virtual continent)
    if (AttributesEx4 & SPELL_ATTR4_CAST_ONLY_IN_OUTLAND)
    {
        uint32 v_map = GetVirtualMapForMapAndZone(map_id, zone_id);
        MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
        if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
            return SPELL_FAILED_INCORRECT_AREA;
    }

    // raid instance limitation
    if (AttributesEx6 & SPELL_ATTR6_NOT_IN_RAID_INSTANCE)
    {
        MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
        if (!mapEntry || mapEntry->IsRaid())
            return SPELL_FAILED_NOT_IN_RAID_INSTANCE;
    }

    // DB base check (if non empty then must fit at least single for allow)
    SpellAreaMapBounds saBounds = sSpellMgr->GetSpellAreaMapBounds(Id);
    if (saBounds.first != saBounds.second)
    {
        for (SpellAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
        {
            if (itr->second.IsFitToRequirements(player, zone_id, area_id))
                return SPELL_CAST_OK;
        }
        return SPELL_FAILED_INCORRECT_AREA;
    }

    // spell checks
    switch (Id)
    {
        case 105896:
        case 105900:
        case 105903:
        case 105984:
        case 106224:
        case 106226:
        case 106227:
        case 106080:
        case 106108:
        case 105925:
        case 110070:
        case 110069: 
        case 110068: 
        case 109075: 
        case 110080:
        case 110079:
        case 110078:
        case 106498:
        case 106368:
            if (!player)
                return SPELL_CAST_OK;
            return (area_id == 5928) ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 103755: // Twilight Epiphany, Archbishop Benedictus, Hour of Twilight
            return ((map_id == 940 && area_id == 5845) ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA);
        case 103534: // Danger, Morchok, Dragon Soul
        case 103536: // Warning, Morchok, Dragon Soul
        case 103541: // Safe, Morchok, Dragon Soul
        {
            if (player)
                if (InstanceScript* pInstance = player->GetInstanceScript())
                    return (pInstance->GetBossState(0) == IN_PROGRESS) ? SPELL_CAST_OK : SPELL_FAILED_DONT_REPORT;
            
            return (area_id == 5926 || area_id == 5923) ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
        case 105009: // Gift of Sargeras, Well of Eternity
            return (map_id == 939)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 100713: // Deluge, Ragnaros, Firelands
        case 101015:
        case 101110: // Rage of Ragnaros, Ragnaros, Firelands
            return (area_id == 5770)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 98226: // Balance Bar, Lord Rhyolith, Firelands
            return (area_id == 5763)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 102668: // Sands of the Hourglass, Murozond, End Time
            return (area_id == 5795)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 98229: // Concentration, Majordomo Staghelm, Firelands
        case 98245:
        case 98252:
        case 98253:
        case 98254:
            return (area_id == 5769)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 99252: // Blaze of Glory, Baleroc
            return (area_id == 5767)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 99837: // Crystal Prison, Shannox
            return (map_id == 720)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 45614: // Shroud of the Scourge
            return (area_id == 4125)? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 43820: // Charm of the Witch Doctor
        case 43822: // Charm of the Raging Defender
        case 43816: // Charm of the Bloodletter
        case 43818: // Charm of Mighty Mojo
            return map_id == 568? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 99659:
        case 62857:
        case 107926:
            return ((player && player->InBattleground()) ? SPELL_FAILED_NOT_IN_BATTLEGROUND : SPELL_CAST_OK);
        case 23333:                                         // Warsong Flag
        case 23335:                                         // Silverwing Flag
            return (map_id == 489 || map_id == 726) && player && player->InBattleground() ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 34976:                                         // Netherstorm Flag
            return map_id == 566 && player && player->InBattleground() ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        case 2584:                                          // Waiting to Resurrect
        case 22011:                                         // Spirit Heal Channel
        case 22012:                                         // Spirit Heal
        case 24171:                                         // Resurrection Impact Visual
        case 42792:                                         // Recently Dropped Flag
        case 43681:                                         // Inactive
        case 44535:                                         // Spirit Heal (mana)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_INCORRECT_AREA;

            return zone_id == 4197 || zone_id == 5095 || (mapEntry->IsBattleground() && player && player->InBattleground()) ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
        case 44521:                                         // Preparation
        {
            if (!player)
                return SPELL_FAILED_REQUIRES_AREA;

            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_INCORRECT_AREA;

            if (!mapEntry->IsBattleground())
                return SPELL_FAILED_REQUIRES_AREA;

            Battleground* bg = player->GetBattleground();
            return bg && bg->GetStatus() == STATUS_WAIT_JOIN ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
        case 32724:                                         // Gold Team (Alliance)
        case 32725:                                         // Green Team (Alliance)
        case 35774:                                         // Gold Team (Horde)
        case 35775:                                         // Green Team (Horde)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_INCORRECT_AREA;

            return mapEntry->IsBattlegroundOrArena() && player && player->InBattleground() ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
        case 32727:                                         // Arena Preparation
        {
            if (!player)
                return SPELL_FAILED_REQUIRES_AREA;

            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_INCORRECT_AREA;

            if (!mapEntry->IsBattleArena())
                return SPELL_FAILED_REQUIRES_AREA;

            Battleground* bg = player->GetBattleground();
            return bg && bg->GetStatus() == STATUS_WAIT_JOIN ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
    }

    // aura limitations
    if (player)
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (!Effects[i].IsAura())
                continue;

            switch (Effects[i].ApplyAuraName)
            {
                case SPELL_AURA_FLY:
                case SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED:
                {
                    if (!player->IsKnowHowFlyIn(map_id, zone_id, Id) && Id != 70766) // Dream Portal on Valithria
                        return SPELL_FAILED_INCORRECT_AREA;
                    break;
                }
            }
        }
    }

    return SPELL_CAST_OK;
}

SpellCastResult SpellInfo::CheckTarget(Unit const* caster, WorldObject const* target, bool implicit) const
{
    if (AttributesEx & SPELL_ATTR1_CANT_TARGET_SELF && caster == target)
        return SPELL_FAILED_BAD_TARGETS;

    // check visibility - ignore stealth for implicit (area) targets
    if (!(AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE) && !caster->canSeeOrDetect(target, implicit))
        return SPELL_FAILED_BAD_TARGETS;

    Unit const* unitTarget = target->ToUnit();

    // creature/player specific target checks
    if (unitTarget)
    {
        if (AttributesEx & SPELL_ATTR1_CANT_TARGET_IN_COMBAT)
        {
            if (unitTarget->isInCombat())
                return SPELL_FAILED_TARGET_AFFECTING_COMBAT;
                // player with active pet counts as a player in combat
            else if (Player const* player = unitTarget->ToPlayer())
                if (Pet* pet = player->GetPet())
                if (pet->getVictim() && !pet->HasUnitState(UNIT_STATE_CONTROLLED))
                return SPELL_FAILED_TARGET_AFFECTING_COMBAT;
        }

        // only spells with SPELL_ATTR3_ONLY_TARGET_GHOSTS can target ghosts
        if (((AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_GHOSTS) != 0) != unitTarget->HasAuraType(SPELL_AURA_GHOST))
        {
            if (AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_GHOSTS)
                return SPELL_FAILED_TARGET_NOT_GHOST;
            else
                return SPELL_FAILED_BAD_TARGETS;
        }

        if (caster != unitTarget)
        {
            if (caster->GetTypeId() == TYPEID_PLAYER)
            {
                // Do not allow these spells to target creatures not tapped by us (Banish, Polymorph, many quest spells)
                if (HasAttribute(SPELL_ATTR2_CANT_TARGET_TAPPED))
                    if (Creature const* targetCreature = unitTarget->ToCreature())
                        if (targetCreature->hasLootRecipient() && !targetCreature->isTappedBy(caster->ToPlayer()))
                            return SPELL_FAILED_CANT_CAST_ON_TAPPED;

                if (AttributesCu & SPELL_ATTR0_CU_PICKPOCKET)
                {
                     if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                         return SPELL_FAILED_BAD_TARGETS;
                     else if ((unitTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) == 0)
                         return SPELL_FAILED_TARGET_NO_POCKETS;
                }

                // Not allow disarm unarmed player
                if (Mechanic == MECHANIC_DISARM)
                {
                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player const* player = unitTarget->ToPlayer();
                        if (!player->GetWeaponForAttack(BASE_ATTACK) || !player->IsUseEquipedWeapon(true))
                            return SPELL_FAILED_TARGET_NO_WEAPONS;
                    }
                    else if (!unitTarget->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID))
                        return SPELL_FAILED_TARGET_NO_WEAPONS;
                }
            }
        }
    }
    // corpse specific target checks
    else if (Corpse const* corpseTarget = target->ToCorpse())
    {
        // cannot target bare bones
        if (corpseTarget->GetType() == CORPSE_BONES)
            return SPELL_FAILED_BAD_TARGETS;
        // we have to use owner for some checks (aura preventing resurrection for example)
        if (Player* owner = ObjectAccessor::FindPlayer(corpseTarget->GetOwnerGUID()))
            unitTarget = owner;
        // we're not interested in corpses without owner
        else
            return SPELL_FAILED_BAD_TARGETS;
    }
    // other types of objects - always valid
    else return SPELL_CAST_OK;

    // corpseOwner and unit specific target checks
    if (AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS && !unitTarget->ToPlayer())
       return SPELL_FAILED_TARGET_NOT_PLAYER;

    if (!IsAllowingDeadTarget() && !unitTarget->isAlive())
       return SPELL_FAILED_TARGETS_DEAD;

    // check this flag only for implicit targets (chain and area), allow to explicitly target units for spells like Shield of Righteousness
    if (implicit && AttributesEx6 & SPELL_ATTR6_CANT_TARGET_CROWD_CONTROLLED && !unitTarget->CanFreeMove())
       return SPELL_FAILED_BAD_TARGETS;

    // checked in Unit::IsValidAttack/AssistTarget, shouldn't be checked for ENTRY targets
    //if (!(AttributesEx6 & SPELL_ATTR6_CAN_TARGET_UNTARGETABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
    //    return SPELL_FAILED_BAD_TARGETS;

    //if (!(AttributesEx6 & SPELL_ATTR6_CAN_TARGET_POSSESSED_FRIENDS)

    if (!CheckTargetCreatureType(unitTarget))
    {
        if (target->GetTypeId() == TYPEID_PLAYER)
            return SPELL_FAILED_TARGET_IS_PLAYER;
        else
            return SPELL_FAILED_BAD_TARGETS;
    }

    // check GM mode and GM invisibility - only for player casts (npc casts are controlled by AI) and negative spells
    if (unitTarget != caster && (caster->IsControlledByPlayer() || !IsPositive()) && unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        if (!unitTarget->ToPlayer()->IsVisible())
            return SPELL_FAILED_BM_OR_INVISGOD;

        if (unitTarget->ToPlayer()->isGameMaster())
            return SPELL_FAILED_BM_OR_INVISGOD;
    }

    // not allow casting on flying player
    if (unitTarget->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return SPELL_FAILED_BAD_TARGETS;

    if (!caster->IsVehicle() && !(caster->GetCharmerOrOwner() == target))
    {
        if (TargetAuraState && !unitTarget->HasAuraState(AuraStateType(TargetAuraState), this, caster))
            return SPELL_FAILED_TARGET_AURASTATE;

        if (TargetAuraStateNot && unitTarget->HasAuraState(AuraStateType(TargetAuraStateNot), this, caster))
            return SPELL_FAILED_TARGET_AURASTATE;
    }

    if (TargetAuraSpell && !unitTarget->HasAura(sSpellMgr->GetSpellIdForDifficulty(TargetAuraSpell, caster)))
        return SPELL_FAILED_TARGET_AURASTATE;

    if (ExcludeTargetAuraSpell && unitTarget->HasAura(sSpellMgr->GetSpellIdForDifficulty(ExcludeTargetAuraSpell, caster)))
        return SPELL_FAILED_TARGET_AURASTATE;

    if (unitTarget->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        if (HasEffect(SPELL_EFFECT_SELF_RESURRECT) || HasEffect(SPELL_EFFECT_RESURRECT) || HasEffect(SPELL_EFFECT_RESURRECT_NEW))
            return SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED;

    return SPELL_CAST_OK;
}

SpellCastResult SpellInfo::CheckExplicitTarget(Unit const* caster, WorldObject const* target, Item const* itemTarget) const
{
    uint32 neededTargets = GetExplicitTargetMask();
    if (!target)
    {
        if (neededTargets & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_GAMEOBJECT_MASK | TARGET_FLAG_CORPSE_MASK))
            if (!(neededTargets & TARGET_FLAG_GAMEOBJECT_ITEM) || !itemTarget)
                return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    if (Unit const* unitTarget = target->ToUnit())
    {
        if (neededTargets & (TARGET_FLAG_UNIT_ENEMY | TARGET_FLAG_UNIT_ALLY | TARGET_FLAG_UNIT_RAID | TARGET_FLAG_UNIT_PARTY | TARGET_FLAG_UNIT_MINIPET | TARGET_FLAG_UNIT_PASSENGER))
        {
            if (neededTargets & TARGET_FLAG_UNIT_ENEMY)
                if (caster->_IsValidAttackTarget(unitTarget, this))
                    return SPELL_CAST_OK;
            if (neededTargets & TARGET_FLAG_UNIT_ALLY
                || (neededTargets & TARGET_FLAG_UNIT_PARTY && caster->IsInPartyWith(unitTarget))
                || (neededTargets & TARGET_FLAG_UNIT_RAID && caster->IsInRaidWith(unitTarget)))
                    if (caster->_IsValidAssistTarget(unitTarget, this))
                        return SPELL_CAST_OK;
            if (neededTargets & TARGET_FLAG_UNIT_MINIPET)
                if (unitTarget->GetGUID() == caster->GetCritterGUID())
                    return SPELL_CAST_OK;
            if (neededTargets & TARGET_FLAG_UNIT_PASSENGER)
                if (unitTarget->IsOnVehicle(caster))
                    return SPELL_CAST_OK;
            return SPELL_FAILED_BAD_TARGETS;
        }
    }
    return SPELL_CAST_OK;
}

bool SpellInfo::CheckTargetCreatureType(Unit const* target) const
{
    // Curse of Doom & Exorcism: not find another way to fix spell target check :/
    if (SpellFamilyName == SPELLFAMILY_WARLOCK && Category == 1179)
    {
        // not allow cast at player
        if (target->GetTypeId() == TYPEID_PLAYER)
            return false;
        else
            return true;
    }
    uint32 creatureType = target->GetCreatureTypeMask();
    return !TargetCreatureType || !creatureType || (creatureType & TargetCreatureType);
}

SpellCastResult SpellInfo::CheckVehicle(Unit const* caster) const
{
    // All creatures should be able to cast as passengers freely, restriction and attribute are only for players
    if (caster->GetTypeId() != TYPEID_PLAYER)
        return SPELL_CAST_OK;

    Vehicle* vehicle = caster->GetVehicle();
    if (vehicle)
    {
        uint16 checkMask = 0;
        for (uint8 effIndex = EFFECT_0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        {
            if (Effects[effIndex].ApplyAuraName == SPELL_AURA_MOD_SHAPESHIFT)
            {
                SpellShapeshiftFormEntry const* shapeShiftFormEntry = sSpellShapeshiftFormStore.LookupEntry(Effects[effIndex].MiscValue);
                if (shapeShiftFormEntry && (shapeShiftFormEntry->flags1 & 1) == 0)  // unk flag
                    checkMask |= VEHICLE_SEAT_FLAG_UNCONTROLLED;
                break;
            }
        }

        if (HasAura(SPELL_AURA_MOUNTED))
            checkMask |= VEHICLE_SEAT_FLAG_CAN_CAST_MOUNT_SPELL;

        if (!checkMask)
            checkMask = VEHICLE_SEAT_FLAG_CAN_ATTACK;

        VehicleSeatEntry const* vehicleSeat = vehicle->GetSeatForPassenger(caster);
        if (!(AttributesEx6 & SPELL_ATTR6_CASTABLE_WHILE_ON_VEHICLE) && !(Attributes & SPELL_ATTR0_CASTABLE_WHILE_MOUNTED)
            && (vehicleSeat->m_flags & checkMask) != checkMask)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        // Can only summon uncontrolled minions/guardians when on controlled vehicle
        if (vehicleSeat->m_flags & (VEHICLE_SEAT_FLAG_CAN_CONTROL | VEHICLE_SEAT_FLAG_UNK2))
        {
            for (uint32 i = EFFECT_0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (Effects[i].Effect != SPELL_EFFECT_SUMMON)
                    continue;

                SummonPropertiesEntry const* props = sSummonPropertiesStore.LookupEntry(Effects[i].MiscValueB);
                if (props && props->Category != SUMMON_CATEGORY_WILD)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }
        }
    }

    return SPELL_CAST_OK;
}

SpellSchoolMask SpellInfo::GetSchoolMask() const
{
    return SpellSchoolMask(SchoolMask);
}

uint32 SpellInfo::GetAllEffectsMechanicMask() const
{
    uint32 mask = 0;
    if (Mechanic)
        mask |= 1 << Mechanic;
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].IsEffect() && Effects[i].Mechanic)
            mask |= 1 << Effects[i].Mechanic;
    return mask;
}

uint32 SpellInfo::GetEffectMechanicMask(uint8 effIndex) const
{
    uint32 mask = 0;
    if (Mechanic)
        mask |= 1<< Mechanic;
    if (Effects[effIndex].IsEffect() && Effects[effIndex].Mechanic)
        mask |= 1<< Effects[effIndex].Mechanic;
    return mask;
}

uint32 SpellInfo::GetSpellMechanicMaskByEffectMask(uint32 effectMask) const
{
    uint32 mask = 0;
    if (Mechanic)
        mask |= 1<< Mechanic;
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if ((effectMask & (1 << i)) && Effects[i].Mechanic)
            mask |= 1<< Effects[i].Mechanic;
    return mask;
}

Mechanics SpellInfo::GetEffectMechanic(uint8 effIndex) const
{
    if (Effects[effIndex].IsEffect() && Effects[effIndex].Mechanic)
        return Mechanics(Effects[effIndex].Mechanic);
    if (Mechanic)
        return Mechanics(Mechanic);
    return MECHANIC_NONE;
}

bool SpellInfo::HasAnyEffectMechanic() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (Effects[i].Mechanic)
            return true;
    return false;
}

uint32 SpellInfo::GetDispelMask() const
{
    return GetDispelMask(DispelType(Dispel));
}

uint32 SpellInfo::GetDispelMask(DispelType type)
{
    // If dispel all
    if (type == DISPEL_ALL)
        return DISPEL_ALL_MASK;
    else
        return uint32(1 << type);
}

uint32 SpellInfo::GetExplicitTargetMask() const
{
    return ExplicitTargetMask;
}

AuraStateType SpellInfo::GetAuraState() const
{
    // Seals
    if (GetSpellSpecific() == SPELL_SPECIFIC_SEAL)
        return AURA_STATE_JUDGEMENT;

    // Conflagrate aura state on Immolate and Shadowflame
    if (SpellFamilyName == SPELLFAMILY_WARLOCK &&
        // Immolate
        ((SpellFamilyFlags[0] & 4)))
        return AURA_STATE_CONFLAGRATE;

    // Faerie Fire (druid versions)
    if (SpellFamilyName == SPELLFAMILY_DRUID && SpellFamilyFlags[0] & 0x400)
        return AURA_STATE_FAERIE_FIRE;

    // Sting (hunter's pet ability)
    if (Category == 1133)
        return AURA_STATE_FAERIE_FIRE;

    // Victorious
    if (SpellFamilyName == SPELLFAMILY_WARRIOR &&  SpellFamilyFlags[1] & 0x00040000)
        return AURA_STATE_WARRIOR_VICTORY_RUSH;

    // Swiftmend state on Regrowth & Rejuvenation
    if (SpellFamilyName == SPELLFAMILY_DRUID && SpellFamilyFlags[0] & 0x50)
        return AURA_STATE_SWIFTMEND;

    // Deadly poison aura state
    if (SpellFamilyName == SPELLFAMILY_ROGUE && SpellFamilyFlags[0] & 0x10000)
        return AURA_STATE_DEADLY_POISON;

    // Enrage aura state
    if (Dispel == DISPEL_ENRAGE)
        return AURA_STATE_ENRAGE;

    // Bleeding aura state
    if (GetAllEffectsMechanicMask() & 1<<MECHANIC_BLEED)
        return AURA_STATE_BLEEDING;

    if (GetSchoolMask() & SPELL_SCHOOL_MASK_FROST)
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (Effects[i].IsAura() && (Effects[i].ApplyAuraName == SPELL_AURA_MOD_STUN
                || Effects[i].ApplyAuraName == SPELL_AURA_MOD_ROOT))
                return AURA_STATE_FROZEN;

    switch (Id)
    {
        case 71465: // Divine Surge
        case 50241: // Evasive Charges
            return AURA_STATE_UNKNOWN22;
        default:
            break;
    }

    return AURA_STATE_NONE;
}

SpellSpecificType SpellInfo::GetSpellSpecific() const
{
    switch (SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            // Food / Drinks (mostly)
            if (AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)
            {
                bool food = false;
                bool drink = false;
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if (!Effects[i].IsAura())
                        continue;
                    switch (Effects[i].ApplyAuraName)
                    {
                        // Food
                        case SPELL_AURA_MOD_REGEN:
                        case SPELL_AURA_OBS_MOD_HEALTH:
                            food = true;
                            break;
                        // Drink
                        case SPELL_AURA_MOD_POWER_REGEN:
                        case SPELL_AURA_OBS_MOD_POWER:
                            drink = true;
                            break;
                        default:
                            break;
                    }
                }

                if (SpellIconID == 358)
                    food = true;

                if (food && drink)
                    return SPELL_SPECIFIC_FOOD_AND_DRINK;
                else if (food)
                    return SPELL_SPECIFIC_FOOD;
                else if (drink)
                    return SPELL_SPECIFIC_DRINK;
            }
            else if ((AttributesEx2 & SPELL_ATTR2_FOOD_BUFF) || SpellIconID == 2560)
                return SPELL_SPECIFIC_WELL_FED;
            // scrolls effects
            else
            {
                SpellInfo const* firstRankSpellInfo = GetFirstRankSpell();
                switch (firstRankSpellInfo->Id)
                {
                    case 8118: // Strength
                    case 8099: // Stamina
                    case 8112: // Spirit
                    case 8096: // Intellect
                    case 8115: // Agility
                    case 8091: // Armor
                        return SPELL_SPECIFIC_SCROLL;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // family flags 18(Molten), 25(Frost/Ice), 28(Mage)
            if (SpellFamilyFlags[0] & 0x12040000)
                return SPELL_SPECIFIC_MAGE_ARMOR;

            // Arcane brillance and Arcane intelect (normal check fails because of flags difference)
            if (SpellFamilyFlags[0] & 0x400)
                return SPELL_SPECIFIC_MAGE_ARCANE_BRILLANCE;

            if ((SpellFamilyFlags[0] & 0x1000000) && Effects[0].ApplyAuraName == SPELL_AURA_MOD_CONFUSE)
                return SPELL_SPECIFIC_MAGE_POLYMORPH;

            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (Id)
            {
                case 12292: // Death Wish
                case 12880: // Enrage (Enrage)
                case 57518: // Enrage (Wrecking Crew)
                    return SPELL_SPECIFIC_WARRIOR_ENRAGE;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Warlock (Bane of Doom | Bane of Agony | Bane of Havoc)
            if (Id == 603 || Id ==  980 || Id == 80240)
                return SPELL_SPECIFIC_BANE;

            // only warlock curses have this
            if (Dispel == DISPEL_CURSE)
                return SPELL_SPECIFIC_CURSE;

            // Warlock (Demon Armor | Fel Armor)
            if (SpellFamilyFlags[1] & 0x20000020)
                return SPELL_SPECIFIC_WARLOCK_ARMOR;

            //seed of corruption and corruption
            if (SpellFamilyFlags[1] & 0x10 || SpellFamilyFlags[0] & 0x2)
                return SPELL_SPECIFIC_WARLOCK_CORRUPTION;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // "Well Fed" buff from Blessed Sunfruit, Blessed Sunfruit Juice, Alterac Spring Water
            if ((Attributes & SPELL_ATTR0_CASTABLE_WHILE_SITTING) &&
                (InterruptFlags & AURA_INTERRUPT_FLAG_MOVE) &&
                (SpellIconID == 52 || SpellIconID == 79))
                return SPELL_SPECIFIC_WELL_FED;

            // Divine Spirit and Prayer of Spirit
            if (SpellFamilyFlags[0] & 0x20)
                return SPELL_SPECIFIC_PRIEST_DIVINE_SPIRIT;

            // Priest (Inner Will | Inner Fire)
            if (Id == 588 || Id == 73413)
                return SPELL_SPECIFIC_PRIEST_SANCTUM;

            // Priest chakras
            if (HasAttribute(SPELL_ATTR9_AFFECTED_BY_SPELLSWAP) || Id == 81209)
                return SPELL_SPECIFIC_CHAKRA;

            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // only hunter stings have this
            if (Dispel == DISPEL_POISON)
                return SPELL_SPECIFIC_STING;

            // only hunter aspects have this (but not all aspects in hunter family)
            if (SpellFamilyFlags.HasFlag(0x00380000, 0x00400000, 0x00001010))
                return SPELL_SPECIFIC_ASPECT;

            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (SpellFamilyFlags[0] & 0x01000002)
                return SPELL_SPECIFIC_BLESSING;

            // Collection of all the seal family flags. No other paladin spell has any of those.
            if (SpellFamilyFlags[1] & 0x80000000 // Seal of Justice
                || SpellFamilyFlags[1] & 0x20000000 // Seal of Righteoussness
                || SpellFamilyFlags[1] & 0x00000800  // Seal of Truth
                || SpellFamilyFlags[1] & 0x02000000) // Seal of Insigth
                return SPELL_SPECIFIC_SEAL;

            if (SpellFamilyFlags[0] & 0x00002190)
                return SPELL_SPECIFIC_HAND;

            // Judgement, Judgement of Truth, Judgement of Righteoussness, Judgement of Light
            if (Id == 20271 || Id == 31804 || Id == 20187 || Id == 54158)
                return SPELL_SPECIFIC_JUDGEMENT;

            // only paladin auras have this (for palaldin class family)
            if (SpellFamilyFlags[2] & 0x00000020)
                return SPELL_SPECIFIC_AURA;

            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // family flags 10 (Lightning), 42 (Earth), 37 (Water), proc shield from T2 8 pieces bonus
            if (SpellFamilyFlags[1] & 0x420
                || SpellFamilyFlags[0] & 0x00000400
                || Id == 23552)
                return SPELL_SPECIFIC_ELEMENTAL_SHIELD;

            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
            if (Id == 48266 || Id == 48263 || Id == 48265)
                return SPELL_SPECIFIC_PRESENCE;
            break;
    }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].Effect == SPELL_EFFECT_APPLY_AURA)
        {
            switch (Effects[i].ApplyAuraName)
            {
                case SPELL_AURA_MOD_CHARM:
                case SPELL_AURA_MOD_POSSESS_PET:
                case SPELL_AURA_MOD_POSSESS:
                case SPELL_AURA_AOE_CHARM:
                    return SPELL_SPECIFIC_CHARM;
                case SPELL_AURA_TRACK_CREATURES:
                case SPELL_AURA_TRACK_RESOURCES:
                case SPELL_AURA_TRACK_STEALTHED:
                    return SPELL_SPECIFIC_TRACKER;
            }
        }
    }

    return SPELL_SPECIFIC_NORMAL;
}

float SpellInfo::GetMinRange(bool positive) const
{
    if (!RangeEntry)
        return 0.0f;
    if (positive)
        return RangeEntry->minRangeFriend;
    return RangeEntry->minRangeHostile;
}

float SpellInfo::GetMaxRange(bool positive, Unit* caster, Spell* spell) const
{
    if (!RangeEntry)
        return 0.0f;
    float range;
    if (positive)
        range = RangeEntry->maxRangeFriend;
    else
        range = RangeEntry->maxRangeHostile;
    if (caster)
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(Id, SPELLMOD_RANGE, range, spell);
    return range;
}

int32 SpellInfo::GetDuration() const
{
    if (!DurationEntry)
        return 0;
    return (DurationEntry->Duration[0] == -1) ? -1 : abs(DurationEntry->Duration[0]);
}

int32 SpellInfo::GetMaxDuration() const
{
    if (!DurationEntry)
        return 0;
    return (DurationEntry->Duration[2] == -1) ? -1 : abs(DurationEntry->Duration[2]);
}

uint32 SpellInfo::CalcCastTime(Unit* caster, Spell* spell) const
{
    int32 castTime = 0;

    // not all spells have cast time index and this is all is pasiive abilities
    if (caster && CastTimeMax > 0)
    {
        castTime = CastTimeMax;
        if (CastTimeMaxLevel > int32(caster->getLevel()))
            castTime = CastTimeMin + int32(caster->getLevel() - 1) * (CastTimeMax - CastTimeMin) / (CastTimeMaxLevel - 1);
    }
    else if (CastTimeEntry)
        castTime = CastTimeEntry->CastTime;

    if (!castTime)
        return 0;
    
    if (caster)
        caster->ModSpellCastTime(this, castTime, spell);

    return (castTime > 0) ? uint32(castTime) : 0;
}

struct VisualIdStruct
{
    uint32 skill;
    uint32 visual;
    uint32 aura;
    uint32 casttime;
};

const VisualIdStruct cVIS[] =
{
    {185, 3881, 67556, 500},    // cooking with Chef Hat.
    {182, 91, 20552, 500},      // Cultivation (Tauren Racial) & Herbalism
    {393, 1008, 68978, 500}     // Flayer (Vorgen Racial) & Flayer
};

void SpellInfo::UpdateCastTimeForProfessionRacicalBoost(Unit const *caster, int32 &castTime) const
{
    if (!caster)
        return;
    
    int count = sizeof(cVIS) / sizeof(VisualIdStruct);
    for (int i = 0; i < count; ++i)
    {
        if (SpellVisual[0] != cVIS[i].visual)
            continue;
        
        if (!IsAbilityOfSkillType(cVIS[i].skill))
            return;
        
        if (!caster->HasAura(cVIS[i].aura))
            return;
        
        castTime = cVIS[i].casttime;
        return;
    }
}

uint32 SpellInfo::GetMaxTicks() const
{
    int32 DotDuration = GetDuration();
    if (DotDuration == 0)
        return 1;

    // 200% limit
    if (DotDuration > 30000)
        DotDuration = 30000;

    for (uint8 x = 0; x < MAX_SPELL_EFFECTS; x++)
    {
        if (Effects[x].Effect == SPELL_EFFECT_APPLY_AURA)
            switch (Effects[x].ApplyAuraName)
            {
                case SPELL_AURA_PERIODIC_DAMAGE:
                case SPELL_AURA_PERIODIC_HEAL:
                case SPELL_AURA_PERIODIC_LEECH:
                    if (Effects[x].Amplitude != 0)
                        return DotDuration / Effects[x].Amplitude;
                    break;
            }
    }

    return 6;
}

uint32 SpellInfo::GetRecoveryTime() const
{
    return RecoveryTime > CategoryRecoveryTime ? RecoveryTime : CategoryRecoveryTime;
}

uint32 SpellInfo::GetStackAmount(Unit *caster) const
{
    // Scent of Blood
    if (Id == 50421)
    {
        if (caster->HasAura(49004))
            return 1;
        else if (caster->HasAura(49508))
            return 2;
        else if (caster->HasAura(49509))
            return 3;
    }
    return StackAmount;
}

int32 SpellInfo::CalcPowerCost(Unit const* caster, SpellSchoolMask schoolMask) const
{
    // Spell drain all exist power on cast (Only paladin lay of Hands)
    if (AttributesEx & SPELL_ATTR1_DRAIN_ALL_POWER)
    {
        // If power type - health drain all
        if (PowerType == POWER_HEALTH)
            return caster->GetHealth();
        // Else drain all power
        if (PowerType < MAX_POWERS)
            return caster->GetPower(Powers(PowerType));

        return 0;
    }

    switch (Id)
    {
        case 98474: // Flame Scythe, Majordomo Staghelm, Firelands
        case 100212:
        case 100213:
        case 100214:
        case 98476: // Leaping Flames, Majordomo Staghelm, Firelands
            return 100;
        default:
            break;
    }

    // Base powerCost
    int32 powerCost = ManaCost;
    // PCT cost from total amount
    if (ManaCostPercentage)
    {
        switch (PowerType)
        {
            // health as power used
            case POWER_HEALTH:
                powerCost += int32(CalculatePct(caster->GetCreateHealth(), ManaCostPercentage));
                break;
            case POWER_MANA:
                powerCost += int32(CalculatePct(caster->GetCreateMana(), ManaCostPercentage));
                break;
            case POWER_RAGE:
            case POWER_FOCUS:
            case POWER_ENERGY:
                powerCost += int32(CalculatePct(caster->GetMaxPower(Powers(PowerType)), ManaCostPercentage));
                break;
            case POWER_RUNES:
            case POWER_RUNIC_POWER:
                break;
            default:
                return 0;
        }
    }
    SpellSchools school = GetFirstSchoolInMask(schoolMask);
    // Flat mod from caster auras by spell school
    powerCost += caster->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + school);
    // Shiv - costs 20 + weaponSpeed*10 energy (apply only to non-triggered spell with energy cost)
    if (AttributesEx4 & SPELL_ATTR4_SPELL_VS_EXTEND_COST)
        powerCost += caster->GetAttackTime(OFF_ATTACK) / 100;
    // Apply cost mod by spell
    if (Player* modOwner = caster->GetSpellModOwner())
        modOwner->ApplySpellMod(Id, SPELLMOD_COST, powerCost);

    if (Attributes & SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION)
        powerCost = int32(powerCost / (1.117f * SpellLevel / caster->getLevel() -0.1327f));

    // PCT mod from user auras by school
    powerCost = int32(powerCost * (1.0f + caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + school)));
    if (powerCost < 0)
        powerCost = 0;
    return powerCost;
}

bool SpellInfo::IsRanked() const
{
    return ChainEntry != NULL;
}

uint8 SpellInfo::GetRank() const
{
    if (!ChainEntry)
        return 1;
    return ChainEntry->rank;
}

SpellInfo const* SpellInfo::GetFirstRankSpell() const
{
    if (!ChainEntry)
        return this;
    return ChainEntry->first;
}
SpellInfo const* SpellInfo::GetLastRankSpell() const
{
    if (!ChainEntry)
        return NULL;
    return ChainEntry->last;
}
SpellInfo const* SpellInfo::GetNextRankSpell() const
{
    if (!ChainEntry)
        return NULL;
    return ChainEntry->next;
}
SpellInfo const* SpellInfo::GetPrevRankSpell() const
{
    if (!ChainEntry)
        return NULL;
    return ChainEntry->prev;
}

SpellInfo const* SpellInfo::GetAuraRankForLevel(uint8 level) const
{
    // ignore passive spells
    if (IsPassive())
        return this;

    bool needRankSelection = false;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (IsPositiveEffect(i) &&
            (Effects[i].Effect == SPELL_EFFECT_APPLY_AURA ||
            Effects[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA_PARTY ||
            Effects[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID) &&
            !Effects[i].ScalingMultiplier)
        {
            needRankSelection = true;
            break;
        }
    }

    // not required
    if (!needRankSelection)
        return this;

    for (SpellInfo const* nextSpellInfo = this; nextSpellInfo != NULL; nextSpellInfo = nextSpellInfo->GetPrevRankSpell())
    {
        // if found appropriate level
        if (uint32(level + 10) >= nextSpellInfo->SpellLevel)
            return nextSpellInfo;

        // one rank less then
    }

    // not found
    return NULL;
}

bool SpellInfo::IsRankOf(SpellInfo const* spellInfo) const
{
    return GetFirstRankSpell() == spellInfo->GetFirstRankSpell();
}

bool SpellInfo::IsDifferentRankOf(SpellInfo const* spellInfo) const
{
    if (Id == spellInfo->Id)
        return false;
    return IsRankOf(spellInfo);
}

bool SpellInfo::IsHighRankOf(SpellInfo const* spellInfo) const
{
    if (ChainEntry && spellInfo->ChainEntry)
    {
        if (ChainEntry->first == spellInfo->ChainEntry->first)
            if (ChainEntry->rank > spellInfo->ChainEntry->rank)
                return true;
    }
    return false;
}

uint32 SpellInfo::_GetExplicitTargetMask() const
{
    bool srcSet = false;
    bool dstSet = false;
    uint32 targetMask = Targets;
    // prepare target mask using effect target entries
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (!Effects[i].IsEffect())
            continue;
        targetMask |= Effects[i].TargetA.GetExplicitTargetMask(srcSet, dstSet);
        targetMask |= Effects[i].TargetB.GetExplicitTargetMask(srcSet, dstSet);

        // add explicit target flags based on spell effects which have EFFECT_IMPLICIT_TARGET_EXPLICIT and no valid target provided
        if (Effects[i].GetImplicitTargetType() != EFFECT_IMPLICIT_TARGET_EXPLICIT)
            continue;

        // extend explicit target mask only if valid targets for effect could not be provided by target types
        uint32 effectTargetMask = Effects[i].GetMissingTargetMask(srcSet, dstSet, targetMask);

        // don't add explicit object/dest flags when spell has no max range
        if (GetMaxRange(true) == 0.0f && GetMaxRange(false) == 0.0f)
            effectTargetMask &= ~(TARGET_FLAG_UNIT_MASK | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_MASK | TARGET_FLAG_DEST_LOCATION);
        targetMask |= effectTargetMask;
    }
    return targetMask;
}
bool SpellInfo::IsAllwaysStackModifers() const
{
    if (IsPassive())
        return true;

    if (SpellFamilyName == SPELLFAMILY_POTION)
        return true;

    switch (Id)
    {
        // warsong flags
        case 23333:
        case 23335:

        case 16191: // mana tide buff 
            return true;
       default:
            break;
    }
    return false;
}
bool SpellInfo::_IsPositiveEffect(uint8 effIndex, bool deep) const
{
    // not found a single positive spell with this attribute
    if (Attributes & SPELL_ATTR0_NEGATIVE_1)
        return false;

    switch (SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (Id)
            {
                case 29214: // Wrath of the Plaguebringer
                case 34700: // Allergic Reaction
                case 54836: // Wrath of the Plaguebringer
                case 61987: // Avenging Wrath Marker
                case 61988: // Divine Shield exclude aura
                case 69708: // Ice Prison (HoR)
                case 70194: // Dark Arrow (HoR)
                case 70070: // Harvest Soul (HoR)
                    return false;
                case 30877: // Tag Murloc
                case 61716: // Rabbit Costume
                case 61734: // Noblegarden Bunny
                case 62344: // Fists of Stone
                case 62478: // Frozen Blows (Hodir)
                case 63512: // Frozen Blows (Hodir)
                    return true;
                default:
                    break;
            }
            break;
        case SPELLFAMILY_WARLOCK:
            // Immolation Aura
            if (Id == 50589)
                return true;
            break;
        case SPELLFAMILY_MAGE:
            // Amplify Magic, Dampen Magic
            if (SpellFamilyFlags[0] == 0x00002000)
                return true;
            // Ignite
            if (SpellIconID == 45)
                return true;
            break;
        case SPELLFAMILY_PRIEST:
            switch (Id)
            {
                case 64844: // Divine Hymn
                case 64904: // Hymn of Hope
                case 47585: // Dispersion
                case 87195: // Paralysis
                case 87192:
                    return true;
                case 47666: // Penance: damage
                    return false;
                default:
                    break;
            }
            break;
        case SPELLFAMILY_HUNTER:
            // Aspect of the Viper
            if (Id == 34074)
                return true;
            break;
        case SPELLFAMILY_SHAMAN:
            if (Id == 30708)
                return false;
            break;
        case SPELLFAMILY_ROGUE:
            switch (Id)
            {
                // Envenom must be considered as a positive effect even though it deals damage
                case 32645:     // Envenom (Rank 1)
                case 32684:     // Envenom (Rank 2)
                case 57992:     // Envenom (Rank 3)
                case 57993:     // Envenom (Rank 4)
                    return true;
                default:
                    break;
            }
            break;
        case SPELLFAMILY_DEATHKNIGHT:
            // Unholy Frenzy
            if (Id == 49016)
                return true;
            break;
        default:
            break;
    }

    switch (Mechanic)
    {
        case MECHANIC_IMMUNE_SHIELD:
            return true;
        default:
            break;
    }

    // Special case: effects which determine positivity of whole spell
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].IsAura() && Effects[i].ApplyAuraName == SPELL_AURA_MOD_STEALTH)
            return true;
    }

    switch (Effects[effIndex].Effect)
    {
        case SPELL_EFFECT_DUMMY:
            // some explicitly required dummy effect sets
            switch (Id)
            {
                case 28441:
                    return false; // AB Effect 000
                default:
                    break;
            }
            break;
        // always positive effects (check before target checks that provided non-positive result in some case for positive effects)
        case SPELL_EFFECT_HEAL:
        case SPELL_EFFECT_LEARN_SPELL:
        case SPELL_EFFECT_SKILL_STEP:
        case SPELL_EFFECT_HEAL_PCT:
        case SPELL_EFFECT_ENERGIZE_PCT:
            return true;
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
            return false;

            // non-positive aura use
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        {
            switch (Effects[effIndex].ApplyAuraName)
            {
                case SPELL_AURA_MOD_DAMAGE_DONE:            // dependent from bas point sign (negative -> negative)
                case SPELL_AURA_MOD_STAT:
                case SPELL_AURA_MOD_SKILL:
                case SPELL_AURA_MOD_DODGE_PERCENT:
                case SPELL_AURA_MOD_HEALING_PCT:
                case SPELL_AURA_MOD_HEALING_DONE:
                case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
                    if (Effects[effIndex].CalcValue() < 0)
                        return false;
                    break;
                case SPELL_AURA_MOD_DAMAGE_TAKEN:           // dependent from bas point sign (positive -> negative)
                    if (Effects[effIndex].CalcValue() > 0)
                        return false;
                    break;
                case SPELL_AURA_MOD_CRIT_PCT:
                case SPELL_AURA_MOD_SPELL_CRIT_CHANCE:
                    if (Effects[effIndex].CalcValue() > 0)
                        return true;                        // some expected positive spells have SPELL_ATTR1_NEGATIVE
                    break;
                case SPELL_AURA_ADD_TARGET_TRIGGER:
                    return true;
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                    if (!deep)
                    {
                        if (SpellInfo const* spellTriggeredProto = sSpellMgr->GetSpellInfo(Effects[effIndex].TriggerSpell))
                        {
                            // negative targets of main spell return early
                            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                            {
                                if (!spellTriggeredProto->Effects[i].Effect)
                                    continue;
                                // if non-positive trigger cast targeted to positive target this main cast is non-positive
                                // this will place this spell auras as debuffs
                                if (_IsPositiveTarget(spellTriggeredProto->Effects[i].TargetA.GetTarget(), spellTriggeredProto->Effects[i].TargetB.GetTarget()) && !spellTriggeredProto->_IsPositiveEffect(i, true))
                                    return false;
                            }
                        }
                    }
                case SPELL_AURA_PROC_TRIGGER_SPELL:
                    // many positive auras have negative triggered spells at damage for example and this not make it negative (it can be canceled for example)
                    if (this->IsPassive())
                        return true;
                    break;
                case SPELL_AURA_MOD_STUN:                   //have positive and negative spells, we can't sort its correctly at this moment.
                    if (effIndex == 0 && Effects[1].Effect == 0 && Effects[2].Effect == 0)
                        return false;                       // but all single stun aura spells is negative
                    break;
                case SPELL_AURA_MOD_PACIFY_SILENCE:
                    if (Id == 24740)             // Wisp Costume
                        return true;
                    return false;
                case SPELL_AURA_MOD_ROOT:
                case SPELL_AURA_MOD_SILENCE:
                case SPELL_AURA_GHOST:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_MOD_STALKED:
                case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                case SPELL_AURA_PREVENT_RESURRECTION:
                    return false;
                case SPELL_AURA_PERIODIC_DAMAGE:            // used in positive spells also.
                    // part of negative spell if casted at self (prevent cancel)
                    if (Effects[effIndex].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                        return false;
                    break;
                case SPELL_AURA_MOD_DECREASE_SPEED:         // used in positive spells also
                    // part of positive spell if casted at self
                    if (Effects[effIndex].TargetA.GetTarget() != TARGET_UNIT_CASTER)
                        return false;
                    // but not this if this first effect (didn't find better check)
                    if (Attributes & SPELL_ATTR0_NEGATIVE_1 && effIndex == 0)
                        return false;
                    break;
                case SPELL_AURA_MECHANIC_IMMUNITY:
                {
                    // non-positive immunities
                    switch (Effects[effIndex].MiscValue)
                    {
                        case MECHANIC_BANDAGE:
                        case MECHANIC_SHIELD:
                        case MECHANIC_MOUNT:
                        case MECHANIC_INVULNERABILITY:
                            return false;
                        default:
                            break;
                    }
                    break;
                }
                case SPELL_AURA_ADD_FLAT_MODIFIER:          // mods
                case SPELL_AURA_ADD_PCT_MODIFIER:
                {
                    // non-positive mods
                    switch (Effects[effIndex].MiscValue)
                    {
                        case SPELLMOD_COST:                 // dependent from bas point sign (negative -> positive)
                            if (Effects[effIndex].CalcValue() > 0)
                            {
                                if (!deep)
                                {
                                    bool negative = true;
                                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                    {
                                        if (i != effIndex)
                                            if (_IsPositiveEffect(i, true))
                                            {
                                                negative = false;
                                                break;
                                            }
                                    }
                                    if (negative)
                                        return false;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    // non-positive targets
    if (!_IsPositiveTarget(Effects[effIndex].TargetA.GetTarget(), Effects[effIndex].TargetB.GetTarget()))
        return false;

    // negative spell if triggered spell is negative
    if (!deep && !Effects[effIndex].ApplyAuraName && Effects[effIndex].TriggerSpell)
    {
        if (SpellInfo const* spellTriggeredProto = sSpellMgr->GetSpellInfo(Effects[effIndex].TriggerSpell))
            if (!spellTriggeredProto->_IsPositiveSpell())
                return false;
    }

    // ok, positive
    return true;
}

bool SpellInfo::_IsPositiveSpell() const
{
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (!_IsPositiveEffect(i, true))
            return false;
    return true;
}

bool SpellInfo::_IsPositiveTarget(uint32 targetA, uint32 targetB)
{
    // non-positive targets
    switch (targetA)
    {
        case TARGET_UNIT_NEARBY_ENEMY:
        case TARGET_UNIT_TARGET_ENEMY:
        case TARGET_UNIT_SRC_AREA_ENEMY:
        case TARGET_UNIT_DEST_AREA_ENEMY:
        case TARGET_UNIT_CONE_ENEMY_24:
        case TARGET_UNIT_CONE_ENEMY_104:
        case TARGET_DEST_DYNOBJ_ENEMY:
        case TARGET_DEST_TARGET_ENEMY:
            return false;
        default:
            break;
    }
    if (targetB)
        return _IsPositiveTarget(targetB, 0);
    return true;
}

bool SpellInfo::_IsCrowdControl(uint8 effMask, bool nodamage) const
{
    for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        if (effMask && !(effMask & (1 << effIndex)))
            continue;

        if (nodamage)
        {
            switch (Effects[effIndex].Effect)
            {
                case SPELL_EFFECT_HEALTH_LEECH:
                case SPELL_EFFECT_SCHOOL_DAMAGE:
                    return false;
            }
        }

        switch (Effects[effIndex].ApplyAuraName)
        {
            case SPELL_AURA_MOD_STUN:
                switch (SpellIconID)
                {
                    case 15:
                    case 457:
                        return false;
                        break;
                    default:
                        break;
                }
            case SPELL_AURA_MOD_CONFUSE:
            case SPELL_AURA_MOD_FEAR:
            case SPELL_AURA_MOD_ROOT:
            case SPELL_AURA_TRANSFORM:
                if (!IsPositive())
                    return true;
                break;
            default: break;
        }
    }
    return false;
}

bool SpellInfo::_IsNeedDelay() const
{
    switch (Id)
    {
        case 379:   // Earth Shield (Shaman)
        case 14157: // Ruthlessness (Rogue)
        case 33110: // Prayer of Mending (Priest)
        case 48503: // Living Seed (Druid)
        case 52752: // Ancestral Awakening (Shaman)
        case 62836: // Slag Imbued (Ignis, Ulduar)
        case 63536: // Slag Imbued (Ignis, Ulduar)
        case 70802: // Mayhem (Rogue)
        case 70157: // Ice Tomb (Sindragosa, ICC)
        case 79128: // Improved Expose Armor
        case 36032: // Arcane Blast
        case 109950: // Fury of Destroyer
        case 63853: // Rapture cooldown for Rapture Trick
            return true;
    }

    // Flurry (Shaman)
    if (SpellFamilyName == SPELLFAMILY_SHAMAN && SpellIconID == 108)
        return true;

    return false;
}

bool SpellInfo::IsBreakingCamouflage() const
{
    // This is bad but I dont't see another way
    // I cannot check spells using any mask
    // Use it only for hunter camouflage
    switch (GetSpellSpecific())
    {
        case SPELL_SPECIFIC_FOOD:
        case SPELL_SPECIFIC_FOOD_AND_DRINK:
        case SPELL_SPECIFIC_WELL_FED:
        case SPELL_SPECIFIC_ASPECT:
            return false;
    }

    switch (Id)
    {
        case 136: // Mend Pet
        case 982: // Revive Pet
        case 1130: // Hunter's Mark
        case 1462: // Beast Lore
        case 1499: // Frost Trap
        case 1543: // Flare
        case 3045: // Rapid Fire
        case 5384: // Feign Death
        case 6197: // Eagle Eye
        case 6991: // Feed Pet
        case 13795: // Immolation Trap
        case 13809: // Ice Trap
        case 13813: // Explosive Trap
        case 19263: // Deterence
        case 19434: // Aimed Shot
        case 23989: // Readiness
        case 26297: // Berserking (Troll Racial)
        case 34477: // Misdirection
        case 34600: // Snake Trap
        case 42292: // PvP Trinket
        case 53271: // Master's Call
        case 56641: // Steady Shot
        case 60192: // Frost Trap launcher
        case 77767: // Cobra Shot
        case 77769: // Trap Launcher
        case 82935: // Immplation Trap launcher
        case 82939: // Explosive Trap launcher
        case 82941: // Ice Trap launcher
        case 82945: // Immolation Trap Launcher
        case 82948: // Snake Trap launcher
        case 93435: // Roar of Courage (Special Ability)
        case 105132:
            return false;
    }

    return true;
}

bool SpellInfo::IsIgnoringCombat() const
{
    if (HasAttribute(SPELL_ATTR4_DAMAGE_DOESNT_BREAK_AURAS))
        return true;

    switch (Id)
    {
        // Slice and Dice
        case 5171:
        case 6434:
        case 6774:
        // Honor Among Thieves
        case 51698:
        case 51699:
        case 51700:
        case 51701:
        case 52916:
        // Divine Hymn
        case 64843:
        case 64844:
        case 70619:
        // Shadowmeld
        case 58984:
        // Blackjack
        case 79124:
        case 79126:
        // Redirect
        case 73981:
        // Venomous Wounds
        case 79136:
        // Master Poisoner
        case 93068:
        // Blind
        case 2094:
        // Ignite
        case 12654:
        // Resistance is Futile
        case 83676:
        // Vendeta
        case 79140:
        // Earth Shield
        case 379:
        // Earthliving Weapon proc
        case 51730:
        // Ancestral Vigor
        case 105284:
        // Ancestral Fortitude
        case 16177:
        case 16236:
            return true;
        default: break;
    }
    
    // Dark Intent
    if (SpellFamilyName == SPELLFAMILY_WARLOCK && (SpellFamilyFlags[2] & 0x00800000))
        return true;
    
    return false;
}

bool SpellInfo::IsCanBeStolen() const
{    
    // some of the rules for those spells that can be stolen by Dark Simulacrum
    // spells should use mana
    if (PowerType != POWER_MANA)
        return false;

    // and should have mana cost
    if (!ManaCost && !ManaCostPercentage)
        return false;

    // special rules
    switch (Id)
    {
        case 633: // Lay on Hands
        case 22812: // Barkskin
        case 24275: // Hammer of Wrath
        case 31935: // Avenger's Shield
        case 53563: // Beaconf of the Light
            return false;
            break;
        default:
            break;
    }

    for (uint8 x = 0; x < MAX_SPELL_EFFECTS; ++x)
    {
        switch (Effects[x].Effect)
        {
            case SPELL_EFFECT_SUMMON:
            case SPELL_EFFECT_SUMMON_PET:
            case SPELL_EFFECT_CAST_BUTTON:
            case SPELL_EFFECT_TAMECREATURE:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            case SPELL_EFFECT_KNOCK_BACK:
                return false;
                break;
            case SPELL_EFFECT_SCHOOL_DAMAGE:
                if (DmgClass == SPELL_DAMAGE_CLASS_MELEE)
                    return false;
                break;
            default:
                break;
        }
        switch (Effects[x].ApplyAuraName)
        {
            case SPELL_AURA_MOD_SHAPESHIFT:
                return false;
                break;
            default:
                break;
        }
    }

    return true;
}

bool SpellInfo::IsNeedAdditionalLosChecks() const
{
    if (HasEffect(SPELL_EFFECT_KNOCK_BACK))
        return true;

    switch(Id)
    {
        case 3600:  // Earthbind Totem
        case 50622: 
        case 44949: // Whirlwind from bladestorm
        case 23455: // Holy Nova
        case 87204: // Sin and Punishment proc
            return true;
        default:break;
    }

    return false;
}

bool SpellInfo::IsRequireAdditionalTargetCheck() const
{
    switch (Id)
    {
        case 2812: // Holy Wrath
            return false;
        case 103327: // Hour of Twilight dmg, Ultraxion, Dragon Soul
            return false;
        default:
            break;
    }

    return true;
}

bool SpellInfo::IsNeedToCheckSchoolImmune() const
{
    // there are a problem with some of spells that does holypower or break cc kinda pvp trinket
    // so we need hack-handle for em

    switch (Id)
    {
        case 42292: // Pvp Trinket
        case 59752: // Every Man for Himself (racical)
        case 25912: // Holy Shock damage
        case 35395: // Crusader Strike
        case 65547: // PvP Trinket, Trial of the Crusader
            return false;
        default:
            break;
    }
    return true;
}

bool SpellInfo::IsShouldProcOnOwner() const
{
    switch (Id)
    {
        case 6358:  // Seduction -> Glyph of Seduction
        case 82739: // Fire Orb -> Ignite
        case 83381: // Kill Command! -> Improved Kill Command
        case 95969: // Frostfire Orb rank 1 -> Fingers of Frost
        case 84721: // Frostfire Orb rank 2 -> Fingers of Frost
        case 3606: // Searing Arrow -> Searing Flames
            return true;
    }

    return false;
}

bool SpellInfo::IsBreakingStealth(Unit* m_caster) const
{
    switch (GetSpellSpecific())
    {
        case SPELL_SPECIFIC_FOOD:
        case SPELL_SPECIFIC_FOOD_AND_DRINK:
        case SPELL_SPECIFIC_WELL_FED:
            return true;
    }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].ApplyAuraName == SPELL_AURA_MOUNTED)
            return true;
    }

    switch(Id)
    {
        case 3600:  // Earthbind Totem
        case 99:    // Demoralizing Roar
        case 50256:
        case 94528: // Flare
        case 76577: // Smoke Bomb
            return false;
        default:
            break;
    }

    if (IsTargetingArea())
    {
        // dispel etc spells
        switch(Effects[EFFECT_0].Effect)
        {
            case SPELL_EFFECT_DISPEL:
            case SPELL_EFFECT_DISPEL_MECHANIC:
            case SPELL_EFFECT_THREAT:
            case SPELL_EFFECT_MODIFY_THREAT_PERCENT:
            case SPELL_EFFECT_DISTRACT:
                return false;
            default:
                break;
        }
    }

    if (HasAttribute(SPELL_ATTR4_DAMAGE_DOESNT_BREAK_AURAS) || HasAttribute(SPELL_ATTR1_NOT_BREAK_STEALTH))
        return false;

    return true;
}

bool SpellInfo::IsPeriodicHeal() const
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_HEAL)
            return true;
    }

    return false;
}

bool SpellInfo::IsInterruptSpell() const
{
    switch (Id)
    {
        case 78675: 
        case 97547: // Solar Beam
        case 80964:
        case 93985: 
        case 80965: // Skull Bash
            return true;
    default:
        break;
    }
    if (HasEffect(SPELL_EFFECT_INTERRUPT_CAST) || HasAura(SPELL_AURA_MOD_SILENCE))
        return true;
    return false;
}

bool SpellInfo::IsBreakingCamouflageAfterHit() const
{
    // Traps
    if (SpellFamilyFlags[1] & 0x8002000 ||
        SpellFamilyFlags[2] & 0x20000)
        return true;

    // Damage casts
    switch (Id)
    {
        case 19434: // Aimed Shot
        case 82928: // Aimed Shot
        case 77767: // Cobra Shot
        case 56641: // Steady Shot
            return true;
    }

    return false;
}

SpellTargetRestrictionsEntry const* SpellInfo::GetSpellTargetRestrictions() const
{
    return SpellTargetRestrictionsId ? sSpellTargetRestrictionsStore.LookupEntry(SpellTargetRestrictionsId) : NULL;
}

SpellEquippedItemsEntry const* SpellInfo::GetSpellEquippedItems() const
{
    return SpellEquippedItemsId ? sSpellEquippedItemsStore.LookupEntry(SpellEquippedItemsId) : NULL;
}

SpellInterruptsEntry const* SpellInfo::GetSpellInterrupts() const
{
    return SpellInterruptsId ? sSpellInterruptsStore.LookupEntry(SpellInterruptsId) : NULL;
}

SpellLevelsEntry const* SpellInfo::GetSpellLevels() const
{
    return SpellLevelsId ? sSpellLevelsStore.LookupEntry(SpellLevelsId) : NULL;
}

SpellPowerEntry const* SpellInfo::GetSpellPower() const
{
    return SpellPowerId ? sSpellPowerStore.LookupEntry(SpellPowerId) : NULL;
}

SpellReagentsEntry const* SpellInfo::GetSpellReagents() const
{
    return SpellReagentsId ? sSpellReagentsStore.LookupEntry(SpellReagentsId) : NULL;
}

SpellScalingEntry const* SpellInfo::GetSpellScaling() const
{
    return SpellScalingId ? sSpellScalingStore.LookupEntry(SpellScalingId) : NULL;
}

SpellShapeshiftEntry const* SpellInfo::GetSpellShapeshift() const
{
    return SpellShapeshiftId ? sSpellShapeshiftStore.LookupEntry(SpellShapeshiftId) : NULL;
}

SpellTotemsEntry const* SpellInfo::GetSpellTotems() const
{
    return SpellTotemsId ? sSpellTotemsStore.LookupEntry(SpellTotemsId) : NULL;
}

SpellAuraOptionsEntry const* SpellInfo::GetSpellAuraOptions() const
{
    return SpellAuraOptionsId ? sSpellAuraOptionsStore.LookupEntry(SpellAuraOptionsId) : NULL;
}

SpellAuraRestrictionsEntry const* SpellInfo::GetSpellAuraRestrictions() const
{
    return SpellAuraRestrictionsId ? sSpellAuraRestrictionsStore.LookupEntry(SpellAuraRestrictionsId) : NULL;
}

SpellCastingRequirementsEntry const* SpellInfo::GetSpellCastingRequirements() const
{
    return SpellCastingRequirementsId ? sSpellCastingRequirementsStore.LookupEntry(SpellCastingRequirementsId) : NULL;
}

SpellCategoriesEntry const* SpellInfo::GetSpellCategories() const
{
    return SpellCategoriesId ? sSpellCategoriesStore.LookupEntry(SpellCategoriesId) : NULL;
}

SpellClassOptionsEntry const* SpellInfo::GetSpellClassOptions() const
{
    return SpellClassOptionsId ? sSpellClassOptionsStore.LookupEntry(SpellClassOptionsId) : NULL;
}

SpellCooldownsEntry const* SpellInfo::GetSpellCooldowns() const
{
    return SpellCooldownsId ? sSpellCooldownsStore.LookupEntry(SpellCooldownsId) : NULL;
}

void SpellInfo::SetDurationIndex(uint32 index)
{
    SpellDurationEntry const* durationIndex = sSpellDurationStore.LookupEntry(index);
    if (!durationIndex)
        return;

    DurationEntry = durationIndex;
}

void SpellInfo::SetRangeIndex(uint32 index)
{
    SpellRangeEntry const* rangeIndex = sSpellRangeStore.LookupEntry(index);
    if (!rangeIndex)
        return;

    RangeEntry = rangeIndex;
}

void SpellInfo::SetCastTimeIndex(uint32 index)
{
    SpellCastTimesEntry const* castTimeIndex = sSpellCastTimesStore.LookupEntry(index);
    if (!castTimeIndex)
        return;

    CastTimeEntry = castTimeIndex;
}

void SpellInfo::_UnloadImplicitTargetConditionLists()
{
    // find the same instances of ConditionList and delete them.
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        ConditionList* cur = Effects[i].ImplicitTargetConditions;
        if (!cur)
            continue;
        for (uint8 j = i; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (Effects[j].ImplicitTargetConditions == cur)
                Effects[j].ImplicitTargetConditions = NULL;
        }
        delete cur;
    }
}

#ifndef DEF_FIRELANDS_H
#define DEF_FIRELANDS_H

#include "ObjectMgr.h"
#define FLScriptName "instance_firelands"

enum Data
{
    DATA_SHANNOX    = 0,
    DATA_RHYOLITH   = 1,
    DATA_BETHTILAC  = 2,
    DATA_ALYSRAZOR  = 3,
    DATA_BALEROC    = 4,
    DATA_STAGHELM   = 5,
    DATA_RAGNAROS   = 6,
    DATA_RIPLIMB    = 7,
    DATA_RAGEFACE   = 8,
    
    // ragnaros from uwow
    DATA_INTRO_EVENT,
    DATA_RAGNAROS_EVENT,
};

enum CreatureIds
{
    NPC_SHANNOX                     = 53691,
    NPC_RAGEFACE                    = 53695, 
    NPC_RIPLIMB                     = 53694,
    NPC_RHYOLITH                    = 52558,
    NPC_BETHTILAC                   = 52498,
    NPC_ALYSRAZOR                   = 52530, 
    NPC_BALEROC                     = 53494,
    NPC_STAGHELM                    = 52571,
    NPC_RAGNAROS_CATA              = 52409,
	
    //other npc's
    NPC_SULFURAS            = 53420,    //ragnaros boss weapon
    NPC_LIVING_METEOR       = 53500,
    NPC_MOLTEN_ELEMENTAR    = 53189,
    
    // alysrazor event
    NPC_BLAZING_MONSTROSITY_LEFT    = 53786,
    NPC_BLAZING_MONSTROSITY_RIGHT   = 53791,
    NPC_EGG_PILE                    = 53795,
    NPC_HARBINGER_OF_FLAME          = 53793,
    NPC_MOLTEN_EGG_TRASH            = 53914,
    NPC_SMOULDERING_HATCHLING       = 53794, 
	
    // Ragnaros from uwow
    NPC_ENGULFING_FLAMES_TRIGGER    = 53485,
    NPC_SPLITTING_BLOW_TRIGGER      = 53393,
    NPC_MAGMA_POOL_TRIGGER          = 53729,
    NPC_PLATFORM_TRIGGER            = 53952,
    NPC_MAGMA_TRAP                  = 53086,
    NPC_SULFURAS_HAMMER             = 53420,
    NPC_SULFURAS_FLAME_WALL         = 38327,
    NPC_SULFURAS_SMASH_TARGET_1     = 53268,
    NPC_LAVA_WAVE                   = 53363,
    NPC_SON_OF_FLAME                = 53140,
    NPC_MOLTEN_SEED                 = 53186,
    NPC_LAVA_SCION                  = 53231,
};

enum GameobjectIds
{
    GO_CIRCLE_OF_THORNS_PORTAL  = 209137,
    GO_CIRCLE_OF_THORNS_PORTAL2 = 209346,

    GO_BRIDGE_OF_RHYOLITH       = 209255,
    GO_FIRE_WALL_BALEROC        = 209066,
    GO_RAID_BRIDGE_FORMING      = 209277,
    GO_RAGNAROS_FLOOR           = 208835,
    GO_STICKY_WEB               = 208877,
    GO_MOLTEN_METEOR            = 208966,
    GO_FIRE_WALL_FENDRAL        = 208906,
    GO_CACHE_OF_THE_FIRELORD    = 208967,
    GO_CACHE_OF_THE_FIRELORD_H  = 209261,

    // quest
    GO_BRANCH_OF_NORDRASSIL     = 209100,
    GO_RHYOLITH_FRAGMENT        = 209033,
    GO_EMBERSTONE_FRAGMENT      = 209035,
    GO_PYRESHELL_FRAGMENT       = 209036,
    GO_OBSIDIAN_FLECKED_CHITIN  = 209037,
};

enum MovePoints
{
    POINT_HAMMER,
    POINT_RAGNAROS_DOWN,         //end of each phase
    POINT_RAGNAROS_UP,           //start of each next phase
    POINT_RAGNAROS_STANDUP,      //only on heroic mode
    POINT_SULFURAS_SMASH,        //target for smashes
};

enum Actions
{
    //Ragnaros
    ACTION_RAGNAROS_DOWN,        //end of each phase
    ACTION_RAGNAROS_UP,          //start of each next phase
    ACTION_RAGNAROS_STANDUP,     //only on heroic mode
    ACTION_SULFURAS_SMASH,       //target for smashes
};

#endif

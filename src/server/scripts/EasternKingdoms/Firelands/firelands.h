#ifndef DEF_FIRELANDS_H
#define DEF_FIRELANDS_H

#define FLScriptName "instance_firelands"

enum Data
{
    DATA_SHANNOX                = 0,
    DATA_RHYOLITH               = 1,
    DATA_BETHTILAC              = 2,
    DATA_ALYSRAZOR              = 3,
    DATA_BALEROC                = 4,
    DATA_STAGHELM               = 5,
    DATA_RAGNAROS               = 6,
    DATA_RIPLIMB                = 7,
    DATA_RAGEFACE               = 8,
    DATA_RHYOLITH_HEALTH_SHARED = 9,
    DATA_EVENT                  = 10,
    DATA_RAGNAROS_FLOOR         = 11,
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
    NPC_RAGNAROS                    = 52409,
    
    // alysrazor event
    NPC_BLAZING_MONSTROSITY_LEFT    = 53786,
    NPC_BLAZING_MONSTROSITY_RIGHT   = 53791,
    NPC_EGG_PILE                    = 53795,
    NPC_HARBINGER_OF_FLAME          = 53793,
    NPC_MOLTEN_EGG_TRASH            = 53914,
    NPC_SMOULDERING_HATCHLING       = 53794,

    NPC_CIRCLE_OF_THRONES_PORTAL    = 54247,
};

enum GameobjectIds
{
    GO_CIRCLE_OF_THORNS_PORTAL1 = 209137,
    GO_CIRCLE_OF_THORNS_PORTAL2 = 209346,
    GO_CIRCLE_OF_THORNS_PORTAL3 = 209098,

    GO_BRIDGE_OF_RHYOLITH       = 209255,
    GO_FIRE_WALL_BALEROC        = 209066,
    GO_RAID_BRIDGE_FORMING      = 209277,
    GO_RAGNAROS_FLOOR           = 208835,
    GO_STICKY_WEB               = 208877,
    GO_MOLTEN_METEOR            = 208966,
    GO_FIRE_WALL_FANDRAL_1      = 208906,
    GO_FIRE_WALL_FANDRAL_2      = 208873,
    GO_SULFURON_KEEP            = 209073,
    GO_CACHE_OF_THE_FIRELORD_10 = 208967,
    GO_CACHE_OF_THE_FIRELORD_25 = 209261,
};

enum QuestDefines
{
    // quest
    GO_BRANCH_OF_NORDRASSIL                     = 209100,

    // Cannot find rhyolith fragment, summon it manualy
    SPELL_CREATE_EMBERSTONE_FRAGMENT            = 100518,
    SPELL_CREATE_CHITINOUS_FRAGMENT             = 100520,
    SPELL_CREATE_PYRESHELL_FRAGMENT             = 100519,

    SPELL_CHARGED_RHYOLITH_FOCUS                = 100481,
    SPELL_CHARGED_EMBERSTONE_FOCUS              = 100499,
    SPELL_CHARGED_CHITINOUS_FOCUS               = 100501,
    SPELL_CHARGED_PYRESHELL_FOCUS               = 100500,

    SPELL_TRANSFORM_CHARGED_RHYOLITH_FOCUS      = 100477,
    SPELL_TRANSFORM_CHARGED_EMBERSTONE_FOCUS    = 100496,
    SPELL_TRANSFORM_CHARGED_CHITINOUS_FOCUS     = 100498,
    SPELL_TRANSFORM_CHARGED_PYRESHELL_FOCUS     = 100497,

    GO_RHYOLITH_FRAGMENT                        = 209033,
    GO_EMBERSTONE_FRAGMENT                      = 209035,
    GO_PYRESHELL_FRAGMENT                       = 209036,
    GO_OBSIDIAN_FLECKED_CHITIN                  = 209037,

    NPC_DULL_RHYOLITH_FOCUS                     = 53951,
    NPC_DULL_EMBERSTONE_FOCUS                   = 53968,
    NPC_DULL_CHITINOUS_FOCUS                    = 53970,
    NPC_DULL_PYRESHELL_FOCUS                    = 53963,

    NPC_CHARGED_RHYOLITH_FOCUS                  = 53955,
    NPC_CHARGED_EMBERSTONE_FOCUS                = 53969,
    NPC_CHARGED_CHITINOUS_FOCUS                 = 53971,
    NPC_CHARGED_PYRESHELL_FOCUS                 = 53967,

    EVENT_PORTALS                               = 28888,

    SPELL_LEGENDARY_PORTAL_OPENING              = 101029,
    SPELL_BRANCH_OF_NORDRASSIL_WIN_COSMETIC     = 100326,
};

#endif

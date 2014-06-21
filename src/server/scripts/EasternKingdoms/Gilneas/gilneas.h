/*
 *
 * Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
 *
 */

#include "ScriptMgr.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "ScriptedVehicleEscortAI.h"
#include "ObjectMgr.h"
#include "Creature.h"
#include "SpellScript.h"
#include "ScriptedCreature.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "SpellInfo.h"
#include "Vehicle.h"
#include "Language.h"

#include "WorldSession.h"
#include "Language.h"
#include "Group.h"

#include <math.h>

#ifndef DEF_GILNEAS_H
#define DEF_GILNEAS_H

enum CreatureIds
{
    NPC_RAMPAGING_WORGEN_1                            = 34884,
    NPC_RAMPAGING_WORGEN_2                            = 35660,
    NPC_SERGEANT_CLEESE                               = 35839,
    NPC_MYRIAM_SPELLWALKER                            = 35872,
    NPC_FRIGHTENED_CITIZEN_1                          = 34981,
    NPC_FRIGHTENED_CITIZEN_2                          = 35836,
    NPC_AFFLICTED_GILNEAN                             = 50471,
    NPC_RAMPAGING_WORGEN                              = 34884,
    NPC_GILNEAS_CITY_GUARD                            = 34916,
    NPC_BLOODFANG_WORGEN                              = 35118,
    NPC_GILNEAN_ROYAL_GUARD                           = 35232,
    NPC_FRENZIED_STALKER                              = 35627,
    NPC_NORTHGATE_REBEL                               = 41015,
    NPC_GILNEAS_CITY_GUARD_PHASE_4                    = 50474,
    NPC_NORTHGATE_REBEL_PHASE_5                       = 36057,
    NPC_BLOODFANG_STALKER_PHASE_5                     = 35229,
    NPC_FORSAKEN_INVADER                              = 34511,
    NPC_FORSAKEN_FOOTSOLDIER                          = 36236,
    NPC_J_CITIZEN                                     = 34981,
    NPC_CW_WORGEN                                     = 35660,
    NPC_CW_CITIZEN                                    = 35836,
    NPC_QEMS_KILL_CREDIT                              = 35830,
    NPC_LORD_GODFREY_QOD                              = 35115,
    NPC_KING_GENN_GREYMANE_QOD                        = 35112,
    NPC_WORGEN_RUNT_FW                                = 35456,
    NPC_WORGEN_RUNT_SW                                = 35188,
    NPC_WORGEN_ALPHA_FW                               = 35170,
    NPC_WORGEN_ALPHA_SW                               = 35167,
    NPC_LORNA_CROWLEY                                 = 35378,
    NPC_JOSIAH_AVERY_WORGEN                           = 35370,
    NPC_GILNEAS_MASTIFF                               = 35631,
    NPC_GENN_HORSE                                    = 35905,
    NPC_KRENNAN_ARANAS                                = 35753,
    NPC_KRENNAN_ARANAS_SAVE                           = 35907,
    NPC_LORD_GOLDFREY                                 = 35906,
    NPC_QSKA_KILL_CREDIT                              = 35753,
    NPC_GUARD_QSKA                                    = 35509,
    NPC_WORGEN_QSKA                                   = 35505,
    NPC_COMMANDEERED_CANNON                           = 35914,
    NPC_BLOODFANG_RIPPER_QSKA                         = 35916,
    NPC_LORD_CROWLEY_QTR                              = 35552,
    NPC_PRINCE_LIAM_QTR                               = 35551,
    NPC_CROWLEYS_HORSE                                = 44428,
    NPC_CROWLEYS_HORSE_S_R                            = 35231,
    NPC_KING_GENN_GREYMANE_TTR                        = 35911,
    NPC_LORD_GODFREY_QLS                              = 36330,
    NPC_KRENNAN_ARANAS_QLS                            = 36331,
    NPC_KING_GENN_GREYMANE_QLS                        = 36332,
    NPC_FORSAKEN_ASSASSIN                             = 36207,
    NPC_HORRID_ABOMINATION_KILL_CREDIT                = 36233,
    NPC_PRINCE_LIAM_GREYMANE_QYCTEA                   = 36140,
    NPC_FORSACEN_CATAPILT                             = 36283,
    NPC_CYNTHIA                                       = 36287,
    NPC_ASHLEY                                        = 36288,
    NPC_JAMES                                         = 36289,
    NPC_QGFB_KILL_CREDIT                              = 36450,
    NPC_DROWNING_WATCHMAN                             = 36440,
    NPC_WAHL                                          = 36458,
    NPC_WAHL_WORGEN                                   = 36462,
    NPC_LUCIUS_THE_CRUEL                              = 36461,
    NPC_MOUNTAIN_HORSE_VEHICLE                        = 36540,
    NPC_MOUNTAIN_HORSE_FOLLOWER                       = 36555,
    NPC_MOUNTAIN_HORSE_KILL_CREDIT                    = 36560,
    NPC_SWIFT_MOUNTAIN_HORSE                          = 36741,
    NPC_QE_MARIE_ALLEN                                = 38853,
    NPC_QE_GWEN_ARMSTEAD                              = 44460,
    NPC_QE_KRENNAN_ARANAS                             = 36138,
    NPC_QE_DUSKHAVEN_WATCHMAN                         = 43907,
    NPC_QE_DUSKHAVEN_WATCHMAN_GUN                     = 37946,
    NPC_QE_LORNA_CROWLEY                              = 51409,
    NPC_QE_YOUNG_MASTIFF                              = 42722,
    NPC_QE_OGRE_AMBUSHER                              = 38762,
    NPC_SWAMP_CROCOLISK                               = 37078,
    NPC_KOROTH_THE_HILLBREAKER_QIAO                   = 36294,
    NPC_KOROTH_THE_HILLBREAKER_QIAO_FRIEND            = 37808,
    NPC_CAPTAIN_ASTHER_QIAO                           = 37806,
    NPC_FORSAKEN_SOLDIER_QIAO                         = 37805,
    NPC_FORSAKEN_CATAPULT_QIAO                        = 37807,
    NPC_DARK_SCOUT                                    = 37953,
    NPC_TOBIAS_MISTMANTLE_QTBWO                       = 38029,
    NPC_TALDOREN_TRACKER                              = 38027,
    NPC_VETERAN_DARK_RANGER                           = 38022,
    NPC_TRIGGER                                       = 35374,
    NPC_LYROS_SWIFTWIND                               = 37870,
    NPC_KING_GENN_GREYMANE_WORGEN                     = 31177,
    NPC_KING_GENN_GREYMANE_HUMAN                      = 38767,
    NPC_LORD_DARIUS_CROWLEY_QNHNB                     = 37195,
    NPC_LORNA_CROWLEY_QNHNB                           = 38768,
    NPC_LORD_GODFREY_QNHNB                            = 38766,
    NPC_TOBIAS_MISTMANTLE_QAOD                        = 38051,
    NPC_LORD_DARIUS_CROWLEY_QAOD                      = 37195,
    NPC_LORD_GODFREY_QBATR                            = 37875,
    NPC_KING_GENN_GREYMANE_QBATR                      = 37876,
    NPC_ENSLAVED_VILLAGER_QLD                         = 37694,
    NPC_PRINCE_LIAM_GREYMANE                          = 38218,
    NPC_MYRIAM_SPELLWAKER                             = 38465,
    NPC_SISTER_ALMYRA                                 = 38466,
    NPC_GILNEAN_MILITIA                               = 38221,
    NPC_DARK_RANGER_ELITE                             = 38464,
    NPC_LORNA_CROWLEY_TBFGC                           = 38426,
    NPC_FREED_EMBERSTONE_VILLAGER                     = 38425,
    NPC_EMBERSTONE_CANNON                             = 38424,
    NPC_WORGEN_WARRIOR                                = 38348,
    NPC_LORD_DARIUS_CROWLEY_TBFGC                     = 38415,
    NPC_GOREROT                                       = 38331,
    NPC_KING_GENN_GREYMANE_TBFGC                      = 38470,
    NPC_LADY_SYLVANAS_WINDRUNNER_TBFGC                = 38469,
    NPC_SOULTETHERED_BANSHEE                          = 38473,
    NPC_KRENNAN_ARANAS_TBFGC                          = 38553,
    NPC_TOBIAS_MISTMANTLE_QTHFS                       = 38507,
    NPC_FORSAKEN_GENERAL_QTHFS                        = 38617,
    NPC_LADY_SYLVANAS_QTHFS                           = 38530,
    NPC_GENERAL_WARHOWL_QTHFS                         = 38533,
    NPC_HIGH_EXECUTOR_CRENSHAW_QTHFS                  = 38537,
    NPC_CAPTURED_RIDING_BAT                           = 38540,
    NPC_GILNEAS_FUNERAL_CAMERA                        = 51083,
    NPC_KING_GENN_GREYMANE_QPB                        = 50893,
    NPC_LORD_DARIUS_CROWLEY_QPB                       = 50902,
    NPC_LORNA_CROWLEY_QPB                             = 50881,
    NPC_CLONE_QPB                                     = 51146,
    NPC_DARK_RANGER_THYALA                            = 36312,
    NPC_ATTACK_MASTIFF                                = 36409,
    NPC_FORSAKEN_CASTAWAY                             = 36488,
    NPC_PANCKED_CITIZEN                               = 34851,
    NPC_BLOODFANG_STALKER_CREDIT                      = 35582,
    NPC_LORD_DARIYS_CROWLEY                           = 35230,
    NPC_STAGECOACH_HARNESS                            = 43336,
    NPC_CAPTUREND_RIDING_BAT                          = 38615,
    NPC_LORNA_CROWLEY_QTHE                            = 36457,
};

enum Menu
{
    MENU_START_BATTLE                                 = 50710,
    MENU_MAIN                                         = 50712,
    MENU_GATHERING_AND_MINING_PROFESSIONS             = 50713,
    MENU_CRAFTING_PROFESSIONS                         = 50714,
    MENU_HERBALISM                                    = 50716,
    MENU_MINING                                       = 50718,
    MENU_SKINNING                                     = 50720,
    MENU_ALCHEMY                                      = 50715,
    MENU_BLACKSMITHING                                = 50719,
    MENU_ENCHANTING                                   = 50725,
    MENU_ENGINERING                                   = 50721,
    MENU_INSCRIPTION                                  = 50717,
    MENU_JEWELCRAFTING                                = 50722,
    MENU_LEATHERWORKING                               = 50723,
    MENU_TAILORING                                    = 50724,
    MENU_HUNTSMAN_BLAKE                               = 50726,
    MENU_VITUS_DARKWALKER                             = 50728,
    MENU_LOREN_THE_FENCE                              = 50730,
    MENU_SISTER_ALMYRA                                = 50732,
    MENU_CELESTINE_OF_THE_HARVES                      = 50734,
    MENU_MYRIAM_SPELLWAKER                            = 50736,
    MENU_SERGEANT_CLEESE                              = 50738,
};

enum GameObjectIds
{
    GO_OFFERINGS                                      = 202319,
    GO_SPARKLES                                       = 197333,
    GO_FIRST_GATE                                     = 196863,
    GO_SECOND_GATE                                    = 196864,
    GO_GREYMANE_GATE                                  = 196401,
    GO_KINGS_GATE                                     = 196412,
    GO_WELL_OF_FURY                                   = 201950,
    GO_WELL_OF_TRANQUILITY                            = 201951,
    GO_WELL_OF_BALANCE                                = 201952,
    GO_DOOR_TO_THE_BASEMENT                           = 196404,
};

enum QuestId
{
    QUEST_EVAC_MERC_SQUA                              = 14098,
    QUEST_LOCKDOWN                                    = 14078,
    QUEST_ROYAL_ORDERS                                = 14099,
    QUEST_BY_THE_SKIN_OF_HIS_TEETH                    = 14154,
    QUEST_THE_REBEL_LORD_ARSENAL                      = 14159,
    QUEST_FROM_THE_SHADOWS                            = 14204,
    QUEST_SAVE_KRENNAN_ARANAS                         = 14293,
    QUEST_TIME_TO_REGROUP                             = 14294,
    QUEST_SACRIFICES                                  = 14212,
    QUEST_LAST_STAND                                  = 14222,
    QUEST_LAST_CHANCE_AT_HUMANITY                     = 14375,
    QUEST_IN_NEED_OF_INGR                             = 14320,
    QUEST_INVASION                                    = 14321,
    QUEST_SAVE_THE_CHILDREN                           = 14368,
    QUEST_GASPING_FOR_BREATH                          = 14395,
    QUEST_GRANDMAS_CAT                                = 14401,
    QUEST_THE_HUNGRY_ETTIN                            = 14416, // todo
    QUEST_TO_GREYMANE_MANOR                           = 14465,
    QUEST_THE_KINGS_OBSERVATORY                       = 14466,
    QUEST_ALAS_GILNEAS                                = 14467,
    QUEST_EXODUS                                      = 24438,
    QUEST_INTRODUCTIONS_ARE_IN_ORDER                  = 24472,
    QUEST_LOSING_YOUR_TAIL                            = 24616,
    QUEST_TAKE_BACK_WHATS_OURS                        = 24646,
    QUEST_NEITHER_HUMAN_NOR_BEAST                     = 24593,
    QUEST_AT_OUR_DOORSTEP                             = 24627,
    QUEST_BETRAYAL_AT_TEMPESTS_REACH                  = 24592,
    QUEST_LIBERATION_DAY                              = 24575,
    QUEST_THE_BATTLE_FOR_GILNEAS_CITY                 = 24904,
    QUEST_THE_HUNT_FOR_SYLVANAS                       = 24902,
    QUEST_SLOWING_THE_INEVITABLE                      = 24920,
    QUEST_STEADY_SHOT                                 = 14276,
    QUEST_IMMOLATE                                    = 14274,
    QUEST_EVISCERATE                                  = 14272,
    QUEST_FLASH_HEAL                                  = 14279,
    QUEST_A_REJUVENATING_TOUCH                        = 14283,
    QUEST_ARCANE_MISSILES                             = 14281,
    QUEST_CHARGE                                      = 14266,
    QUEST_OLD_DIVISIONS                               = 14157,
    QUEST_RUTTHERAN_VILLANGE                          = 14434,
    QUEST_AS_THE_LAND_SHATTERS                        = 14396,
    QUEST_NOT_QUITE_SHIPSHATE                         = 14404,
    QUEST_PIECES_OF_THE_PAST                          = 24495,
    QUEST_YOU_CANT_TAKE_EM_ALONE                      = 14348, // RETEST
    QUEST_EVACUATION                                  = 14397,
};

enum Movies
{
    MOVIE_WORGEN_FORM                                 = 21,
    CINEMATIC_FORSAKEN                                = 168,
    CINEMATIC_TELESCOPE                               = 167,
};

enum Phases
{
    EXODUS_PHASE_MASK                                 = 33685504,
};

enum Actions
{
    ACTION_JUST_CITIZEN                              = 1,
    ACTION_CITIZEN_AND_WORGEN                        = 2,

    TYPE_FIRST_WAVE                                  = 0,
    TYPE_SECOND_WAVE                                 = 1,

    ACTION_START_EVENT                               = 3,

    ACTION_EVENT_DONE                                = 1,

    ACTION_SUMMON_LUCIUS                             = 1,
    ACTION_CATCH_CHANCE                              = 2,
    ACTION_CHANCE_DESPAWN                            = 3,
    POINT_CATCH_CHANCE                               = 4,

    ACTION_RESCUED_HORSE                             = 1,

    POINT_BANNER                                     = 1,
    ACTION_KOROTH_ATTACK                             = 2,

    ACTION_PLAYER_IS_FREE                            = 1,

    ACTION_BACK                                      = 2,

    POINT_NEAR_CROWLEY                               = 1,

    PHASE_NONE                                       = 0,
    PHASE_INTRO                                      = 1,
    PHASE_MERCHANT_SQUARE                            = 2,
    PHASE_MILITARY_DISTRICT                          = 3,
    PHASE_GREYMANE_COURT                             = 4,

    DATA_WP_PAUSE                                    = 5,
    DATA_CURRENT_PHASE                               = 6,

    ACTION_CANNON_AT_POSITION                        = 7,
    ACTION_GOREROT_FIGHT                             = 8,
    ACTION_GOREROT_DIED                              = 9,
    ACTION_ATTACK_SYLVANAS                           = 10,
    ACTION_FINISH_TBFGC                              = 11,
    ACTION_BATTLE_COMPLETE                           = 12,
    ACTION_CAN_ATTACK                                = 13,

    ACTION_DESPAWN                                   = 7,

    PHASE_ONE                                        = 6,
};

enum SpellIds
{
    SPELL_FROSTBOLT_VISUAL_ONLY             = 74277, // Dummy spell, visual only
    SPELL_ENRAGE                            = 8599,
    SPELL_SHOOT                             = 20463,
    SPELL_CATACLYSM_TYPE_1                  = 80133,
    SPELL_CATACLYSM_TYPE_2                  = 68953,
    SPELL_CATACLYSM_TYPE_3                  = 80134,
    SPELL_CW_WORGEN_ENRAGE                  = 56646,
    SPELL_HORRIFYING_HOWL                   = 75355,
    SPELL_DEMORALIZING_SHOUT                = 61044,
    SPELL_GILNEAS_PRISON_PERIODIC_FORCECAST = 66914,
    SPELL_WORGEN_BITE                       = 72870,
    SPELL_SUMMON_MASTIFF                    = 67807,
    SPELL_ATTACK_LURKER                     = 67805,
    SPELL_SHADOWSTALKER_STEALTH             = 5916,
    SPELL_UNDYING_FRENZY                    = 80515,
    SPELL_CANNON_FIRE                       = 68235,
    SPELL_SHOOT_QSKA                        = 48424,
    SPELL_CANNON_CAMERA                     = 93522,
    SPELL_THROW_TORCH                       = 67063,
    SPELL_PLAY_MOVIE                        = 93477,
    SPELL_HIDEOUS_BITE_WOUND                = 76642,
    SPELL_CURSE_OF_THE_WORGEN               = 68630,
    SPELL_LAST_STAND_COMPLETE               = 72788,
    SPELL_BACKSTAB                          = 75360,
    SPELL_KEG_PLACED                        = 68555,
    SPELL_SHOOT_QYCTEA                      = 68559,   // 68559 - 
    SPELL_RESTITCHING                       = 68864,
    SPELL_EXPLOSION                         = 68560,
    SPELL_EXPLOSION_POISON                  = 42266,
    SPELL_EXPLOSION_BONE_TYPE_ONE           = 42267,
    SPELL_EXPLOSION_BONE_TYPE_TWO           = 42274,
    SPELL_LAUNCH                            = 66251,
    SPELL_SAVE_CYNTHIA                      = 68597,
    SPELL_SAVE_ASHLEY                       = 68598,
    SPELL_SAVE_JAMES                        = 68596,
    SPELL_RESCUE_DROWNING_WATCHMAN          = 68735,
    SPELL_SUMMON_SPARKLES                   = 69253,
    SPELL_DROWNING                          = 68730,
    SPELL_CATCH_CAT                         = 68743,
    SPELL_ROPE_CHANNEL                      = 68940,
    SPELL_ROPE_IN_HORSE                     = 68908,
    SPELL_THROW_BOULDER                     = 72768,
    SPELL_PUSH_BANNER                       = 70511,
    SPELL_CLEAVE                            = 16044,
    SPELL_DEMORALIZING_SHOUT_QIAO           = 16244,
    SPELL_AIMED_SHOT                        = 15620,
    SPELL_FREEZING_TRAP                     = 70794,
    SPELL_WAR_STOMP                         = 71019,
    SPELL_TALDOREN_WELL                     = 71200,
    SPELL_WORGEN_COMBAT_TRANSFORM_FX        = 81768,
    SPELL_AI_REACTION                       = 61899,
    SPELL_SOLDIER_OF_TBFGC                  = 72069,
    SPELL_CHALLENGING_SHOUT                 = 1161,
    SPELL_MULTI_SHOT                        = 31942,
    SPELL_SHOOT_TBFGC                       = 6660,
    SPELL_IMPROVED_WING_CLIP                = 47168,
    SPELL_SUNDER_ARMOR                      = 15572,
    SPELL_FLASH_HEAL                        = 17843,
    SPELL_HOLY_NOVA                         = 85965,
    SPELL_ARCANE_INTELLECT                  = 13326,
    SPELL_BLIZZARD                          = 33418,
    SPELL_FIRE_BLAST                        = 20795,
    SPELL_FROSTBOLT                         = 20792,
    SPELL_KNOCKBACK                         = 68683,
    SPELL_THUNDERCLAP                       = 8078,
    SPELL_SMASH                             = 71774,
    SPELL_GOREROT_THREAT                    = 72135,
    SPELL_SLICE_AND_DICE                    = 6434,
    SPELL_TAUNT                             = 37548,
    SPELL_FERAL_LEAP                        = 71921,
    SPELL_INTERCEPT                         = 58769,
    SPELL_GREYMANE_TRANSFORM                = 86141,
    SPELL_GREYMANE_JUMP                     = 72107,
    SPELL_BANSHEE_QUEENS_WAIL               = 72113,
    SPELL_SHOOT_LIAM                        = 61, // 72116
    SPELL_BFGC_COMPLETE                     = 72349,
    SPELL_FADE_TO_BLACK                     = 89404,
    SPELL_MIRROR_IMAGE                      = 45204,
    SPELL_SHOOT_LOTP                        = 16100,
    SPELL_KNOCKBACK_LOTP                    = 68683,
    SPELL_DEMORALIZING_ROAR                 = 15971,
    SPELL_LEAP                              = 68687,
    SPELL_TAUNT_LOTP                        = 26281,
    SPELL_PLAGUE_BARREL_LAUNCH              = 71804,
    SPELL_SWING_TORCH                       = 70631,
    SPELL_FAKE_SHOT                         = 7105,
    SPELL_FADE_TO_BLACK_2                   = 94053,
    SPELL_FIERY_BOULDER                     = 68591,
    SPELL_LEARN_TWO_FORMS                   = 72857,
    SPELL_BURNING                           = 72839,
    SPELL_SHOOT_QGC                         = 41440,

    PHASE_QUEST_ZONE_SPECIFIC_1             = 59073,
    //PHASE_QUEST_ZONE_SPECIFIC_2             = 59074,
    PHASE_QUEST_ZONE_SPECIFIC_3             = 59087,
    //PHASE_QUEST_ZONE_SPECIFIC_4             = 67789,
    //PHASE_QUEST_ZONE_SPECIFIC_5             = 68480,
    PHASE_QUEST_ZONE_SPECIFIC_6             = 68481,
    PHASE_QUEST_ZONE_SPECIFIC_7             = 68482,
    PHASE_QUEST_ZONE_SPECIFIC_8             = 68483,
    PHASE_QUEST_ZONE_SPECIFIC_11            = 69484,
    PHASE_QUEST_ZONE_SPECIFIC_12            = 69485,
    PHASE_QUEST_ZONE_SPECIFIC_19            = 74096,

    SPELL_QUEST_INVISIBILITY_DETECTION_1    = 60922,
    SPELL_QUEST_INVISIBILITY_DETECTION_2    = 49417,
    SPELL_QUEST_INVISIBILITY_DETECTION_3    = 49416,
    //SPELL_QUEST_INVISIBILITY_DETECTION_4    = 80818,
    //SPELL_QUEST_INVISIBILITY_DETECTION_5    = 94704,
    //SPELL_QUEST_INVISIBILITY_DETECTION_6    = 94567,
};

enum NpcTexts
{
    SAY_PRINCE_LIAM_GREYMANE_1                          = -1638000,
    SAY_PRINCE_LIAM_GREYMANE_2                          = -1638001,
    SAY_PRINCE_LIAM_GREYMANE_3                          = -1638002,
    DELAY_SAY_PRINCE_LIAM_GREYMANE                      = 20000,

    YELL_PRINCE_LIAM_GREYMANE_1                         = -1638025,
    YELL_PRINCE_LIAM_GREYMANE_2                         = -1638026,
    YELL_PRINCE_LIAM_GREYMANE_3                         = -1638027,
    YELL_PRINCE_LIAM_GREYMANE_4                         = -1638028,
    YELL_PRINCE_LIAM_GREYMANE_5                         = -1638029,
    DELAY_YELL_PRINCE_LIAM_GREYMANE                     = 2000,

    SAY_PANICKED_CITIZEN_1                              = -1638016,
    SAY_PANICKED_CITIZEN_2                              = -1638017,
    SAY_PANICKED_CITIZEN_3                              = -1638018,
    SAY_PANICKED_CITIZEN_4                              = -1638019,

    SAY_GILNEAS_CITY_GUARD_GATE_1                       = -1638022,
    SAY_GILNEAS_CITY_GUARD_GATE_2                       = -1638023,
    SAY_GILNEAS_CITY_GUARD_GATE_3                       = -1638024,

    SAY_CITIZEN_1                                       = -1638003,
    SAY_CITIZEN_2                                       = -1638004,
    SAY_CITIZEN_3                                       = -1638005,
    SAY_CITIZEN_4                                       = -1638006,
    SAY_CITIZEN_5                                       = -1638007,
    SAY_CITIZEN_6                                       = -1638008,
    SAY_CITIZEN_7                                       = -1638009,
    SAY_CITIZEN_8                                       = -1638010,
    SAY_CITIZEN_1b                                      = -1638011,
    SAY_CITIZEN_2b                                      = -1638012,
    SAY_CITIZEN_3b                                      = -1638013,
    SAY_CITIZEN_4b                                      = -1638014,
    SAY_CITIZEN_5b                                      = -1638015,

    LIAM_INTRO_1                                        = -1977000,
    LIAM_INTRO_2                                        = -1977001,
    LIAM_INTRO_3                                        = -1977002,
    CITIZEN_SAY_WHAT_AT_THE_ROOF                        = -1977003,
    LIAM_RANDOM_YELL                                    = -1977019,   // (-1977019 to -1977023)
                                                                      
    PANICKED_CITIZEN_RANDOM_SAY                         = -1977152,  // (-1977152 to -1977154)
    GILNEAS_CITY_GUARD_RANDOM_SAY                       = -1977155,  // (-1977155 to -1977157)
    GUARD_RANDOM_YELL                                   = -1977161,  //  -1977161 to -1977163

    NPC_J_CITIZEN_RANDOM_SAY                            = -1977004,  // (-1977004 to -1977011)
    NPC_CW_CITIZEN_RANDOM_SAY                           = -1977012,  // (-1977012 to -1977018)

    NPC_LORD_GODFREY_QOD_SAY                            = -1977024,
    NPC_KING_GENN_GREYMANE_QOD_SAY                      = -1977025,

    RANDOM_JOSIAH_YELL                                  = -1977063,  // -1977063 to -1977068
    SAY_WORGEN_BITE                                     = -1977069,

    HORSE_SAY_SAVE_ARANAS                               = -1977026,
    ARANAS_SAY_SAVE_ME                                  = -1977027,
    ARANAS_THANK                                        = -1977028,
    GOLDFREY_SAY_ARANAS_WITH_US                         = -1977029,
    ARANAS_SAY_GENN_HORSE                               = -1977140,
    GREYMANE_RANDOM_YELL                                = -1977158,  // -1977158 to -1977160

    GENN_SAY_IF                                         = -1977031,
    CROWLEY_SAY_NEED                                    = -1977032,
    LIAM_SAY_STAY                                       = -1977033,
    CROWLEY_SAY_NO_CHANCE                               = -1977034,
    CROWLEY_SAY_MY_MEN                                  = -1977035,
    GENN_SAY_WE_FOOLS                                   = -1977036,

    CROWLEY_SAY_START                                   = -1977037,
    CROWLEY_RANDOM_SAY                                  = -1977038,  // (-1977038 -1977040)

    ARANAS_SAY_YOU_CAN_CONTROL                          = -1977070,
    GODFREY_SAY_PUT_DOWN                                = -1977071,
    GREYMANE_SAY_TELL_ME_GODFREY                        = -1977072,
    GREYMANE_SAY_DIDNT_THINK_SO                         = -1977073,

    NPC_FORSAKEN_ASSASSIN_SAY                           = -1977041,

    NPC_HORRID_ABOMINATION_RANDOM_SAY                   = -1977042,

    NPC_CYNTHIA_SAY                                     = -1977144,
    NPC_ASHLEY_SAY                                      = -1977145,
    NPC_JAMES_SAY                                       = -1977146,

    DROWNING_WATCHMAN_RANDOM_SAY                        = -1977141,  // -1977141 to -1977143

    SAY_THIS_CAT_IS_MINE                                = -1977048,
    YELL_DONT_MESS                                      = -1977049,

    LORNA_YELL_CARRIAGE                                 = -1977050,

    SURVIVOR_RANDOM_SAY                                 = -1977051,  // -1977051 to -1977054

    KOROTH_YELL_WHO_STEAL_BANNER                        = -1977085,
    KOROTH_YELL_FIND_YOU                                = -1977086,
    LIAN_SAY_HERE_FORSAKEN                              = -1977087,
    LIAM_YELL_YOU_CANT                                  = -1977088,
    CAPITAN_YELL_WILL_ORDER                             = -1977089,
    KOROTH_YELL_MY_BANNER                               = -1977090,

    NPC_DARK_SCOUT_SAY_CATCH                            = -1977055,
    NPC_DARK_SCOUT_SAY_FREE                             = -1977056,
    NPC_DARK_SCOUT_SAY_HOW                              = -1977057,

    TOBIAS_SAY_DISTRACT_RANGERS                         = -1977061,
    TOBIAS_SAY_FALL_BACK                                = -1977062,

    LYROS_SAY_WELL_OF_FURY                              = -1977074,
    VASSANDRA_SAY_WELL_OF_TRANQUILITY                   = -1977075,
    TALRAN_SAY_WELL_OF_BALANCE                          = -1977076,
    LORNA_YELL_FATHER                                   = -1977077,
    CROWLEY_YELL_LORNA                                  = -1977078,
    GODFREY_SAY_LOW                                     = -1977079,
    CROWLEY_YELL_FROG                                   = -1977080,
    GREYMANE_SAY_NO_FRIEND                              = -1977081,
    GODFREY_YELL_CANT_BE                                = -1977082,
    CROWLEY_SAY_YES_GENN                                = -1977083,
    CREYMANE_SAY_HELD                                   = -1977084,

    TOBIAS_SAY_FORSAKEN                                 = -1977058,
    CROWLEY_SAY_IMMEDIATELY                             = -1977059,
    TOBIAS_SAY_OK                                       = -1977060,

    GENN_SAY_ALL_IS_DONE                                = -1977091,
    GODFREY_SAY_BETTER_DIED                             = -1977092,

    VILLAGER_RANDOM_SAY                                 = -1977093,  // (-1977093 to -1977095)


    LIAM_BATTLE_SPEACH_1                                = -1977096,
    LIAM_BATTLE_SPEACH_2                                = -1977097,
    LIAM_BATTLE_SPEACH_3                                = -1977098,
    LIAM_BATTLE_SPEACH_4                                = -1977099,
    LIAM_BATTLE_SPEACH_5                                = -1977100,
    LIAM_BATTLE_SPEACH_6                                = -1977101,
    LIAM_BATTLE_FOR_GILNEAS                             = -1977102,
    LIAM_RANDOM_BATTLE_SPEACH                           = -1977103,  // -1977103 to -1977106
    LIAM_BATTLE_BATTLE_1                                = -1977107,
    LIAM_BATTLE_BATTLE_2                                = -1977108,
    LIAM_BATTLE_BATTLE_3                                = -1977109,
    LIAM_BATTLE_DEATH_1                                 = -1977110,
    LIAM_BATTLE_DEATH_2                                 = -1977111,
    LIAM_BATTLE_DEATH_3                                 = -1977112,
    LORNA_BATTLE_BATTLE_1                               = -1977113,
    LORNA_BATTLE_BATTLE_2                               = -1977114,
    GOREROT_YELL_CRUSH                                  = -1977115,
    CROWLEY_BATTLE_BATTLE_1                             = -1977116,
    CROWLEY_BATTLE_BATTLE_2                             = -1977117,
    GREYMANE_BATTLE_BATTLE_1                            = -1977118,
    GREYMANE_BATTLE_BATTLE_2                            = -1977119,
    GREYMANE_YELL_LIAM                                  = -1977120,
    SYLVANAS_YELL_ENOUGH                                = -1977121,
    SYLVANAS_KILL_LIAM                                  = -1977122,

    TOBIAS_SAY_GO                                       = -1977123,
    TOBIAS_RAID_BOSS_WHISPER                            = -1977124,
    TOBIAS_SAY_NOT_SEEN                                 = -1977125,
    TOBIAS_SAY_DID_YOU_HEAR                             = -1977126,
    TOBIAS_SAY_HIDE                                     = -1977127,
    FORSAKEN_GENERAL_YELL                               = -1977128,
    WARHOWL_SAY_LOSING                                  = -1977129,
    WARHOWL_SAY_PLAGUE                                  = -1977130,
    WARHOWL_SAY_GOOD_BY                                 = -1977131,
    SYLVANAS_SAY_ASSURE                                 = -1977132,
    SYLVANAS_SAY_TONE                                   = -1977133,
    SYLVANAS_SAY_GO_WITH_HONOR                          = -1977134,
    SYLVANAS_SAY_WHAT_QUESTION                          = -1977135,
    CRENSHAW_SAY_ORDER                                  = -1977136,
    CRENSHAW_AS_YOU_WISH                                = -1977137,
    PLAYER_SAY_ME_NOOB                                  = -1977138,
    SYLVANAS_YELL_FISH                                  = -1977139,

    GREYMANE_SAY_QPB_1                                  = -1977147,
    GREYMANE_SAY_QPB_2                                  = -1977148,
    GREYMANE_SAY_QPB_3                                  = -1977149,
    LORNA_SAY_QPB                                       = -1977150,
    CROWLEY_SAY_QPB                                     = -1977151,

    HUNTSMAN_BLAKE_SAY_1                                = -1977165,
    HUNTSMAN_BLAKE_SAY_2                                = -1977166,
    VITUS_DARKWALKER_SAY_1                              = -1977167,
    VITUS_DARKWALKER_SAY_2                              = -1977168,
    LOREN_THE_FENCE_SAY_1                               = -1977169,
    LOREN_THE_FENCE_SAY_2                               = -1977170,
    SISTER_ALMYRA_SAY_1                                 = -1977171,
    SISTER_ALMYRA_SAY_2                                 = -1977172,
    CELESTINE_OF_THE_HARVES_SAY_1                       = -1977173,
    CELESTINE_OF_THE_HARVES_SAY_2                       = -1977174,
    MYRIAM_SPELLWAKER_SAY_1                             = -1977175,
    MYRIAM_SPELLWAKER_SAY_2                             = -1977176,
    SERGEANT_CLEESE_SAY_1                               = -1977177,
    SERGEANT_CLEESE_SAY_2                               = -1977178,

    SAY_MASTIFF                                         = -1977030,

    TIME_TO_START_BATTLE                                = -1977164,
};

const uint16 PanickedCitizenRandomEmote[5] =
{
    EMOTE_ONESHOT_COWER,
    EMOTE_ONESHOT_TALK,
    EMOTE_ONESHOT_CRY,
    EMOTE_ONESHOT_BEG,
    EMOTE_ONESHOT_EXCLAMATION,
};

enum SoundIds
{
    SOUND_SWORD_FLESH                                 = 143,
    SOUND_SWORD_PLATE                                 = 147,
    SOUND_WORGEN_ATTACK                               = 558,
    DELAY_SOUND                                       = 500,
    DELAY_ANIMATE                                     = 2000
};

struct Coord
{
    float x;
    float y;
    float z;
};

struct CrowFlyPosition
{
    Coord FirstCoord;
    Coord SecondCoord;
};

struct Psc
{
    uint64 uiPlayerGUID;
    uint32 uiPersonalTimer;
};

struct Psc_qtr
{
    uint64 uiPlayerGUID;
    uint32 uiPersonalTimer;
    uint32 uiSpeachId;
};

struct Psc_qiao
{
    uint64 uiPlayerGUID;
    uint64 uiCapitanGUID;
    uint32 uiEventTimer;
    uint8 uiPhase;
};

struct sSoldier
{
    Creature* soldier;
    float follow_angle;
    float follow_dist;
};

struct Psc_qthfs
{
    uint64 uiPlayerGUID;
    uint64 uiTobiasGUID;
};

const CrowFlyPosition CrowFlyPos[12]=
{
    {{-1398.24f, 1455.26f, 39.6586f}, {-1403.93f, 1464.87f, 47.7066f}},
    {{-1628.58f, 1320.29f, 27.7544f}, {-1626.90f, 1306.58f, 34.9702f}},
    {{-1799.76f, 1564.33f, 34.9408f}, {-1788.64f, 1561.74f, 38.4683f}},
    {{-1840.45f, 2299.17f, 50.2894f}, {-1850.23f, 2302.62f, 52.4776f}},
    {{-1978.48f, 2319.58f, 36.5107f}, {-1979.80f, 2329.24f, 38.8598f}},
    {{-1914.39f, 2406.48f, 37.4498f}, {-1916.48f, 2416.10f, 39.2891f}},
    {{-1844.85f, 2328.28f, 47.8401f}, {-1836.64f, 2327.05f, 50.0315f}},
    {{-1845.20f, 2502.86f, 6.67603f}, {-1839.71f, 2508.70f, 9.67311f}},
    {{-2031.81f, 2280.29f, 28.7353f}, {-2043.98f, 2286.43f, 32.0705f}},
    {{-2164.61f, 2213.12f, 27.4170f}, {-2169.48f, 2224.64f, 29.1592f}},
    {{-1775.46f, 2380.44f, 51.9086f}, {-1767.75f, 2385.99f, 55.8622f}},
    {{-1650.79f, 2507.28f, 109.893f}, {-1645.28f, 2506.02f, 115.819f}},
};

static float WorgenPosT1[4][4] =
{
    {-1718.18f, 1526.45f, 55.9076f, 4.74478f},
    {-1718.06f, 1515.83f, 55.3631f, 4.73569f},
    {-1718.02f, 1487.10f, 57.0588f, 4.64537f},
    {-1701.28f, 1470.35f, 52.2872f, 5.55485f},
};

static float WorgenPosT2[8][4] =
{
    {-1634.06f, 1486.73f, 73.3780f, 0.76234f},
    {-1635.34f, 1487.13f, 72.2655f, 0.89272f},
    {-1635.80f, 1488.80f, 71.0234f, 0.79533f},
    {-1630.88f, 1483.50f, 72.9698f, 0.75763f},
    {-1629.91f, 1482.39f, 71.7020f, 0.71836f},
    {-1628.88f, 1481.65f, 70.6151f, 0.69480f},
    {-1633.23f, 1484.80f, 74.9893f, 0.77727f},
    {-1672.09f, 1448.92f, 52.2870f, 0.78175f},
};

const float WorgenSummonPos[13][4]=
{
    {-1715.219f, 1352.839f, 19.8645f, 2.72649f},
    {-1721.182f, 1350.429f, 19.8656f, 2.48614f},
    {-1746.523f, 1361.108f, 19.8681f, 1.85957f},
    {-1724.385f, 1348.462f, 19.6781f, 2.10692f},
    {-1734.542f, 1344.554f, 19.8769f, 1.65637f},
    {-1732.773f, 1367.837f, 19.9010f, 1.10063f},
    {-1744.358f, 1363.382f, 19.8996f, 2.06127f},
    {-1719.358f, 1357.512f, 19.7791f, 2.91488f},
    {-1728.276f, 1353.201f, 19.6823f, 2.25370f},
    {-1726.747f, 1364.599f, 19.9263f, 2.71766f},
    {-1737.693f, 1352.986f, 19.8711f, 1.96818f},
    {-1734.391f, 1359.887f, 19.9064f, 2.48052f},
    {-1730.286f, 1390.384f, 20.7707f, 4.35712f},
};

const float SecondRoundWP[90][3]=
{
    {-1537.78f, 1571.12f, 29.207f}, {-1533.40f, 1570.51f, 28.250f}, {-1530.03f, 1570.12f, 27.192f},
    {-1520.44f, 1568.08f, 26.536f}, {-1514.90f, 1567.33f, 24.815f}, {-1508.30f, 1566.47f, 21.499f},
    {-1504.83f, 1566.05f, 20.485f}, {-1500.33f, 1564.15f, 20.485f}, {-1496.13f, 1560.47f, 20.487f},
    {-1493.69f, 1557.95f, 20.496f}, {-1489.17f, 1552.62f, 20.548f}, {-1484.62f, 1547.30f, 20.537f},
    {-1475.44f, 1536.72f, 20.486f}, {-1472.40f, 1533.84f, 20.557f}, {-1466.43f, 1530.20f, 20.568f},
    {-1462.22f, 1528.52f, 20.490f}, {-1456.87f, 1528.10f, 20.521f}, {-1452.68f, 1528.32f, 20.520f},
    {-1450.72f, 1529.05f, 20.491f}, {-1441.65f, 1534.30f, 20.486f}, {-1436.88f, 1539.17f, 20.486f},
    {-1429.22f, 1548.88f, 20.780f}, {-1421.11f, 1560.26f, 20.483f}, {-1417.00f, 1567.17f, 20.481f},
    {-1415.29f, 1575.15f, 20.481f}, {-1416.18f, 1585.83f, 20.482f}, {-1420.30f, 1595.49f, 20.483f},
    {-1425.69f, 1608.41f, 20.483f}, {-1430.94f, 1617.50f, 20.483f}, {-1438.97f, 1627.24f, 20.483f},
    {-1449.65f, 1627.17f, 20.483f}, {-1459.95f, 1625.12f, 20.483f}, {-1473.70f, 1622.50f, 20.483f},
    {-1484.10f, 1621.03f, 20.483f}, {-1498.98f, 1621.02f, 20.483f}, {-1515.94f, 1625.35f, 20.483f},
    {-1526.11f, 1627.95f, 20.483f}, {-1532.26f, 1630.36f, 20.483f}, {-1541.54f, 1635.26f, 21.221f},
    {-1553.93f, 1641.79f, 20.485f}, {-1563.23f, 1646.65f, 20.485f}, {-1568.20f, 1652.36f, 20.485f},
    {-1568.86f, 1656.04f, 20.485f}, {-1567.76f, 1660.69f, 20.485f}, {-1564.39f, 1665.54f, 20.485f},
    {-1559.28f, 1670.33f, 20.485f}, {-1551.64f, 1677.52f, 20.485f}, {-1541.45f, 1687.11f, 20.542f},
    {-1531.94f, 1696.07f, 20.485f}, {-1527.52f, 1697.55f, 20.485f}, {-1522.81f, 1697.94f, 20.485f},
    {-1517.89f, 1696.54f, 20.485f}, {-1512.67f, 1693.46f, 20.485f}, {-1506.65f, 1689.89f, 20.485f},
    {-1500.63f, 1686.32f, 20.485f}, {-1491.86f, 1680.55f, 20.485f}, {-1483.49f, 1674.21f, 20.485f},
    {-1478.02f, 1669.84f, 20.485f}, {-1469.92f, 1663.16f, 20.485f}, {-1463.61f, 1656.18f, 20.485f},
    {-1455.17f, 1646.96f, 20.485f}, {-1450.85f, 1641.46f, 20.485f}, {-1445.68f, 1634.26f, 20.485f},
    {-1441.69f, 1628.50f, 20.485f}, {-1435.71f, 1619.88f, 20.485f}, {-1429.73f, 1611.25f, 20.485f},
    {-1425.96f, 1605.36f, 20.485f}, {-1422.86f, 1599.09f, 20.485f}, {-1420.26f, 1592.59f, 20.485f},
    {-1418.18f, 1585.91f, 20.485f}, {-1418.93f, 1580.32f, 20.485f}, {-1421.21f, 1575.11f, 20.494f},
    {-1424.48f, 1568.92f, 20.485f}, {-1437.43f, 1543.18f, 20.485f}, {-1439.98f, 1540.78f, 20.485f},
    {-1444.94f, 1536.17f, 20.485f}, {-1450.01f, 1533.64f, 20.485f}, {-1456.70f, 1532.84f, 20.485f},
    {-1461.44f, 1533.95f, 20.485f}, {-1466.03f, 1536.72f, 20.485f}, {-1479.46f, 1547.95f, 20.816f},
    {-1484.96f, 1552.27f, 20.781f}, {-1498.70f, 1562.71f, 20.488f}, {-1503.01f, 1566.29f, 20.485f},
    {-1505.76f, 1568.03f, 20.485f}, {-1510.27f, 1569.94f, 22.966f}, {-1514.79f, 1571.84f, 25.271f},
    {-1517.16f, 1572.83f, 26.209f}, {-1525.59f, 1576.24f, 26.537f}, {-1532.30f, 1578.97f, 26.871f},
};

const float CheckSpellPos[16][3]=
{
    {-2119.36f, 2663.58f, 8.25703f},
    {-2132.94f, 2665.22f, 8.25549f},
    {-2135.20f, 2658.72f, 19.1616f},
    {-2113.73f, 2677.08f, 14.1064f},
    {-2124.71f, 2677.31f, 13.6799f},
    {-2128.56f, 2665.52f, 15.2329f},
    {-2115.82f, 2668.72f, 13.6927f},
    {-2106.92f, 2686.66f, 18.2360f},
    {-2098.44f, 2694.41f, 20.4828f},
    {-2197.74f, 2594.95f, 20.9434f},
    {-2206.71f, 2587.34f, 18.6410f},
    {-2229.45f, 2568.61f, 15.6442f},
    {-2236.70f, 2560.28f, 19.7311f},
    {-2215.77f, 2568.64f, 14.0592f},
    {-2224.96f, 2579.85f, 14.0884f},
    {-2216.07f, 2578.79f, 14.5102f},
};

const float HarnessWP[198][3]=
{
    {-1672.86f, 2516.55f, 97.8940f}, {-1679.42f, 2510.07f, 97.8836f}, {-1680.65f, 2508.83f, 97.8603f},
    {-1682.52f, 2506.88f, 97.7850f}, {-1683.74f, 2505.62f, 97.7018f}, {-1685.97f, 2502.77f, 97.4225f},
    {-1687.54f, 2500.73f, 97.0150f}, {-1689.60f, 2497.77f, 96.3618f}, {-1691.56f, 2494.33f, 95.3775f},
    {-1693.19f, 2491.63f, 94.4497f}, {-1700.21f, 2480.20f, 89.6461f}, {-1700.21f, 2480.20f, 89.6461f},
    {-1701.82f, 2477.10f, 88.3208f}, {-1703.74f, 2472.21f, 86.3349f}, {-1704.93f, 2468.06f, 84.8038f},
    {-1704.84f, 2464.45f, 83.8521f}, {-1703.79f, 2460.39f, 83.1022f}, {-1700.58f, 2452.06f, 81.6085f},
    {-1699.17f, 2447.38f, 80.6009f}, {-1698.72f, 2443.68f, 79.6616f}, {-1698.72f, 2439.83f, 78.5806f},
    {-1699.03f, 2435.89f, 77.3782f}, {-1699.89f, 2431.95f, 76.0359f}, {-1701.14f, 2428.75f, 74.8227f},
    {-1703.10f, 2424.78f, 73.0883f}, {-1705.94f, 2420.79f, 71.0919f}, {-1714.40f, 2409.30f, 65.7187f},
    {-1722.15f, 2398.06f, 62.1128f}, {-1724.34f, 2394.89f, 61.4252f}, {-1726.05f, 2392.11f, 60.9496f},
    {-1727.84f, 2388.56f, 60.5629f}, {-1729.32f, 2384.89f, 60.3927f}, {-1730.61f, 2380.17f, 60.3907f},
    {-1731.72f, 2376.48f, 60.6323f}, {-1734.81f, 2367.06f, 62.1949f}, {-1736.12f, 2363.44f, 63.0386f},
    {-1737.71f, 2359.56f, 63.9578f}, {-1739.30f, 2356.05f, 64.7262f}, {-1741.02f, 2352.60f, 65.5823f},
    {-1743.04f, 2347.89f, 66.4771f}, {-1744.94f, 2342.87f, 67.4789f}, {-1745.71f, 2339.46f, 68.1087f},
    {-1746.41f, 2334.04f, 68.9845f}, {-1747.39f, 2329.95f, 69.6796f}, {-1749.05f, 2324.61f, 70.6220f},
    {-1750.63f, 2319.84f, 71.4891f}, {-1752.79f, 2314.18f, 72.6934f}, {-1755.33f, 2308.29f, 74.1239f},
    {-1757.75f, 2303.24f, 75.3545f}, {-1760.74f, 2297.82f, 76.6115f}, {-1763.27f, 2293.36f, 77.5786f},
    {-1765.84f, 2288.51f, 78.6195f}, {-1768.73f, 2283.31f, 79.7442f}, {-1771.96f, 2279.18f, 80.5957f},
    {-1775.49f, 2274.83f, 81.5637f}, {-1778.19f, 2271.45f, 82.3589f}, {-1781.42f, 2267.77f, 83.2574f},
    {-1785.36f, 2263.79f, 84.2335f}, {-1788.89f, 2260.24f, 85.1407f}, {-1792.43f, 2256.86f, 86.0662f},
    {-1795.10f, 2254.78f, 86.7011f}, {-1797.38f, 2253.36f, 87.1831f}, {-1800.51f, 2251.78f, 87.7887f},
    {-1803.51f, 2250.49f, 88.2882f}, {-1806.86f, 2249.12f, 88.7494f}, {-1810.80f, 2247.67f, 89.1012f},
    {-1815.26f, 2245.94f, 89.2768f}, {-1819.50f, 2243.74f, 89.3141f}, {-1823.01f, 2241.15f, 89.3152f},
    {-1827.05f, 2237.52f, 89.3152f}, {-1829.64f, 2234.24f, 89.3152f}, {-1832.17f, 2230.31f, 89.3152f},
    {-1834.04f, 2227.36f, 89.3152f}, {-1847.04f, 2207.00f, 89.3152f}, {-1849.34f, 2203.49f, 89.3152f},
    {-1852.05f, 2199.83f, 89.3152f}, {-1854.51f, 2196.88f, 89.3152f}, {-1857.23f, 2193.82f, 89.3152f},
    {-1860.41f, 2190.41f, 89.3152f}, {-1863.10f, 2187.34f, 89.3152f}, {-1865.37f, 2183.95f, 89.3152f},
    {-1867.30f, 2180.23f, 89.3152f}, {-1869.10f, 2176.31f, 89.3152f}, {-1870.31f, 2172.17f, 89.3152f},
    {-1870.89f, 2167.54f, 89.3152f}, {-1871.25f, 2162.77f, 89.3152f}, {-1871.65f, 2157.78f, 89.3152f},
    {-1872.13f, 2151.85f, 89.3152f}, {-1872.48f, 2146.26f, 89.3152f}, {-1872.56f, 2140.54f, 89.3152f},
    {-1872.73f, 2134.82f, 89.3152f}, {-1872.89f, 2129.81f, 89.3152f}, {-1873.05f, 2124.45f, 89.3152f},
    {-1873.13f, 2090.28f, 89.3152f}, {-1872.86f, 2085.45f, 89.3152f}, {-1872.56f, 2081.02f, 89.3152f},
    {-1872.78f, 2076.59f, 89.3152f}, {-1873.25f, 2072.27f, 89.3180f}, {-1873.69f, 2068.15f, 89.3180f},
    {-1874.60f, 2064.66f, 89.3180f}, {-1876.12f, 2060.87f, 89.3180f}, {-1881.88f, 2047.09f, 89.3180f},
    {-1883.68f, 2042.54f, 89.3180f}, {-1885.03f, 2038.56f, 89.3180f}, {-1885.82f, 2035.27f, 89.3180f},
    {-1886.43f, 2030.41f, 89.3180f}, {-1886.48f, 2025.63f, 89.3180f}, {-1886.04f, 2008.03f, 89.3180f},
    {-1885.73f, 2004.31f, 89.3180f}, {-1884.86f, 1999.37f, 89.3180f}, {-1883.38f, 1993.25f, 89.3180f},
    {-1881.41f, 1987.39f, 89.3180f}, {-1879.74f, 1982.54f, 89.3180f}, {-1878.41f, 1976.86f, 89.3180f},
    {-1877.40f, 1970.76f, 89.1784f}, {-1876.46f, 1964.89f, 89.1753f}, {-1876.09f, 1959.19f, 89.1462f},
    {-1875.95f, 1944.25f, 89.1361f}, {-1876.19f, 1936.32f, 89.1361f}, {-1877.17f, 1931.75f, 89.1370f},
    {-1878.18f, 1927.44f, 89.1357f}, {-1879.19f, 1923.12f, 89.1357f}, {-1880.41f, 1918.98f, 89.1464f},
    {-1881.69f, 1915.72f, 89.1504f}, {-1883.60f, 1912.25f, 89.1504f}, {-1885.76f, 1909.50f, 89.1504f},
    {-1888.09f, 1907.39f, 89.1504f}, {-1891.11f, 1905.41f, 89.1504f}, {-1893.98f, 1904.40f, 89.1504f},
    {-1897.16f, 1903.79f, 89.2858f}, {-1900.89f, 1903.62f, 89.2858f}, {-1989.27f, 1901.26f, 89.2858f},
    {-1992.73f, 1901.72f, 89.2858f}, {-1996.43f, 1902.81f, 89.2858f}, {-2000.06f, 1904.07f, 89.3037f},
    {-2004.22f, 1905.61f, 88.6846f}, {-2017.88f, 1910.70f, 87.2574f}, {-2021.90f, 1911.90f, 86.6607f},
    {-2026.83f, 1912.84f, 85.7119f}, {-2031.60f, 1913.18f, 84.5859f}, {-2037.07f, 1912.87f, 83.0551f},
    {-2042.77f, 1912.41f, 81.2983f}, {-2047.88f, 1911.23f, 79.5770f}, {-2052.35f, 1909.89f, 77.9273f},
    {-2057.29f, 1908.11f, 75.8834f}, {-2061.05f, 1906.53f, 74.1705f}, {-2064.73f, 1904.76f, 72.3419f},
    {-2068.25f, 1902.93f, 70.4748f}, {-2070.72f, 1901.40f, 69.0681f}, {-2073.34f, 1899.65f, 67.5201f},
    {-2076.22f, 1897.46f, 65.7030f}, {-2079.34f, 1895.00f, 63.6801f}, {-2082.45f, 1892.54f, 61.6431f},
    {-2085.66f, 1889.83f, 59.4803f}, {-2099.73f, 1876.36f, 49.4650f}, {-2103.91f, 1871.35f, 46.4651f},
    {-2105.24f, 1868.12f, 45.0425f}, {-2106.48f, 1863.99f, 43.3699f}, {-2107.32f, 1860.36f, 41.9599f},
    {-2108.37f, 1856.66f, 40.4503f}, {-2109.77f, 1852.58f, 38.7254f}, {-2111.57f, 1848.53f, 36.9247f},
    {-2114.09f, 1843.80f, 34.7661f}, {-2116.31f, 1840.10f, 33.0838f}, {-2118.76f, 1836.69f, 31.4792f},
    {-2122.27f, 1832.33f, 29.3811f}, {-2125.37f, 1829.00f, 27.6824f}, {-2129.17f, 1825.38f, 25.7498f},
    {-2133.01f, 1822.33f, 23.9862f}, {-2136.39f, 1819.85f, 22.5346f}, {-2140.01f, 1817.73f, 21.1382f},
    {-2144.40f, 1815.56f, 19.6024f}, {-2148.64f, 1813.93f, 18.3443f}, {-2153.52f, 1812.32f, 17.0885f},
    {-2157.99f, 1810.99f, 16.0627f}, {-2163.95f, 1809.84f, 14.7597f}, {-2169.38f, 1809.12f, 13.6626f},
    {-2173.67f, 1808.70f, 13.0417f}, {-2179.26f, 1808.29f, 12.5947f}, {-2214.60f, 1807.81f, 11.7800f},
    {-2222.59f, 1805.70f, 11.6520f}, {-2227.72f, 1803.75f, 11.6930f}, {-2232.96f, 1801.84f, 11.7494f},
    {-2240.47f, 1799.69f, 11.8492f}, {-2247.15f, 1798.05f, 11.9347f}, {-2252.60f, 1796.76f, 11.9457f},
    {-2257.29f, 1795.35f, 11.8931f}, {-2263.04f, 1793.44f, 11.7971f}, {-2269.95f, 1790.94f, 11.6187f},
    {-2273.97f, 1789.73f, 11.4691f}, {-2278.09f, 1788.93f, 11.3426f}, {-2281.56f, 1788.42f, 11.2754f},
    {-2291.59f, 1786.91f, 11.2732f}, {-2296.98f, 1784.70f, 11.2823f}, {-2302.69f, 1781.34f, 11.2281f},
    {-2307.05f, 1776.95f, 11.1321f}, {-2310.64f, 1772.97f, 11.0177f}, {-2319.35f, 1762.46f, 11.0401f},
};

const float AstherWP[18][3]=
{
    {-2129.99f, 1824.12f, 25.234f}, {-2132.93f, 1822.23f, 23.984f}, {-2135.81f, 1820.23f, 22.770f},
    {-2138.72f, 1818.29f, 21.595f}, {-2141.77f, 1816.57f, 20.445f}, {-2144.88f, 1814.96f, 19.380f},
    {-2147.19f, 1813.85f, 18.645f}, {-2150.51f, 1812.73f, 17.760f}, {-2153.88f, 1811.80f, 16.954f},
    {-2157.28f, 1810.95f, 16.194f}, {-2160.69f, 1810.20f, 15.432f}, {-2164.12f, 1809.46f, 14.688f},
    {-2167.55f, 1808.81f, 13.961f}, {-2171.01f, 1808.27f, 13.316f}, {-2174.32f, 1808.00f, 12.935f},
    {-2177.11f, 1807.75f, 12.717f}, {-2179.79f, 1807.67f, 12.573f}, {-2183.06f, 1807.59f, 12.504f},
};

const float KorothWP[14][3]=
{
    {-2213.64f, 1863.51f, 15.404f}, {-2212.69f, 1860.14f, 15.302f}, {-2211.15f, 1853.31f, 15.078f},
    {-2210.39f, 1849.90f, 15.070f}, {-2209.11f, 1845.65f, 15.377f}, {-2206.19f, 1839.29f, 15.147f},
    {-2204.92f, 1836.03f, 14.420f}, {-2203.76f, 1832.73f, 13.432f}, {-2201.68f, 1826.04f, 12.296f},
    {-2200.68f, 1822.69f, 12.194f}, {-2199.22f, 1818.77f, 12.175f}, {-2196.29f, 1813.86f, 12.253f},
    {-2192.46f, 1811.06f, 12.445f}, {-2186.79f, 1808.90f, 12.513f},
};

static float WorgensBackPos[8][3] =
{
    {-2148.38f, 1606.51f, -43.7285f},
    {-2131.82f, 1588.68f, -48.6709f},
    {-2124.88f, 1575.87f, -49.9036f},
    {-2114.25f, 1538.97f, -14.2722f},
    {-2124.04f, 1601.32f, -43.8004f},
    {-2115.53f, 1595.73f, -46.1633f},
    {-2107.05f, 1590.90f, -47.7032f},
    {-2092.86f, 1584.43f, -20.3403f}
};

const float TobiasWP[4][3]=
{
    {-2073.85f, 1295.89f, -84.1445f},
    {-2071.12f, 1283.95f, -85.7678f},
    {-2073.66f, 1293.04f, -84.5568f},
    {-2073.56f, 1308.08f, -83.2562f},
};

static float MilSumPos[5][4] =
{
    {-1417.38f, 1268.43f, 36.4288f, 1.79838f}, {-1405.55f, 1271.08f, 36.4288f, 1.79838f}, {-1407.71f, 1270.60f, 36.4288f, 1.79838f},
    {-1414.01f, 1269.18f, 36.4288f, 1.79838f}, {-1410.82f, 1269.88f, 36.4288f, 1.79838f},
};

static float FWaveWP[96][3] =
{
    {-1413.55f, 1280.21f, 36.4288f}, {-1416.87f, 1293.56f, 36.4288f}, {-1420.47f, 1308.52f, 36.4288f},
    {-1423.92f, 1321.51f, 34.7850f}, {-1428.06f, 1337.45f, 35.0664f}, {-1430.56f, 1351.07f, 35.5555f},
    {-1432.38f, 1359.40f, 35.5555f}, {-1437.86f, 1371.31f, 35.5555f}, {-1441.41f, 1384.88f, 35.5555f},
    {-1448.19f, 1391.53f, 35.5555f}, {-1459.86f, 1393.28f, 35.5555f}, {-1473.94f, 1393.60f, 35.5555f},
    {-1481.17f, 1399.55f, 35.5555f}, {-1487.36f, 1404.91f, 35.5555f}, {-1490.94f, 1412.63f, 35.5555f},
    {-1496.62f, 1419.40f, 35.5555f}, {-1505.01f, 1426.68f, 35.5555f}, {-1518.00f, 1428.23f, 35.5555f},
    {-1527.14f, 1427.80f, 35.5555f}, {-1536.65f, 1427.62f, 35.5555f}, {-1542.92f, 1420.95f, 35.5555f},
    {-1548.23f, 1413.49f, 35.5555f}, {-1550.40f, 1402.23f, 35.5555f}, {-1552.52f, 1388.96f, 35.5668f},
    {-1553.37f, 1378.18f, 35.5867f}, {-1553.83f, 1368.68f, 35.6061f}, {-1556.94f, 1358.32f, 35.5574f},
    {-1557.53f, 1350.16f, 35.5574f}, {-1559.71f, 1341.59f, 35.5574f}, {-1562.41f, 1331.79f, 35.5574f},
    {-1565.47f, 1323.50f, 35.5574f}, {-1574.10f, 1317.31f, 35.5561f}, {-1581.09f, 1316.72f, 32.4373f},
    {-1589.80f, 1315.95f, 26.8717f}, {-1601.94f, 1314.38f, 19.2131f}, {-1612.36f, 1313.10f, 18.4492f},
    {-1626.31f, 1309.77f, 20.3373f}, {-1634.95f, 1306.97f, 19.6632f}, {-1645.84f, 1304.63f, 19.7690f},
    {-1657.72f, 1304.68f, 19.7826f}, {-1666.91f, 1306.79f, 19.7826f}, {-1673.02f, 1309.37f, 19.7826f},
    {-1682.11f, 1309.02f, 19.7826f}, {-1692.83f, 1307.45f, 19.7826f}, {-1702.07f, 1308.50f, 19.7826f},
    {-1710.78f, 1310.87f, 19.7826f}, {-1720.07f, 1313.75f, 19.7826f}, {-1729.48f, 1316.44f, 19.7826f},
    {-1738.53f, 1319.14f, 19.7827f}, {-1744.64f, 1325.33f, 19.7910f}, {-1743.33f, 1332.14f, 19.7600f},
    {-1739.08f, 1339.84f, 19.7900f}, {-1735.41f, 1346.33f, 19.8821f}, {-1737.87f, 1355.13f, 19.7836f},
    {-1742.57f, 1366.28f, 19.7368f}, {-1742.55f, 1378.77f, 19.7637f}, {-1738.08f, 1383.00f, 19.8238f},
    {-1731.88f, 1389.21f, 20.2902f}, {-1725.81f, 1395.97f, 21.6685f}, {-1708.44f, 1393.12f, 21.6687f},
    {-1725.65f, 1396.38f, 21.6688f}, {-1731.76f, 1389.38f, 20.3340f}, {-1739.26f, 1381.20f, 19.9172f},
    {-1748.73f, 1378.62f, 19.8574f}, {-1755.88f, 1386.24f, 19.8921f}, {-1762.23f, 1389.28f, 19.8999f},
    {-1771.95f, 1385.38f, 19.6324f}, {-1783.07f, 1381.10f, 19.6560f}, {-1789.69f, 1378.86f, 19.7935f},
    {-1793.91f, 1388.03f, 19.8703f}, {-1797.87f, 1396.60f, 19.9024f}, {-1802.12f, 1406.22f, 19.8385f},
    {-1803.79f, 1416.60f, 19.9540f}, {-1803.84f, 1428.15f, 19.7620f}, {-1803.67f, 1436.52f, 19.5282f},
    {-1803.33f, 1450.17f, 18.9785f}, {-1804.19f, 1463.43f, 18.9459f}, {-1805.42f, 1475.28f, 19.3649f},
    {-1804.10f, 1489.54f, 19.6081f}, {-1802.48f, 1506.96f, 19.7809f}, {-1801.51f, 1517.42f, 19.7809f},
    {-1800.07f, 1533.10f, 20.3703f}, {-1798.80f, 1546.33f, 22.6018f}, {-1796.41f, 1566.48f, 20.6045f},
    {-1791.83f, 1578.21f, 20.5046f}, {-1785.51f, 1590.68f, 20.5046f}, {-1778.93f, 1601.81f, 20.4943f},
    {-1770.99f, 1613.75f, 20.4806f}, {-1763.55f, 1624.02f, 20.4549f}, {-1756.86f, 1632.89f, 20.4574f},
    {-1749.70f, 1642.31f, 20.4742f}, {-1741.83f, 1650.73f, 20.4769f}, {-1729.43f, 1649.46f, 20.5003f},
    {-1718.56f, 1643.08f, 20.4859f}, {-1709.44f, 1635.46f, 20.4868f}, {-1703.89f, 1621.66f, 20.488f},
};

static float SWaveWP[96][3] =
{
    {-1414.28f, 1281.94f, 36.4280f}, {-1416.92f, 1294.20f, 36.4280f}, {-1419.48f, 1305.69f, 36.4280f},
    {-1423.35f, 1320.92f, 34.9943f}, {-1428.30f, 1340.61f, 35.2325f}, {-1431.12f, 1352.39f, 35.5555f},
    {-1428.25f, 1363.49f, 35.5555f}, {-1415.41f, 1367.46f, 35.5555f}, {-1404.02f, 1368.76f, 35.5555f},
    {-1402.89f, 1381.81f, 35.5555f}, {-1403.01f, 1395.25f, 35.5555f}, {-1403.17f, 1410.33f, 35.5555f},
    {-1403.29f, 1419.51f, 35.5555f}, {-1415.19f, 1420.97f, 35.5555f}, {-1425.01f, 1420.85f, 35.5555f},
    {-1440.90f, 1415.44f, 35.5555f}, {-1449.79f, 1417.30f, 35.5555f}, {-1460.94f, 1422.83f, 35.5555f},
    {-1471.50f, 1421.22f, 35.5561f}, {-1483.56f, 1416.41f, 35.5561f}, {-1491.02f, 1408.94f, 35.5526f},
    {-1497.86f, 1401.01f, 35.5544f}, {-1503.09f, 1390.99f, 35.5544f}, {-1503.35f, 1380.46f, 35.5559f},
    {-1503.22f, 1366.37f, 35.5559f}, {-1503.67f, 1353.95f, 35.5559f}, {-1502.12f, 1343.28f, 35.5559f},
    {-1514.94f, 1339.25f, 35.5559f}, {-1527.78f, 1335.34f, 35.5559f}, {-1541.24f, 1331.24f, 35.5559f},
    {-1553.27f, 1327.01f, 35.5559f}, {-1575.15f, 1317.28f, 35.6661f}, {-1592.38f, 1315.01f, 25.1970f},
    {-1603.89f, 1313.98f, 18.2722f}, {-1618.07f, 1311.27f, 19.5098f}, {-1630.88f, 1307.71f, 19.6631f},
    {-1635.25f, 1316.63f, 19.7302f}, {-1633.90f, 1330.20f, 20.0334f}, {-1634.21f, 1342.10f, 20.0680f},
    {-1634.19f, 1354.70f, 20.0336f}, {-1633.62f, 1367.65f, 20.0763f}, {-1633.01f, 1380.93f, 19.9831f},
    {-1635.32f, 1391.15f, 20.0172f}, {-1640.15f, 1398.41f, 19.9265f}, {-1648.97f, 1394.12f, 19.7717f},
    {-1658.94f, 1384.64f, 15.1348f}, {-1664.69f, 1379.02f, 15.1348f}, {-1666.38f, 1367.59f, 15.1348f},
    {-1679.83f, 1357.66f, 15.1350f}, {-1685.89f, 1348.24f, 15.0921f}, {-1693.74f, 1345.67f, 15.1352f},
    {-1705.31f, 1347.81f, 19.8963f}, {-1720.84f, 1349.75f, 19.7692f}, {-1732.15f, 1357.35f, 19.7361f},
    {-1747.93f, 1371.39f, 20.0557f}, {-1742.55f, 1378.77f, 19.7637f}, {-1738.08f, 1383.00f, 19.8238f},
    {-1731.88f, 1389.21f, 20.2902f}, {-1725.81f, 1395.97f, 21.6685f}, {-1728.80f, 1415.51f, 21.6690f},
    {-1721.26f, 1399.50f, 21.6688f}, {-1728.82f, 1392.81f, 21.2507f}, {-1736.99f, 1385.54f, 19.8094f},
    {-1745.34f, 1383.50f, 19.4523f}, {-1754.56f, 1384.33f, 19.8305f}, {-1759.61f, 1393.94f, 19.4199f},
    {-1765.72f, 1405.63f, 19.7639f}, {-1770.79f, 1417.34f, 19.7827f}, {-1770.45f, 1427.12f, 19.8364f},
    {-1769.89f, 1438.66f, 19.9248f}, {-1769.39f, 1446.68f, 19.9417f}, {-1777.06f, 1452.54f, 19.3633f},
    {-1786.10f, 1453.36f, 19.3591f}, {-1796.93f, 1453.80f, 19.2225f}, {-1802.89f, 1454.04f, 18.9536f},
    {-1804.83f, 1461.50f, 18.8724f}, {-1805.31f, 1472.68f, 19.3047f}, {-1806.14f, 1482.08f, 19.4244f},
    {-1807.22f, 1492.52f, 19.6339f}, {-1804.81f, 1503.11f, 19.7793f}, {-1801.64f, 1514.61f, 19.7814f},
    {-1799.94f, 1527.42f, 20.1764f}, {-1800.05f, 1535.51f, 20.4954f}, {-1798.68f, 1550.11f, 22.7365f},
    {-1796.03f, 1568.14f, 20.4859f}, {-1793.33f, 1575.31f, 20.5061f}, {-1787.77f, 1588.54f, 20.5061f},
    {-1783.55f, 1598.15f, 20.4917f}, {-1773.91f, 1612.26f, 20.4835f}, {-1767.50f, 1620.57f, 20.4691f},
    {-1754.33f, 1636.88f, 20.4678f}, {-1746.97f, 1644.35f, 20.4766f}, {-1732.60f, 1651.56f, 20.4996f},
    {-1723.52f, 1646.36f, 20.4860f}, {-1714.70f, 1640.66f, 20.4860f}, {-1701.76f, 1629.37f, 20.4883f},
};

static float TWaveWP[87][3] =
{
    {-1412.03f, 1273.98f, 36.4282f}, {-1414.81f, 1285.10f, 36.4282f}, {-1418.16f, 1299.11f, 36.4282f},
    {-1421.74f, 1313.09f, 36.4282f}, {-1424.13f, 1321.95f, 34.7826f}, {-1428.55f, 1339.75f, 35.1950f},
    {-1433.06f, 1356.87f, 35.5555f}, {-1443.26f, 1362.80f, 35.5555f}, {-1457.30f, 1356.49f, 35.5555f},
    {-1467.95f, 1351.41f, 35.5555f}, {-1479.48f, 1348.99f, 35.5555f}, {-1489.47f, 1345.82f, 35.5555f},
    {-1502.09f, 1342.30f, 35.5563f}, {-1514.73f, 1338.77f, 35.5563f}, {-1524.50f, 1336.05f, 35.5563f},
    {-1536.50f, 1332.70f, 35.5563f}, {-1549.44f, 1329.09f, 35.5563f}, {-1566.80f, 1322.35f, 35.5563f},
    {-1575.15f, 1317.28f, 35.6661f}, {-1592.38f, 1315.01f, 25.1970f}, {-1603.89f, 1313.98f, 18.2722f},
    {-1618.07f, 1311.27f, 19.5098f}, {-1630.88f, 1307.71f, 19.6631f}, {-1635.25f, 1316.63f, 19.7302f},
    {-1633.90f, 1330.20f, 20.0334f}, {-1634.21f, 1342.10f, 20.0680f}, {-1634.19f, 1354.70f, 20.0336f},
    {-1633.62f, 1367.65f, 20.0763f}, {-1633.01f, 1380.93f, 19.9831f}, {-1635.32f, 1391.15f, 20.0172f},
    {-1640.15f, 1398.41f, 19.9265f}, {-1648.97f, 1394.12f, 19.7717f}, {-1658.94f, 1384.64f, 15.1348f},
    {-1664.69f, 1379.02f, 15.1348f}, {-1666.38f, 1367.59f, 15.1348f}, {-1679.83f, 1357.66f, 15.1350f},
    {-1685.89f, 1348.24f, 15.0921f}, {-1693.74f, 1345.67f, 15.1352f}, {-1705.31f, 1347.81f, 19.8963f},
    {-1720.84f, 1349.75f, 19.7692f}, {-1732.15f, 1357.35f, 19.7361f}, {-1747.93f, 1371.39f, 20.0557f},
    {-1742.55f, 1378.77f, 19.7637f}, {-1738.08f, 1383.00f, 19.8238f}, {-1731.88f, 1389.21f, 20.2902f},
    {-1725.81f, 1395.97f, 21.6685f}, {-1702.28f, 1418.09f, 21.6690f}, {-1721.26f, 1399.50f, 21.6688f},
    {-1728.82f, 1392.81f, 21.2507f}, {-1736.99f, 1385.54f, 19.8094f}, {-1745.34f, 1383.50f, 19.4523f},
    {-1754.56f, 1384.33f, 19.8305f}, {-1759.61f, 1393.94f, 19.4199f}, {-1765.72f, 1405.63f, 19.7639f},
    {-1770.79f, 1417.34f, 19.7827f}, {-1770.45f, 1427.12f, 19.8364f}, {-1769.89f, 1438.66f, 19.9248f},
    {-1769.39f, 1446.68f, 19.9417f}, {-1764.81f, 1453.63f, 20.2949f}, {-1761.72f, 1458.27f, 20.4706f},
    {-1761.05f, 1464.88f, 20.9578f}, {-1759.51f, 1472.06f, 22.2850f}, {-1761.05f, 1479.98f, 23.8829f},
    {-1761.02f, 1487.69f, 25.4303f}, {-1760.98f, 1496.08f, 26.2232f}, {-1761.44f, 1506.22f, 26.2232f},
    {-1761.94f, 1510.83f, 26.2229f}, {-1768.24f, 1510.83f, 26.4891f}, {-1784.71f, 1510.93f, 19.8397f},
    {-1794.80f, 1511.92f, 19.7824f}, {-1800.90f, 1514.54f, 19.7824f}, {-1800.16f, 1526.06f, 19.9329f},
    {-1799.62f, 1535.14f, 20.4793f}, {-1798.86f, 1546.31f, 22.5989f}, {-1796.76f, 1564.72f, 21.1471f},
    {-1793.73f, 1574.39f, 20.5042f}, {-1788.02f, 1587.81f, 20.5056f}, {-1781.75f, 1601.10f, 20.4851f},
    {-1773.87f, 1612.23f, 20.4828f}, {-1764.28f, 1625.17f, 20.3991f}, {-1755.13f, 1635.73f, 20.4639f},
    {-1746.64f, 1645.04f, 20.4758f}, {-1734.06f, 1651.90f, 20.4941f}, {-1724.22f, 1646.66f, 20.4858f},
    {-1715.14f, 1640.71f, 20.4858f}, {-1701.02f, 1635.61f, 20.4884f}, {-1692.68f, 1633.03f, 20.4884f},
};

static float LornaWP[78][3] =
{
    {-1551.90f, 1284.75f, 13.992f}, {-1561.35f, 1282.32f, 21.372f}, {-1581.19f, 1277.05f, 35.878f},
    {-1587.59f, 1275.81f, 35.880f}, {-1591.07f, 1284.64f, 35.878f}, {-1591.32f, 1291.64f, 35.981f},
    {-1579.00f, 1296.02f, 35.879f}, {-1569.90f, 1303.33f, 35.879f}, {-1568.92f, 1315.40f, 35.556f},
    {-1565.98f, 1319.25f, 35.556f}, {-1572.58f, 1318.18f, 35.556f}, {-1574.10f, 1317.31f, 35.556f},
    {-1581.09f, 1316.72f, 32.437f}, {-1589.80f, 1315.95f, 26.871f}, {-1601.94f, 1314.38f, 19.213f},
    {-1612.36f, 1313.10f, 18.449f}, {-1626.31f, 1309.77f, 20.337f}, {-1634.95f, 1306.97f, 19.663f},
    {-1645.84f, 1304.63f, 19.769f}, {-1657.72f, 1304.68f, 19.782f}, {-1666.91f, 1306.79f, 19.782f},
    {-1673.02f, 1309.37f, 19.782f}, {-1682.11f, 1309.02f, 19.782f}, {-1692.83f, 1307.45f, 19.782f},
    {-1702.07f, 1308.50f, 19.782f}, {-1710.78f, 1310.87f, 19.782f}, {-1720.07f, 1313.75f, 19.782f},
    {-1729.48f, 1316.44f, 19.782f}, {-1738.53f, 1319.14f, 19.782f}, {-1744.64f, 1325.33f, 19.791f},
    {-1743.33f, 1332.14f, 19.760f}, {-1739.08f, 1339.84f, 19.790f}, {-1735.41f, 1346.33f, 19.882f},
    {-1737.87f, 1355.13f, 19.783f}, {-1742.57f, 1366.28f, 19.736f}, {-1742.55f, 1378.77f, 19.763f},
    {-1738.08f, 1383.00f, 19.823f}, {-1731.88f, 1389.21f, 20.290f}, {-1725.81f, 1395.97f, 21.668f},
    {-1731.88f, 1389.21f, 20.290f}, {-1725.81f, 1395.97f, 21.668f}, {-1708.44f, 1393.12f, 21.668f},
    {-1725.65f, 1396.38f, 21.668f}, {-1731.76f, 1389.38f, 20.334f}, {-1739.26f, 1381.20f, 19.917f},
    {-1748.73f, 1378.62f, 19.857f}, {-1755.88f, 1386.24f, 19.892f}, {-1762.23f, 1389.28f, 19.899f},
    {-1771.95f, 1385.38f, 19.632f}, {-1783.07f, 1381.10f, 19.656f}, {-1789.69f, 1378.86f, 19.793f},
    {-1793.91f, 1388.03f, 19.870f}, {-1797.87f, 1396.60f, 19.902f}, {-1802.12f, 1406.22f, 19.838f},
    {-1803.79f, 1416.60f, 19.954f}, {-1803.84f, 1428.15f, 19.762f}, {-1803.67f, 1436.52f, 19.528f},
    {-1803.33f, 1450.17f, 18.978f}, {-1804.19f, 1463.43f, 18.945f}, {-1805.42f, 1475.28f, 19.364f},
    {-1804.10f, 1489.54f, 19.608f}, {-1802.48f, 1506.96f, 19.780f}, {-1801.51f, 1517.42f, 19.780f},
    {-1800.07f, 1533.10f, 20.370f}, {-1798.80f, 1546.33f, 22.601f}, {-1796.41f, 1566.48f, 20.604f},
    {-1791.83f, 1578.21f, 20.504f}, {-1785.51f, 1590.68f, 20.504f}, {-1778.93f, 1601.81f, 20.494f},
    {-1770.99f, 1613.75f, 20.480f}, {-1763.55f, 1624.02f, 20.454f}, {-1756.86f, 1632.89f, 20.457f},
    {-1749.70f, 1642.31f, 20.474f}, {-1741.83f, 1650.73f, 20.476f}, {-1729.43f, 1649.46f, 20.500f},
    {-1718.56f, 1643.08f, 20.485f}, {-1709.44f, 1635.46f, 20.486f}, {-1703.89f, 1621.66f, 20.488f},
};

static float FDarkRangerWP[31][3] =
{
    {-1572.91f, 1319.44f, 35.556f}, {-1546.53f, 1328.34f, 35.556f}, {-1531.53f, 1333.13f, 35.556f},
    {-1518.78f, 1335.67f, 35.556f}, {-1507.45f, 1338.38f, 35.556f}, {-1503.32f, 1347.66f, 35.556f},
    {-1503.03f, 1358.11f, 35.556f}, {-1503.48f, 1374.93f, 35.556f}, {-1503.04f, 1391.36f, 35.556f},
    {-1491.28f, 1393.23f, 35.556f}, {-1477.47f, 1393.06f, 35.556f}, {-1463.66f, 1393.34f, 35.556f},
    {-1445.90f, 1394.58f, 35.556f}, {-1440.95f, 1407.66f, 35.556f}, {-1441.15f, 1413.96f, 35.556f},
    {-1445.75f, 1421.83f, 35.556f}, {-1455.19f, 1427.97f, 35.556f}, {-1463.82f, 1429.19f, 35.556f},
    {-1472.95f, 1423.35f, 35.556f}, {-1484.01f, 1414.69f, 35.556f}, {-1494.87f, 1406.82f, 35.556f},
    {-1500.12f, 1394.48f, 35.556f}, {-1502.17f, 1383.12f, 35.556f}, {-1502.36f, 1367.71f, 35.556f},
    {-1502.69f, 1350.91f, 35.556f}, {-1509.18f, 1344.53f, 35.556f}, {-1517.96f, 1338.98f, 35.556f},
    {-1529.58f, 1336.49f, 35.556f}, {-1543.75f, 1332.65f, 35.556f}, {-1559.08f, 1328.04f, 35.556f},
    {-1565.85f, 1321.06f, 35.556f},
};

static float SDarkRangerWP[31][3] =
{
    {-1570.32f, 1318.69f, 35.556f}, {-1561.46f, 1322.08f, 35.556f}, {-1558.38f, 1334.64f, 35.556f},
    {-1556.23f, 1352.71f, 35.556f}, {-1554.95f, 1368.66f, 35.656f}, {-1554.41f, 1380.55f, 35.596f},
    {-1555.58f, 1388.50f, 35.621f}, {-1548.59f, 1401.41f, 35.557f}, {-1548.38f, 1409.81f, 35.557f},
    {-1548.21f, 1420.64f, 35.557f}, {-1539.33f, 1425.07f, 35.557f}, {-1527.78f, 1425.25f, 35.557f},
    {-1515.68f, 1420.99f, 35.557f}, {-1505.62f, 1411.56f, 35.556f}, {-1493.82f, 1400.40f, 35.556f},
    {-1486.97f, 1394.43f, 35.556f}, {-1473.41f, 1390.72f, 35.555f}, {-1461.84f, 1390.88f, 35.555f},
    {-1449.61f, 1390.36f, 35.555f}, {-1442.35f, 1385.61f, 35.555f}, {-1438.42f, 1371.34f, 35.555f},
    {-1444.42f, 1363.58f, 35.555f}, {-1456.25f, 1358.35f, 35.555f}, {-1469.77f, 1351.00f, 35.555f},
    {-1485.67f, 1346.50f, 35.555f}, {-1499.77f, 1342.79f, 35.556f}, {-1516.27f, 1338.11f, 35.556f},
    {-1532.80f, 1333.51f, 35.556f}, {-1548.94f, 1328.85f, 35.556f}, {-1568.00f, 1322.04f, 35.556f},
};

static float SummonCannonPos[3][4] =
{
    {-1542.55f, 1290.96f, 6.23982f, 3.49002f}, {-1541.04f, 1283.65f, 6.35803f, 3.34331f}, {-1543.06f, 1287.44f, 7.07661f, 3.44634f},
};

static float CannonWP[11][3] =
{
    {-1551.90f, 1284.75f, 13.992f}, {-1561.35f, 1282.32f, 21.372f}, {-1581.19f, 1277.05f, 35.878f},
    {-1587.59f, 1275.81f, 35.880f}, {-1591.07f, 1284.64f, 35.878f}, {-1591.32f, 1291.64f, 35.981f},
    {-1579.00f, 1296.02f, 35.879f}, {-1569.90f, 1303.33f, 35.879f}, {-1568.92f, 1315.40f, 35.556f},
    {-1565.98f, 1319.25f, 35.556f}, {-1572.58f, 1318.18f, 35.556f},
};

static float WorgenSummPos[12][3] =
{
    {-1805.18f, 1313.98f, 19.9797f}, {-1802.93f, 1311.73f, 19.9736f}, {-1799.92f, 1310.26f, 19.9844f},
    {-1799.59f, 1317.28f, 19.9762f}, {-1795.82f, 1313.91f, 19.9859f}, {-1799.38f, 1321.75f, 19.9737f},
    {-1794.08f, 1320.83f, 19.8059f}, {-1791.56f, 1317.74f, 19.7885f}, {-1796.55f, 1308.39f, 19.9725f},
    {-1799.02f, 1314.42f, 19.9754f}, {-1803.21f, 1317.87f, 19.9814f}, {-1795.42f, 1317.05f, 19.9774f},
};

static float FWorgenWP[46][3] =
{
    {-1787.53f, 1325.74f, 19.8783f}, {-1779.44f, 1333.76f, 19.9688f}, {-1770.88f, 1341.79f, 19.7133f},
    {-1764.44f, 1348.68f, 19.4973f}, {-1753.58f, 1355.86f, 19.7710f}, {-1746.08f, 1365.29f, 19.7652f},
    {-1742.12f, 1376.55f, 19.9143f}, {-1735.84f, 1383.94f, 19.8177f}, {-1727.33f, 1393.39f, 21.5739f},
    {-1713.13f, 1406.73f, 21.6684f}, {-1721.26f, 1399.50f, 21.6688f}, {-1728.82f, 1392.81f, 21.2507f},
    {-1736.99f, 1385.54f, 19.8094f}, {-1745.34f, 1383.50f, 19.4523f}, {-1754.56f, 1384.33f, 19.8305f},
    {-1759.61f, 1393.94f, 19.4199f}, {-1765.72f, 1405.63f, 19.7639f}, {-1770.79f, 1417.34f, 19.7827f},
    {-1770.45f, 1427.12f, 19.8364f}, {-1769.89f, 1438.66f, 19.9248f}, {-1769.39f, 1446.68f, 19.9417f},
    {-1777.06f, 1452.54f, 19.3633f}, {-1786.10f, 1453.36f, 19.3591f}, {-1796.93f, 1453.80f, 19.2225f},
    {-1802.89f, 1454.04f, 18.9536f}, {-1804.83f, 1461.50f, 18.8724f}, {-1805.31f, 1472.68f, 19.3047f},
    {-1806.14f, 1482.08f, 19.4244f}, {-1807.22f, 1492.52f, 19.6339f}, {-1804.81f, 1503.11f, 19.7793f},
    {-1801.64f, 1514.61f, 19.7814f}, {-1799.94f, 1527.42f, 20.1764f}, {-1800.05f, 1535.51f, 20.4954f},
    {-1798.68f, 1550.11f, 22.7365f}, {-1796.03f, 1568.14f, 20.4859f}, {-1793.33f, 1575.31f, 20.5061f},
    {-1787.77f, 1588.54f, 20.5061f}, {-1783.55f, 1598.15f, 20.4917f}, {-1773.91f, 1612.26f, 20.4835f},
    {-1767.50f, 1620.57f, 20.4691f}, {-1754.33f, 1636.88f, 20.4678f}, {-1746.97f, 1644.35f, 20.4766f},
    {-1732.60f, 1651.56f, 20.4996f}, {-1723.52f, 1646.36f, 20.4860f}, {-1714.70f, 1640.66f, 20.4860f},
    {-1701.76f, 1629.37f, 20.4883f},
};

static float GreymaneWP[12][3] =
{
    {-1797.62f, 1709.17f, 22.1583f}, {-1786.33f, 1700.06f, 22.1583f}, {-1774.75f, 1690.42f, 22.1583f},
    {-1763.11f, 1680.88f, 22.1583f}, {-1751.00f, 1670.80f, 22.1583f}, {-1743.49f, 1664.47f, 20.4806f},
    {-1734.04f, 1656.68f, 20.4838f}, {-1724.69f, 1648.78f, 20.4952f}, {-1713.05f, 1639.43f, 20.4859f},
    {-1704.06f, 1632.17f, 20.4883f}, {-1691.19f, 1626.66f, 20.4883f}, {-1680.21f, 1629.98f, 20.4883f},
};

const float BatWP[25][3]=
{
    {-1623.96f, 1678.45f, 24.895f}, {-1387.62f, 1635.81f, 50.594f}, {-1351.91f, 1643.61f, 54.694f},
    {-1276.31f, 1669.73f, 63.570f}, {-1252.37f, 1685.45f, 66.732f}, {-1226.15f, 1692.88f, 69.788f},
    {-1210.95f, 1693.08f, 71.477f}, {-1181.78f, 1681.70f, 74.933f}, {-1155.08f, 1665.27f, 78.418f},
    {-1092.38f, 1623.88f, 86.757f}, {-1061.24f, 1617.66f, 90.315f}, {-1005.78f, 1628.92f, 96.749f},
    {-986.882f, 1638.44f, 99.158f}, {-974.702f, 1667.29f, 102.61f}, {-968.120f, 1683.84f, 104.58f},
    {-972.953f, 1711.52f, 107.82f}, {-1003.23f, 1723.95f, 111.92f}, {-1026.01f, 1713.48f, 114.76f},
    {-1098.00f, 1684.50f, 123.43f}, {-1262.51f, 1632.93f, 90.103f}, {-1353.43f, 1605.96f, 72.326f},
    {-1563.83f, 1657.32f, 59.702f}, {-1569.20f, 1658.74f, 59.382f}, {-1638.03f, 1671.92f, 30.168f},
    {-1667.43f, 1666.94f, 22.131f},
};

const float CataCastPos[5][3]=
{
    {-1274.92f, 1632.53f, 12.775f},
    {-1260.25f, 1580.49f, 20.574f},
    {-1256.74f, 1683.23f, 18.304f},
    {-1285.08f, 1708.93f, 9.4898f},
    {-1291.34f, 1721.51f, 10.621f},
};

static float LiamPosition[3] ={-1920.43f, 2308.94f, 40.301f};

#define DELAY_EMOTE_PANICKED_CITIZEN                      urand(5000,  15000)  // 5-15 second time
#define DELAY_SAY_PANICKED_CITIZEN                        urand(30000, 120000) // 30sec - 1.5min
#define DELAY_SAY_GILNEAS_CITY_GUARD_GATE                 urand(30000, 120000) // 30sec - 1.5min
#define PATHS_COUNT_PANICKED_CITIZEN                      8
#define CD_ENRAGE                                         30000
#define SUMMON1_TTL                                       300000
#define PATHS_COUNT                                       2
#define DOOR_TIMER                                        30 * IN_MILLISECONDS
#define Gilneas2                                          654

//bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

const char* GossipMenu[]=
{
    "Расскажи мне подробнее о профессиях, связанных с собирательством и добычей ресурсов.",
    "Расскажи мне подробнее о ремесленных профессиях.",
    "Расскажи мне о травничестве.",
    "Расскажи мне о горном деле.",
    "Расскажи мне о снятии шкур.",
    "Расскажи мне о алхимии.",
    "Расскажи мне о кузнечном деле.",
    "Расскажи мне о наложении чар.",
    "Расскажи мне о инженерном деле.",
    "Расскажи мне о начертании.",
    "Расскажи мне о ювелирном деле.",
    "Расскажи мне о кожевничестве.",
    "Расскажи мне о портняжном деле.",
    "Обучи меня алхимии.",
    "Обучи меня травничеству.",
    "Обучи меня начертанию.",
    "Обучи меня горному делу.",
    "Обучи меня кузнечному делу.",
    "Обучи меня снятию шкур.",
    "Обучи меня инженерному делу.",
    "Обучи меня ювелирному делу.",
    "Обучи меня кожевничеству.",
    "Обучи меня портняжному делу.",
    "Обучи меня наложению чар.",
    "Здесь небезопасно. Спустись в подвал фермы Алленов.",
    "Быстрее, беги к остальным в подвал.",
    "Твоя мать уже вся извелась! Быстрее, беги к ней в подвал!",
    "Я готов присоединится к битве",
    "Я буду ждать следующую группу атакующих!",
    "Я готов сражаться против отрекшихся мразей!",
    "Я еще не готов!",
};

#endif
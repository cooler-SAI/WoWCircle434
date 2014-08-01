/*
*
* Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
* Copyright (C) 2011-2012 ArkCORE <http://www.arkania.net/>
*
*/

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Creature.h"
#include "Vehicle.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"

#ifndef DEF_LOST_ISLES_H
#define DEF_LOST_ISLES_H


enum eCreatures
{
    NPC_GIZMO                               = 36600,
    NPC_CRACK                               = 39198,
    NPC_FRIGHTENED_MINER                    = 35810,
    NPC_MINE_TROUBLES_ORE_CART              = 35814,
    NPC_MINE_TROUBLES_KILL_CREDIT           = 35816,
    NPC_SMART_MINING_MONKEY                 = 35812,
    NPC_ORC_SCOUT                           = 36042,
    NPC_ORC_SCOUT_2                         = 36100,
    NPC_BASTIA                              = 36585,
    NPC_PRIDE_OF_KEZAN                      = 39074,
    NPC_SLING_ROCKET                        = 36514,
    NPC_IT_A_TOWN_IN_A_BOX_KILL_CREDIT      = 38024,
    NPC_SPINY_RAPTOR                        = 38187,
    NPC_MECHASHARK_X_STEAM                  = 38318,
    NPC_NAGA_HATCHLING                      = 44589,
    NPC_NAGA_HATCHLING_KILL_CREDIT          = 38413,
    NPC_NAGA_HATCHLING_1                    = 44580,
    NPC_NAGA_HATCHLING_2                    = 44579,
    NPC_NAGA_HATCHLING_3                    = 44578,
    NPC_NAGA_HATCHLING_4                    = 38412,
    NPC_FACELESS_OF_THE_DEEP_VOID_ZONE      = 38450,
    NPC_FACELESS_OF_THE_DEEP                = 38448,
    NPC_KEZAN_CITIZEN                       = 38745,
    NPC_GOBLIN_SURVIVOR                     = 38409,
    NPC_OOMLOT_WARRIOR                      = 38531,
    NPC_SUPER_BOOSTER_ROCKET_BOOTS          = 38802,
    NPC_FLYING_BOMBER                       = 38929,
    NPC_SASSY_HARDWRENCH                    = 38869,
    NPC_BASTIA_2                            = 39152,
    NPC_MINE_CART                           = 39329,
    NPC_ACE                                 = 34957,
    NPC_ASISTANT_GREELY                     = 39199,
    NPC_IZZY                                = 34959,
    NPC_GOBBER                              = 39205,
    NPC_CAULDRON_TRIGGER                    = 41505,
    NPC_SWEET_OIL_KILL_CREDIT               = 39393,
    NPC_FOOT_BOMB_UNIFORM                   = 47956,
    NPC_ULTIMATE_FOOT_BOMB_UNIFORM          = 39592,
    NPC_TRADE_PRINCE_GALLYWIX               = 39582,
    NPC_THRALL                              = 39594,
    NPC_GOBLIN_ZOMBIE                       = 38753,
};

enum eSpells
{
    SPELL_DEAD_STILL                        = 69010,
    SPELL_FLASH_EFFECT                      = 70649,
    SPELL_CRACK_INVOK                       = 72058,
    SPELL_DEFIBRILLATE                      = 54732,
    SPELL_MINE_TROUBLES_ORE_CART_CHAIN      = 68122,
    SPELL_EXPLODING_BANANAS                 = 67919,
    SPELL_NITRO_POTASSIUM_BANANAS           = 67917,
    SPELL_BOMB                              = 8858,
    SPELL_WEED_WHACKER                      = 68212,
    SPELL_ORC_SCOUT                         = 68338,
    SPELL_IFRARED_HEAT_FOALS                = 69303,
    SPELL_CYCLONE_OF_THE_ELEMENTS           = 68436,
    SPELL_UP_UP_AWAY_KILL_CREDIT_EXPLOSION  = 66127,
    SPELL_ITS_A_TOWN_IN_A_BOX_PLUNGER_EFF   = 68935,
    SPELL_ITS_A_TOWN_IN_A_BOX_EFF1          = 71093,
    SPELL_ITS_A_TOWN_IN_A_BOX_EFF2          = 71095,
    SPELL_ITS_A_TOWN_IN_A_BOX_EFF3          = 71096,
    SPELL_ITS_A_TOWN_IN_A_BOX_EFF4          = 71097,
    SPELL_SUMMON_SPINY_RAPTOR_EGG           = 66726,
    SPELL_IRRESISTIBLE_POOL_PONY            = 71914,
    SPELL_IRRESISTIBLE_POOL_PONY_EFF        = 71917,
    SPELL_FIRE_CANNON                       = 72206,
    SPELL_CANNON_BLAST                      = 72207,
    SPELL_ZVSBRB_SUMMON_SPELL               = 72889,
    SPELL_ZVSBRB_DAMAGE_TRIGGER             = 72885,
    SPELL_FLYING_BOMBERS_ROPE_LADDER        = 73149,
    SPELL_CAMERA_SHAKE                      = 69235,
    SPELL_UNDEFINED_SPELL_1                 = 93668,
    SPELL_METEOR_IMPACT                     = 87701,
    SPELL_VOLCANOTH_DEAD_EXPLOSION          = 73193,
    SPELL_MECHANISM_EXPLOSION               = 93569,
    SPELL_DALARAN_FORGEARMS_SMOKEFX         = 71094,
    SPELL_VOLCANIC_SKYBOX                   = 90418,
    SPELL_REVENUE_STREAM                    = 74005,
    SPELL_UNLOAD_TOXIC_ASSETS               = 74000,
    SPELL_YOU_ARE_FIRED                     = 74004,
    SPELL_CALL_A_MEETING                    = 81000,
    SPELL_DOWNSIZING                        = 74003,
    SPELL_ROCKIN_POWER_INFUSED_ROCKET_BOOTS = 72971,
    SPELL_SUPER_BOOSTER_ROCKET_BOOTS        = 72887,
    
    SPELL_QUEST_PHASE_05                    = 68750,

    PHASE_QUEST_ZONE_SPECIFIC_4             = 67789,
    PHASE_QUEST_ZONE_SPECIFIC_6             = 68481,
    PHASE_QUEST_ZONE_SPECIFIC_9             = 69077,
};

enum Texts
{
    SAY_GYRO                                = 0
};

enum objects
{
    GO_PIEGE                                = 201972,
    GO_UNDEFINED_1                          = 201974,
};

enum eQuests
{
    QUEST_DONT_GO_TO_THE_LIGHT              = 14239,
    QUEST_MINER_TROUBLES                    = 14021,
    QUEST_WEED_WHACKER                      = 14236,
    QUEST_TO_THE_CLIFFS                     = 14240,
    QUEST_IFRARED_IFDRADEAD                 = 14238,
    QUEST_PRECIOUS_CARGO                    = 14242,
    QUEST_WARCHIEVS_REVENGE                 = 14243,
    QUEST_UP_UP_AWAY                        = 14244,
    QUEST_ITS_A_TOWN_IN_A_BOX               = 14245,
    QUEST_A_GOBLIN_IN_SHARKS_CLOTHING       = 24817,
    QUEST_IRRESISTIBLE_POOL_PONY            = 24864,
    QUEST_SURREND_OR_ELSE                   = 24868,
    QUEST_GET_BACK_TO_TOWN                  = 24897,
    QUEST_ZOMBIES_VS_SUUPER_BOOSTER_R_B     = 24942,
    QUEST_OLD_FRIENDS                       = 25023,
    QUEST_LETS_RIDE                         = 25100,
    QUEST_WILD_MINE_CART_RIDE               = 25184,
    QUEST_GOOD_BYE_SWEET_OIL                = 25207,
    QUEST_THE_SLAVE_PITS                    = 25213,
    QUEST_VICTORY                           = 25265,
    QUEST_THE_PRIDE_OF_KEZAN                = 25066,
    QUEST_FINAL_CONFRONTATION               = 25251,
};

const Position cPostitions[2] =
{
    {578.49f,   3132.37f,   0.26f},
    {862.0f,    2778.87f,   114.0f},
};

#define SAY_D_A             "Ecrouabouille, what are you doing sitting there? You do not recognize the one who is lying?"
#define SAY_D_B             "That's $ N! It is only thanks to $ he: she, if we still breath instead of being pieces of rind grids has Kezan."
#define SAY_D_C             "That's $ N! Sorry, doc, I thought $ ghe death:she death; !"
#define SAY_D_D             "Remains back, I'm $ ghe: she; revive! Hoping that this soggy defibrillator will not kill us all!"
#define SAY_D_E             "Caution! OFF!"
#define SAY_D_F             "That's all I can do. C'' is a $ ghe: she; to react now. You hear me, $ N? Come on, wake up now! Do not go to the Light!"
#define SAY_D_G             "You made the right choice. We need you so much, $ N. Try not to get killed here. "
#define SAY_D_H             "There are other survivors that I must attend. I'll see you on the shore."
#define NAGA_SAY_A          "WHO OSE?"
#define NAGA_SAY_B          "Small goblins? I remember the creation of your race."
#define NAGA_SAY_C          "Your threats do not impress me, nor the naga. Get ready to disappear from this reality."
#define NAGA_SAY_D          "Now, young $ N, you will die!"
#define QUEST_RENDER_CRACK  "I want to let these little demons, $ N. Nagas will never attack until we have grandchildren."
#define QUEST_RESET_CRACK   "You are $ gpret, ready to force their leader to get $ gmy friend: my friend; ?"
#define CRACK_PROVOC        "Come on, Chief naga, come out of your hiding and surrendered for $ N and Bilgewater Cartel!"
#define CRACK_EVADE         "Hula treasure, it smells bad. I am size!"
#define SAY_0               "Ok, here we go. You cover me, okay?"
#define SAY_1               "We touch the jackpot with this place!"
#define SAY_2               "Wow, what is that all these cave paintings? Oh, I hear monkeys, the?"
#define SAY_3               "Move to the next."
#define SAY_4               "It will suffice for now. I can go it alone. Thank you for escorting me, $ N."
#define SAY_5               "Would you take a gyrocopter?"
#define SAY_6               "Hang on $ N! We need makes a clearance here, NOW!"
#define SAY_7               "OH MY GOD! BEHIND YOU!"
#define SAY_8               "WE BURN !!"
#define SAY_9               "Climb in the back. We're going. I know where are our old friends the orcs"

#endif
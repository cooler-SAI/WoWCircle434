#ifndef DEF_THRONEOFTHEFOURWINDS_H
#define DEF_THRONEOFTHEFOURWINDS_H

enum Data
{
    DATA_CONCLAVE_OF_WIND_EVENT,
    DATA_ALAKIR_EVENT,
};

enum Data64
{
    DATA_ANSHAL,
    DATA_NEZIR,
    DATA_ROHASH,
    DATA_ALAKIR,
};

enum CreatureIds
{
    BOSS_ANSHAL                     = 45870,
    BOSS_NEZIR                      = 45871,
    BOSS_ROHASH                     = 45872,

    BOSS_ALAKIR                     = 46753,

    // Conclave of Wind
    NPC_SOOTHING_BREEZE             = 46246,
    NPC_RAVENOUS_CREEPER            = 45812,
    NPC_RAVENOUS_CREEPER_TRIGGER    = 45813,
    NPC_ICE_PATCH                   = 46186,
    NPC_CYCLONE                     = 46419,
};

enum InstanceSpells
{
    SPELL_PRE_COMBAT_EFFECT_ANSHAL  = 85537,
    SPELL_PRE_COMBAT_EFFECT_NEZIR   = 85532,
    SPELL_PRE_COMBAT_EFFECT_ROHASH  = 85538,
};

enum ObjectIds
{
    GO_FLOOR_ALAKIR                     = 207737,
    GO_HEART_OF_THE_WIND				= 207891,
};

enum areatrig
{
    AREA_TO4W = 5638,
};

#endif
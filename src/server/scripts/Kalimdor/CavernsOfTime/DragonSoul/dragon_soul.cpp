#include "ScriptPCH.h"
#include "dragon_soul.h"

enum Adds
{
    NPC_HARBRINGER_OF_TWILIGHT      = 55969,
    NPC_HARBRINGER_OF_DESTRUCTION   = 55967,
    NPC_ARCANE_WARDEN               = 56141,
    NPC_FORCE_OF_DESTRUCTION        = 56143,
    NPC_TIME_WARDEN_1               = 56142,
    NPC_PORTENT_OF_TWILIGHT         = 56144,
    NPC_TIME_WARDEN_2               = 57474,
    NPC_CHAMPION_OF_TIME            = 55913,
    NPC_DREAM_WARDEN                = 56140,
    NPC_LIFE_WARDEN_1               = 56139,
    NPC_LIFE_WARDEN_2               = 57473,
    NPC_CHAMPION_OF_LIFE            = 55911,
    NPC_CHAMPION_OF_MAGIC           = 55912,
    NPC_CHAMPION_OF_EMERALD_DREAM   = 55914,
    NPC_FACELESS_DESTROYER          = 57746,
    NPC_FACELESS_CORRUPTOR          = 57749,
    NPC_RUIN_TENTACLE               = 57751,
    NPC_DREAM_WARDEN_1              = 56140,
    NPC_DREAM_WARDEN_2              = 57475,
    NPC_TWILIGHT_ASSAULTER_1        = 56249,
    NPC_TWILIGHT_ASSAULTER_2        = 56250,
    NPC_TWILIGHT_ASSAULTER_3        = 56251,
    NPC_TWILIGHT_ASSAULTER_4        = 56252,
    NPC_TWILIGHT_ASSAULTER_5        = 57795,

    NPC_ANCIENT_WATER_LORD          = 57160,
    NPC_EARTHEN_DESTROYER           = 57158,
    NPC_EARTHEN_SOLDIER             = 57159,

    // deathwing 56173 (boss)
    // deathwing 57962 (near boss)
    // deathwing 53879 (spine)
    // deathwing 55971 (near tower)
};

enum Spells
{
    // Ancient Water Lord
    SPELL_DRENCHED              = 107801,
    SPELL_FLOOD_AOE             = 107796,
    SPELL_FLOOD                 = 107797,
    SPELL_FLOOD_CHANNEL         = 107791,

    // Earthen Destroyer
    SPELL_BOULDER_SMASH_AOE     = 107596,
    SPELL_BOULDER_SMASH         = 107597,
    SPELL_DUST_STORM            = 107682,

    // Earthen Soldier
    SPELL_SHADOW_BOLT           = 95440,
    SPELL_TWILIGHT_CORRUPTION   = 107852,
    SPELL_TWILIGHT_RAGE         = 107872,
};

class npc_dragon_soul_ancient_water_lord : public CreatureScript
{
    public:
        npc_dragon_soul_ancient_water_lord() : CreatureScript("npc_dragon_soul_ancient_water_lord") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_ancient_water_lordAI (pCreature);
        }

        struct npc_dragon_soul_ancient_water_lordAI : public ScriptedAI
        {
            npc_dragon_soul_ancient_water_lordAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
            }

            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {

            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };
};

void AddSC_dragon_soul()
{
    new npc_dragon_soul_ancient_water_lord();
}
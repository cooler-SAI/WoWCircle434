#include "ScriptPCH.h"
#include "deadmines.h"

enum ScriptTexts
{
    SAY_AGGRO   = 7,
    SAY_KILL    = 1,
    SAY_DEATH   = 0,
    SAY_SPELL1  = 2,
    SAY_SPELL2  = 3,
    SAY_SPELL3  = 4,
    SAY_SPELL4  = 5,
    SAY_SPELL5  = 6,
};
enum Spells
{
    SPELL_THRIST_OF_BLOOD           = 88736,
    SPELL_THRIST_OF_BLOOD_AURA      = 88737,
    SPELL_THRIST_OF_BLOOD_AURA_H    = 91862,
    SPELL_SWIPE                     = 88839,
    SPELL_VANISH                    = 88840,
    SPELL_GO_FOR_THE_THROAT         = 88836,
    SPELL_VAPOR                     = 88831,

    SPELL_CONDENSATION_1            = 92013,
    SPELL_CONDENSATION_2            = 92017,
    SPELL_CONDENSATION_3            = 92021,
    SPELL_SWIRLING_VAPOR            = 92007,
    SPELL_CONDENSING_VAPOR          = 92008,
    SPELL_FREEZING_VAPOR            = 92011,
    SPELL_VAPOR_ANIMUS              = 92038,
    SPELL_COALESCE                  = 92042,

    SPELL_FOG_1                     = 88768,
    SPELL_FOG_2                     = 88755,
};

enum Adds
{
    NPC_VAPOR   = 47714, 

    NPC_DUMMY_1 = 47242,
    NPC_DUMMY_2 = 45979,
};

enum Events
{
    EVENT_SWIPE         = 1,
    EVENT_SUMMON_VAPOR  = 2,
    EVENT_APPEAR        = 3,

    EVENT_VAPOR_ATTACK  = 4,
    EVENT_COALESCE      = 5,
};

class boss_admiral_ripsnarl : public CreatureScript
{
    public:
        boss_admiral_ripsnarl() : CreatureScript("boss_admiral_ripsnarl") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_admiral_ripsnarlAI (pCreature);
        }

        struct boss_admiral_ripsnarlAI : public BossAI
        {
            boss_admiral_ripsnarlAI(Creature* pCreature) : BossAI(pCreature, DATA_ADMIRAL)
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            uint8 stage;

            void Reset()
            {
                _Reset();
                stage = 0;
                SetFog(false);
            }

            void SetFog(bool enabled)
            {
                std::list<Creature*> dummies;
                GetCreatureListWithEntryInGrid(dummies, me, NPC_DUMMY_1, 75);
                for (std::list<Creature*>::const_iterator itr = dummies.begin(); itr != dummies.end(); ++itr)
                    if (enabled)
                        (*itr)->CastSpell(*itr, SPELL_FOG_1, true);
                    else
                        (*itr)->RemoveAurasDueToSpell(SPELL_FOG_1);

                std::list<Creature*> bigDummies;
                GetCreatureListWithEntryInGrid(bigDummies, me, NPC_DUMMY_2, 75);
                for (std::list<Creature*>::const_iterator itr = bigDummies.begin(); itr != bigDummies.end(); ++itr)
                    if (enabled)
                        (*itr)->CastSpell(*itr, SPELL_FOG_2, true);
                    else
                        (*itr)->RemoveAurasDueToSpell(SPELL_FOG_2);
            }
            void Vanish()
            {
                me->RemoveAurasDueToSpell(SPELL_THRIST_OF_BLOOD_AURA);
                me->RemoveAurasDueToSpell(SPELL_THRIST_OF_BLOOD_AURA_H);
                DoCast(me, SPELL_VANISH, true);
                events.ScheduleEvent(EVENT_APPEAR, 25000);
            }
            void Appear()
            {
                me->RemoveAurasDueToSpell(SPELL_VANISH);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    DoCast(target, SPELL_GO_FOR_THE_THROAT);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(75) && stage == 0)
                {
                    stage = 1;
                    Talk(SAY_SPELL4);
                    Vanish();
                    events.ScheduleEvent(EVENT_SUMMON_VAPOR, 5000);
                    SetFog(true);
                    return;
                }
                if (me->HealthBelowPct(50) && stage == 1)
                {
                    stage = 2;
                    Talk(SAY_SPELL4);
                    Vanish();
                    return;
                }
                if (me->HealthBelowPct(25) && stage == 2)
                {
                    stage = 3;
                    Talk(SAY_SPELL4);
                    Vanish();
                    return;
                }
                if (me->HealthBelowPct(10) && stage == 3)
                {
                    stage = 4;
                    for (uint32 i = 0; i < 3; ++i)
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(target, SPELL_VAPOR);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWIPE:
                            DoCast(me->getVictim(), SPELL_SWIPE);
                            events.ScheduleEvent(EVENT_SWIPE, urand(8000, 10000));
                            break;
                        case EVENT_SUMMON_VAPOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                                if (TempSummon* summon = me->SummonCreature(47714, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN))
                                    summon->Attack(target, true);
                                //DoCast(target, SPELL_VAPOR, true);
                            events.ScheduleEvent(EVENT_SUMMON_VAPOR, 5000);
                            break;
                        case EVENT_APPEAR:
                            Appear();
                            break;
                    }
                }

                if (!me->HasAura(SPELL_VANISH))
                    DoMeleeAttackIfReady();
            }

            void EnterCombat(Unit* who) 
            {
                DoCast(me, SPELL_THRIST_OF_BLOOD);
                events.ScheduleEvent(EVENT_SWIPE, urand(5000, 10000));
                Talk(SAY_AGGRO);
                DoZoneInCombat();
                SetFog(false);
                instance->SetBossState(DATA_ADMIRAL, IN_PROGRESS);
            }

            void KilledUnit(Unit * victim)
            {
                Talk(SAY_KILL);
            }


            void JustDied(Unit* killer)
            {
                _JustDied();
                Talk(SAY_DEATH);
                SetFog(false);
            }
        };
};

class npc_vapor : public CreatureScript
{
    public:
        npc_vapor() : CreatureScript("npc_vapor") { }
     
        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_vaporAI (pCreature);
        }
     
        struct npc_vaporAI : public ScriptedAI
        {
            npc_vaporAI(Creature *c) : ScriptedAI(c) 
            {
            }

            void Reset()
            {
                condensationApplied = !IsHeroic();
                events.Reset();
            }

            void EnterCombat(Unit* who)
            {
                events.ScheduleEvent(EVENT_VAPOR_ATTACK, 4000);
            }
     
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (!condensationApplied)
                {
                    DoCast(me, SPELL_CONDENSATION_1, true);
                    condensationApplied = true;
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_VAPOR_ATTACK:
                            if (me->HasAura(SPELL_CONDENSATION_1))
                                DoCastVictim(SPELL_SWIRLING_VAPOR);
                            else if (me->HasAura(SPELL_CONDENSATION_2))
                                DoCastVictim(SPELL_CONDENSING_VAPOR);
                            else if (me->HasAura(SPELL_CONDENSATION_3))
                                DoCastVictim(SPELL_FREEZING_VAPOR);
                            events.ScheduleEvent(EVENT_VAPOR_ATTACK, urand(4000, 8000));
                            break;
                        case EVENT_COALESCE:
                            DoCastAOE(SPELL_COALESCE);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_VAPOR_ANIMUS)
                    events.ScheduleEvent(EVENT_COALESCE, urand(2000, 5000));
            }

        protected:
            EventMap events;
            bool condensationApplied;
        };
};



void AddSC_boss_admiral_ripsnarl()
{
    new boss_admiral_ripsnarl();
    new npc_vapor();
}
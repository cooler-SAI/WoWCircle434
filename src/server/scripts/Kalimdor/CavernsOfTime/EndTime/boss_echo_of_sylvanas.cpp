#include "ScriptPCH.h"
#include "end_time.h"

enum Yells
{
};

enum Spells
{
    SPELL_SHRIEK_OF_THE_HIGHBORNE   = 101412,
    SPELL_UNHOLY_SHOT               = 101411,
    SPELL_BLACK_ARROW               = 101404,
    //SPELL_...                       = 100865,
    //                                = 100862,
};

enum Events
{
    EVENT_SHRIEK_OF_THE_HIGHBORNE   = 1,
    EVENT_UNHOLY_SHOT               = 2,
    EVENT_BLACK_ARROW               = 3,
};

enum Adds
{
    NPC_GHOUL_1         = 54197,
    NPC_BRITTLE_GHOUL   = 54952,
};

class boss_echo_of_sylvanas : public CreatureScript
{
    public:
        boss_echo_of_sylvanas() : CreatureScript("boss_echo_of_sylvanas") { }

         CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_echo_of_sylvanasAI(creature);
        }

        struct boss_echo_of_sylvanasAI : public BossAI
        {
            boss_echo_of_sylvanasAI(Creature* creature) : BossAI(creature, DATA_ECHO_OF_SYLVANAS)
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

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(ETScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset()
            {
                _Reset();
            }

            void JustDied(Unit* killer)
            {
                _JustDied();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_UNHOLY_SHOT, urand(5000, 20000));
                events.ScheduleEvent(EVENT_SHRIEK_OF_THE_HIGHBORNE, urand(5000, 20000));
                DoZoneInCombat();
                instance->SetBossState(DATA_ECHO_OF_SYLVANAS, IN_PROGRESS);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNHOLY_SHOT:
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(pTarget, SPELL_UNHOLY_SHOT);
                            events.ScheduleEvent(EVENT_UNHOLY_SHOT, urand(10000, 20000));
                            break;
                        case EVENT_SHRIEK_OF_THE_HIGHBORNE:
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(pTarget, SPELL_SHRIEK_OF_THE_HIGHBORNE);
                            events.ScheduleEvent(EVENT_SHRIEK_OF_THE_HIGHBORNE, urand(15000, 21000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

void AddSC_boss_echo_of_sylvanas()
{
    new boss_echo_of_sylvanas;
}

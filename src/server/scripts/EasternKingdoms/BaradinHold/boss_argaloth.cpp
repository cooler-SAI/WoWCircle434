#include "baradin_hold.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"

enum Spells
{
   SPELL_CONSUMING_DARKNESS      = 88954,
   SPELL_FEL_FIRESTORM           = 88972,
   SPELL_METEOR_SLASH            = 88942,
   SPELL_BERSERK                 = 47008,
   SPELL_FEL_FLAMES_DAMAGE       = 88999
};

enum Events
{
   EVENT_CONSUMING_DARKNESS = 1,
   EVENT_METEOR_SLASH       = 2,
   EVENT_BERSERK            = 3,
};

class boss_argaloth : public CreatureScript
{
    public:
        boss_argaloth() : CreatureScript("boss_argaloth") { }
 
        struct boss_argalothAI : public BossAI
        {
            boss_argalothAI(Creature* creature) : BossAI(creature, DATA_ARGALOTH), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            private:
                InstanceScript* instance;
                EventMap events;
                uint8 phase;
                SummonList summons;

            void Reset()
            {
                _Reset();
                events.ScheduleEvent(EVENT_CONSUMING_DARKNESS, 24 *IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_METEOR_SLASH, 14 *IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BERSERK, 300 *IN_MILLISECONDS);
                summons.DespawnAll();
                phase = 0;
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_FEL_FLAMES:
                        summon->CastSpell(summon, SPELL_FEL_FLAMES_DAMAGE, true);
                        break;
                }
                summons.Summon(summon);
            }
                        
            void JustReachedHome()
            {
                Reset();
            }
 
            void KilledUnit(Unit* victim){}
 
            void MoveInLineOfSight(Unit* who){}
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CONSUMING_DARKNESS:
                            {
                                CustomSpellValues values;
                                values.AddSpellMod(SPELLVALUE_MAX_TARGETS, RAID_MODE(3, 8, 3, 8));
                                me->CastCustomSpell(SPELL_CONSUMING_DARKNESS, values, NULL, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
                                events.RescheduleEvent(EVENT_CONSUMING_DARKNESS, 24 * IN_MILLISECONDS);
                                break;
                            }
                        case EVENT_METEOR_SLASH:
                            DoCast(me->getVictim(), SPELL_METEOR_SLASH);
                            events.RescheduleEvent(EVENT_METEOR_SLASH, 14 * IN_MILLISECONDS);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            events.RescheduleEvent(EVENT_BERSERK, 300 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->GetHealthPct() <= 66.6f && phase == 0)
                {
                    DoCast(me, SPELL_FEL_FIRESTORM);
                    phase = 1;
                }
                else if (me->GetHealthPct() <= 33.3f && phase == 1)
                {
                    DoCast(me, SPELL_FEL_FIRESTORM);
                    phase = 2;
                }
                                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_argalothAI(creature);
        }
};

void AddSC_boss_argaloth()
{
    new boss_argaloth();
}
#include "ScriptPCH.h"
#include "end_time.h"
#include "Group.h"

enum Yells
{
};

enum Spells
{
    SPELL_INFINITE_BREATH   = 102569,
    SPELL_TEMPORAL_BLAST    = 102381,
    SPELL_TAIL_SWEEP        = 108589,
    SPELL_FADING            = 107550,

    SPELL_KILL_MUROZOND     = 110158,
};

enum Events
{
    EVENT_INFINITE_BREATH   = 1,
    EVENT_TEMPORAL_BLAST    = 2,
    EVENT_TAIL_SWEEP        = 3,
    EVENT_DESPAWN           = 4,
};

enum Other
{
    POINT_LAND  = 1,
};

const Position landPos = {4169.71f, -433.40f, 120.0f, 2.59f};

class boss_murozond : public CreatureScript
{
    public:
        boss_murozond() : CreatureScript("boss_murozond") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_murozondAI(creature);
        }

        struct boss_murozondAI : public BossAI
        {
            boss_murozondAI(Creature* creature) : BossAI(creature, DATA_MUROZOND)
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
                bSpawned = false;
                bDead = false;
                //me->SetCanFly(true);
                //me->SetDisableGravity(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
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

            /*void MoveInLineOfSight(Unit* who)
            {
                if (bSpawned)
                    return;

                if (!me->IsWithinDistInMap(who, 200.0f))
                    return;

                me->GetMotionMaster()->MovePoint(POINT_LAND, landPos);

                bSpawned = true;
            }*/

            /*void JustReachedHome()
            {
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE)
                {
                    if (id == POINT_LAND)
                    {
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        me->SetHomePosition(landPos);
                    }
                }
            }*/

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_INFINITE_BREATH, urand(10000, 20000));
                events.ScheduleEvent(EVENT_TAIL_SWEEP, 10000);
                events.ScheduleEvent(EVENT_TEMPORAL_BLAST, 5000);
                instance->SetBossState(DATA_MUROZOND, IN_PROGRESS);
            }

            void SetData(uint32 type, uint32 data)
            {

            }

            void CompleteEncounter()
            {
                if (instance)
                {
                    // Achievement
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_KILL_MUROZOND, 0, 0, me); 
                    
                    // Guild Achievement
                    Map::PlayerList const &PlayerList = instance->instance->GetPlayers();
                    if (!PlayerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (Player* pPlayer = i->getSource())
                                if (Group* pGroup = pPlayer->GetGroup())
                                    if (pPlayer->GetGuildId() && pGroup->IsGuildGroup(pPlayer->GetGuildId(), true, true))
                                    {
                                        pGroup->UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_KILL_MUROZOND, 0, 0, NULL, me);
                                        break;
                                    }
                        }
                    }
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_KILL_MUROZOND, me); 
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (!bDead && me->HealthBelowPct(15))
                {
                    me->RemoveAllAuras();
                    DoCast(me, SPELL_FADING, true);
                    CompleteEncounter();
                    events.ScheduleEvent(EVENT_DESPAWN, 3000);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INFINITE_BREATH:
                            DoCastVictim(SPELL_INFINITE_BREATH);
                            events.ScheduleEvent(EVENT_INFINITE_BREATH, 15000);
                            break;
                        case EVENT_TAIL_SWEEP:
                            break;
                        case EVENT_TEMPORAL_BLAST:
                            DoCastAOE(SPELL_TEMPORAL_BLAST);
                            events.ScheduleEvent(EVENT_TEMPORAL_BLAST, 22000);
                            break;
                        case EVENT_DESPAWN:
                            instance->SetBossState(DATA_MUROZOND, DONE);
                            me->DespawnOrUnsummon(500);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            bool bSpawned;
            bool bDead;
        };   
};

void AddSC_boss_murozond()
{
    new boss_murozond();
}

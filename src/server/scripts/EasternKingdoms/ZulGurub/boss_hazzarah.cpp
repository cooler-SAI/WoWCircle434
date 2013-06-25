#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

enum Yells
{
    SAY_AGGRO               = 0,
    SAY_DEATH               = 1,
    SAY_DREAMSTATE          = 2,
    SAY_KILL                = 3,
    SAY_DREAMSTATE_ANNOUNCE = 4
};

enum Spells
{
    SPELL_EARTH_SHOCK       = 96650,
    SPELL_WRATH             = 96652,
    SPELL_AWAKEN_NIGHTMARES = 96658,
    SPELL_NIGHTMARE         = 96670,
    SPELL_WAKING_NIGHTMARE  = 96757,
    SPELL_CONSUME_SOUL      = 96758
};

enum Events
{
    EVENT_EARTH_SHOCK = 1,
    EVENT_WRATH       = 2
};

enum Points
{
    POINT_NIGHTMARE_TARGET = 522840
};

typedef std::list<WorldObject*> ObjectList;

class npc_nightmare_illusion : public CreatureScript
{
    public:
        npc_nightmare_illusion() : CreatureScript("npc_nightmare_illusion") { }

        struct npc_nightmare_illusionAI : public ScriptedAI
        {
            npc_nightmare_illusionAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                SetCombatMovement(false);
                me->SetSpeed(MOVE_RUN, 0.25f, true);
                //me->SetWalk(true);
                aiSet = false;
            }

            void JustDied(Unit* /*killer*/)
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!nightmareTarget || !nightmareOwner || !nightmareTarget->isAlive() || !nightmareOwner->isAlive())
                {
                    me->DespawnOrUnsummon();
                    return;
                }
                if (aiSet)
                    return;
                aiSet = true;
                me->SetTarget(nightmareTarget->GetGUID());
                DoCast(nightmareTarget, SPELL_WAKING_NIGHTMARE, true);
                Position pos;
                nightmareTarget->GetPosition(&pos);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(POINT_NIGHTMARE_TARGET, pos);
            }

            void SetNightmareTarget(Unit* owner, Unit* target)
            {
                nightmareOwner = owner;
                nightmareTarget = target;
            }

            void MovementInform(uint32 type, uint32 pointId)
            {
                if (pointId == POINT_NIGHTMARE_TARGET)
                {
                    me->CastSpell(nightmareTarget, SPELL_CONSUME_SOUL, true);
                    CAST_AI(BossAI, nightmareOwner->GetAI())->KilledUnit(nightmareTarget);
                    me->DespawnOrUnsummon();
                }
            }

        protected:
            bool aiSet;
            Unit* nightmareOwner;
            Unit* nightmareTarget;

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetZulGurubAI<npc_nightmare_illusionAI>(creature);
        }
};

class boss_hazzarah : public CreatureScript
{
    public:
        boss_hazzarah() : CreatureScript("boss_hazzarah") { }

        struct boss_hazzarahAI : public BossAI
        {
            boss_hazzarahAI(Creature* creature) : BossAI(creature, DATA_CACHE_OF_MADNESS_BOSS)
            {
            }

            void Reset()
            {
                _Reset();
                summonedAt66Pct = false;
                summonedAt33Pct = false;
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_EARTH_SHOCK, 8000);
                events.ScheduleEvent(EVENT_WRATH, 500);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* victim)
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (!summonedAt66Pct && HealthBelowPct(67))
                {
                    summonedAt66Pct = true;
                    NightmarePhase();
                }
                if (!summonedAt33Pct && HealthBelowPct(34))
                {
                    summonedAt33Pct = true;
                    NightmarePhase();
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EARTH_SHOCK:
                            DoCast(me->getVictim(), SPELL_EARTH_SHOCK);
                            events.ScheduleEvent(EVENT_EARTH_SHOCK, 10000);
                            break;
                        case EVENT_WRATH:
                            DoCast(me->getVictim(), SPELL_WRATH);
                            events.ScheduleEvent(EVENT_WRATH, urand(3000, 5000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            ObjectList* GetWorldObjectsInDist(WorldObject* obj, float dist)
            {
                ObjectList* targets = new ObjectList();
                if (obj)
                {
                    Trinity::AllWorldObjectsInRange u_check(obj, dist);
                    Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(obj, *targets, u_check);
                    obj->VisitNearbyObject(dist, searcher);
                }
                return targets;
            }

            void NightmarePhase()
            {
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);

                // Gather all nearby player GUIDs
                uint64 playerGuids[5];
                uint32 playerCount = 0;
                ObjectList* units = GetWorldObjectsInDist(me, 100);
                for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                {
                    if (!(*itr) || (*itr)->GetTypeId() != TYPEID_PLAYER)
                        continue;

                    if (Player* nearby = (*itr)->ToPlayer())
                        if (nearby->isAlive())
                            playerGuids[playerCount++] = nearby->GetGUID();
                }

                // Select random 4 players to be sleeped
                uint32 stunnedPlayerIndexes[4];
                uint32 stunnedPlayerCount = 0;
                for (uint32 i = 0; i < playerCount - 1; ++i)
                {
                    uint32 randomIndex;
                    while (true)
                    {
                        randomIndex = urand(0, playerCount - 1);
                        for (uint32 j = 0; j < stunnedPlayerCount; ++j)
                            if (stunnedPlayerIndexes[j] == randomIndex)
                                continue;
                        break;
                    }
                    stunnedPlayerIndexes[stunnedPlayerCount++] = randomIndex;
                }

                // Spawn nightmare for each player
                for (uint32 i = 0; i < stunnedPlayerCount; ++i)
                {
                    uint64 guid = playerGuids[stunnedPlayerIndexes[i]];
                    playerGuids[stunnedPlayerIndexes[i]] = 0;
                    if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                    {
                        Position pos;
                        player->GetPosition(&pos);
                        player->MovePosition(pos, 30, (float)rand_norm() * static_cast<float>(2 * M_PI));
                        if (TempSummon* summon = me->SummonCreature(52284, pos, TEMPSUMMON_DEAD_DESPAWN))
                            if (npc_nightmare_illusion::npc_nightmare_illusionAI* ai = (npc_nightmare_illusion::npc_nightmare_illusionAI*)summon->GetAI())
                                ai->SetNightmareTarget(me, player);
                    }
                }

                if (stunnedPlayerCount)
                {
                    // Announce to the only player that is not sleeping
                    for (uint32 i = 0; i < playerCount; ++i)
                    {
                        uint64 guid = playerGuids[i];
                        if (guid)
                        {
                            playerGuids[i] = 0;
                            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                                Talk(SAY_DREAMSTATE_ANNOUNCE, player->GetGUID());
                        }
                    }

                    Talk(SAY_DREAMSTATE);
                    DoCastAOE(SPELL_AWAKEN_NIGHTMARES);
                }
            }

        protected:
            bool summonedAt66Pct;
            bool summonedAt33Pct;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_hazzarahAI(creature);
        }
};
void AddSC_boss_hazzarah()
{
    new npc_nightmare_illusion();
    new boss_hazzarah();
}

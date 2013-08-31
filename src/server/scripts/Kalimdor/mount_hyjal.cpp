#include "ScriptPCH.h"

enum Spells
{
    // Garr
    SPELL_ANTIMAGIC_PULSE   = 93506,
    SPELL_MAGMA_SHACKLES    = 93507,
    SPELL_MASSIVE_ERUPTION  = 93508,

    // Firesworn
    SPELL_ERUPTION          = 93512,
    SPELL_IMMOLATE          = 93663,

};

enum Events
{
    // Garr
    EVENT_MAGMA_SHACKLES    = 1,
    EVENT_MASSIVE_ERUPTION  = 2,
    EVENT_ANTIMAGIC_PULSE   = 3,

    // Firesworn
    EVENT_IMMOLATE          = 4,
};

class npc_garr : public CreatureScript
{
    public:
        npc_garr() : CreatureScript("npc_garr") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_garrAI(creature);
        }

        struct npc_garrAI : public ScriptedAI
        {
            npc_garrAI(Creature* creature) : ScriptedAI(creature)
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
            }
            
            EventMap events;

            void Reset()
            {
                me->setActive(true);
                events.Reset();
            }

            void EnterCombat(Unit* attacker)
            {
                events.ScheduleEvent(EVENT_MAGMA_SHACKLES, 30000);
                events.ScheduleEvent(EVENT_MASSIVE_ERUPTION, 32000);
                events.ScheduleEvent(EVENT_ANTIMAGIC_PULSE, urand(12000, 16000));
            }

            void UpdateAI(const uint32 diff)
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
                        case EVENT_MAGMA_SHACKLES:
                            DoCastAOE(SPELL_MAGMA_SHACKLES);
                            events.ScheduleEvent(EVENT_MAGMA_SHACKLES, 30000);
                            break;
                        case EVENT_MASSIVE_ERUPTION:
                            DoCastAOE(SPELL_MASSIVE_ERUPTION);
                            events.ScheduleEvent(EVENT_MASSIVE_ERUPTION, 30000);
                            break;
                        case EVENT_ANTIMAGIC_PULSE:
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(pTarget, SPELL_ANTIMAGIC_PULSE);
                            events.ScheduleEvent(EVENT_ANTIMAGIC_PULSE, urand(12000, 16000));
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };
};

class npc_garr_firesworn : public CreatureScript
{
    public:
        npc_garr_firesworn() : CreatureScript("npc_garr_firesworn") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_garr_fireswornAI(creature);
        }

        struct npc_garr_fireswornAI : public ScriptedAI
        {
            npc_garr_fireswornAI(Creature* creature) : ScriptedAI(creature)
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
            }
            
            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* attacker)
            {
                events.ScheduleEvent(EVENT_IMMOLATE, urand(5000, 7000));
            }

            void JustDied(Unit* killer)
            {
                DoCastAOE(SPELL_ERUPTION);
            }

            void UpdateAI(const uint32 diff)
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
                        case EVENT_IMMOLATE:
                            DoCast(me->getVictim(), SPELL_IMMOLATE);
                            events.ScheduleEvent(EVENT_IMMOLATE, urand(10000, 14000));
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }   
        };
}; 
/* class npc_lycanthoth : public CreatureScript
{
    public:
        npc_lycanthoth() : CreatureScript("npc_lycanthoth") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_lycanthothAI(creature);
        }

        struct npc_lycanthothAI : public ScriptedAI
        {
            npc_lycanthothAI(Creature* creature) : ScriptedAI(creature)
            {
            }
            

            void JustDied(Unit* killer)
            {
                Unit* originalKiller = killer->GetCharmerOrOwnerOrSelf();
                if (originalKiller && (originalKiller->GetTypeId() == TYPEID_PLAYER))
                    killer->CastSpell(killer, (originalKiller->ToPlayer()->GetTeamId() == TEAM_HORDE ? 74077 : 74078), true);
            } 
        };
}; */

class npc_marion_wormswing : public CreatureScript
{
    public:
        npc_marion_wormswing() : CreatureScript("npc_marion_wormswing") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_marion_wormswingAI(pCreature);
        }

        struct npc_marion_wormswingAI : public ScriptedAI
        {
            npc_marion_wormswingAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
            }

            void DamageTaken(Unit* attacker, uint32 &damage)
            {
                if (damage > me->GetHealth() || me->HealthBelowPct(50) || me->HealthBelowPctDamaged(50, damage))
                {
                    if (Player* owner = attacker->GetCharmerOrOwnerPlayerOrPlayerItself())
                        if (owner->GetQuestStatus(25731) == QUEST_STATUS_INCOMPLETE)
                        {
                            owner->KilledMonsterCredit(41169, 0);
                            owner->KilledMonsterCredit(41170, 0);
                        }
                    me->setFaction(35);
                    me->DespawnOrUnsummon(5000);
                }
            }
        };
};

/* class go_harpy_signal_fire : public GameObjectScript
{
    public:
        go_harpy_signal_fire() : GameObjectScript("go_harpy_signal_fire") {}

        bool OnGossipHello(Player* pPlayer, GameObject* pGo)
        {
            if (!pGo->FindNearestCreature(41112, 100.0f) && pPlayer->GetQuestStatus(25731) == QUEST_STATUS_INCOMPLETE)
                pGo->SummonCreature(41112, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
            return false;
        }
}; */

class at_king_of_the_spider_hill : public AreaTriggerScript
{
public:
    at_king_of_the_spider_hill() : AreaTriggerScript("at_king_of_the_spider_hill") { }
 
    bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
    {
        if (player)
            player->CompletedAchievement(sAchievementMgr->GetAchievement(5872));
        return true;
    }
};

void AddSC_mount_hyjal()
{
    new npc_garr();
    new npc_garr_firesworn();
    // new npc_lycanthoth();
    new npc_marion_wormswing();
    // new go_harpy_signal_fire();
    new at_king_of_the_spider_hill();
}
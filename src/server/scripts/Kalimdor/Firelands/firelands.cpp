#include "ScriptPCH.h"
#include "MoveSplineInit.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "firelands.h"

enum Spells
{
    // Ancient Core Hound
    SPELL_DINNER_TIME                           = 99693,
    SPELL_DINNER_TIME_VEHICLE                   = 99694,
    SPELL_FLAME_BREATH                          = 99736,
    SPELL_TERRIFYING_ROAR                       = 99692,

    // Ancient Lava Dweller
    SPELL_LAVA_SHOWER                           = 97549,
    SPELL_LAVA_SHOWER_MISSILE                   = 97551,
    SPELL_LAVA_SPIT                             = 97306,

    // Fire Scorpion
    SPELL_FIERY_BLOOD                           = 99993,
    SPELL_SLIGHTLY_WARM_PINCERS                 = 99984,

    // Fire Turtle Hatchling 
    SPELL_SHELL_SPIN                            = 100263,

    // Fire Archon
    SPELL_FLAME_TORRENT                         = 100795,
    SPELL_FIERY_TORMENT                         = 100797,
    SPELL_FIERY_TORMENT_DMG                     = 100802,

    // Molten Lord
    SPELL_FLAME_STOMP                           = 99530,
    SPELL_MELT_ARMOR                            = 99532,
    SPELL_SUMMON_LAVA_JETS                      = 99555,
    SPELL_SUMMON_LAVA_JET                       = 99538,

    // Molten Flamefather
    SPELL_EARTHQUAKE                            = 100724,
    SPELL_MAGMA_CONDUIT                         = 100728,

    // Magma Conduit
    SPELL_VOLCANO_SMOKE                         = 97699,
    SPELL_VOLCANO_BASE                          = 98250,
    SPELL_SUMMON_MAGMAKIN                       = 100746,
    SPELL_SUMMON_MAGMAKIN_DMG                   = 100748,

    // Magmakin
    SPELL_ERUPTION                              = 100755,

    // Volcanus
    SPELL_FLAMEWAKE                             = 100191,

    // Harbinger of Flame
    SPELL_FIRE_IT_UP                            = 100093,
};

enum Adds
{    
    NPC_MAGMAKIN        = 54144,
    NPC_MAGMA_CONDUIT   = 54145, // 97699, 98250, 100746

    NPC_STALKER         = 45979,

    NPC_LAVA            = 53585,
    NPC_MOLTEN_ERUPTER  = 53617,
    NPC_MOLTEN_ERUPTION = 53621,
    NPC_MOLTEN_SPEWER   = 53445,

};

enum Events
{
    // Ancient Core Hound
    EVENT_DINNER_TIME                   = 1,
    EVENT_FLAME_BREATH                  = 2,
    EVENT_TERRIFYING_ROAR               = 3,

    // Ancient Lava Dweller
    EVENT_LAVA_SHOWER                   = 4,

    // Fire Turtle Hatchling
    EVENT_SHELL_SPIN                    = 5,

    // Fire Archon
    EVENT_FIERY_TORMENT                 = 6,
    EVENT_FLAME_TORRENT                 = 7,

    // Molten Lord
    EVENT_MELT_ARMOR                    = 8,
    EVENT_FLAME_STOMP                   = 9,
    EVENT_SUMMON_LAVA_JETS              = 10,

    // Molten Flamefather
    EVENT_EARTHQUAKE                    = 11,
    EVENT_MAGMA_CONDUIT                 = 12,

    // Volcanus
    EVENT_FLAMEWAKE                     = 13,
};

class npc_firelands_ancient_core_hound : public CreatureScript
{
public:
    npc_firelands_ancient_core_hound() : CreatureScript("npc_firelands_ancient_core_hound") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_ancient_core_houndAI (pCreature);
    }

    struct npc_firelands_ancient_core_houndAI : public ScriptedAI
    {
        npc_firelands_ancient_core_houndAI(Creature* pCreature) : ScriptedAI(pCreature)
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
            events.ScheduleEvent(EVENT_DINNER_TIME, urand (15000, 20000));
            events.ScheduleEvent(EVENT_TERRIFYING_ROAR, urand(8000, 20000));
            events.ScheduleEvent(EVENT_FLAME_BREATH, urand(5000, 10000));
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_DINNER_TIME:
                    DoCastVictim(SPELL_DINNER_TIME);
                    events.ScheduleEvent(EVENT_DINNER_TIME, urand(30000, 40000));
                    break;
                case EVENT_FLAME_BREATH:
                    DoCastVictim(SPELL_FLAME_BREATH);
                    events.ScheduleEvent(EVENT_FLAME_BREATH, urand(15000, 20000));
                    break;
                case EVENT_TERRIFYING_ROAR:
                    DoCast(me, SPELL_TERRIFYING_ROAR);
                    events.ScheduleEvent(EVENT_TERRIFYING_ROAR, urand(30000, 35000));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_firelands_ancient_lava_dweller : public CreatureScript
{
public:
    npc_firelands_ancient_lava_dweller() : CreatureScript("npc_firelands_ancient_lava_dweller") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_ancient_lava_dwellerAI (pCreature);
    }

    struct npc_firelands_ancient_lava_dwellerAI : public Scripted_NoMovementAI
    {
        npc_firelands_ancient_lava_dwellerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_LAVA_SHOWER, urand(15000, 20000));
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_LAVA_SHOWER:
                    DoCast(me, SPELL_LAVA_SHOWER);
                    events.ScheduleEvent(EVENT_LAVA_SHOWER, urand(45000, 55000));
                    break;
                }
            }

            DoSpellAttackIfReady(SPELL_LAVA_SPIT);
        }
    };
};

class npc_firelands_fire_scorpion : public CreatureScript
{
public:
    npc_firelands_fire_scorpion() : CreatureScript("npc_firelands_fire_scorpion") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_fire_scorpionAI (pCreature);
    }

    struct npc_firelands_fire_scorpionAI : public ScriptedAI
    {
        npc_firelands_fire_scorpionAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
        }

        void JustDied(Unit* killer)
        {
            DoCast(me, SPELL_FIERY_BLOOD);
        }
    };
};

class npc_firelands_fire_turtle_hatchling : public CreatureScript
{
public:
    npc_firelands_fire_turtle_hatchling() : CreatureScript("npc_firelands_fire_turtle_hatchling") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_fire_turtle_hatchlingAI (pCreature);
    }

    struct npc_firelands_fire_turtle_hatchlingAI : public ScriptedAI
    {
        npc_firelands_fire_turtle_hatchlingAI(Creature* pCreature) : ScriptedAI(pCreature)
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
            events.ScheduleEvent(EVENT_SHELL_SPIN, urand(10000, 20000));
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_SHELL_SPIN:
                    DoCast(me, SPELL_SHELL_SPIN);
                    events.ScheduleEvent(EVENT_SHELL_SPIN, urand(35000, 50000));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_firelands_flame_archon : public CreatureScript
{
public:
    npc_firelands_flame_archon() : CreatureScript("npc_firelands_flame_archon") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_flame_archonAI (pCreature);
    }

    struct npc_firelands_flame_archonAI : public ScriptedAI
    {
        npc_firelands_flame_archonAI(Creature* pCreature) : ScriptedAI(pCreature)
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
            events.ScheduleEvent(EVENT_FLAME_TORRENT, 10000);
            events.ScheduleEvent(EVENT_FIERY_TORMENT, 20000);
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_FLAME_TORRENT:
                    DoCast(me, SPELL_FLAME_TORRENT);
                    events.ScheduleEvent(EVENT_FLAME_TORRENT, 40000);
                    break;
                case EVENT_FIERY_TORMENT:
                    DoCast(me, SPELL_FIERY_TORMENT);
                    events.ScheduleEvent(EVENT_FIERY_TORMENT, 40000);
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };
};

class npc_firelands_molten_lord : public CreatureScript
{
public:
    npc_firelands_molten_lord() : CreatureScript("npc_firelands_molten_lord") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_molten_lordAI (pCreature);
    }

    struct npc_firelands_molten_lordAI : public ScriptedAI
    {
        npc_firelands_molten_lordAI(Creature* pCreature) : ScriptedAI(pCreature)
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
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void JustDied(Unit* killer)
        {
            if (pInstance)
            {
                Map::PlayerList const& PlayerList = pInstance->instance->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                    {
                        if (Player* pPlayer = itr->getSource())
                        {
                            if (!pPlayer->IsAtGroupRewardDistance(me))
                                continue;

                            // Only mages, warlocks priests, shamans and druids
                            // in caster specialization can accept quest
                            if (!(pPlayer->getClass() == CLASS_MAGE ||
                                pPlayer->getClass() == CLASS_WARLOCK ||
                                pPlayer->getClass() == CLASS_PRIEST ||
                                (pPlayer->getClass() == CLASS_SHAMAN && (pPlayer->GetPrimaryTalentTree(pPlayer->GetActiveSpec()) == TALENT_TREE_SHAMAN_ELEMENTAL || pPlayer->GetPrimaryTalentTree(pPlayer->GetActiveSpec()) == TALENT_TREE_SHAMAN_RESTORATION)) ||
                                (pPlayer->getClass() == CLASS_DRUID && (pPlayer->GetPrimaryTalentTree(pPlayer->GetActiveSpec()) == TALENT_TREE_DRUID_BALANCE || pPlayer->GetPrimaryTalentTree(pPlayer->GetActiveSpec()) == TALENT_TREE_DRUID_RESTORATION))))
                                continue;

                            if (pPlayer->GetTeam() == ALLIANCE && pPlayer->GetQuestStatus(QUEST_YOU_TIME_HAS_COME_1) == QUEST_STATUS_NONE)
                                pPlayer->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_YOU_TIME_HAS_COME_1), NULL);
                            else if (pPlayer->GetTeam() == HORDE && pPlayer->GetQuestStatus(QUEST_YOU_TIME_HAS_COME_2) == QUEST_STATUS_NONE)
                                pPlayer->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_YOU_TIME_HAS_COME_2), NULL);
                        }
                    }
                }
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_FLAME_STOMP, 5000);
            events.ScheduleEvent(EVENT_MELT_ARMOR, urand(3000, 7000));
            events.ScheduleEvent(EVENT_SUMMON_LAVA_JETS, 10000);
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_FLAME_STOMP:
                    DoCast(me, SPELL_FLAME_STOMP);
                    events.ScheduleEvent(EVENT_FLAME_STOMP, urand(10000, 18000));
                    break;
                case EVENT_MELT_ARMOR:
                    DoCastVictim(SPELL_MELT_ARMOR);
                    events.ScheduleEvent(EVENT_MELT_ARMOR, urand(7000, 14000));
                    break;
                case EVENT_SUMMON_LAVA_JETS:
                    DoCast(me, SPELL_SUMMON_LAVA_JETS);
                    events.ScheduleEvent(EVENT_SUMMON_LAVA_JETS, urand(20000, 25000));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_firelands_molten_flamefather : public CreatureScript
{
public:
    npc_firelands_molten_flamefather() : CreatureScript("npc_firelands_molten_flamefather") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_molten_flamefatherAI (pCreature);
    }

    struct npc_firelands_molten_flamefatherAI : public ScriptedAI
    {
        npc_firelands_molten_flamefatherAI(Creature* pCreature) : ScriptedAI(pCreature), summons(me)
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
        SummonList summons;

        void Reset()
        {
            events.Reset();
            summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_EARTHQUAKE, urand(5000, 10000));
            events.ScheduleEvent(EVENT_MAGMA_CONDUIT, urand(6000, 7000));
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            if (me->isInCombat())
                DoZoneInCombat(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            summons.Despawn(summon);
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_EARTHQUAKE:
                    DoCastAOE(SPELL_EARTHQUAKE);
                    events.ScheduleEvent(EVENT_EARTHQUAKE, urand(10000, 15000));
                    break;
                case EVENT_MAGMA_CONDUIT:
                    DoCastAOE(SPELL_MAGMA_CONDUIT);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_firelands_magma_conduit : public CreatureScript
{
public:
    npc_firelands_magma_conduit() : CreatureScript("npc_firelands_magma_conduit") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_magma_conduitAI (pCreature);
    }

    struct npc_firelands_magma_conduitAI : public Scripted_NoMovementAI
    {
        npc_firelands_magma_conduitAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature), summons(me)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        SummonList summons;

        void Reset()
        {
            summons.DespawnAll();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            if (me->isInCombat())
                DoZoneInCombat(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            summons.Despawn(summon);
        }

        void JustDied(Unit* killer)
        {
            me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
            {
                me->DespawnOrUnsummon();
                return;
            }
        }
    };
};

class npc_firelands_magmakin : public CreatureScript
{
public:
    npc_firelands_magmakin() : CreatureScript("npc_firelands_magmakin") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_firelands_magmakinAI (pCreature);
    }

    struct npc_firelands_magmakinAI : public ScriptedAI
    {
        npc_firelands_magmakinAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetSpeed(MOVE_RUN, 2.0f);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (me->GetDistance(me->getVictim()) < 2.0f)
                DoCastAOE(SPELL_ERUPTION, true);                
        }
    };
};

class spell_firelands_ancient_core_hound_dinner_time : public SpellScriptLoader
{
public:
    spell_firelands_ancient_core_hound_dinner_time() :  SpellScriptLoader("spell_firelands_ancient_core_hound_dinner_time") { }

    class spell_firelands_ancient_core_hound_dinner_time_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_firelands_ancient_core_hound_dinner_time_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(true, UNIT_STATE_STUNNED);
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(false, UNIT_STATE_STUNNED);                
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_firelands_ancient_core_hound_dinner_time_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_firelands_ancient_core_hound_dinner_time_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_firelands_ancient_core_hound_dinner_time_AuraScript();
    }
};

class spell_firelands_ancient_core_hound_flame_breath : public SpellScriptLoader
{
public:
    spell_firelands_ancient_core_hound_flame_breath() :  SpellScriptLoader("spell_firelands_ancient_core_hound_flame_breath") { }

    class spell_firelands_ancient_core_hound_flame_breath_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_firelands_ancient_core_hound_flame_breath_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(true, UNIT_STATE_STUNNED);
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(false, UNIT_STATE_STUNNED);                
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_firelands_ancient_core_hound_flame_breath_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_firelands_ancient_core_hound_flame_breath_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_firelands_ancient_core_hound_flame_breath_AuraScript();
    }
};

class spell_firelands_ancient_lava_dweller_lava_shower : public SpellScriptLoader
{
public:
    spell_firelands_ancient_lava_dweller_lava_shower() : SpellScriptLoader("spell_firelands_ancient_lava_dweller_lava_shower") { }

    class spell_firelands_ancient_lava_dweller_lava_shower_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_firelands_ancient_lava_dweller_lava_shower_AuraScript);

        void PeriodicTick(AuraEffect const* aurEff)
        {
            if (!GetCaster())
                return;

            if (GetCaster()->GetAI())
                if (Unit* pTarget = GetCaster()->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    GetCaster()->CastSpell(pTarget, SPELL_LAVA_SHOWER_MISSILE, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_firelands_ancient_lava_dweller_lava_shower_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_firelands_ancient_lava_dweller_lava_shower_AuraScript();
    }
};

class spell_firelands_fire_turtle_hatchling_shell_spin : public SpellScriptLoader
{
public:
    spell_firelands_fire_turtle_hatchling_shell_spin() :  SpellScriptLoader("spell_firelands_fire_turtle_hatchling_shell_spin") { }

    class spell_firelands_fire_turtle_hatchling_shell_spin_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_firelands_fire_turtle_hatchling_shell_spin_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(true, UNIT_STATE_ROOT);
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(false, UNIT_STATE_ROOT);                
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_firelands_fire_turtle_hatchling_shell_spin_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_firelands_fire_turtle_hatchling_shell_spin_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_firelands_fire_turtle_hatchling_shell_spin_AuraScript();
    }
};

class spell_firelands_flame_archon_fiery_torment : public SpellScriptLoader
{
public:
    spell_firelands_flame_archon_fiery_torment() :  SpellScriptLoader("spell_firelands_flame_archon_fiery_torment") { }

    class spell_firelands_flame_archon_fiery_torment_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_firelands_flame_archon_fiery_torment_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(true, UNIT_STATE_STUNNED);
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget())
                return;

            GetTarget()->SetControlled(false, UNIT_STATE_STUNNED);                
        }

        void PeriodicTick(AuraEffect const* aurEff)
        {
            if (!GetCaster())
                return;

            if (GetCaster()->GetAI())
                if (Unit* pTarget = GetCaster()->GetAI()->SelectTarget(SELECT_TARGET_NEAREST, 0, 100.0f, true))
                    GetCaster()->CastSpell(pTarget, SPELL_FIERY_TORMENT_DMG, true);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_firelands_flame_archon_fiery_torment_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_firelands_flame_archon_fiery_torment_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_firelands_flame_archon_fiery_torment_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_firelands_flame_archon_fiery_torment_AuraScript();
    }
};

class spell_firelands_molten_lord_summon_lava_jets : public SpellScriptLoader
{
public:
    spell_firelands_molten_lord_summon_lava_jets() : SpellScriptLoader("spell_firelands_molten_lord_summon_lava_jets") { }

    class spell_firelands_molten_lord_summon_lava_jets_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_firelands_molten_lord_summon_lava_jets_SpellScript);


        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            GetCaster()->CastSpell(GetHitUnit(), SPELL_SUMMON_LAVA_JET, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_firelands_molten_lord_summon_lava_jets_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_firelands_molten_lord_summon_lava_jets_SpellScript();
    }
};

class npc_firelands_dull_focus : public CreatureScript
{
public:
    npc_firelands_dull_focus() : CreatureScript("npc_firelands_dull_focus") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript* pInstance = pCreature->GetInstanceScript();
        if (!pInstance)
            return true;

        if (!pPlayer)
            return true;

        if (pPlayer->GetQuestStatus(QUEST_DELEGATION) != QUEST_STATUS_INCOMPLETE)
            return true;

        uint32 spellId = 0;

        switch (pCreature->GetEntry())
        {
        case NPC_DULL_RHYOLITH_FOCUS:   spellId = SPELL_CHARGED_RHYOLITH_FOCUS;     break;
        case NPC_DULL_EMBERSTONE_FOCUS: spellId = SPELL_CHARGED_EMBERSTONE_FOCUS;   break;
        case NPC_DULL_CHITINOUS_FOCUS:  spellId = SPELL_CHARGED_CHITINOUS_FOCUS;    break;
        case NPC_DULL_PYRESHELL_FOCUS:  spellId = SPELL_CHARGED_PYRESHELL_FOCUS;    break;
        default: break;
        }

        if (spellId)
        {
            pPlayer->CastSpell(pPlayer, spellId, true);
            pCreature->DespawnOrUnsummon();
        }
        return true;
    }
};

class npc_firelands_circle_of_thorns_portal : public CreatureScript
{
public:
    npc_firelands_circle_of_thorns_portal() : CreatureScript("npc_firelands_circle_of_thorns_portal") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript* pInstance = pCreature->GetInstanceScript();
        if (!pInstance)
            return true;

        if (!pPlayer)
            return true;

        if (pInstance->GetData(DATA_EVENT) != DONE)
            return true;

        bool bIn = (pCreature->GetPositionZ() <= 100.0f);

        if (bIn)
            pPlayer->NearTeleportTo(504.0634f, 476.2563f,  246.7454f,  2.30f, false);
        else
            pPlayer->NearTeleportTo(173.1530f, 283.1553f,  84.60362f,  3.69f, false); 

        return true;
    }
};

class npc_firelands_volcanus : public CreatureScript
{
public:
    npc_firelands_volcanus() : CreatureScript("npc_firelands_volcanus") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_firelands_volcanusAI(creature);
    }

    struct npc_firelands_volcanusAI : public CreatureAI
    {
        npc_firelands_volcanusAI(Creature* creature) : CreatureAI(creature)
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

        void Reset()
        {
            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 7);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 7);
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_FLAMEWAKE, 3000);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* pStalker = me->SummonCreature(NPC_STALKER, me->GetHomePosition(), TEMPSUMMON_TIMED_DESPAWN, 10000))
            {
                pStalker->RemoveAllAuras();
                pStalker->CastSpell(pStalker, SPELL_BRANCH_OF_NORDRASSIL_WIN_COSMETIC);
            }
            me->DespawnOrUnsummon(500);
        }

        void UpdateAI(uint32 const diff)
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
                case EVENT_FLAMEWAKE:
                    DoCastAOE(SPELL_FLAMEWAKE);
                    DoCast(me, SPELL_FIRE_IT_UP, true);
                    events.ScheduleEvent(EVENT_FLAMEWAKE, 10000);
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
};

class spell_firelands_siphon_essence : public SpellScriptLoader
{
public:
    spell_firelands_siphon_essence() : SpellScriptLoader("spell_firelands_siphon_essence") { }

    class spell_firelands_siphon_essence_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_firelands_siphon_essence_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            if (!GetCaster())
                return;

            GetCaster()->CastSpell(GetCaster(), SPELL_SIPHON_ESSENCE_CREDIT, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_firelands_siphon_essence_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_firelands_siphon_essence_SpellScript();
    }
};

class npc_firelands_instance_portal : public CreatureScript
{
public:
    npc_firelands_instance_portal() : CreatureScript("npc_firelands_instance_portal") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_firelands_instance_portalAI(creature);
    }

    struct npc_firelands_instance_portalAI : public CreatureAI
    {
        npc_firelands_instance_portalAI(Creature* creature) : CreatureAI(creature) { }

        void OnSpellClick(Unit* clicker)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
                if (instance->GetBossState(DATA_BALEROC) != DONE)
                    return;

            if (me->GetEntry() == NPC_CIRCLE_OF_THRONES_PORTAL_2)
                clicker->NearTeleportTo(362.498f, -97.7795f, 78.3288f, 3.64774f, false);
            else if (me->GetEntry() == NPC_CIRCLE_OF_THRONES_PORTAL_3)
                clicker->NearTeleportTo(-359.944f, 206.012f, 52.32f, 3.64774f, false);
        }

        void UpdateAI(uint32 const diff) { }
    };
};

void AddSC_firelands()
{
    new npc_firelands_ancient_core_hound();
    new npc_firelands_ancient_lava_dweller();
    new npc_firelands_fire_scorpion();
    new npc_firelands_fire_turtle_hatchling();
    new npc_firelands_flame_archon();
    new npc_firelands_molten_lord();
    new npc_firelands_molten_flamefather();
    new npc_firelands_magma_conduit();
    new npc_firelands_magmakin();

    new spell_firelands_ancient_core_hound_dinner_time();
    new spell_firelands_ancient_core_hound_flame_breath();
    new spell_firelands_ancient_lava_dweller_lava_shower();
    new spell_firelands_fire_turtle_hatchling_shell_spin();
    new spell_firelands_flame_archon_fiery_torment();
    new spell_firelands_molten_lord_summon_lava_jets();

    new npc_firelands_dull_focus();
    new npc_firelands_circle_of_thorns_portal();
    new npc_firelands_volcanus();
    new spell_firelands_siphon_essence();
    new npc_firelands_instance_portal();
}
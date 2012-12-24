#include "ScriptPCH.h"
#include "firelands.h"

enum Adds
{    
    NPC_MAGMAKIN        = 54144,
    NPC_MAGMA_CONDUIT   = 54145, // 97699, 98250, 100746
};

enum Spells
{
    // Ancient Core Hound
    SPELL_DINNER_TIME           = 99693,
    SPELL_DINNER_TIME_VEHICLE   = 99694,
    SPELL_FLAME_BREATH          = 99736,
    SPELL_TERRIFYING_ROAR       = 99692,

    // Ancient Lava Dweller
    SPELL_LAVA_SHOWER           = 97549,
    SPELL_LAVA_SHOWER_MISSILE   = 97551,
    SPELL_LAVA_SPIT             = 97306,

    // Fire Scorpion
    SPELL_FIERY_BLOOD           = 99993,
    SPELL_SLIGHTLY_WARM_PINCERS = 99984,

    // Fire Turtle Hatchling 
    SPELL_SHELL_SPIN            = 100263,

    // Fire Archon
    SPELL_FLAME_TORRENT         = 100795,
    SPELL_FIERY_TORMENT         = 100797,
    SPELL_FIERY_TORMENT_DMG     = 100802,
};

enum Events
{
    // Ancient Core Hound
    EVENT_DINNER_TIME       = 1,
    EVENT_FLAME_BREATH      = 2,
    EVENT_TERRIFYING_ROAR   = 3,

    // Ancient Lava Dweller
    EVENT_LAVA_SHOWER       = 4,

    // Fire Turtle Hatchling
    EVENT_SHELL_SPIN        = 5,

    // Fire Archon
    EVENT_FIERY_TORMENT     = 6,
    EVENT_FLAME_TORRENT     = 7,
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
            npc_firelands_ancient_lava_dwellerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
            }

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

void AddSC_firelands()
{
    new npc_firelands_ancient_core_hound();
    new npc_firelands_ancient_lava_dweller();
    new npc_firelands_fire_scorpion();
    new npc_firelands_fire_turtle_hatchling();
    new npc_firelands_flame_archon();
    new spell_firelands_ancient_core_hound_dinner_time();
    new spell_firelands_ancient_core_hound_flame_breath();
    new spell_firelands_ancient_lava_dweller_lava_shower();
    new spell_firelands_fire_turtle_hatchling_shell_spin();
    new spell_firelands_flame_archon_fiery_torment();
}
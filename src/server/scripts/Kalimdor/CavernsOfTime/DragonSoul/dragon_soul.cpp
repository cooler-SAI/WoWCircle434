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

    NPC_CRIMSON_GLOBULE             = 57386,
    NPC_ACIDIC_GLOBULE              = 57333,
    NPC_GLOWING_GLOBULE             = 57387,
    NPC_DARK_GLOBULE                = 57382,
    NPC_SHADOWED_GLOBULE            = 57388,
    NPC_COBALT_GLOBULE              = 57384,

    // deathwing 56173 (boss)
    // deathwing 57962 (near boss)
    // deathwing 53879 (spine)
    // deathwing 55971 (near tower)
};

enum Spells
{
    // Ancient Water Lord
    SPELL_DRENCHED                  = 107801,
    SPELL_FLOOD_AOE                 = 107796,
    SPELL_FLOOD                     = 107797,
    SPELL_FLOOD_CHANNEL             = 107791,

    // Earthen Destroyer
    SPELL_BOULDER_SMASH_AOE         = 107596,
    SPELL_BOULDER_SMASH             = 107597,
    SPELL_DUST_STORM                = 107682,

    // Earthen Soldier
    SPELL_SHADOW_BOLT               = 95440,
    SPELL_TWILIGHT_CORRUPTION       = 107852,
    SPELL_TWILIGHT_RAGE             = 107872,

    // Crimson Globule
    SPELL_CRIMSON_BLOOD_OF_SHUMA    = 110750,
    SPELL_SEARING_BLOOD             = 108218,

    // Acidic Globule
    SPELL_ACIDIC_BLOOD_OF_SHUMA     = 110743,
    SPELL_DIGESTIVE_ACID            = 108419,

    // Glowing Globule
    SPELL_GLOWING_BLOOD_OF_SHUMA_1  = 110753, // dummy
    SPELL_GLOWING_BLOOD_OF_SHUMA_2  = 108221, // haste

    // Dark Globule
    SPELL_BLACK_BLOOD_OF_SHUMA      = 110746,
    SPELL_PSYCHIC_SLICE             = 105671,

    // Shadowed Globule
    SPELL_SHADOWED_BLOOD_OF_SHUMA   = 110748,
    SPELL_DEEP_CORRUPTION           = 108220,
    SPELL_DEEP_CORRUPTION_AURA      = 109389,
    SPELL_DEEP_CORRUPTION_DMG       = 109390,

    // Cobalt Globule
    SPELL_COBALT_BLOOD_OF_SHUMA     = 110747,
    SPELL_MANA_VOID                 = 108223,
    SPELL_MANA_VOID_BURN            = 108222,
    SPELL_MANA_VOID_DUMMY           = 108224, // calculate power
    SPELL_MANA_DIFFUSION            = 108228,
};

enum Events
{
    // Crimson Globule
    EVENT_SEARING_BLOOD     = 1,

    // Acidic Globule
    EVENT_DIGESTIVE_ACID    = 2,

    // Dark Globule
    EVENT_PSYCHIC_SLICE     = 3,

    // Shadowed Globule
    EVENT_DEEP_CORRUPTION   = 4,

    // Cobalt Globule
    EVENT_MANA_VOID         = 5,

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

class npc_dragon_soul_crimson_globule : public CreatureScript
{
    public:
        npc_dragon_soul_crimson_globule() : CreatureScript("npc_dragon_soul_crimson_globule") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_crimson_globuleAI (pCreature);
        }

        struct npc_dragon_soul_crimson_globuleAI : public ScriptedAI
        {
            npc_dragon_soul_crimson_globuleAI(Creature* pCreature) : ScriptedAI(pCreature)
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

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_SEARING_BLOOD, urand(7000, 14000));
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
                    me->CastCustomSpell(SPELL_SEARING_BLOOD, SPELLVALUE_MAX_TARGETS, RAID_MODE(3, 8), me); 
                    events.ScheduleEvent(EVENT_SEARING_BLOOD, (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA_2) ? 7000 : 14000));
                }
                
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
        };
};

class npc_dragon_soul_acidic_globule : public CreatureScript
{
    public:
        npc_dragon_soul_acidic_globule() : CreatureScript("npc_dragon_soul_acidic_globule") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_acidic_globuleAI (pCreature);
        }

        struct npc_dragon_soul_acidic_globuleAI : public ScriptedAI
        {
            npc_dragon_soul_acidic_globuleAI(Creature* pCreature) : ScriptedAI(pCreature)
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

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_DIGESTIVE_ACID, urand(7000, 14000));
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
                    DoCastAOE(SPELL_DIGESTIVE_ACID);
                    events.ScheduleEvent(EVENT_DIGESTIVE_ACID, (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA_2) ? 7000 : 14000));
                }
                
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
        };
};

class npc_dragon_soul_dark_globule : public CreatureScript
{
    public:
        npc_dragon_soul_dark_globule() : CreatureScript("npc_dragon_soul_dark_globule") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_dark_globuleAI (pCreature);
        }

        struct npc_dragon_soul_dark_globuleAI : public ScriptedAI
        {
            npc_dragon_soul_dark_globuleAI(Creature* pCreature) : ScriptedAI(pCreature)
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

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_PSYCHIC_SLICE, urand(7000, 14000));
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
                    DoCastVictim(SPELL_PSYCHIC_SLICE);
                    events.ScheduleEvent(EVENT_PSYCHIC_SLICE, (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA_2) ? 7000 : 14000));
                }
                
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
        };
};

class npc_dragon_soul_shadowed_globule : public CreatureScript
{
    public:
        npc_dragon_soul_shadowed_globule() : CreatureScript("npc_dragon_soul_shadowed_globule") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_shadowed_globuleAI (pCreature);
        }

        struct npc_dragon_soul_shadowed_globuleAI : public ScriptedAI
        {
            npc_dragon_soul_shadowed_globuleAI(Creature* pCreature) : ScriptedAI(pCreature)
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

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_DEEP_CORRUPTION, urand(12000, 24000));
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
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        DoCast(pTarget, SPELL_DEEP_CORRUPTION);
                    events.ScheduleEvent(EVENT_DEEP_CORRUPTION, (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA_2) ? 12000 : 24000));
                }
                
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
        };
};

class npc_dragon_soul_cobalt_globule : public CreatureScript
{
    public:
        npc_dragon_soul_cobalt_globule() : CreatureScript("npc_dragon_soul_cobalt_globule") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_dragon_soul_cobalt_globuleAI (pCreature);
        }

        struct npc_dragon_soul_cobalt_globuleAI : public ScriptedAI
        {
            npc_dragon_soul_cobalt_globuleAI(Creature* pCreature) : ScriptedAI(pCreature)
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

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_MANA_VOID, 3000);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoCastAOE(SPELL_MANA_DIFFUSION);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                    DoCast(me, SPELL_MANA_VOID);
                
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
        };
};

class npc_dragon_soul_teleport : public CreatureScript
{
    public:
        npc_dragon_soul_teleport() : CreatureScript("npc_dragon_soul_teleport") { }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            if (pPlayer->isInCombat())
                return true;

            if (InstanceScript* pInstance = pCreature->GetInstanceScript())
            {
                switch (pCreature->GetEntry())
                {
                    case NPC_EIENDORMI:
                        if (pInstance->GetBossState(DATA_MORCHOK) == DONE)
                            pPlayer->NearTeleportTo(teleportPos[1].GetPositionX(), teleportPos[1].GetPositionY(), teleportPos[1].GetPositionZ(), teleportPos[1].GetOrientation());
                        break;
                    case NPC_TRAVEL_TO_WYRMREST_TEMPLE:
                        pPlayer->NearTeleportTo(teleportPos[0].GetPositionX(), teleportPos[0].GetPositionY(), teleportPos[0].GetPositionZ(), teleportPos[0].GetOrientation());
                        break;
                    default:
                        break;
                }

            }
            return true;
        }
};

class spell_dragon_soul_shadowed_globule_deep_corruption : public SpellScriptLoader
{
    public:
        spell_dragon_soul_shadowed_globule_deep_corruption() : SpellScriptLoader("spell_dragon_soul_shadowed_globule_deep_corruption") { }

        class spell_dragon_soul_shadowed_globule_deep_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dragon_soul_shadowed_globule_deep_corruption_AuraScript);

            void HandlePeriodicTick(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;

                if (Aura* aur = GetAura())
                {
                    if (aur->GetStackAmount() >= 5)
                    {
                        GetCaster()->CastSpell(GetCaster(), SPELL_DEEP_CORRUPTION_DMG, true);
                        aur->Remove();
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dragon_soul_shadowed_globule_deep_corruption_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dragon_soul_shadowed_globule_deep_corruption_AuraScript();
        }
};

class spell_dragon_soul_cobalt_globule_mana_void : public SpellScriptLoader
{
    public:
        spell_dragon_soul_cobalt_globule_mana_void() : SpellScriptLoader("spell_dragon_soul_cobalt_globule_mana_void") { }

        class spell_dragon_soul_cobalt_globule_mana_void_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dragon_soul_cobalt_globule_mana_void_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                targets.remove_if(ManaCheck());
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dragon_soul_cobalt_globule_mana_void_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dragon_soul_cobalt_globule_mana_void_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

        private:
            class ManaCheck
            {
                public:
                    ManaCheck() {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || unit->ToUnit()->getPowerType() != POWER_MANA);
                    }
            };

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dragon_soul_cobalt_globule_mana_void_SpellScript();
        }
};

void AddSC_dragon_soul()
{
    //new npc_dragon_soul_ancient_water_lord();
    new npc_dragon_soul_crimson_globule();
    new npc_dragon_soul_acidic_globule();
    new npc_dragon_soul_dark_globule();
    new npc_dragon_soul_shadowed_globule();
    new npc_dragon_soul_cobalt_globule();
    new npc_dragon_soul_teleport();
    new spell_dragon_soul_shadowed_globule_deep_corruption();
    new spell_dragon_soul_cobalt_globule_mana_void();
}
/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dru_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Containers.h"

enum DruidSpells
{
    DRUID_INCREASED_MOONFIRE_DURATION   = 38414,
    DRUID_NATURES_SPLENDOR              = 57865,
    DRUID_LIFEBLOOM_FINAL_HEAL          = 33778,
    DRUID_SAVAGE_ROAR                   = 62071,
    DRUID_ITEM_T8_BALANCE_RELIC         = 64950,
    DRUID_FAERIE_FIRE                   = 91565,
};

class spell_dru_insect_swarm : public SpellScriptLoader
{
    public:
        spell_dru_insect_swarm() : SpellScriptLoader("spell_dru_insect_swarm") { }

        class spell_dru_insect_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_insect_swarm_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                    if (AuraEffect const* relicAurEff = caster->GetAuraEffect(DRUID_ITEM_T8_BALANCE_RELIC, EFFECT_0))
                        amount += relicAurEff->GetAmount() / aurEff->GetTotalTicks();
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_insect_swarm_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_insect_swarm_AuraScript();
        }
};

enum EclipseSpells
{
    SPELL_DRUID_WRATH                    = 5176,
    SPELL_DRUID_STARFIRE                 = 2912,
    SPELL_DRUID_STARSURGE                = 78674,
    SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE = 89265,
    SPELL_DRUID_STARSURGE_ENERGIZE       = 86605,
    SPELL_DRUID_LUNAR_ECLIPSE_MARKER     = 67484, // Will make the yellow arrow on eclipse bar point to the blue side (lunar)
    SPELL_DRUID_SOLAR_ECLIPSE_MARKER     = 67483, // Will make the yellow arrow on eclipse bar point to the yellow side (solar)
    SPELL_DRUID_SOLAR_ECLIPSE            = 48517,
    SPELL_DRUID_LUNAR_ECLIPSE            = 48518,
    SPELL_DRUID_EUPHORIA_ENERGIZE        = 81069,  
};

// Wrath, Starfire, and Starsurge
class spell_dru_eclipse_energize : public SpellScriptLoader
{
public:
    spell_dru_eclipse_energize() : SpellScriptLoader("spell_dru_eclipse_energize") { }

    class spell_dru_eclipse_energize_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_eclipse_energize_SpellScript);

        int32 energizeAmount;

        bool Load()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (GetCaster()->ToPlayer()->getClass() != CLASS_DRUID)
                return false;

            energizeAmount = 0;

            return true;
        }

        void HandleEnergize(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();

            // No boomy, no deal.
            if (caster->GetPrimaryTalentTree(caster->GetActiveSpec()) != TALENT_TREE_DRUID_BALANCE)
                return;

            switch(GetSpellInfo()->Id)
            {
                case SPELL_DRUID_WRATH:
                {
                    energizeAmount = -GetSpellInfo()->Effects[effIndex].BasePoints; // -13

                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_LUNAR_ECLIPSE_MARKER, true);

                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE))
                    {
                        // Euphoria
                        // To do: which spell we have to use?
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_DRUID, 4431, 0))
                            if (roll_chance_i(aurEff->GetAmount()))
                                //caster->CastCustomSpell(caster, SPELL_DRUID_EUPHORIA_ENERGIZE, &energizeAmount, 0, 0, true);
                                energizeAmount *=2;

                        // Item - Druid T12 Balance 4P Bonus
                        if (caster->HasAura(99049))
                            energizeAmount -= 3;
                    }

                    // If we are set to fill the lunar side or we've just logged in with 0 power..
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                    {
                        caster->CastCustomSpell(caster,SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE,&energizeAmount,0,0,true);
                    }
                    // The energizing effect brought us out of the solar eclipse, remove the aura
                    if (caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) <= 0)
                        caster->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
                    break;
                }
                case SPELL_DRUID_STARFIRE:
                {
                    energizeAmount = GetSpellInfo()->Effects[effIndex].BasePoints; // 20

                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_SOLAR_ECLIPSE_MARKER, true);

                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE))
                    {
                        // Euphoria
                        // To do: which spell we have to use?
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_DRUID, 4431, 0))
                            if (roll_chance_i(aurEff->GetAmount()))
                                //caster->CastCustomSpell(caster, SPELL_DRUID_EUPHORIA_ENERGIZE, &energizeAmount, 0, 0, true);
                                energizeAmount *=2;

                        // Item - Druid T12 Balance 4P Bonus
                        if (caster->HasAura(99049))
                            energizeAmount += 5;
                    }

                    // If we are set to fill the solar side or we've just logged in with 0 power..
                    if (!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                    {
                        caster->CastCustomSpell(caster,SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE,&energizeAmount,0,0,true);
                    }
                    // The energizing effect brought us out of the lunar eclipse, remove the aura
                    if (caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) >= 0)
                        caster->RemoveAura(SPELL_DRUID_LUNAR_ECLIPSE);
                    break;
                }
                case SPELL_DRUID_STARSURGE:
                {
                    // If we are set to fill the solar side or we've just logged in with 0 power (confirmed with sniffs)
                    if ((!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                        || (!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->GetPower(POWER_ECLIPSE) == 0))
                    {
                        energizeAmount = GetSpellInfo()->Effects[effIndex].BasePoints; // 15
                        caster->CastCustomSpell(caster,SPELL_DRUID_STARSURGE_ENERGIZE,&energizeAmount,0,0,true);

                        // If the energize was due to 0 power, cast the eclipse marker aura
                        if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                            caster->CastSpell(caster,SPELL_DRUID_SOLAR_ECLIPSE_MARKER,true);
                    }
                    else if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                    {
                        energizeAmount = -GetSpellInfo()->Effects[effIndex].BasePoints; // -15
                        caster->CastCustomSpell(caster,SPELL_DRUID_STARSURGE_ENERGIZE,&energizeAmount,0,0,true);
                    }
                    // The energizing effect brought us out of the lunar eclipse, remove the aura
                    if (caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) >= 0)
                        caster->RemoveAura(SPELL_DRUID_LUNAR_ECLIPSE);
                    // The energizing effect brought us out of the solar eclipse, remove the aura
                    else if (caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) <= 0)
                        caster->RemoveAura(SPELL_DRUID_SOLAR_ECLIPSE);
                    break;
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_eclipse_energize_SpellScript::HandleEnergize, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_eclipse_energize_SpellScript;
    }
};

enum LunarShowerSpells
{
    SPELL_DRUID_MOONFIRE    = 8921,
    SPELL_DRUID_SUNFIRE     = 93402,
};

// Moonfire and Sunfire
class spell_dru_lunar_shower_energize : public SpellScriptLoader
{
public:
    spell_dru_lunar_shower_energize() : SpellScriptLoader("spell_dru_lunar_shower_energize") { }

    class spell_dru_lunar_shower_energize_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_lunar_shower_energize_SpellScript);

        bool Load()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (GetCaster()->ToPlayer()->getClass() != CLASS_DRUID)
                return false;

            if (!GetCaster()->HasAura(81006) &&
                !GetCaster()->HasAura(81191) &&
                !GetCaster()->HasAura(81192))
                return false;

            return true;
        }

        void HandleEnergize(SpellEffIndex /*effIndex*/)
        {
            Player* caster = GetCaster()->ToPlayer();

            // No boomy, no deal.
            if (caster->GetPrimaryTalentTree(caster->GetActiveSpec()) != TALENT_TREE_DRUID_BALANCE)
                return;

            switch(GetSpellInfo()->Id)
            {
                case SPELL_DRUID_SUNFIRE:
                {
                    int32 energizeAmount = -8;
                    
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_LUNAR_ECLIPSE_MARKER, true);
                    
                    // If we are set to fill the lunar side or we've just logged in with 0 power..
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                    {
                        caster->CastCustomSpell(caster,SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE,&energizeAmount,0,0,true);
                    }
                    // The energizing effect brought us out of the solar eclipse, remove the aura
                    if (caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) <= 0)
                        caster->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
                    break;
                }
                case SPELL_DRUID_MOONFIRE:
                {
                    int32 energizeAmount = 8;
                    
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_SOLAR_ECLIPSE_MARKER, true);
                    
                    // If we are set to fill the solar side or we've just logged in with 0 power..
                    if (!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                    {
                        caster->CastCustomSpell(caster,SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE,&energizeAmount,0,0,true);
                    }
                    // The energizing effect brought us out of the lunar eclipse, remove the aura
                    if (caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) >= 0)
                        caster->RemoveAura(SPELL_DRUID_LUNAR_ECLIPSE);
                    break;
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_lunar_shower_energize_SpellScript::HandleEnergize, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_lunar_shower_energize_SpellScript;
    }
};

class spell_dru_lifebloom : public SpellScriptLoader
{
    public:
        spell_dru_lifebloom() : SpellScriptLoader("spell_dru_lifebloom") { }

        class spell_dru_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_lifebloom_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(DRUID_LIFEBLOOM_FINAL_HEAL))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                // final heal
                int32 stack = GetStackAmount();
                int32 healAmount = aurEff->GetAmount();
                (GetCaster() ? GetCaster(): GetTarget())->CastCustomSpell(GetTarget(), DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, &stack, NULL, true, NULL, aurEff, GetCasterGUID());
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* target = GetUnitOwner())
                {
                    if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                    {
                        // final heal
                        int32 healAmount = aurEff->GetAmount() / aurEff->GetBase()->GetStackAmount();
                        int32 dispell_stacks = dispelInfo->GetRemovedCharges();
                        if (Unit* caster = GetCaster())
                        {
                            target->CastCustomSpell(target, DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, &dispell_stacks, NULL, true, NULL, NULL, GetCasterGUID());
                            return;
                        }

                        target->CastCustomSpell(target, DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, &dispell_stacks, NULL, true, NULL, NULL, GetCasterGUID());
                    }
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dru_lifebloom_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_dru_lifebloom_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_lifebloom_AuraScript();
        }
};

class spell_dru_predatory_strikes : public SpellScriptLoader
{
    public:
        spell_dru_predatory_strikes() : SpellScriptLoader("spell_dru_predatory_strikes") { }

        class spell_dru_predatory_strikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_predatory_strikes_AuraScript);

            void UpdateAmount(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    target->UpdateAttackPowerAndDamage();
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dru_predatory_strikes_AuraScript::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dru_predatory_strikes_AuraScript::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_predatory_strikes_AuraScript();
        }
};

class spell_dru_savage_roar : public SpellScriptLoader
{
    public:
        spell_dru_savage_roar() : SpellScriptLoader("spell_dru_savage_roar") { }

        class spell_dru_savage_roar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_savage_roar_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetShapeshiftForm() != FORM_CAT)
                    return SPELL_FAILED_ONLY_SHAPESHIFT;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dru_savage_roar_SpellScript::CheckCast);
            }
        };

        class spell_dru_savage_roar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_savage_roar_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(DRUID_SAVAGE_ROAR))
                    return false;
                return true;
            }

            void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->CastSpell(target, DRUID_SAVAGE_ROAR, true, NULL, aurEff, GetCasterGUID());
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(DRUID_SAVAGE_ROAR);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dru_savage_roar_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dru_savage_roar_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dru_savage_roar_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_savage_roar_AuraScript();
        }
};

enum StarfallSpells
{
    DRUID_STARFALL_AURA                 = 48505,
};


class spell_dru_starfall_dummy : public SpellScriptLoader
{
    public:
        spell_dru_starfall_dummy() : SpellScriptLoader("spell_dru_starfall_dummy") { }
        class spell_dru_starfall_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_starfall_dummy_SpellScript);
            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                // Shapeshifting into an animal form or mounting cancels the effect
                if (caster->GetCreatureType() == CREATURE_TYPE_BEAST || caster->IsMounted())
                {
                    caster->RemoveAurasDueToSpell(DRUID_STARFALL_AURA);
                    return;
                }

                // Any effect which causes you to lose control of your character will supress the starfall effect.
                if (caster->HasUnitState(UNIT_STATE_CONTROLLED))
                    return;

                caster->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetExplTargetUnit());
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); )
                {
                    if (!(*itr)->ToUnit()->isInCombat() || !(*itr)->IsWithinLOSInMap(GetCaster()))
                        itr = targets.erase(itr);
                    else
                        ++itr;
                }

                if (AuraEffect * eff = GetCaster()->GetAuraEffect(DRUID_STARFALL_AURA, 1))
                {
                    int32 stars = eff->GetAmount() - targets.size();
                    if (stars <= 0)
                    {
                        GetCaster()->RemoveAurasDueToSpell(DRUID_STARFALL_AURA);
                    }
                    else 
                    {
                        eff->SetAmount(stars);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dru_starfall_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_starfall_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dru_starfall_dummy_SpellScript();
        }
};

// 40121 - Swift Flight Form (Passive)
class spell_dru_swift_flight_passive : public SpellScriptLoader
{
    public:
        spell_dru_swift_flight_passive() : SpellScriptLoader("spell_dru_swift_flight_passive") { }

        class spell_dru_swift_flight_passive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_swift_flight_passive_AuraScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                if (Player* caster = GetCaster()->ToPlayer())
                    if (caster->GetSkillValue(SKILL_RIDING) >= 375)
                        amount = 310;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_passive_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_swift_flight_passive_AuraScript();
        }
};

class spell_dru_t10_restoration_4p_bonus : public SpellScriptLoader
{
    public:
        spell_dru_t10_restoration_4p_bonus() : SpellScriptLoader("spell_dru_t10_restoration_4p_bonus") { }

        class spell_dru_t10_restoration_4p_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_t10_restoration_4p_bonus_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster()->ToPlayer()->GetGroup())
                {
                    targets.clear();
                    targets.push_back(GetCaster());
                }
                else
                {
                    targets.remove(GetExplTargetUnit());
                    std::list<Unit*> tempTargets;
                    for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                        if ((*itr)->GetTypeId() == TYPEID_PLAYER && GetCaster()->IsInRaidWith((*itr)->ToUnit()))
                            tempTargets.push_back((*itr)->ToUnit());

                    if (tempTargets.empty())
                    {
                        targets.clear();
                        FinishCast(SPELL_FAILED_DONT_REPORT);
                        return;
                    }

                    Unit* target = Trinity::Containers::SelectRandomContainerElement(tempTargets);
                    targets.clear();
                    targets.push_back(target);
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_t10_restoration_4p_bonus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dru_t10_restoration_4p_bonus_SpellScript();
        }
};

// Primal Madness
// 5229 Enrage
// 5217 Name: Tiger's Fury
// 50334 Berserk
class spell_dru_primal_madness : public SpellScriptLoader
{
    public:
        spell_dru_primal_madness() : SpellScriptLoader("spell_dru_primal_madness") {}

        class spell_dru_primal_madness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_primal_madness_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->getClass() != CLASS_DRUID)
                        return;

                    if (caster->HasAura(80316) || caster->HasAura(80317))
                    {
                       if (caster->GetShapeshiftForm() == FORM_BEAR)
                        {
                            int spell_id = caster->HasAura(80316) ? 80316 : 80317;
                            int bp0 = int32(sSpellMgr->GetSpellInfo(spell_id)->Effects[0].BasePoints);
                            caster->CastCustomSpell(caster, 17080, &bp0, NULL, NULL, true);
                        }
                        else if (caster->GetShapeshiftForm() == FORM_CAT)
                        {
                            int spell_id = caster->HasAura(80316) ? 80879 : 80886;
                            caster->CastSpell(caster, spell_id , true);

                            int bp = int32(sSpellMgr->GetSpellInfo(spell_id)->Effects[0].BasePoints);
                            caster->ModifyPower(POWER_ENERGY, bp);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_dru_primal_madness_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dru_primal_madness_SpellScript();
        }
};

// Feral Aggression - Faerie Fire
class spell_dru_faerie_fire : public SpellScriptLoader
{
public:
    spell_dru_faerie_fire() : SpellScriptLoader("spell_dru_faerie_fire") {}

    class spell_dru_faerie_fire_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_faerie_fire_SpellScript);

        bool Load()
        {
            return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void ChangeStackAmount()
        {
            if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_DRUID, 960, 0))
            {
                if (GetHitUnit() && GetCaster()->IsInFeralForm())
                {
                    if (Aura* faerieFire = GetHitUnit()->GetAura(DRUID_FAERIE_FIRE, GetCaster()->GetGUID()))
                    {
                        uint8 stacks = faerieFire->GetStackAmount() + aurEff->GetAmount() - 1;
                        if (stacks > 3)
                            stacks = 3;

                        faerieFire->SetStackAmount(stacks);
                    }
                }
            }
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_dru_faerie_fire_SpellScript::ChangeStackAmount);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_faerie_fire_SpellScript();
    }
};

void AddSC_druid_spell_scripts()
{
    new spell_dru_insect_swarm();
    new spell_dru_lifebloom();
    new spell_dru_predatory_strikes();
    new spell_dru_savage_roar();
    new spell_dru_starfall_dummy();
    new spell_dru_swift_flight_passive();
    new spell_dru_t10_restoration_4p_bonus();
    new spell_dru_eclipse_energize();
    new spell_dru_primal_madness();
    new spell_dru_lunar_shower_energize();
    new spell_dru_faerie_fire();
}

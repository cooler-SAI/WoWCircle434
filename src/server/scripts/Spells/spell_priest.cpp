/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

enum PriestSpells
{
    PRIEST_POWER_WORD_SHIELD                    = 17,
    PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL           = 48153,
    PRIEST_SPELL_PENANCE_R1                     = 47540,
    PRIEST_SPELL_PENANCE_R1_DAMAGE              = 47758,
    PRIEST_SPELL_PENANCE_R1_HEAL                = 47757,
    PRIEST_SPELL_REFLECTIVE_SHIELD_TRIGGERED    = 33619,
    PRIEST_SPELL_REFLECTIVE_SHIELD_R1           = 33201,
    PRIEST_SPELL_VAMPIRIC_TOUCH_DISPEL          = 87204,
    PRIEST_SPELL_EMPOWERED_RENEW                = 63544,
    PRIEST_ICON_ID_EMPOWERED_RENEW_TALENT       = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING           = 2874,
    PRIEST_SHADOW_WORD_DEATH                    = 32409,
    PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH      = 107903,
    PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH         = 107904,
    PRIEST_GLYPH_OF_SHADOW                      = 107906,
    PRIEST_IMPROVED_DEVOURING_PLAGUE_DAMAGE     = 63675,
    PRIEST_IMPROVED_DEVOURING_PLAGUE_HEAL       = 75999,
    PRIEST_SILENCE_INTERRUPT                    = 32747
};

// Guardian Spirit
class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL))
                    return false;
                return true;
            }

            bool Load()
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
                // remove the aura now, we don't want 40% healing bonus
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_1);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

class spell_pri_mana_burn : public SpellScriptLoader
{
    public:
        spell_pri_mana_burn() : SpellScriptLoader("spell_pri_mana_burn") { }

        class spell_pri_mana_burn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mana_burn_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* unitTarget = GetHitUnit())
                    unitTarget->RemoveAurasWithMechanic((1 << MECHANIC_FEAR) | (1 << MECHANIC_POLYMORPH));
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pri_mana_burn_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mana_burn_SpellScript;
        }
};

class spell_pri_mind_sear : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") { }

        class spell_pri_mind_sear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_sear_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove_if(Trinity::ObjectGUIDCheck(GetCaster()->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT)));
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mind_sear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_sear_SpellScript();
        }
};

class spell_pri_pain_and_suffering_proc : public SpellScriptLoader
{
    public:
        spell_pri_pain_and_suffering_proc() : SpellScriptLoader("spell_pri_pain_and_suffering_proc") { }

        // 47948 Pain and Suffering (proc)
        class spell_pri_pain_and_suffering_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_pain_and_suffering_proc_SpellScript);

            void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
            {
                // Refresh Shadow Word: Pain on target
                if (Unit* unitTarget = GetHitUnit())
                    if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0, 0, GetCaster()->GetGUID()))
                        aur->GetBase()->RefreshDuration();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_pain_and_suffering_proc_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_pain_and_suffering_proc_SpellScript;
        }
};

class spell_pri_penance : public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SPELL_PENANCE_R1))
                    return false;
                // can't use other spell than this penance due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(PRIEST_SPELL_PENANCE_R1) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(PRIEST_SPELL_PENANCE_R1_DAMAGE, rank, true))
                    return false;
                if (!sSpellMgr->GetSpellWithRank(PRIEST_SPELL_PENANCE_R1_HEAL, rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    if (!unitTarget->isAlive())
                        return;

                    uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);

                    if (caster->IsFriendlyTo(unitTarget))
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(PRIEST_SPELL_PENANCE_R1_HEAL, rank), false, 0);
                    else
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(PRIEST_SPELL_PENANCE_R1_DAMAGE, rank), false, 0);
                }
            }

            SpellCastResult CheckCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Unit* target = GetExplTargetUnit())
                    if (!caster->IsFriendlyTo(target) && !caster->IsValidAttackTarget(target))
                        return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                // add dummy effect spell handler to Penance
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_penance_SpellScript;
        }
};

// Reflective Shield
class spell_pri_reflective_shield_trigger : public SpellScriptLoader
{
    public:
        spell_pri_reflective_shield_trigger() : SpellScriptLoader("spell_pri_reflective_shield_trigger") { }

        class spell_pri_reflective_shield_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_reflective_shield_trigger_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SPELL_REFLECTIVE_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(PRIEST_SPELL_REFLECTIVE_SHIELD_R1))
                    return false;
                return true;
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetAttacker() == target)
                    return;

                if (dmgInfo.GetAttacker()->isTotem())
                    return;

                if (GetCaster())
                    if (AuraEffect* talentAurEff = target->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PRIEST, 4880, 0))
                    {
                        int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
                        target->CastCustomSpell(dmgInfo.GetAttacker(), PRIEST_SPELL_REFLECTIVE_SHIELD_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
                    }
            }

            void Register()
            {
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_reflective_shield_trigger_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_reflective_shield_trigger_AuraScript();
        }
};

enum PrayerOfMending
{
    SPELL_T9_HEALING_2_PIECE = 67201,
};
// Prayer of Mending Heal
class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
public:
    spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

    class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetOriginalCaster())
            {
                if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_T9_HEALING_2_PIECE, EFFECT_0))
                {
                    int32 heal = GetHitHeal();
                    AddPct(heal, aurEff->GetAmount());
                    SetHitHeal(heal);
                }
            }

        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_prayer_of_mending_heal_SpellScript();
    }
};

class spell_pri_vampiric_touch : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SPELL_VAMPIRIC_TOUCH_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (!GetCaster() || !dispelInfo->GetDispeller())
                    return;

                if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PRIEST, 1869, EFFECT_1))
                    if (roll_chance_i(aurEff->GetSpellInfo()->Effects[EFFECT_0].BasePoints))
                        dispelInfo->GetDispeller()->CastSpell(dispelInfo->GetDispeller(), PRIEST_SPELL_VAMPIRIC_TOUCH_DISPEL, true, 0, aurEff, GetCaster()->GetGUID());

            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }
};

class spell_pri_shadow_word_death : public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

        class spell_pri_shadow_word_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

            void HandleDamage()
            {
                int32 damage = GetHitDamage();

                // Pain and Suffering reduces damage
                if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(47580, 1))
                    AddPct(damage, aurEff->GetAmount());
                else if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(47581, 1))
                    AddPct(damage, aurEff->GetAmount());

                // Item - Priest T13 Shadow 2P Bonus (Shadow Word: Death)
                if (AuraEffect * aurEff = GetCaster()->GetAuraEffect(105843, 1))
                    AddPct(damage, -aurEff->GetAmount());

                GetCaster()->CastCustomSpell(GetCaster(), PRIEST_SHADOW_WORD_DEATH, &damage, 0, 0, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_shadow_word_death_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadow_word_death_SpellScript();
        }
};

class spell_pri_shadowform : public SpellScriptLoader
{
    public:
        spell_pri_shadowform() : SpellScriptLoader("spell_pri_shadowform") { }

        class spell_pri_shadowform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowform_AuraScript);

            bool Validate(SpellInfo const* /*entry*/)
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH) ||
                    !sSpellMgr->GetSpellInfo(PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), GetTarget()->HasAura(PRIEST_GLYPH_OF_SHADOW) ? PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(GetTarget()->HasAura(PRIEST_GLYPH_OF_SHADOW) ? PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_shadowform_AuraScript();
        }
};

// Improved Devouring Plague healing part
class spell_pri_improved_devouring_plague : public SpellScriptLoader
{
    public:
    spell_pri_improved_devouring_plague() : SpellScriptLoader("spell_pri_improved_devouring_plague") {}

    class spell_pri_improved_devouring_plague_SpellScript : public SpellScript
    {
            PrepareSpellScript(spell_pri_improved_devouring_plague_SpellScript);

        bool Validate(SpellEntry * entry)
        {
            if (entry->Id == PRIEST_IMPROVED_DEVOURING_PLAGUE_DAMAGE)
                return true;
            return false;
        }

        void HandleDamage()
        {
            int32 damage = GetHitDamage();
            int32 heal = int32(CalculatePct(damage, 15));
            GetCaster()->CastCustomSpell(GetCaster(), PRIEST_IMPROVED_DEVOURING_PLAGUE_HEAL, &heal, 0, 0, true);
        }
        void Register()
        {
            AfterHit += SpellHitFn(spell_pri_improved_devouring_plague_SpellScript::HandleDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_improved_devouring_plague_SpellScript();
    }
};

class spell_pri_silence : public SpellScriptLoader
{
public:
    spell_pri_silence() : SpellScriptLoader("spell_pri_silence") {}

    class spell_pri_silence_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_silence_SpellScript);

        void HandleBeforeHit()
        {
            if (Unit* target = GetExplTargetUnit())
                if (target->GetTypeId() != TYPEID_PLAYER)
                    GetCaster()->CastSpell(target, PRIEST_SILENCE_INTERRUPT, true);
            
        }
        void Register()
        {
            BeforeHit += SpellHitFn(spell_pri_silence_SpellScript::HandleBeforeHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_silence_SpellScript();
    }
};

// Power Word: Shield
class spell_pri_power_word_shield : public SpellScriptLoader
{
public:
    spell_pri_power_word_shield() : SpellScriptLoader("spell_pri_power_word_shield") { }

    class spell_pri_power_word_shield_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_power_word_shield_AuraScript);

        bool t13bumped;

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(PRIEST_POWER_WORD_SHIELD))
                return false;
            return true;
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
        {
            Unit *caster = GetCaster();
            if(!caster)
                return;
            t13bumped = false;

            //+87% from sp bonus
            float bonus = 0.87f;
            amount += caster->SpellBaseHealingBonusDone(SpellSchoolMask(GetSpellInfo()->SchoolMask)) * bonus;

            // Improved Power Word: Shield
            if (AuraEffect const* IPWS = caster->GetAuraEffect(14768, 0))
                AddPct(amount, IPWS->GetAmount());
            else if (AuraEffect const* IPWS = caster->GetAuraEffect(14748, 0))    // Rank 1
                AddPct(amount, IPWS->GetAmount());

            // Focused Power
            // Reuse variable, not sure if this code below can be moved before Twin Disciplines
            amount *= caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_DONE_PERCENT);

            // Mastery Priest
            if (Aura* aur = caster->GetAura(77484))
                AddPct(amount, aur->GetEffect(0)->GetAmount());

            // T13 Priest 
            if (AuraEffect * eff = caster->GetAuraEffect(105832, 0))
            {
                if (roll_chance_i(eff->GetAmount()))
                {
                    t13bumped = true;
                    amount *= 2;
                }
            }
        }
        void HandleEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit *caster = GetCaster();
            if (!caster)
                return;

            // Rapture
            if (GetRemoveMod() != AURA_REMOVE_BY_ENEMY_SPELL && GetRemoveMod() != AURA_REMOVE_BY_ENEMY_SPELL)
                return;

            // Rapture cooldown
            if (caster->HasAura(63853))
                return;

            if (AuraEffect* auraEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PRIEST, 2894, 0))
            {
                float multiplier = float(auraEff->GetAmount());
                if (t13bumped)
                    multiplier *= 2;

                int32 basepoints0 = int32(CalculatePct(caster->GetMaxPower(POWER_MANA), multiplier));
                caster->CastCustomSpell(caster, 47755, &basepoints0, NULL, NULL, true);
            }

            caster->CastSpell(caster, 63853, true);
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            Unit* caster = GetCaster();
            if(!caster || !target)
                return;

            if (!target->HasAura(6788))
                caster->CastSpell(target, 6788, true);

             // Glyph of Power Word: Shield
            if (AuraEffect* glyph = caster->GetAuraEffect(55672, 0))
            {
                // instantly heal m_amount% of the absorb-value
                int32 heal = glyph->GetAmount() * GetEffect(0)->GetAmount()/100;
                caster->CastCustomSpell(GetUnitOwner(), 56160, &heal, NULL, NULL, true, 0, GetEffect(0));
            }
        }
        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            AfterEffectApply   += AuraEffectApplyFn(spell_pri_power_word_shield_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove  += AuraEffectRemoveFn(spell_pri_power_word_shield_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_power_word_shield_AuraScript();
    }
};

void AddSC_priest_spell_scripts()
{
    new spell_pri_guardian_spirit();
    new spell_pri_mana_burn();
    new spell_pri_pain_and_suffering_proc();
    new spell_pri_penance();
    new spell_pri_reflective_shield_trigger();
    new spell_pri_mind_sear();
    new spell_pri_prayer_of_mending_heal();
    new spell_pri_vampiric_touch();
    new spell_pri_shadow_word_death();
    new spell_pri_shadowform();
    new spell_pri_improved_devouring_plague();
    new spell_pri_silence();
    new spell_pri_power_word_shield();
}

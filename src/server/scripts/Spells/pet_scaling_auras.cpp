/*
 * Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
 *
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum ScalingAuras
{
    DEATHKNIGHT_SCALING_1       = 54566, // Stamina, Strength, Magic Damage Done
    DEATHKNIGHT_SCALING_2       = 51996, // All Damage Done, Attack Speed, Charm Immunity
    DEATHKNIGHT_SCALING_3       = 61697, // All Hit chance, Spell Hit Chance, Sleep Immunity
    DEATHKNIGHT_SCALING_5       = 110474,// Critical chance, Magic Resistance 
    HUNTER_SCALING_4            = 61017, // All Hit chance, Spell Hit Chance, Expertise from hit
    HUNTER_SCALING_5            = 89446, // All Critical Chance, Attack Speed, Magic Resistance
    WARLOCK_SCALING_5           = 61013, // All Hit Chance, Spell Hit Chance, Expertise from hit
    SHAMAN_SCALING              = 61783, // All Hit Chance, Spell Hit Chance, All Damage Done
    WARLOCK_SCALING_6           = 89953, // All Hit chance, Attack Speed, Magic Resistance 
    PRIEST_SCALING_5            = 89962, // All Critical Chance, Magic Resistance, Crit Damage Bonus
    MAGE_SCALING_5              = 89764, // All Critical Chance, Attack Speed, Magic Resistance
};

class spell_pet_stat_calculate : public SpellScriptLoader
{
    public:
        spell_pet_stat_calculate() : SpellScriptLoader("spell_pet_stat_calculate") { }

        class spell_pet_stat_calculate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pet_stat_calculate_AuraScript);

            bool Load()
            {
                if (!GetCaster() || !GetCaster()->GetOwner() || GetCaster()->GetOwner()->GetTypeId() != TYPEID_PLAYER)
                    return false;

                return true;
            }

            void CalculateAmountHasteSpell(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // Haste from equipment
                    float hasteAmount = owner->GetRatingBonusValue(CR_HASTE_MELEE);
                    // Auras
                    hasteAmount += owner->GetTotalAuraModifier(SPELL_AURA_MOD_MELEE_HASTE);
                    hasteAmount += owner->GetTotalAuraModifier(SPELL_AURA_MOD_MELEE_HASTE_3);
                    
                    amount += int32(hasteAmount);
                }
            }

            void CalculateAmountMeleeHit(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float HitMelee = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    HitMelee += owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
                    // Increase hit melee from meele hit ratings
                    HitMelee += owner->GetRatingBonusValue(CR_HIT_MELEE);

                    amount += int32(HitMelee);
                }
            }

            void CalculateAmountSpellHit(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float HitSpell = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_SPELL_HIT_CHANCE
                    HitSpell += owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE);
                    // Increase hit spell from spell hit ratings
                    HitSpell += owner->GetRatingBonusValue(CR_HIT_SPELL);

                    amount += int32(HitSpell);
                }
            }

            void CalculateAmountMeleeCrit(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float CritMelee = 0.0f;
                    // Crit from Agility
                    CritMelee += owner->GetMeleeCritFromAgility();
                    // Increase crit from SPELL_AURA_MOD_WEAPON_CRIT_PERCENT
                    CritMelee += owner->GetTotalAuraModifier(SPELL_AURA_MOD_WEAPON_CRIT_PERCENT);
                    // Increase crit from SPELL_AURA_MOD_CRIT_PCT
                    CritMelee += owner->GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
                    // Increase crit melee from melee crit ratings
                    CritMelee += owner->GetRatingBonusValue(CR_CRIT_MELEE);

                    amount += int32(CritMelee);
                }
            }

            void CalculateAmountSpellCrit(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float CritSpell = 0.0f;
                    // Crit from Intellect
                    CritSpell += owner->GetSpellCritFromIntellect();
                    // Increase crit from SPELL_AURA_MOD_SPELL_CRIT_CHANCE
                    CritSpell += owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
                    // Increase crit from SPELL_AURA_MOD_CRIT_PCT
                    CritSpell += owner->GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
                    // Increase crit spell from spell crit ratings
                    CritSpell += owner->GetRatingBonusValue(CR_CRIT_SPELL);

                    amount += int32(CritSpell);
                }
            }

            void CalculateAmountExpertise(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float Expertise = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    Expertise += owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
                    // Increase hit melee from meele hit ratings
                    Expertise += owner->GetRatingBonusValue(CR_HIT_MELEE);

                    amount += int32(Expertise);
                }
            }

            void CalculateAmountExpertiseFromHit(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float Expertise = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    Expertise += owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
                    // Increase hit melee from meele hit ratings
                    Expertise += owner->GetRatingBonusValue(CR_HIT_MELEE);

                    amount += int32(Expertise);
                }
            }

            void CalculateAmountSpellPenetration(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
                {
                    // For others recalculate it from:
                    float SpellPenetration = 0.0f;
                    // Increase spell penetration from SPELL_AURA_MOD_TARGET_RESISTANCE
                    SpellPenetration += owner->GetTotalAuraModifier(SPELL_AURA_MOD_TARGET_RESISTANCE);
                    // Increase spell penetration from items
                    SpellPenetration -= owner->GetSpellPenetrationItemMod();

                    amount += int32(SpellPenetration);
                }
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                    case DEATHKNIGHT_SCALING_2:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountHasteSpell, EFFECT_1, SPELL_AURA_MELEE_SLOW);
                        break;
                    case SHAMAN_SCALING:
                    case DEATHKNIGHT_SCALING_3:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountMeleeHit, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellHit, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
                        break;
                    case HUNTER_SCALING_4:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountMeleeHit, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellHit, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountExpertiseFromHit, EFFECT_2, SPELL_AURA_MOD_EXPERTISE);
                        break;
                    case HUNTER_SCALING_5:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountMeleeCrit, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountHasteSpell, EFFECT_1, SPELL_AURA_MELEE_SLOW);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellPenetration, EFFECT_2, SPELL_AURA_MOD_TARGET_RESISTANCE);
                        break;
                    case WARLOCK_SCALING_5:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountMeleeHit, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellHit, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountExpertiseFromHit, EFFECT_2, SPELL_AURA_MOD_EXPERTISE);
                        break;
                    case WARLOCK_SCALING_6:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellCrit, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountHasteSpell, EFFECT_1, SPELL_AURA_MELEE_SLOW);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellPenetration, EFFECT_2, SPELL_AURA_MOD_TARGET_RESISTANCE);
                        break;
                    case DEATHKNIGHT_SCALING_5:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountMeleeCrit, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellPenetration, EFFECT_2, SPELL_AURA_MOD_TARGET_RESISTANCE);
                        break;
                    case PRIEST_SCALING_5:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellCrit, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellPenetration, EFFECT_1, SPELL_AURA_MOD_TARGET_RESISTANCE);
                        break;
                    case MAGE_SCALING_5:
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellCrit, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountHasteSpell, EFFECT_1, SPELL_AURA_MELEE_SLOW);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pet_stat_calculate_AuraScript::CalculateAmountSpellPenetration, EFFECT_2, SPELL_AURA_MOD_TARGET_RESISTANCE);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pet_stat_calculate_AuraScript();
        }
};

void AddSC_pet_spell_scripts()
{
    new spell_pet_stat_calculate();
}
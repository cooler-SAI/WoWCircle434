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
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum WarriorSpells
{
    SPELL_LAST_STAND_TRIGGERED              = 12976,
    SPELL_DEEP_WOUNDS_RANK_1                = 12162,
    SPELL_DEEP_WOUNDS_RANK_2                = 12850,
    SPELL_DEEP_WOUNDS_RANK_3                = 12868,
    SPELL_DEEP_WOUNDS_RANK_PERIODIC         = 12721,
    SPELL_JUGGERNAUT_CRIT_BONUS_TALENT      = 64976,
    SPELL_JUGGERNAUT_CRIT_BONUS_BUFF        = 65156,
    SPELL_INTERCEPT                         = 20252, 
    SPELL_CHARGE                            = 34846,
    SPELL_SLAM                              = 50782,
    ICON_ID_SUDDEN_DEATH                    = 1989,
    SPELL_BLOODTHIRST                       = 23885,
    SPELL_VICTORY_RUSH                      = 34428,
    SPELL_VICTORY_RUSH_AURA                 = 32216,
    SPELL_COLOSSUS_SMASH                    = 86346
};

/// Updated 4.3.4
class spell_warr_last_stand : public SpellScriptLoader
{
    public:
        spell_warr_last_stand() : SpellScriptLoader("spell_warr_last_stand") { }

        class spell_warr_last_stand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_last_stand_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LAST_STAND_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(GetEffectValue()));
                    caster->CastCustomSpell(caster, SPELL_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                }
            }

            void Register()
            {
                // add dummy effect spell handler to Last Stand
                OnEffectHit += SpellEffectFn(spell_warr_last_stand_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_last_stand_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_deep_wounds : public SpellScriptLoader
{
    public:
        spell_warr_deep_wounds() : SpellScriptLoader("spell_warr_deep_wounds") { }

        class spell_warr_deep_wounds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_deep_wounds_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEEP_WOUNDS_RANK_1) || !sSpellMgr->GetSpellInfo(SPELL_DEEP_WOUNDS_RANK_2) || !sSpellMgr->GetSpellInfo(SPELL_DEEP_WOUNDS_RANK_3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    // apply percent damage mods
                    damage = caster->SpellDamageBonusDone(target, GetSpellInfo(), damage, SPELL_DIRECT_DAMAGE);

                    ApplyPct(damage, 16 * sSpellMgr->GetSpellRank(GetSpellInfo()->Id));

                    damage = target->SpellDamageBonusTaken(caster, GetSpellInfo(), damage, SPELL_DIRECT_DAMAGE);

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_DEEP_WOUNDS_RANK_PERIODIC);
                    uint32 ticks = spellInfo->GetDuration() / spellInfo->Effects[EFFECT_0].Amplitude;

                    // Add remaining ticks to damage done
                    if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_DEEP_WOUNDS_RANK_PERIODIC, EFFECT_0, caster->GetGUID()))
                        damage += aurEff->GetAmount() * (ticks - aurEff->GetTickNumber());

                    damage = damage / ticks;

                    caster->CastCustomSpell(target, SPELL_DEEP_WOUNDS_RANK_PERIODIC, &damage, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_deep_wounds_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_deep_wounds_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_charge : public SpellScriptLoader
{
    public:
        spell_warr_charge() : SpellScriptLoader("spell_warr_charge") { }

        class spell_warr_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_charge_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGERNAUT_CRIT_BONUS_TALENT) || !sSpellMgr->GetSpellInfo(SPELL_JUGGERNAUT_CRIT_BONUS_BUFF) || !sSpellMgr->GetSpellInfo(SPELL_CHARGE))
                    return false;
                return true;
            }
            void  HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return;

                int32 chargeBasePoints0 = GetEffectValue();
                Player* caster = GetCaster()->ToPlayer();
                caster->CastCustomSpell(caster, SPELL_CHARGE, &chargeBasePoints0, NULL, NULL, true);

                //Juggernaut crit bonus
                if (caster->HasAura(SPELL_JUGGERNAUT_CRIT_BONUS_TALENT))
                {
                    caster->CastSpell(caster, SPELL_JUGGERNAUT_CRIT_BONUS_BUFF, true);
                    caster->AddSpellCooldown(SPELL_INTERCEPT, 0, time(NULL) + 30);
                }
            }

            void HandleCharge(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), 96273, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_charge_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_slam : public SpellScriptLoader
{
    public:
        spell_warr_slam() : SpellScriptLoader("spell_warr_slam") { }

        class spell_warr_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_slam_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAM))
                    return false;
                return true;
            }
            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetEffectValue();
                if (GetHitUnit())
                {
                    GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SLAM, &bp0, NULL, NULL, true, 0);

                    // Single-Minded Fury
                    if (AuraEffect * auraeff = GetCaster()->GetAuraEffect(81099, 0))
                        if (auraeff->GetAmount())
                            GetCaster()->CastCustomSpell(GetHitUnit(), 81101, &bp0, NULL, NULL, true, 0);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_slam_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_slam_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_concussion_blow : public SpellScriptLoader
{
    public:
        spell_warr_concussion_blow() : SpellScriptLoader("spell_warr_concussion_blow") { }

        class spell_warr_concussion_blow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_concussion_blow_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                SetHitDamage(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), GetEffectValue()));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_concussion_blow_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_concussion_blow_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_bloodthirst : public SpellScriptLoader
{
    public:
        spell_warr_bloodthirst() : SpellScriptLoader("spell_warr_bloodthirst") { }

        class spell_warr_bloodthirst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bloodthirst_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                ApplyPct(damage, GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK));

                if (Unit* target = GetHitUnit())
                {
                    damage = GetCaster()->SpellDamageBonusDone(target, GetSpellInfo(), uint32(damage), SPELL_DIRECT_DAMAGE);
                    damage = target->SpellDamageBonusTaken(GetCaster(), GetSpellInfo(), uint32(damage), SPELL_DIRECT_DAMAGE);
                }
                SetHitDamage(damage);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                GetCaster()->CastCustomSpell(GetCaster(), SPELL_BLOODTHIRST, &damage, NULL, NULL, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_bloodthirst_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnEffectHit += SpellEffectFn(spell_warr_bloodthirst_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_bloodthirst_SpellScript();
        }
};

class spell_warr_whirlwind: public SpellScriptLoader
{
    public:
        spell_warr_whirlwind(): SpellScriptLoader("spell_warr_whirlwind") { }

    class spell_warr_whirlwind_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warr_whirlwind_SpellScript);

        void SelectTargets()
        {

            if (GetCaster()->GetTypeId() == TYPEID_PLAYER && GetSpell()->GetUniqueTargets().size() >= 4)
                GetCaster()->ToPlayer()->SpellCooldownReduction(GetSpellInfo()->Id, 6000);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_warr_whirlwind_SpellScript::SelectTargets);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_whirlwind_SpellScript();
    }
};

class spell_warr_victory_rush : public SpellScriptLoader
{
public:
    spell_warr_victory_rush() : SpellScriptLoader("spell_warr_victory_rush") { }

    class spell_warr_victory_rush_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_victory_rush_SpellScript);

        bool Validate(SpellInfo const* /*SpellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_VICTORY_RUSH))
                return false;
            return true;
        }

        void HandleBeforeHit()
        {
            GetCaster()->RemoveAurasDueToSpell(SPELL_VICTORY_RUSH_AURA);
        }
        void Register()
        {
            OnCast += SpellCastFn(spell_warr_victory_rush_SpellScript::HandleBeforeHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_warr_victory_rush_SpellScript();
    }
};

class spell_warr_colossus_smash : public SpellScriptLoader
{
public:
    spell_warr_colossus_smash() : SpellScriptLoader("spell_warr_colossus_smash") { }

    class spell_warr_colossus_smash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_colossus_smash_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_COLOSSUS_SMASH))
                return false;
            return true;
        }

        void HandleAfterHit()
        {
            Unit * caster = GetCaster();
            Unit * target = GetHitUnit();
            if (caster && target)
            {
                caster->AddAura(SPELL_COLOSSUS_SMASH, target);
            }
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_warr_colossus_smash_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_colossus_smash_SpellScript();
    }
};

void AddSC_warrior_spell_scripts()
{
    new spell_warr_last_stand();
    new spell_warr_deep_wounds();
    new spell_warr_charge();
    new spell_warr_slam();
    new spell_warr_concussion_blow();
    new spell_warr_bloodthirst();
    new spell_warr_whirlwind();
    new spell_warr_victory_rush();
    new spell_warr_colossus_smash();
}

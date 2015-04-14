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
 * Scripts for spells with SPELLFAMILY_DEATHKNIGHT and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dk_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum DeathKnightSpells
{
    DK_SPELL_RUNIC_POWER_ENERGIZE               = 49088,
    DK_SPELL_ANTI_MAGIC_SHELL_TALENT            = 51052,
    DK_SPELL_CORPSE_EXPLOSION_TRIGGERED         = 43999,
    DK_SPELL_CORPSE_EXPLOSION_VISUAL            = 51270,
    DK_SPELL_GHOUL_EXPLODE                      = 47496,
    DK_SPELL_SCOURGE_STRIKE_TRIGGERED           = 70890,
    DK_SPELL_BLOOD_BOIL_TRIGGERED               = 65658,
    DK_SPELL_WILL_OF_THE_NECROPOLIS_TALENT_R1   = 49189,
    DK_SPELL_WILL_OF_THE_NECROPOLIS_AURA_R1     = 52284,
    DK_SPELL_BLOOD_PRESENCE                     = 48266,
    DK_SPELL_IMPROVED_BLOOD_PRESENCE_TRIGGERED  = 63611,
    DK_SPELL_UNHOLY_PRESENCE                    = 48265,
    DK_SPELL_IMPROVED_UNHOLY_PRESENCE_TRIGGERED = 63622,
    SPELL_DK_ITEM_T8_MELEE_4P_BONUS             = 64736,
    DK_SPELL_BLACK_ICE_R1                       = 49140,
};

// 50462 - Anti-Magic Shell (on raid member)
class spell_dk_anti_magic_shell_raid : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_raid() : SpellScriptLoader("spell_dk_anti_magic_shell_raid") { }

        class spell_dk_anti_magic_shell_raid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_raid_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // TODO: this should absorb limited amount of damage, but no info on calculation formula
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_raid_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_raid_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_raid_AuraScript();
        }
};

// 48707 - Anti-Magic Shell (on self)
class spell_dk_anti_magic_shell_self : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_self() : SpellScriptLoader("spell_dk_anti_magic_shell_self") { }

        class spell_dk_anti_magic_shell_self_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_self_AuraScript);

            uint32 absorbPct, hpPct;
            uint8 magicSuppressionRank;
            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                magicSuppressionRank = 0;

                if (AuraEffect const* absorbBonus = GetCaster()->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_DEATHKNIGHT, 99, 0))
                    magicSuppressionRank = sSpellMgr->GetSpellRank(absorbBonus->GetId());

                hpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_RUNIC_POWER_ENERGIZE))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = GetCaster()->CountPctFromMaxHealth(hpPct);
                const_cast<AuraEffect*>(aurEff)->SetBaseAmount(int32(absorbPct));
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                absorbAmount = std::min(CalculatePct(dmgInfo.GetDamage(), absorbPct), GetTarget()->CountPctFromMaxHealth(hpPct));
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & /*dmgInfo*/, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();

                // Magic Suppression - produces energy from damage
                if (magicSuppressionRank)
                {
                    // damage absorbed by Anti-Magic Shell energizes the DK with additional runic power.
                    // This, if I'm not mistaken, shows that we get back ~20% of the absorbed damage as runic power.
                    int32 bp = std::max(uint32(1), absorbAmount * magicSuppressionRank / 227 / 3) * 10;
                    target->CastCustomSpell(target, DK_SPELL_RUNIC_POWER_ENERGIZE, &bp, NULL, NULL, true, NULL, aurEff);
                }
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_self_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Absorb, EFFECT_0);
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_self_AuraScript();
        }
};

// 50461 - Anti-Magic Zone
class spell_dk_anti_magic_zone : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_zone() : SpellScriptLoader("spell_dk_anti_magic_zone") { }

        class spell_dk_anti_magic_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_zone_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_ANTI_MAGIC_SHELL_TALENT))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                SpellInfo const* talentSpell = sSpellMgr->GetSpellInfo(DK_SPELL_ANTI_MAGIC_SHELL_TALENT);
                amount = talentSpell->Effects[EFFECT_0].CalcValue(GetCaster());
                if (GetCaster()->GetOwner())
                    if (Player* player = GetCaster()->GetOwner()->ToPlayer())
                        amount += int32(2 * player->GetTotalAttackPowerValue(BASE_ATTACK));
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_zone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_zone_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_zone_AuraScript();
        }
};

class spell_dk_death_gate : public SpellScriptLoader
{
    public:
        spell_dk_death_gate() : SpellScriptLoader("spell_dk_death_gate") {}

        class spell_dk_death_gate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetCaster()->getClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, GetEffectValue(), false);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_SpellScript::CheckClass);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_SpellScript();
        }
};

class spell_dk_death_pact : public SpellScriptLoader
{
    public:
        spell_dk_death_pact() : SpellScriptLoader("spell_dk_death_pact") { }

        class spell_dk_death_pact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_pact_SpellScript);

            SpellCastResult CheckCast()
            {
                // Check if we have valid targets, otherwise skip spell casting here
                if (Player* player = GetCaster()->ToPlayer())
                    for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                        if (Creature* undeadPet = (*itr)->ToCreature())
                            if (undeadPet->isAlive() &&
                                undeadPet->GetOwnerGUID() == player->GetGUID() &&
                                undeadPet->GetCreatureType() == CREATURE_TYPE_UNDEAD &&
                                undeadPet->IsWithinDist(player, 100.0f, false))
                                return SPELL_CAST_OK;

                return SPELL_FAILED_NO_PET;
            }

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                Unit* unit_to_add = NULL;
                for (std::list<WorldObject*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                {
                    if (Unit* unit = (*itr)->ToUnit())
                    if (unit->GetOwnerGUID() == GetCaster()->GetGUID() && unit->GetCreatureType() == CREATURE_TYPE_UNDEAD)
                    {
                        unit_to_add = unit;
                        break;
                    }
                }

                unitList.clear();
                if (unit_to_add)
                    unitList.push_back(unit_to_add);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_pact_SpellScript::CheckCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_death_pact_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_pact_SpellScript();
        }
};

// 55090 Scourge Strike (55265, 55270, 55271)
class spell_dk_scourge_strike : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike() : SpellScriptLoader("spell_dk_scourge_strike") { }

        class spell_dk_scourge_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_SpellScript);
            float multiplier;

            bool Load()
            {
                multiplier = 1.0f;
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_SCOURGE_STRIKE_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    multiplier = (GetEffectValue() * unitTarget->GetDiseasesByCaster(caster->GetGUID()) / 100.f);
                    SpellInfo const* shadowPart = sSpellMgr->GetSpellInfo(DK_SPELL_SCOURGE_STRIKE_TRIGGERED);

                    // Death Knight T8 Melee 4P Bonus
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_ITEM_T8_MELEE_4P_BONUS, EFFECT_0))
                        AddPct(multiplier, aurEff->GetAmount());

                    // Second part is also dependent on the increasing damage effects
                    Unit::AuraEffectList const& damageAuras = caster->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                    for (Unit::AuraEffectList::const_iterator i = damageAuras.begin(); i != damageAuras.end(); ++i)
                    {
                        if (((*i)->GetMiscValue() & shadowPart->GetSchoolMask()) || ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
                            AddPct(multiplier, (*i)->GetAmount());
                    }
                    caster->ToPlayer()->ApplySpellMod(DK_SPELL_SCOURGE_STRIKE_TRIGGERED, SPELLMOD_DAMAGE, multiplier);
                }
            }

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = (GetHitDamage() + GetAbsorbedDamage()) * multiplier;

                    if (AuraEffect* aurEff = caster->GetAuraEffectOfRankedSpell(DK_SPELL_BLACK_ICE_R1, EFFECT_0))
                        AddPct(bp, aurEff->GetAmount());

                    caster->CastCustomSpell(unitTarget, DK_SPELL_SCOURGE_STRIKE_TRIGGERED, &bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_dk_scourge_strike_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_scourge_strike_SpellScript();
        }
};

// 48721 Blood Boil
class spell_dk_blood_boil : public SpellScriptLoader
{
    public:
        spell_dk_blood_boil() : SpellScriptLoader("spell_dk_blood_boil") { }

        class spell_dk_blood_boil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_blood_boil_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_BLOOD_BOIL_TRIGGERED))
                    return false;
                return true;
            }

            bool Load()
            {
                _executed = false;
                return GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCaster()->getClass() == CLASS_DEATH_KNIGHT;
            }

            void HandleAfterHit()
            {
                if (_executed || !GetHitUnit())
                    return;

                _executed = true;
                GetCaster()->CastSpell(GetCaster(), DK_SPELL_BLOOD_BOIL_TRIGGERED, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_dk_blood_boil_SpellScript::HandleAfterHit);
            }

            bool _executed;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_blood_boil_SpellScript();
        }
};

// 50365, 50371 Improved Blood Presence
class spell_dk_improved_blood_presence : public SpellScriptLoader
{
public:
    spell_dk_improved_blood_presence() : SpellScriptLoader("spell_dk_improved_blood_presence") { }

    class spell_dk_improved_blood_presence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_improved_blood_presence_AuraScript);

        bool Validate(SpellInfo const* /*entry*/)
        {
            if (!sSpellMgr->GetSpellInfo(DK_SPELL_BLOOD_PRESENCE) || !sSpellMgr->GetSpellInfo(DK_SPELL_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (!target->HasAura(DK_SPELL_BLOOD_PRESENCE) && !target->HasAura(DK_SPELL_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
            {
                int32 basePoints1 = aurEff->GetAmount();
                target->CastCustomSpell(target, DK_SPELL_IMPROVED_BLOOD_PRESENCE_TRIGGERED, NULL, &basePoints1, NULL, true, 0, aurEff);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (!target->HasAura(DK_SPELL_BLOOD_PRESENCE))
                target->RemoveAura(DK_SPELL_IMPROVED_BLOOD_PRESENCE_TRIGGERED);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_improved_blood_presence_AuraScript();
    }
};

// 50391, 50392 Improved Unholy Presence
class spell_dk_improved_unholy_presence : public SpellScriptLoader
{
public:
    spell_dk_improved_unholy_presence() : SpellScriptLoader("spell_dk_improved_unholy_presence") { }

    class spell_dk_improved_unholy_presence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_improved_unholy_presence_AuraScript);

        bool Validate(SpellInfo const* /*entry*/)
        {
            if (!sSpellMgr->GetSpellInfo(DK_SPELL_UNHOLY_PRESENCE) || !sSpellMgr->GetSpellInfo(DK_SPELL_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (target->HasAura(DK_SPELL_UNHOLY_PRESENCE) && !target->HasAura(DK_SPELL_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
            {
                // Not listed as any effect, only base points set in dbc
                int32 basePoints0 = aurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                target->CastCustomSpell(target, DK_SPELL_IMPROVED_UNHOLY_PRESENCE_TRIGGERED, &basePoints0, &basePoints0, &basePoints0, true, 0, aurEff);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAura(DK_SPELL_IMPROVED_UNHOLY_PRESENCE_TRIGGERED);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_improved_unholy_presence_AuraScript();
    }
};

class spell_dk_death_grip : public SpellScriptLoader
{
    public:
        spell_dk_death_grip() : SpellScriptLoader("spell_dk_death_grip") { }

        class spell_dk_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_grip_SpellScript);
            const SpellInfo *info;
            bool Load()
            {
                info = sSpellMgr->GetSpellInfo(49575);
                return true;
            }
            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Unit* target = GetHitUnit();

                Position pos;
                if (target == GetSpell()->GetUnitTarget()) 
                    pos = *GetExplTargetDest();
                else
                {
                    Unit * unitTarget = GetSpell()->GetUnitTarget();
                    unitTarget->GetContactPoint(target, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                    float angle = unitTarget->GetRelativeAngle(target);
                    unitTarget->GetFirstCollisionPosition(pos, unitTarget->GetObjectSize(), angle);
                }
                if (target)
                {
                    if (!target->HasAuraType(SPELL_AURA_DEFLECT_SPELLS) && !target->IsImmunedToSpell(info)) // Deterrence
                        target->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), damage, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_grip_SpellScript();
        }
};

void AddSC_deathknight_spell_scripts()
{
    new spell_dk_anti_magic_shell_raid();
    new spell_dk_anti_magic_shell_self();
    new spell_dk_anti_magic_zone();
    new spell_dk_death_gate();
    new spell_dk_death_pact();
    new spell_dk_scourge_strike();
    new spell_dk_blood_boil();
    new spell_dk_improved_blood_presence();
    new spell_dk_improved_unholy_presence();
    new spell_dk_death_grip();
}

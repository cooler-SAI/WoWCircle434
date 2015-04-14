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
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

enum MageSpells
{
    SPELL_MAGE_COLD_SNAP                         = 11958,
    SPELL_MAGE_SQUIRREL_FORM                     = 32813,
    SPELL_MAGE_GIRAFFE_FORM                      = 32816,
    SPELL_MAGE_SERPENT_FORM                      = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                   = 32818,
    SPELL_MAGE_WORGEN_FORM                       = 32819,
    SPELL_MAGE_SHEEP_FORM                        = 32820,
    SPELL_MAGE_GLYPH_OF_BLAST_WAVE               = 62126,
    SPELL_MAGE_FROSTBOLT                         = 116,
    SPELL_MAGE_TIME_WARP                         = 80353,
    SPELL_MAGE_TEMPORAL_DISPLACEMENT             = 80354,
    SPELL_MAGE_HYPOTERMIA                        = 41425,
    SPELL_MAGE_FROST_NOVA                        = 122,
    SPELL_MAGE_GLYPH_OF_ICE_BLOCK                = 56372
};

enum OtherSpells
{
    SHAMAN_SPELL_SATED                           = 57724,
    SHAMAN_SPELL_EXHAUSTION                      = 57723,
    HUNTER_SPELL_INSANITY                        = 95809
};

class spell_mage_cold_snap : public SpellScriptLoader
{
    public:
        spell_mage_cold_snap() : SpellScriptLoader("spell_mage_cold_snap") { }

        class spell_mage_cold_snap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_cold_snap_SpellScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();

                caster->RemoveSpellCooldown(120, true, true); // Cone of Cold
                caster->RemoveSpellCooldown(122, true, true); // Frost Nova
                caster->RemoveSpellCooldown(11426, true, true); // Ice Barrier
                caster->RemoveSpellCooldown(12472, true, true); // Icy Veins
                caster->RemoveSpellCooldown(31687, true, true); // Summon Water Elemental
                caster->RemoveSpellCooldown(44572, true, true); // Deep Freeze
                caster->RemoveSpellCooldown(45438, true, true); // Ice Block
                caster->RemoveSpellCooldown(82676, true, true); // Ring of Frost

                // Frostfire orb cooldown
                if (caster->GetAuraEffect(SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, SPELLFAMILY_MAGE, 4650, 1))
                {
                    caster->RemoveSpellCooldown(92283, true, true);
                }

                caster->SendClearCooldownMap(caster);
            }

            void Register()
            {
                // add dummy effect spell handler to Cold Snap
                OnEffectHit += SpellEffectFn(spell_mage_cold_snap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mage_cold_snap_SpellScript();
        }
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA   = 18744,
};

// TODO: move out of here and rename - not a mage spell
class spell_mage_polymorph_cast_visual : public SpellScriptLoader
{
    public:
        spell_mage_polymorph_cast_visual() : SpellScriptLoader("spell_mage_polymorph_visual") { }

        class spell_mage_polymorph_cast_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_polymorph_cast_visual_SpellScript);

            static const uint32 PolymorhForms[6];

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                // check if spell ids exist in dbc
                for (uint32 i = 0; i < 6; i++)
                    if (!sSpellMgr->GetSpellInfo(PolymorhForms[i]))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
            }

            void Register()
            {
                // add dummy effect spell handler to Polymorph visual
                OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mage_polymorph_cast_visual_SpellScript();
        }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual_SpellScript::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

class spell_mage_incanters_absorbtion_base_AuraScript : public AuraScript
{
    public:
        enum Spells
        {
            SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED = 44413,
            SPELL_MAGE_INCANTERS_ABSORBTION_R1 = 44394,
        };

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return sSpellMgr->GetSpellInfo(SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED)
                && sSpellMgr->GetSpellInfo(SPELL_MAGE_INCANTERS_ABSORBTION_R1);
        }

        void Trigger(AuraEffect* aurEff, DamageInfo & /*dmgInfo*/, uint32 & absorbAmount)
        {
            Unit* target = GetTarget();

            if (AuraEffect const* incantersAbsorption = target->GetDummyAuraEffect(SPELLFAMILY_GENERIC, 2941, EFFECT_0))
            {
                int32 bp = CalculatePct(absorbAmount, incantersAbsorption->GetAmount());
                target->CastCustomSpell(target, SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
            }
        }
};

// Incanter's Absorption
class spell_mage_incanters_absorbtion_absorb : public SpellScriptLoader
{
public:
    spell_mage_incanters_absorbtion_absorb() : SpellScriptLoader("spell_mage_incanters_absorbtion_absorb") { }

    class spell_mage_incanters_absorbtion_absorb_AuraScript : public spell_mage_incanters_absorbtion_base_AuraScript
    {
        PrepareAuraScript(spell_mage_incanters_absorbtion_absorb_AuraScript);

        void Register()
        {
             AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_incanters_absorbtion_absorb_AuraScript::Trigger, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_mage_incanters_absorbtion_absorb_AuraScript();
    }
};

// Incanter's Absorption
class spell_mage_incanters_absorbtion_manashield : public SpellScriptLoader
{
public:
    spell_mage_incanters_absorbtion_manashield() : SpellScriptLoader("spell_mage_incanters_absorbtion_manashield") { }

    class spell_mage_incanters_absorbtion_manashield_AuraScript : public spell_mage_incanters_absorbtion_base_AuraScript
    {
        PrepareAuraScript(spell_mage_incanters_absorbtion_manashield_AuraScript);

        void Register()
        {
             AfterEffectManaShield += AuraEffectManaShieldFn(spell_mage_incanters_absorbtion_manashield_AuraScript::Trigger, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_mage_incanters_absorbtion_manashield_AuraScript();
    }
};

class spell_mage_living_bomb : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb() : SpellScriptLoader("spell_mage_living_bomb") { }

        class spell_mage_living_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_living_bomb_AuraScript);

            bool Validate(SpellInfo const* spell)
            {
                if (!sSpellMgr->GetSpellInfo(uint32(spell->Effects[EFFECT_1].CalcValue())))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if (removeMode != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetTarget(), uint32(aurEff->GetAmount()), true, NULL, aurEff);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mage_living_bomb_AuraScript();
        }
};

class spell_mage_frostbolt: public SpellScriptLoader
{
public:
    spell_mage_frostbolt(): SpellScriptLoader("spell_mage_frostbolt"){}

    class spell_mage_frostbolt_SpellScript: public SpellScript
    {
        enum Spells
        {

        };

        PrepareSpellScript(spell_mage_frostbolt_SpellScript);

        bool Validate(const SpellInfo* /*spell*/)
        {
            return true;
        }
        void HandleOnHit()
        {
            // Early Frost
            if (Player * player = dynamic_cast<Player*>(GetCaster()))
            {
                if (AuraEffect* aurEff = player->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_MAGE, 189, 0))
                {
                    uint32 spellId = 0;
                    switch (aurEff->GetId())
                    {
                        case 83049:
                            spellId = 83162;
                            break;
                        case 83050:
                            spellId = 83239;
                            break;
                    }
                    Spell* spell = GetSpell();
                    if(spellId && !player->HasAura(spellId) && !spell->isearlyfrostaffected)
                    {
                        player->CastSpell(player, spellId, true);
                        player->RemoveAurasDueToSpell(94315);
                    }
                }
            }
        }
        void Register()
        {
            OnCast += SpellCastFn(spell_mage_frostbolt_SpellScript::HandleOnHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_mage_frostbolt_SpellScript();
    }
};

// Cauterize
class spell_mage_cauterize : public SpellScriptLoader
{
public:
    spell_mage_cauterize() : SpellScriptLoader("spell_mage_cauterize") { }

    class spell_mage_cauterize_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_cauterize_AuraScript);

        uint32 absorbChance;
        uint32 healtPct;

        enum Spell
        {
            SPELL_MAGE_CAUTERIZE = 87023
        };

        bool Load()
        {
            absorbChance = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
            healtPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
            return GetUnitOwner()->ToPlayer();
        }

        void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            amount = -1;
        }

        void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
        {
            Unit* target = GetTarget();

            if (dmgInfo.GetDamage() < target->GetHealth())
                return;

            if (target->ToPlayer()->HasSpellCooldown(SPELL_MAGE_CAUTERIZE))
                return;

            if (!roll_chance_i(absorbChance))
                return;

            int bp1 = target->CountPctFromMaxHealth(healtPct);
            target->CastCustomSpell(target, SPELL_MAGE_CAUTERIZE, NULL, &bp1, NULL, true);
            target->ToPlayer()->AddSpellCooldown(SPELL_MAGE_CAUTERIZE, 0, time(NULL) + 60);

            absorbAmount = dmgInfo.GetDamage();
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_cauterize_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize_AuraScript::Absorb, EFFECT_0);
        }
    };

    AuraScript *GetAuraScript() const
    {
        return new spell_mage_cauterize_AuraScript();
    }
};

/// Updated 4.3.4
class spell_mage_time_warp : public SpellScriptLoader
{
public:
    spell_mage_time_warp() : SpellScriptLoader("spell_mage_time_warp") { }

    class spell_mage_time_warp_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_time_warp_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_MAGE_TIME_WARP))
                return false;

            return true;
        }

        void RemoveInvalidTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(Trinity::UnitAuraCheck(true, SHAMAN_SPELL_SATED));
            targets.remove_if(Trinity::UnitAuraCheck(true, SHAMAN_SPELL_EXHAUSTION));
            targets.remove_if(Trinity::UnitAuraCheck(true, HUNTER_SPELL_INSANITY));
            targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
        }

        void ApplyDebuff()
        {
            if (Unit* target = GetHitUnit())
                target->CastSpell(target, SPELL_MAGE_TEMPORAL_DISPLACEMENT, true);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
            AfterHit += SpellHitFn(spell_mage_time_warp_SpellScript::ApplyDebuff);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_mage_time_warp_SpellScript();
    }
};

// 12355 - Impact
class spell_mage_impact : public SpellScriptLoader
{
    public:
        spell_mage_impact() : SpellScriptLoader("spell_mage_impact") { }

        class spell_mage_impact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_impact_SpellScript);

            bool Load()
            {
                bombs = 0;
                impactAuras.clear();
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();
                if (!target || !caster)
                    return;

                targets.remove_if(ImpactTargetCheck(target));

                std::list<AuraEffect*> aurasPeriodic = target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
                aurasPeriodic.remove_if(ImpactAuraCheck(caster->GetGUID()));
                if (aurasPeriodic.empty())
                    return;

                for (std::list<AuraEffect*>::const_iterator itr = aurasPeriodic.begin(); itr != aurasPeriodic.end(); ++itr)
                {
                    switch ((*itr)->GetId())
                    {
                        case 44457: // Living Bomb
                        case 83853: // Combustion
                        case 11366: // Pyroblast
                        case 92315: // Pyroblast!
                        case 12654: // Ignite
                        {
                            impactAuraData aura;
                            aura.spellId = (*itr)->GetId();
                            aura.effMask = (*itr)->GetBase()->GetEffectMask();
                            aura.effIndex = uint8((*itr)->GetEffIndex());
                            aura.damage = (*itr)->GetFixedDamageInfo().GetFixedDamage();
                            aura.duration = (*itr)->GetBase()->GetDuration();
                            impactAuras.push_back(aura); 
                            break;
                        }
                        default:
                            break;
                    }

                    // temporary hack
                    if ((*itr)->GetId() == 44457)
                    {
                        (*itr)->GetBase()->RefreshDuration();
                        (*itr)->RecalculateAmount();
                    }
                }
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();
                Unit* newTarget = GetHitUnit();
                if (!caster || !target)
                    return;

                if (!newTarget || newTarget->GetGUID() == target->GetGUID())
                    return;

                if (impactAuras.empty())
                    return;

                for (std::vector<impactAuraData>::const_iterator itr = impactAuras.begin(); itr != impactAuras.end(); ++itr)
                {
                    if ((*itr).spellId == 44457)
                        if (bombs >= 2)
                            continue;

                    if (Aura* aur = caster->AddAura(sSpellMgr->GetSpellInfo((*itr).spellId), (*itr).effMask, newTarget))
                    {
                        if ((*itr).spellId == 44457)
                            bombs++;

                        aur->SetMaxDuration((*itr).duration);
                        aur->SetDuration((*itr).duration);
                        if (AuraEffect* aurEff = aur->GetEffect((*itr).effIndex))
                            aurEff->GetFixedDamageInfo().SetFixedDamage((*itr).damage);

                    }
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_impact_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_mage_impact_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        private:

            struct impactAuraData
            {
                uint32 spellId;
                int32 damage;
                uint32 duration;
                uint8 effMask;
                uint8 effIndex;
            };
            std::vector<impactAuraData> impactAuras;
            uint8 bombs;

            class ImpactTargetCheck
            {
                public:
                    ImpactTargetCheck(Unit* originalTarget) : _originalTarget(originalTarget) { }

                    bool operator() (WorldObject* unit)
                    {
                        
                        return ((unit->GetGUID() == _originalTarget->GetGUID()) || (unit->ToUnit() && !unit->ToUnit()->isInCombat()));
                    }

                private:
                    Unit* _originalTarget;
            };

            class ImpactAuraCheck
            {
                public:
                    ImpactAuraCheck(uint64 guid) : _guid(guid) { }

                    bool operator() (AuraEffect* effect)
                    {
                        if (!(effect->GetSpellInfo()->SchoolMask & SPELL_SCHOOL_MASK_FIRE))
                            return true;

                        return _guid != effect->GetCasterGUID();
                    }

                private:
                    uint64 _guid;
            };
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mage_impact_SpellScript();
        }
};

class spell_mage_ice_block : public SpellScriptLoader
{
public:
    spell_mage_ice_block() : SpellScriptLoader("spell_mage_ice_block") { }

    class spell_mage_ice_block_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_ice_block_SpellScript);

        bool Load()
        {
            return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void ApplyAura(SpellEffIndex /*effIndex*/)
        {
            Player* caster = GetCaster()->ToPlayer();

            caster->CastSpell(caster, SPELL_MAGE_HYPOTERMIA, true);

            if (caster->HasAura(SPELL_MAGE_GLYPH_OF_ICE_BLOCK))
                caster->RemoveSpellCooldown(SPELL_MAGE_FROST_NOVA, true);
        }

        void Register()
        {
            // add dummy effect spell handler to Cold Snap
            OnEffectHit += SpellEffectFn(spell_mage_ice_block_SpellScript::ApplyAura, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_mage_ice_block_SpellScript();
    }
};


void AddSC_mage_spell_scripts()
{
    new spell_mage_cold_snap();
    new spell_mage_incanters_absorbtion_absorb();
    new spell_mage_incanters_absorbtion_manashield();
    new spell_mage_polymorph_cast_visual();
    new spell_mage_living_bomb();
    new spell_mage_frostbolt();
    new spell_mage_cauterize();
    new spell_mage_time_warp();
    new spell_mage_impact();
    new spell_mage_ice_block();
}

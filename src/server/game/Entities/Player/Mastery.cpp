
#include "Player.h"

void Player::ApplyMasterySpells()
{
    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            if (uint32 spellId = talentTabEntry->spellIds[i])
            {
                if (!HasSpell(spellId))
                    learnSpell(spellId, true);
            }

            UpdateMastery();
        }
    }
}

void Player::RemoveMasterySpells()
{
    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            if (uint32 spell = talentTabEntry->spellIds[i])
                removeSpell(spell);
        }
    }
}

const uint32 ClassWithPetsMask = (1 << (CLASS_HUNTER - 1)) | (1 << (CLASS_DEATH_KNIGHT - 1)) | (1 << (CLASS_MAGE - 1)) | (1 << (CLASS_WARLOCK - 1));

bool Player::MasteryAffectsPet() const
{
    uint32 classMask = getClassMask();
    return (classMask & ClassWithPetsMask) > 0;
}

uint8 Player::GetMasteryScalingValue(SpellInfo const* spellInfo, int32& amount) const
{
    if (!spellInfo || !spellInfo->HasAttribute(SPELL_ATTR8_MASTERY))
        return 0;
    
    amount = 0;

    switch (spellInfo->Id)
    {
        case 77215: // Mastery: Potent Afflictions, spell has no amount;
            amount = 163;
            break;
        case 76808:
            amount = 250; // Mastery: Executioner, spell has no amount;
            break;
        case 76658: // Mastery: Essence of the Viper, amount is second value, but spell has three effects
            amount = 100;
            break;
    }

    if (amount != 0)
        return MAX_SPELL_EFFECTS;

    // Value stored in first spell effect from end wich have dummy aura
    for (uint8 i = MAX_SPELL_EFFECTS-1; i < MAX_SPELL_EFFECTS; --i)
    {
        if (!spellInfo->Effects[i].Effect)
            continue;

        amount = spellInfo->Effects[i].BasePoints;
        return i;
    }

    return 0;
}

void Player::UpdateMastery()
{
    if (!HasMastery())
        return;

    float masteryPoints = GetMasteryPoints();
    SetFloatValue(PLAYER_MASTERY, GetMasteryPoints());

    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            uint32 spellId = talentTabEntry->spellIds[i];
            if (!spellId)
                continue;

            if (!HasAura(spellId))
                CastSpell(this, spellId, true);

            if (Aura* aura = GetAura(spellId))
            {
                int32 amount = 0;
                uint8 effectIndex = GetMasteryScalingValue(aura->GetSpellInfo(), amount);
                {
                    amount *= masteryPoints;
                    amount /= 100.0f;
                }

                for (uint8 j = 0; j < effectIndex; ++j)
                {
                    if (AuraEffect* eff = aura->GetEffect(j))
                    {
                        eff->SetCanBeRecalculated(true);
                        eff->ChangeAmount(amount, false, true);
                    }
                }

                UpdateMasteryDependentBuffs(spellId);
            }
        }
    }
}

float Player::GetMasteryPoints()
{
    return GetRatingBonusValue(CR_MASTERY) + float(GetTotalAuraModifier(SPELL_AURA_MASTERY));
}

void Player::UpdateMasteryDependentBuffs(uint32 spellId)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        uint32 applyAuraName = spellInfo->Effects[i].ApplyAuraName;
        if (applyAuraName != SPELL_AURA_ADD_FLAT_MODIFIER && applyAuraName != SPELL_AURA_ADD_PCT_MODIFIER)
            continue;

        if (AuraEffect* masteryBuff = GetAuraEffect(spellId, i))
        {
            Unit::VisibleAuraMap const *visibleAuras = GetVisibleAuras();
            for (Unit::VisibleAuraMap::const_iterator itr = visibleAuras->begin(); itr != visibleAuras->end(); ++itr)
            {
                Aura* aura = itr->second->GetBase();
                if (masteryBuff->IsAffectingSpell(aura->GetSpellInfo()))
                    aura->RecalculateAmountOfEffects();
            }
        }

        if (MasteryAffectsPet())
        {
            Pet* pet = GetPet();
            if (!pet || !pet->GetOwner())
                continue;

            if (AuraEffect* masteryBuff = pet->GetOwner()->GetAuraEffect(spellId, i))
            {
                Unit::AuraApplicationMap const& uAuras = pet->GetAppliedAuras();
                for (Unit::AuraApplicationMap::const_iterator itr = uAuras.begin(); itr != uAuras.end(); ++itr)
                {
                    Aura* aura = itr->second->GetBase();
                    if (masteryBuff->IsAffectingSpell(aura->GetSpellInfo()))
                        aura->RecalculateAmountOfEffects();
                }
            }
        }
    }
}

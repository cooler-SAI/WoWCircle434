#include "gamePCH.h"
#include "Player.h"

void Player::ApplyMasterySpells()
{
    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            if (uint32 spell = talentTabEntry->spellIds[i])
                learnSpell(spell, true);
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
    if ((classMask & ClassWithPetsMask) == 0)
        return false;

    if (!GetPet())
        return false;

    return true;
}

void Player::UpdateMastery()
{
    if (!HasMastery())
        return;

    float masteryPoints = GetRatingBonusValue(CR_MASTERY);
    masteryPoints += float(GetTotalAuraModifier(SPELL_AURA_MASTERY));

    SetFloatValue(PLAYER_MASTERY, masteryPoints);
}
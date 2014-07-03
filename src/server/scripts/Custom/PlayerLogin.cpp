/*
*
* Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
*
*/

#include "ScriptPCH.h"
#include "SpellAuraEffects.h"

#define SPELL_KEYS_TO_THE_HOT_ROD    91551
#define Gilneas2                     654
#define Gilneas                      638
#define GilneasPhase1                655
#define GilneasPhase2                656

class PlayerLoginScript : public PlayerScript
{
public:
    PlayerLoginScript() : PlayerScript("PlayerLoginScript") { }

    void OnEndWatching(Player* player)
    {
        if (player->HasAura(91847))
        {
            //player->CastSpell(player, 74100, false);

            // this is big hack - must teleport player to Lost Islands, but zone not scripted yet
            WorldLocation loc;
            loc.m_mapId = 1;
            loc.m_positionX = 1440.89f;
            loc.m_positionY = -5021.77f;
            loc.m_positionZ = 12.0647f;
            player->RemoveAllAuras();
            player->SetHomebind(loc, 374);
            player->TeleportTo(loc);
        }
    }

    void OnLogin(Player* player)
    {
        if (player->GetMapId() == Gilneas2 && player->GetPhaseMask() == 1)
        {
            std::set<uint32> terrainswap;
            std::set<uint32> phaseId;
            terrainswap.insert(Gilneas);
            phaseId.insert(2);

            player->ToPlayer()->GetSession()->SendSetPhaseShift(phaseId, terrainswap);
        }

        if (player->HasAura(SPELL_KEYS_TO_THE_HOT_ROD))
        {
            player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
            player->CastSpell(player, SPELL_KEYS_TO_THE_HOT_ROD, false);
        }

        uint32 uiSpellId = 0;
        Unit::AuraEffectList const& phases = player->GetAuraEffectsByType(SPELL_AURA_PHASE);
        if (!phases.empty())
            for (Unit::AuraEffectList::const_iterator itr = phases.begin(); itr != phases.end(); ++itr)
                uiSpellId = (*itr)->GetSpellInfo()->Id;

        if (uiSpellId)
        {
            player->RemoveAura(uiSpellId);
            player->CastSpell(player, uiSpellId, false);
        }

        uiSpellId = 0;
        Unit::AuraEffectList const& overrided = player->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SPELLS);
        if (!overrided.empty())
            for (Unit::AuraEffectList::const_iterator itr = overrided.begin(); itr != overrided.end(); ++itr)
                uiSpellId = (*itr)->GetSpellInfo()->Id;

        if (uiSpellId)
        {
            player->RemoveAura(uiSpellId);
            player->CastSpell(player, uiSpellId, false);
        }
    }
};

void AddSC_player_login()
{
    new  PlayerLoginScript();
}
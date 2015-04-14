/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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


#include "Common.h"
#include "DatabaseEnv.h"
#include "Opcodes.h"
#include "Log.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ObjectAccessor.h"
#include "UpdateMask.h"

void WorldSession::HandleLearnTalentOpcode(WorldPacket & recvData)
{
    uint32 talentId, requestedRank;
    recvData >> talentId >> requestedRank;

    if (_player->LearnTalent(talentId, requestedRank))
        _player->SendTalentsInfoData(false);
}

void WorldSession::HandleLearnPreviewTalents(WorldPacket& recvPacket)
{
    int32 tabPage;
    uint32 talentsCount;
    recvPacket >> tabPage;    // talent tree

    // prevent cheating (selecting new tree with points already in another)
    if (tabPage >= 0)   // -1 if player already has specialization
    {
        if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(_player->GetPrimaryTalentTree(_player->GetActiveSpec())))
        {
            if (talentTabEntry->tabpage != uint32(tabPage))
            {
                recvPacket.rfinish();
                return;
            }
        }
    }

    recvPacket >> talentsCount;

    uint32 talentId, talentRank;

    for (uint32 i = 0; i < talentsCount; ++i)
    {
        recvPacket >> talentId >> talentRank;

        if (!_player->LearnTalent(talentId, talentRank))
        {
            recvPacket.rfinish();
            break;
        }
    }

    _player->SendTalentsInfoData(false);

    if (_player->HasMastery())
        _player->ApplyMasterySpells();
}

void WorldSession::HandleTalentWipeConfirmOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // prevent reset talents if player has immunity auras (possible exploits)
    if (GetPlayer()->HasAuraType(SPELL_AURA_SCHOOL_IMMUNITY))
        return;

    if (!_player->ResetTalents())
    {
        WorldPacket data(MSG_TALENT_WIPE_CONFIRM, 8+4);    //you have not any talent
        data << uint64(0);
        data << uint32(0);
        SendPacket(&data);
        return;
    }

    _player->SendTalentsInfoData(false);
    unit->CastSpell(_player, 14867, true);                  //spell: "Untalent Visual Effect"
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPacket& recvData)
{
    uint32 skillId;
    recvData >> skillId;

    if (!IsProfessionSkill(skillId))
        return;

    GetPlayer()->SetSkill(skillId, 0, 0, 0);
}

void WorldSession::HandleArcheologyRequestHistory(WorldPacket& recvPacket)
{

    uint32 count = GetPlayer()->GetArchaeologyMgr().GetCompletedProjects().size();
    if (count <= 0)
        return;

    WorldPacket data(SMSG_RESEARCH_SETUP_HISTORY);
    data.WriteBits(count,22);

    for (CompletedProjectMap::const_iterator itr = GetPlayer()->GetArchaeologyMgr().GetCompletedProjects().begin(); itr != GetPlayer()->GetArchaeologyMgr().GetCompletedProjects().end(); ++itr)
    {
        data << uint32(itr->second.projectId); //Project Id
        data << uint32(itr->second.count);   //Count
        data << uint32(itr->second.TimeCreated);   //Time create
    }

    SendPacket(&data);
}

void WorldSession::HandleSetPrimaryTree(WorldPacket& recvPacket)
{
    uint32 talentTabId;

    recvPacket >> talentTabId;
}
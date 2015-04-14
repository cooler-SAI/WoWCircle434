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
#include "ObjectMgr.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "WorldPacket.h"

#include "ArenaTeam.h"
#include "BattlegroundMgr.h"
#include "BattlegroundAV.h"
#include "BattlegroundAB.h"
#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "BattlegroundNA.h"
#include "BattlegroundBE.h"
#include "BattlegroundRL.h"
#include "BattlegroundSA.h"
#include "BattlegroundDS.h"
#include "BattlegroundRV.h"
#include "BattlegroundIC.h"
#include "BattlegroundTP.h"
#include "BattlegroundBFG.h"
#include "Chat.h"
#include "Map.h"
#include "MapInstanced.h"
#include "MapManager.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "SharedDefines.h"
#include "Formulas.h"
#include "DisableMgr.h"

/*********************************************************/
/***            BATTLEGROUND MANAGER                   ***/
/*********************************************************/

BattlegroundMgr::BattlegroundMgr() : m_NextRatedArenaUpdate(sWorld->getIntConfig(CONFIG_ARENA_RATED_UPDATE_TIMER)), m_ArenaTesting(false), m_Testing(false)
{ }

BattlegroundMgr::~BattlegroundMgr()
{
    DeleteAllBattlegrounds();
}

void BattlegroundMgr::DeleteAllBattlegrounds()
{
    for (BattlegroundDataContainer::iterator itr1 = bgDataStore.begin(); itr1 != bgDataStore.end(); ++itr1)
    {
        BattlegroundData& data = itr1->second;

        while (!data.m_Battlegrounds.empty())
            delete data.m_Battlegrounds.begin()->second;
        data.m_Battlegrounds.clear();

        while (!data.BGFreeSlotQueue.empty())
            delete data.BGFreeSlotQueue.front();
    }

    bgDataStore.clear();
}

// used to update running battlegrounds, and delete finished ones
void BattlegroundMgr::Update(uint32 diff)
{
    for (BattlegroundDataContainer::iterator itr1 = bgDataStore.begin(); itr1 != bgDataStore.end(); ++itr1)
    {
        BattlegroundContainer& bgs = itr1->second.m_Battlegrounds;
        if (!bgs.size())
            continue;
        BattlegroundContainer::iterator itrDelete = bgs.begin();
        // first one is template and should not be deleted
        for (BattlegroundContainer::iterator itr = ++itrDelete; itr != bgs.end();)
        {
            itrDelete = itr++;
            Battleground* bg = itrDelete->second;

            bg->Update(diff);
            if (bg->ToBeDeleted())
            {
                itrDelete->second = NULL;
                bgs.erase(itrDelete);
                BattlegroundClientIdsContainer& clients = itr1->second.m_ClientBattlegroundIds[bg->GetBracketId()];
                if (!clients.empty())
                    clients.erase(bg->GetClientInstanceID());

                delete bg;
            }
        }
    }

    // update events timer
    for (int qtype = BATTLEGROUND_QUEUE_NONE; qtype < MAX_BATTLEGROUND_QUEUE_TYPES; ++qtype)
        m_BattlegroundQueues[qtype].UpdateEvents(diff);

    // update scheduled queues
    if (!m_QueueUpdateScheduler.empty())
    {
        std::vector<uint64> scheduled;
        std::swap(scheduled, m_QueueUpdateScheduler);

        for (uint8 i = 0; i < scheduled.size(); i++)
        {
            uint32 arenaMMRating = scheduled[i] >> 32;
            uint8 arenaType = scheduled[i] >> 24 & 255;
            BattlegroundQueueTypeId bgQueueTypeId = BattlegroundQueueTypeId(scheduled[i] >> 16 & 255);
            BattlegroundTypeId bgTypeId = BattlegroundTypeId((scheduled[i] >> 8) & 255);
            BattlegroundBracketId bracket_id = BattlegroundBracketId(scheduled[i] & 255);
            m_BattlegroundQueues[bgQueueTypeId].BattlegroundQueueUpdate(diff, bgTypeId, bracket_id, arenaType, arenaMMRating > 0, arenaMMRating);
        }
    }

    // if rating difference counts, maybe force-update queues
    if (sWorld->getIntConfig(CONFIG_ARENA_MAX_RATING_DIFFERENCE) && sWorld->getIntConfig(CONFIG_ARENA_RATED_UPDATE_TIMER))
    {
        // it's time to force update
        if (m_NextRatedArenaUpdate < diff)
        {
            // forced update for rated arenas (scan all, but skipped non rated)
            sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BattlegroundMgr: UPDATING ARENA QUEUES");
            for (int qtype = BATTLEGROUND_QUEUE_2v2; qtype <= BATTLEGROUND_QUEUE_5v5; ++qtype)
                for (int bracket = BG_BRACKET_ID_FIRST; bracket < MAX_BATTLEGROUND_BRACKETS; ++bracket)
                    m_BattlegroundQueues[qtype].BattlegroundQueueUpdate(diff, BATTLEGROUND_AA, BattlegroundBracketId(bracket), BattlegroundMgr::BGArenaType(BattlegroundQueueTypeId(qtype)), true, 0);

            m_NextRatedArenaUpdate = sWorld->getIntConfig(CONFIG_ARENA_RATED_UPDATE_TIMER);
        }
        else
            m_NextRatedArenaUpdate -= diff;
    }
}

void BattlegroundMgr::BuildBattlegroundStatusPacket(WorldPacket *data, Battleground *bg, Player* player, uint8 QueueSlot, uint8 StatusID, uint32 Time1, uint32 Time2, uint8 arenatype, uint8 uiFrame)
{
    ObjectGuid playerGuid = player->GetGUID();	
    ObjectGuid bgGuid;

    if (bg)
        bgGuid = bg->GetGUID();	
    else	
        StatusID = STATUS_NONE;

    switch (StatusID)
    {
    case STATUS_NONE:
        {
            data->Initialize(SMSG_BATTLEFIELD_STATUS, 1 + 8 + 12);

            data->WriteByteMask(playerGuid[0]);
            data->WriteByteMask(playerGuid[4]);
            data->WriteByteMask(playerGuid[7]);
            data->WriteByteMask(playerGuid[1]);
            data->WriteByteMask(playerGuid[6]);
            data->WriteByteMask(playerGuid[3]);
            data->WriteByteMask(playerGuid[5]);
            data->WriteByteMask(playerGuid[2]);

            data->WriteByteSeq(playerGuid[5]);
            data->WriteByteSeq(playerGuid[6]);
            data->WriteByteSeq(playerGuid[7]);
            data->WriteByteSeq(playerGuid[2]);
            *data << uint32(1);                         // unk, always 1
            data->WriteByteSeq(playerGuid[3]);
            data->WriteByteSeq(playerGuid[1]);
            *data << uint32(QueueSlot);                 // Queue slot
            *data << uint32(Time1);                     // Join Time
            data->WriteByteSeq(playerGuid[0]);
            data->WriteByteSeq(playerGuid[4]);
            break;
        }
    case STATUS_WAIT_QUEUE:
        {
            data->Initialize(SMSG_BATTLEFIELD_STATUS_QUEUED, 3 + 16 + 24 + 3);

            data->WriteByteMask(playerGuid[3]);
            data->WriteByteMask(playerGuid[0]);
            data->WriteByteMask(bgGuid[3]);
            data->WriteByteMask(playerGuid[2]);
            data->WriteBit(true);                                  // Eligible In Queue
            data->WriteBit(false);                                  // Join Failed
            data->WriteByteMask(bgGuid[2]);
            data->WriteByteMask(playerGuid[1]);
            data->WriteByteMask(bgGuid[0]);
            data->WriteByteMask(bgGuid[6]);
            data->WriteByteMask(bgGuid[4]);
            data->WriteByteMask(playerGuid[6]);
            data->WriteByteMask(playerGuid[7]);
            data->WriteByteMask(bgGuid[7]);
            data->WriteByteMask(bgGuid[5]);
            data->WriteByteMask(playerGuid[4]);
            data->WriteByteMask(playerGuid[5]);
            data->WriteBit(bg->isRated());                      // Is Rated
            data->WriteBit(false);                                  // Waiting On Other Activity
            data->WriteByteMask(bgGuid[1]);

            data->WriteByteSeq(playerGuid[0]);
            *data << uint32(bg->isArena() ? arenatype : (bg->GetTypeID() == BATTLEGROUND_RATED_10_VS_10) ? 10 : 1); // Player count, 1 for bgs, 2-3-5 for arena (2v2, 3v3, 5v5)
            data->WriteByteSeq(bgGuid[5]);
            data->WriteByteSeq(playerGuid[3]);
            *data << uint32(Time1);                             // Estimated Wait Time
            data->WriteByteSeq(bgGuid[7]);
            data->WriteByteSeq(bgGuid[1]);
            data->WriteByteSeq(bgGuid[2]);
            *data << uint8(0);                                  // unk
            data->WriteByteSeq(bgGuid[4]);
            data->WriteByteSeq(playerGuid[2]);
            *data << uint8(bg->isArena() ? arenatype : 0);      // Player count on rated mode
            data->WriteByteSeq(bgGuid[6]);
            data->WriteByteSeq(playerGuid[7]);
            data->WriteByteSeq(bgGuid[3]);
            data->WriteByteSeq(playerGuid[6]);
            data->WriteByteSeq(bgGuid[0]);
            *data << uint32(Time2);                             // Join Time
            *data << uint32(QueueSlot);                         // Queue slot
            *data << uint8(bg->GetMinLevel());                  // Min Level
            *data << uint32(GetMSTimeDiffToNow(Time2));         // Time since joined
            data->WriteByteSeq(playerGuid[1]);
            data->WriteByteSeq(playerGuid[5]);
            *data << uint32(bg->GetClientInstanceID());         // Client Instance ID
            data->WriteByteSeq(playerGuid[4]);
            break;
        }
    case STATUS_WAIT_JOIN:
        {
            data->Initialize(SMSG_BATTLEFIELD_STATUS_NEEDCONFIRMATION, 16 + 3 + 24 + 3);

            *data << uint32(bg->GetClientInstanceID());         // Client Instance ID
            *data << uint32(Time1);                             // Time until closed
            *data << uint8(1);                                  // unk
            *data << uint32(QueueSlot);                         // Queue slot
            *data << uint32(Time2);                             // Join Time
            *data << uint8(bg->GetMinLevel());                  // Min Level
            *data << uint32(bg->isArena() ? arenatype : (bg->GetTypeID() == BATTLEGROUND_RATED_10_VS_10) ? 10 : 1); // Player count, 1 for bgs, 2-3-5 for arena (2v2, 3v3, 5v5)
            *data << uint32(bg->GetMapId());                    // Map Id
            *data << uint8((bg->GetTypeID() == BATTLEGROUND_RATED_10_VS_10) ? 10 : 0);                          // Player count on rated mode

            data->WriteByteMask(playerGuid[5]);
            data->WriteByteMask(playerGuid[2]);
            data->WriteByteMask(playerGuid[1]);
            data->WriteByteMask(bgGuid[2]);
            data->WriteByteMask(playerGuid[4]);
            data->WriteByteMask(bgGuid[6]);
            data->WriteByteMask(bgGuid[3]);
            data->WriteBit(bg->isRated());                      // Is Rated
            data->WriteByteMask(playerGuid[7]);
            data->WriteByteMask(playerGuid[3]);
            data->WriteByteMask(bgGuid[7]);
            data->WriteByteMask(bgGuid[0]);
            data->WriteByteMask(bgGuid[4]);
            data->WriteByteMask(playerGuid[6]);
            data->WriteByteMask(bgGuid[1]);
            data->WriteByteMask(bgGuid[5]);
            data->WriteByteMask(playerGuid[0]);

            data->WriteByteSeq(bgGuid[6]);
            data->WriteByteSeq(bgGuid[5]);
            data->WriteByteSeq(bgGuid[7]);
            data->WriteByteSeq(bgGuid[2]);
            data->WriteByteSeq(playerGuid[0]);
            data->WriteByteSeq(playerGuid[7]);
            data->WriteByteSeq(bgGuid[4]);
            data->WriteByteSeq(playerGuid[1]);
            data->WriteByteSeq(bgGuid[0]);
            data->WriteByteSeq(playerGuid[4]);
            data->WriteByteSeq(bgGuid[1]);
            data->WriteByteSeq(playerGuid[5]);
            data->WriteByteSeq(bgGuid[3]);
            data->WriteByteSeq(playerGuid[6]);
            data->WriteByteSeq(playerGuid[2]);
            data->WriteByteSeq(playerGuid[3]);
            break;
        }
    case STATUS_IN_PROGRESS:
        {
            data->Initialize(SMSG_BATTLEFIELD_STATUS_ACTIVE, 3 + 16 + 28 + 3);

            data->WriteByteMask(playerGuid[2]);
            data->WriteByteMask(playerGuid[7]);
            data->WriteByteMask(bgGuid[7]);
            data->WriteByteMask(bgGuid[1]);
            data->WriteByteMask(playerGuid[5]);
            data->WriteBit(bg->isArena() ? uiFrame : player->GetBGTeam() == ALLIANCE);    // Arena Frames on Arenas and Faction on Battlegrounds
            data->WriteByteMask(bgGuid[0]);
            data->WriteByteMask(playerGuid[1]);
            data->WriteByteMask(bgGuid[3]);
            data->WriteByteMask(playerGuid[6]);
            data->WriteByteMask(bgGuid[5]);
            data->WriteByteMask(bg->isRated());                                  // Is Rated
            data->WriteByteMask(playerGuid[4]);
            data->WriteByteMask(bgGuid[6]);
            data->WriteByteMask(bgGuid[4]);
            data->WriteByteMask(bgGuid[2]);
            data->WriteByteMask(playerGuid[3]);
            data->WriteByteMask(playerGuid[0]);

            data->WriteByteSeq(bgGuid[4]);
            data->WriteByteSeq(bgGuid[5]);
            data->WriteByteSeq(playerGuid[5]);
            data->WriteByteSeq(bgGuid[1]);
            data->WriteByteSeq(bgGuid[6]);
            data->WriteByteSeq(bgGuid[3]);
            data->WriteByteSeq(bgGuid[7]);
            data->WriteByteSeq(playerGuid[6]);

            *data << uint32(Time1);                                             // Join Time
            *data << uint8(arenatype);                                          // Teamsize (0 if not arena)

            data->WriteByteSeq(playerGuid[4]);
            data->WriteByteSeq(playerGuid[1]);

            *data << uint32(QueueSlot);                                         // Queue slot
            *data << uint8(bg->isArena() ? arenatype : 0);                      // Player count on rated mode
            *data << uint32(1);                                                 // unk
            *data << uint32(bg->GetMapId());                                    // Map Id
            *data << uint8(bg->GetMinLevel());                                  // Min Level
            *data << uint32(Time2);                                             // Elapsed Time

            data->WriteByteSeq(playerGuid[2]);
            *data << uint32(getMSTimeDiff(bg->GetRemainingTime(), Time2));      // Remaining Time

            data->WriteByteSeq(playerGuid[0]);
            data->WriteByteSeq(playerGuid[3]);
            data->WriteByteSeq(bgGuid[2]);

            *data << uint32(bg->GetClientInstanceID());                         // Client Instance ID

            data->WriteByteSeq(bgGuid[0]);
            data->WriteByteSeq(playerGuid[7]);
            break;
        }
    case STATUS_WAIT_LEAVE:
        break;
    }
}

void BattlegroundMgr::BuildPvpLogDataPacket(WorldPacket* data, Battleground* bg, ByteBuffer* buff)
{
    bool isRated = bg->isRated();               // type (normal=0/rated=1) -- ATM arena or bg, RBG NYI
    bool isArena = bg->isArena();               // Arena names

    size_t newSize = size_t((8 + (!isArena ? 12 : 0) + 8 + 8 + 1) * bg->GetPlayerScoresSize());
    *buff = ByteBuffer(newSize);
    data->Initialize(SMSG_PVP_LOG_DATA, (2 + (isArena ? 16 : 0) + 21 + (bg->GetPlayerScoresSize() * 39) + 1) / 8 + (isRated ? 24 : 0) + (isArena ? 26 : 0) + 2 + (bg->GetStatus() == STATUS_WAIT_LEAVE ? 1 : 0) + newSize);
    data->WriteBit(isArena);
    data->WriteBit(isRated);

    if (isArena)
    {
        for (int8 i = 0; i < BG_TEAMS_COUNT; ++i)
        {
            uint32 nameSize = 0;
            if (ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdByIndex(i)))
                nameSize = uint32(at->GetName().length());

            *data << WriteAsUnaligned<8>(nameSize);
        }
    }

    uint32 tempTotalSize = 0;
    uint32 tempPlayerSize = 0;

    size_t countPos = data->wpos();
    uint8 countBit = data->bitpos();
    uint32 count = 0;

    data->WriteBits<int>(count, 21);
    for (Battleground::BattlegroundScoreMap::const_iterator itr = bg->GetPlayerScoresBegin(); itr != bg->GetPlayerScoresEnd(); ++itr)
    {
        Player* player = ObjectAccessor::FindPlayer(itr->first);
        if (!player)
            continue;

        ObjectGuid playerGUID = itr->first;
        if (!bg->IsPlayerInBattleground(playerGUID))
        {
            sLog->outError(LOG_FILTER_BATTLEGROUND, "Player " UI64FMTD " has scoreboard entry for battleground %u but is not in battleground!", uint64(playerGUID), bg->GetTypeID(true));
            continue;
        }

        // Client can accept count > 80, but allocate memory only for 80 (Max player in bg)
        ++count;
        if (count <= 80)
        {
            BattlegroundScore* second = itr->second;
            uint32 team = player->GetBGTeam();

            data->WriteBit(false);              // Unk 1
            data->WriteBit(false);              // Unk 2
            data->WriteByteMask(playerGUID[2]);
            data->WriteBit(!isArena);
            data->WriteBit(false);              // Unk 4
            data->WriteBit(isRated);
            data->WriteBit(false);              // Unk 6
            data->WriteByteMask(playerGUID[3]);
            data->WriteByteMask(playerGUID[0]);
            data->WriteByteMask(playerGUID[5]);
            data->WriteByteMask(playerGUID[1]);
            data->WriteByteMask(playerGUID[6]);
            data->WriteBit(!bg->GetTeamIndexByTeamId(team));
            data->WriteByteMask(playerGUID[7]);

            *buff << uint32(second->HealingDone);             // healing done
            *buff << uint32(second->DamageDone);              // damage done

            if (!isArena) // Unk 3 prolly is (bg)
            {
                *buff << uint32(second->BonusHonor / 100);
                *buff << uint32(second->Deaths);
                *buff << uint32(second->HonorableKills);
            }

            buff->WriteByteSeq(playerGUID[4]);
            *buff << uint32(second->KillingBlows);

            if (isRated)
                *buff << uint32(bg->GetMatchMakingRatingChange(itr->first));

            buff->WriteByteSeq(playerGUID[5]);

            // if (unk 6) << uint32() unk
            // if (unk 2) << uint32() unk

            buff->WriteByteSeq(playerGUID[1]);
            buff->WriteByteSeq(playerGUID[6]);

            *buff << uint32(player->GetActiveTalentTree());

            switch (bg->GetTypeID(true))                             // Custom values
            {
            case BATTLEGROUND_RB:
                switch (bg->GetMapId())
                {
                case 489:
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundWGScore*)second)->FlagCaptures);        // flag captures
                    *buff << uint32(((BattlegroundWGScore*)second)->FlagReturns);         // flag returns
                    break;
                case 566:
                    data->WriteBits(0x00000001, 24);
                    *buff << uint32(((BattlegroundEYScore*)second)->FlagCaptures);        // flag captures
                    break;
                case 529:
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundABScore*)second)->BasesAssaulted);      // bases asssulted
                    *buff << uint32(((BattlegroundABScore*)second)->BasesDefended);       // bases defended
                    break;
                case 30:
                    data->WriteBits(0x00000005, 24);
                    *buff << uint32(((BattlegroundAVScore*)second)->GraveyardsAssaulted); // GraveyardsAssaulted
                    *buff << uint32(((BattlegroundAVScore*)second)->GraveyardsDefended);  // GraveyardsDefended
                    *buff << uint32(((BattlegroundAVScore*)second)->TowersAssaulted);     // TowersAssaulted
                    *buff << uint32(((BattlegroundAVScore*)second)->TowersDefended);      // TowersDefended
                    *buff << uint32(((BattlegroundAVScore*)second)->MinesCaptured);       // MinesCaptured
                    break;
                case 607:
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundSAScore*)second)->demolishers_destroyed);
                    *buff << uint32(((BattlegroundSAScore*)second)->gates_destroyed);
                    break;
                case 628:                                   // IC
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundICScore*)second)->BasesAssaulted);       // bases asssulted
                    *buff << uint32(((BattlegroundICScore*)second)->BasesDefended);        // bases defended
                    break;
                case 726:
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundTPScore*)second)->FlagCaptures);         // flag captures
                    *buff << uint32(((BattlegroundTPScore*)second)->FlagReturns);          // flag returns
                    break;
                case 761:
                    data->WriteBits(0x00000002, 24);
                    *buff << uint32(((BattlegroundBFGScore*)second)->BasesAssaulted);      // bases asssulted
                    *buff << uint32(((BattlegroundBFGScore*)second)->BasesDefended);       // bases defended
                    break;
                default:
                    data->WriteBits(0, 24);
                    break;
                }
                break;
            case BATTLEGROUND_AV:
                data->WriteBits(0x00000005, 24);
                *buff << uint32(((BattlegroundAVScore*)second)->GraveyardsAssaulted); // GraveyardsAssaulted
                *buff << uint32(((BattlegroundAVScore*)second)->GraveyardsDefended);  // GraveyardsDefended
                *buff << uint32(((BattlegroundAVScore*)second)->TowersAssaulted);     // TowersAssaulted
                *buff << uint32(((BattlegroundAVScore*)second)->TowersDefended);      // TowersDefended
                *buff << uint32(((BattlegroundAVScore*)second)->MinesCaptured);       // MinesCaptured
                break;
            case BATTLEGROUND_WS:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundWGScore*)second)->FlagCaptures);        // flag captures
                *buff << uint32(((BattlegroundWGScore*)second)->FlagReturns);         // flag returns
                break;
            case BATTLEGROUND_AB:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundABScore*)second)->BasesAssaulted);      // bases asssulted
                *buff << uint32(((BattlegroundABScore*)second)->BasesDefended);       // bases defended
                break;
            case BATTLEGROUND_EY:
                data->WriteBits(0x00000001, 24);
                *buff << uint32(((BattlegroundEYScore*)second)->FlagCaptures);        // flag captures
                break;
            case BATTLEGROUND_SA:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundSAScore*)second)->demolishers_destroyed);
                *buff << uint32(((BattlegroundSAScore*)second)->gates_destroyed);
                break;
            case BATTLEGROUND_IC:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundICScore*)second)->BasesAssaulted);       // bases asssulted
                *buff << uint32(((BattlegroundICScore*)second)->BasesDefended);        // bases defended
                break;
            case BATTLEGROUND_TP:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundTPScore*)second)->FlagCaptures);         // flag captures
                *buff << uint32(((BattlegroundTPScore*)second)->FlagReturns);          // flag returns
                break;
            case BATTLEGROUND_BFG:
                data->WriteBits(0x00000002, 24);
                *buff << uint32(((BattlegroundBFGScore*)second)->BasesAssaulted);      // bases asssulted
                *buff << uint32(((BattlegroundBFGScore*)second)->BasesDefended);       // bases defended
                break;
            case BATTLEGROUND_NA:
            case BATTLEGROUND_BE:
            case BATTLEGROUND_AA:
            case BATTLEGROUND_RL:
            case BATTLEGROUND_DS:
            case BATTLEGROUND_RV:
                data->WriteBits(0, 24);
                break;
            default:
                data->WriteBits(0, 24);
                break;
            }
            data->WriteByteMask(playerGUID[4]);

            buff->WriteByteSeq(playerGUID[0]);
            buff->WriteByteSeq(playerGUID[3]);

            buff->WriteByteSeq(playerGUID[7]);
            buff->WriteByteSeq(playerGUID[2]);
        }
    }

    data->WriteBitsAt(count, 21, countPos, countBit);

    data->WriteBit(bg->GetStatus() == STATUS_WAIT_LEAVE);    // If Ended
}

void BattlegroundMgr::FinishAndSendPvpLogDataPacket(WorldPacket* data, Battleground* bg, ByteBuffer* buff, Player* player)
{
    ByteBuffer finishbuff = *buff;
    WorldPacket finishData = *data;
    if (bg->isRated())                                             // arena TODO : Fix Order on Rated Implementation
    {
        uint8 index = bg->GetTeamIndexByTeamId(bg->GetPlayerTeam(player->GetGUID()));
        uint32 matchmakingValue_first = bg->GetArenaMatchmakerRatingByIndex(index);
        uint32 matchmakingValue_second = bg->GetArenaMatchmakerRatingByIndex(!index);
        finishData << uint32(matchmakingValue_first);    // Matchmaking Value 1
        finishData << uint32(0);                         // Think this field is not used (old field - mmr change, has been deleted after 4.3~)
        finishData << uint32(0);                         // Think this field is not used (old field - mmr change, has been deleted after 4.3~)
        finishData << uint32(matchmakingValue_second);   // Matchmaking Value 2
        finishData << uint32(0);                         // Think this field is not used (old field - mmr change, has been deleted after 4.3~)
        finishData << uint32(0);                         // Think this field is not used (old field - mmr change, has been deleted after 4.3~)
        sLog->outDebug(LOG_FILTER_BATTLEGROUND, "matchmaking rating values first team [ %d ], second team [ %d ]", matchmakingValue_first, matchmakingValue_second);
    }

    finishData.append(finishbuff);

    if (bg->isArena())
    {
        for (int8 i = 0; i < BG_TEAMS_COUNT; ++i)
        {
            uint32 size = 0;
            if (ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdByIndex(i)))
                finishData << WriteBuffer(at->GetName().c_str(), at->GetName().length());
        }
    }

    finishData << uint8(bg->GetPlayersCountByTeam(HORDE));

    if (bg->GetStatus() == STATUS_WAIT_LEAVE)
        finishData << uint8(bg->GetWinner());                    // who win

    finishData << uint8(bg->GetPlayersCountByTeam(ALLIANCE));

    player->GetSession()->SendPacket(&finishData);
}

void BattlegroundMgr::BuildStatusFailedPacket(WorldPacket* data, Battleground* bg, Player* player, uint8 QueueSlot, GroupJoinBattlegroundResult result)
{
    ObjectGuid guidBytes1 = player->GetGUID(); // player who caused the error
    ObjectGuid guidBytes2 = bg->GetGUID();
    ObjectGuid unkGuid3 = 0;

    data->Initialize(SMSG_BATTLEFIELD_STATUS_FAILED, 3 + 8 + 8 + 24);

    data->WriteByteMask(guidBytes2[3]);
    data->WriteByteMask(unkGuid3[3]);
    data->WriteByteMask(guidBytes1[3]);
    data->WriteByteMask(unkGuid3[0]);
    data->WriteByteMask(guidBytes2[6]);
    data->WriteByteMask(guidBytes1[5]);
    data->WriteByteMask(guidBytes1[6]);
    data->WriteByteMask(guidBytes1[4]);

    data->WriteByteMask(guidBytes1[2]);
    data->WriteByteMask(unkGuid3[1]);
    data->WriteByteMask(guidBytes2[1]);
    data->WriteByteMask(unkGuid3[5]);
    data->WriteByteMask(unkGuid3[6]);
    data->WriteByteMask(guidBytes1[1]);
    data->WriteByteMask(guidBytes2[7]);
    data->WriteByteMask(unkGuid3[4]);

    data->WriteByteMask(guidBytes2[2]);
    data->WriteByteMask(guidBytes2[5]);
    data->WriteByteMask(unkGuid3[7]);
    data->WriteByteMask(guidBytes2[4]);
    data->WriteByteMask(guidBytes2[0]);
    data->WriteByteMask(guidBytes1[0]);
    data->WriteByteMask(unkGuid3[2]);
    data->WriteByteMask(guidBytes1[7]);

    data->WriteByteSeq(guidBytes2[1]);

    *data << uint32(bg->GetStatus());           // Status
    *data << uint32(QueueSlot);                 // Queue slot

    data->WriteByteSeq(guidBytes1[6]);
    data->WriteByteSeq(guidBytes1[3]);
    data->WriteByteSeq(guidBytes1[7]);
    data->WriteByteSeq(guidBytes1[4]);
    data->WriteByteSeq(guidBytes2[0]);
    data->WriteByteSeq(guidBytes1[5]);
    data->WriteByteSeq(guidBytes2[7]);
    data->WriteByteSeq(guidBytes2[6]);
    data->WriteByteSeq(guidBytes2[2]);
    data->WriteByteSeq(unkGuid3[6]);
    data->WriteByteSeq(unkGuid3[3]);
    data->WriteByteSeq(guidBytes1[1]);
    data->WriteByteSeq(guidBytes2[3]);
    data->WriteByteSeq(unkGuid3[0]);
    data->WriteByteSeq(unkGuid3[1]);
    data->WriteByteSeq(unkGuid3[4]);
    data->WriteByteSeq(guidBytes1[0]);
    data->WriteByteSeq(guidBytes2[5]);
    data->WriteByteSeq(unkGuid3[7]);
    data->WriteByteSeq(guidBytes2[4]);
    data->WriteByteSeq(guidBytes1[2]);

    *data << uint32(result);                    // Result

    data->WriteByteSeq(unkGuid3[2]);

    *data << uint32(player->GetBattlegroundQueueJoinTime(bg->GetTypeID())); // Join Time

    data->WriteByteSeq(unkGuid3[5]);
}

void BattlegroundMgr::BuildUpdateWorldStatePacket(WorldPacket* data, uint32 field, uint32 value)
{
    data->Initialize(SMSG_UPDATE_WORLD_STATE, 4+4);
    *data << uint32(field);
    *data << uint32(value);
}

void BattlegroundMgr::BuildPlayerLeftBattlegroundPacket(WorldPacket* data, uint64 guid)
{
    ObjectGuid guidBytes = guid;

    data->Initialize(SMSG_BATTLEGROUND_PLAYER_LEFT, 8);

    data->WriteByteMask(guidBytes[7]);
    data->WriteByteMask(guidBytes[6]);
    data->WriteByteMask(guidBytes[2]);
    data->WriteByteMask(guidBytes[4]);
    data->WriteByteMask(guidBytes[5]);
    data->WriteByteMask(guidBytes[1]);
    data->WriteByteMask(guidBytes[3]);
    data->WriteByteMask(guidBytes[0]);

    data->WriteByteSeq(guidBytes[4]);
    data->WriteByteSeq(guidBytes[2]);
    data->WriteByteSeq(guidBytes[5]);
    data->WriteByteSeq(guidBytes[7]);
    data->WriteByteSeq(guidBytes[0]);
    data->WriteByteSeq(guidBytes[6]);
    data->WriteByteSeq(guidBytes[1]);
    data->WriteByteSeq(guidBytes[3]);
}

void BattlegroundMgr::BuildPlayerJoinedBattlegroundPacket(WorldPacket* data, uint64 guid)
{
    ObjectGuid guidBytes = guid;

    data->Initialize(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);

    data->WriteByteMask(guidBytes[0]);
    data->WriteByteMask(guidBytes[4]);
    data->WriteByteMask(guidBytes[3]);
    data->WriteByteMask(guidBytes[5]);
    data->WriteByteMask(guidBytes[7]);
    data->WriteByteMask(guidBytes[6]);
    data->WriteByteMask(guidBytes[2]);
    data->WriteByteMask(guidBytes[1]);

    data->WriteByteSeq(guidBytes[1]);
    data->WriteByteSeq(guidBytes[5]);
    data->WriteByteSeq(guidBytes[3]);
    data->WriteByteSeq(guidBytes[2]);
    data->WriteByteSeq(guidBytes[0]);
    data->WriteByteSeq(guidBytes[7]);
    data->WriteByteSeq(guidBytes[4]);
    data->WriteByteSeq(guidBytes[6]);
}

Battleground* BattlegroundMgr::GetBattlegroundThroughClientInstance(uint32 instanceId, BattlegroundTypeId bgTypeId)
{
    //cause at HandleBattlegroundJoinOpcode the clients sends the instanceid he gets from
    //SMSG_BATTLEFIELD_LIST we need to find the battleground with this clientinstance-id
    Battleground* bg = GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return NULL;

    if (bg->isArena())
        return GetBattleground(instanceId, bgTypeId);

    BattlegroundDataContainer::const_iterator it = bgDataStore.find(bgTypeId);
    if (it == bgDataStore.end())
        return NULL;

    for (BattlegroundContainer::const_iterator itr = it->second.m_Battlegrounds.begin(); itr != it->second.m_Battlegrounds.end(); ++itr)
    {
        if (itr->second->GetClientInstanceID() == instanceId)
            return itr->second;
    }

    return NULL;
}

Battleground* BattlegroundMgr::GetBattleground(uint32 instanceId, BattlegroundTypeId bgTypeId)
{
    if (!instanceId)
        return NULL;

    BattlegroundDataContainer::const_iterator begin, end;

    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
    {
        begin = bgDataStore.begin();
        end = bgDataStore.end();
    }
    else
    {
        end = bgDataStore.find(bgTypeId);
        if (end == bgDataStore.end())
            return NULL;
        begin = end++;
    }

    for (BattlegroundDataContainer::const_iterator it = begin; it != end; ++it)
    {
        BattlegroundContainer const& bgs = it->second.m_Battlegrounds;
        BattlegroundContainer::const_iterator itr = bgs.find(instanceId);
        if (itr != bgs.end())
            return itr->second;
    }

    return NULL;
}

Battleground* BattlegroundMgr::GetBattlegroundTemplate(BattlegroundTypeId bgTypeId)
{
    BattlegroundDataContainer::const_iterator itr = bgDataStore.find(bgTypeId);
    if (itr == bgDataStore.end())
        return NULL;

    BattlegroundContainer const& bgs = itr->second.m_Battlegrounds;
    //map is sorted and we can be sure that lowest instance id has only BG template
    return bgs.empty() ? NULL : bgs.begin()->second;
}

uint32 BattlegroundMgr::CreateClientVisibleInstanceId(BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id)
{
    if (IsArenaType(bgTypeId))
        return 0;                                           //arenas don't have client-instanceids

    // we create here an instanceid, which is just for
    // displaying this to the client and without any other use..
    // the client-instanceIds are unique for each battleground-type
    // the instance-id just needs to be as low as possible, beginning with 1
    // the following works, because std::set is default ordered with "<"
    // the optimalization would be to use as bitmask std::vector<uint32> - but that would only make code unreadable

    BattlegroundClientIdsContainer& clientIds = bgDataStore[bgTypeId].m_ClientBattlegroundIds[bracket_id];
    uint32 lastId = 0;
    for (BattlegroundClientIdsContainer::const_iterator itr = clientIds.begin(); itr != clientIds.end();)
    {
        if ((++lastId) != *itr)                             //if there is a gap between the ids, we will break..
            break;
        lastId = *itr;
    }

    clientIds.insert(++lastId);
    return lastId;
}

// create a new battleground that will really be used to play
Battleground* BattlegroundMgr::CreateNewBattleground(BattlegroundTypeId originalBgTypeId, PvPDifficultyEntry const* bracketEntry, uint8 arenaType, bool isRated)
{
    BattlegroundTypeId bgTypeId = originalBgTypeId;
    bool isRandom = false;

    switch (originalBgTypeId)
    {
    case BATTLEGROUND_RB:
    case BATTLEGROUND_RATED_10_VS_10:
        isRandom = true;
    case BATTLEGROUND_AA:
        bgTypeId = GetRandomBG(originalBgTypeId);
        break;
    default:
        break;
    }

    // get the template BG
    Battleground* bg_template = GetBattlegroundTemplate(bgTypeId);

    if (!bg_template)
    {
        sLog->outError(LOG_FILTER_BATTLEGROUND, "Battleground: CreateNewBattleground - bg template not found for %u", bgTypeId);
        return NULL;
    }

    Battleground* bg = NULL;
    // create a copy of the BG template
    switch (bgTypeId)
    {
    case BATTLEGROUND_AV:
        bg = new BattlegroundAV(*(BattlegroundAV*)bg_template);
        break;
    case BATTLEGROUND_WS:
        bg = new BattlegroundWS(*(BattlegroundWS*)bg_template);
        break;
    case BATTLEGROUND_AB:
        bg = new BattlegroundAB(*(BattlegroundAB*)bg_template);
        break;
    case BATTLEGROUND_NA:
        bg = new BattlegroundNA(*(BattlegroundNA*)bg_template);
        break;
    case BATTLEGROUND_BE:
        bg = new BattlegroundBE(*(BattlegroundBE*)bg_template);
        break;
    case BATTLEGROUND_EY:
        bg = new BattlegroundEY(*(BattlegroundEY*)bg_template);
        break;
    case BATTLEGROUND_RL:
        bg = new BattlegroundRL(*(BattlegroundRL*)bg_template);
        break;
    case BATTLEGROUND_SA:
        bg = new BattlegroundSA(*(BattlegroundSA*)bg_template);
        break;
    case BATTLEGROUND_DS:
        bg = new BattlegroundDS(*(BattlegroundDS*)bg_template);
        break;
    case BATTLEGROUND_RV:
        bg = new BattlegroundRV(*(BattlegroundRV*)bg_template);
        break;
    case BATTLEGROUND_IC:
        bg = new BattlegroundIC(*(BattlegroundIC*)bg_template);
        break;
    case BATTLEGROUND_TP:
        bg = new BattlegroundTP(*(BattlegroundTP*)bg_template);
        break;
    case BATTLEGROUND_BFG:
        bg = new BattlegroundBFG(*(BattlegroundBFG*)bg_template);
        break;
    case BATTLEGROUND_RB:
    case BATTLEGROUND_AA:
        bg = new Battleground(*bg_template);
        break;
    default:
        return NULL;
    }

    bg->SetBracket(bracketEntry);
    bg->SetInstanceID(sMapMgr->GenerateInstanceId());
    bg->SetClientInstanceID(CreateClientVisibleInstanceId(isRandom ? BATTLEGROUND_RB : bgTypeId, bracketEntry->GetBracketId()));
    bg->Reset();                     // reset the new bg (set status to status_wait_queue from status_none)
    bg->SetStatus(STATUS_WAIT_JOIN); // start the joining of the bg
    bg->SetArenaType(arenaType);
    bg->SetTypeID(originalBgTypeId);
    bg->SetRandomTypeID(bgTypeId);
    bg->SetRated(isRated);
    bg->SetRandom(isRandom);
    bg->SetGuid(MAKE_NEW_GUID(bgTypeId, 0, HIGHGUID_TYPE_BATTLEGROUND));

    return bg;
}

// used to create the BG templates
bool BattlegroundMgr::CreateBattleground(CreateBattlegroundData& data)
{
    // Create the BG
    Battleground* bg = NULL;
    switch (data.bgTypeId)
    {
    case BATTLEGROUND_AV:
        bg = new BattlegroundAV;
        break;
    case BATTLEGROUND_WS:
        bg = new BattlegroundWS;
        break;
    case BATTLEGROUND_AB:
        bg = new BattlegroundAB;
        break;
    case BATTLEGROUND_NA:
        bg = new BattlegroundNA;
        break;
    case BATTLEGROUND_BE:
        bg = new BattlegroundBE;
        break;
    case BATTLEGROUND_EY:
        bg = new BattlegroundEY;
        break;
    case BATTLEGROUND_RL:
        bg = new BattlegroundRL;
        break;
    case BATTLEGROUND_SA:
        bg = new BattlegroundSA;
        break;
    case BATTLEGROUND_DS:
        bg = new BattlegroundDS;
        break;
    case BATTLEGROUND_RV:
        bg = new BattlegroundRV;
        break;
    case BATTLEGROUND_IC:
        bg = new BattlegroundIC;
        break;
    case BATTLEGROUND_AA:
        bg = new Battleground;
        break;
    case BATTLEGROUND_RB:
        bg = new Battleground;
        bg->SetRandom(true);
        break;
    case BATTLEGROUND_RATED_10_VS_10:
        bg = new Battleground;
        bg->SetRandom(true);
        break;
    case BATTLEGROUND_TP:
        bg = new BattlegroundTP;
        break;
    case BATTLEGROUND_BFG:
        bg = new BattlegroundBFG;
        break;
    default:
        return false;
    }

    bg->SetMapId(data.MapID);
    bg->SetTypeID(data.bgTypeId);
    bg->SetInstanceID(0);
    bg->SetArenaorBGType(data.IsArena);
    bg->SetMinPlayersPerTeam(data.MinPlayersPerTeam);
    bg->SetMaxPlayersPerTeam(data.MaxPlayersPerTeam);
    bg->SetMinPlayers(data.MinPlayersPerTeam* 2);
    bg->SetMaxPlayers(data.MaxPlayersPerTeam* 2);
    bg->SetName(data.BattlegroundName);
    bg->SetTeamStartLoc(ALLIANCE, data.Team1StartLocX, data.Team1StartLocY, data.Team1StartLocZ, data.Team1StartLocO);
    bg->SetTeamStartLoc(HORDE,    data.Team2StartLocX, data.Team2StartLocY, data.Team2StartLocZ, data.Team2StartLocO);
    bg->SetStartMaxDist(data.StartMaxDist);
    bg->SetLevelRange(data.LevelMin, data.LevelMax);
    bg->SetScriptId(data.scriptId);
    bg->SetGuid(MAKE_NEW_GUID(data.bgTypeId, 0, HIGHGUID_TYPE_BATTLEGROUND));

    AddBattleground(bg);

    return true;
}

void BattlegroundMgr::CreateInitialBattlegrounds()
{
    uint32 oldMSTime = getMSTime();
    //                                               0   1                  2                  3       4       5                 6               7              8            9             10      11
    QueryResult result = WorldDatabase.Query("SELECT id, MinPlayersPerTeam, MaxPlayersPerTeam, MinLvl, MaxLvl, AllianceStartLoc, AllianceStartO, HordeStartLoc, HordeStartO, StartMaxDist, Weight, ScriptName FROM battleground_template");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 battlegrounds. DB table `battleground_template` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 bgTypeId = fields[0].GetUInt32();
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeId, NULL))
            continue;

        // can be overwrite by values from DB
        BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);
        if (!bl)
        {
            sLog->outError(LOG_FILTER_BATTLEGROUND, "Battleground ID %u not found in BattlemasterList.dbc. Battleground not created.", bgTypeId);
            continue;
        }

        CreateBattlegroundData data;
        data.bgTypeId = BattlegroundTypeId(bgTypeId);
        data.IsArena = (bl->type == TYPE_ARENA);
        data.MinPlayersPerTeam = fields[1].GetUInt16();
        data.MaxPlayersPerTeam = fields[2].GetUInt16();
        data.LevelMin = fields[3].GetUInt8();
        data.LevelMax = fields[4].GetUInt8();
        float spawn = fields[9].GetFloat();
        data.StartMaxDist = float(spawn * spawn);

        data.scriptId = sObjectMgr->GetScriptId(fields[11].GetCString());
        data.BattlegroundName = bl->name;
        data.MapID = bl->mapid[0];

        if (data.MaxPlayersPerTeam == 0 || data.MinPlayersPerTeam > data.MaxPlayersPerTeam)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `battleground_template` for id %u has bad values for MinPlayersPerTeam (%u) and MaxPlayersPerTeam(%u)",
                data.bgTypeId, data.MinPlayersPerTeam, data.MaxPlayersPerTeam);
            continue;
        }

        if (data.LevelMin == 0 || data.LevelMax == 0 || data.LevelMin > data.LevelMax)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `battleground_template` for id %u has bad values for LevelMin (%u) and LevelMax(%u)",
                data.bgTypeId, data.LevelMin, data.LevelMax);
            continue;
        }

        if (data.bgTypeId == BATTLEGROUND_AA || data.bgTypeId == BATTLEGROUND_RB || data.bgTypeId == BATTLEGROUND_RATED_10_VS_10)
        {
            data.Team1StartLocX = 0;
            data.Team1StartLocY = 0;
            data.Team1StartLocZ = 0;
            data.Team1StartLocO = fields[6].GetFloat();
            data.Team2StartLocX = 0;
            data.Team2StartLocY = 0;
            data.Team2StartLocZ = 0;
            data.Team2StartLocO = fields[8].GetFloat();
        }
        else
        {
            uint32 startId = fields[5].GetUInt32();
            if (WorldSafeLocsEntry const* start = sWorldSafeLocsStore.LookupEntry(startId))
            {
                data.Team1StartLocX = start->x;
                data.Team1StartLocY = start->y;
                data.Team1StartLocZ = start->z;
                data.Team1StartLocO = fields[6].GetFloat();
            }
            else
            {
                sLog->outError(LOG_FILTER_SQL, "Table `battleground_template` for id %u have non-existed WorldSafeLocs.dbc id %u in field `AllianceStartLoc`. BG not created.", data.bgTypeId, startId);
                continue;
            }

            startId = fields[7].GetUInt32();
            if (WorldSafeLocsEntry const* start = sWorldSafeLocsStore.LookupEntry(startId))
            {
                data.Team2StartLocX = start->x;
                data.Team2StartLocY = start->y;
                data.Team2StartLocZ = start->z;
                data.Team2StartLocO = fields[8].GetFloat();
            }
            else
            {
                sLog->outError(LOG_FILTER_SQL, "Table `battleground_template` for id %u have non-existed WorldSafeLocs.dbc id %u in field `HordeStartLoc`. BG not created.", data.bgTypeId, startId);
                continue;
            }
        }

        if (!CreateBattleground(data))
            continue;

        if (data.IsArena)
        {
            if (data.bgTypeId != BATTLEGROUND_AA)
                m_ArenaSelectionWeights[data.bgTypeId] = fields[10].GetUInt8();
        }
        else if (data.bgTypeId != BATTLEGROUND_RB)
            m_BGSelectionWeights[data.bgTypeId] = fields[10].GetUInt8();

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u battlegrounds in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlegroundMgr::BuildBattlegroundListPacket(WorldPacket* data, uint64 guid, Player* player, BattlegroundTypeId bgTypeId)
{
    if (!player)
        return;

    BattlegroundDataContainer::iterator it = bgDataStore.find(bgTypeId);
    if (it == bgDataStore.end())
        return;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(it->second.m_Battlegrounds.begin()->second->GetMapId(), player->getLevel());
    if (!bracketEntry)
        return;

    uint32 winner_conquest = !player->GetRandomWinner() ? BG_REWARD_WINNER_CONQUEST_FIRST : BG_REWARD_WINNER_CONQUEST_LAST;
    uint32 winner_honor = !player->GetRandomWinner() ? BG_REWARD_WINNER_HONOR_FIRST : BG_REWARD_WINNER_HONOR_LAST;
    uint32 loser_honor = !player->GetRandomWinner() ? BG_REWARD_LOSER_HONOR_FIRST : BG_REWARD_LOSER_HONOR_LAST;

    ObjectGuid guidBytes = guid;

    data->Initialize(SMSG_BATTLEFIELD_LIST);

    *data
        << uint32(winner_conquest)                        // Winner Conquest Reward or Random Winner Conquest Reward
        << uint32(winner_conquest)                        // Winner Conquest Reward or Random Winner Conquest Reward
        << uint32(loser_honor)                            // Loser Honor Reward or Random Loser Honor Reward
        << uint32(bgTypeId)                               // battleground id
        << uint32(loser_honor)                            // Loser Honor Reward or Random Loser Honor Reward
        << uint32(winner_honor)                           // Winner Honor Reward or Random Winner Honor Reward
        << uint32(winner_honor)                           // Winner Honor Reward or Random Winner Honor Reward
        << uint8(bracketEntry->maxLevel)                  // max level
        << uint8(bracketEntry->minLevel);                 // min level

    data->WriteByteMask(guidBytes[0]);
    data->WriteByteMask(guidBytes[1]);
    data->WriteByteMask(guidBytes[7]);
    data->WriteBit(false);                                  // unk
    data->WriteBit(false);                                  // unk

    uint32 countPos = data->wpos();
    uint32 countBit = data->bitpos();
    data->WriteBits<int>(0, 24);

    data->WriteByteMask(guidBytes[6]);
    data->WriteByteMask(guidBytes[4]);
    data->WriteByteMask(guidBytes[2]);
    data->WriteByteMask(guidBytes[3]);
    data->WriteBit(true);                                   // unk
    data->WriteByteMask(guidBytes[5]);
    data->WriteBit(true);                                   // unk

    data->WriteByteSeq(guidBytes[6]);
    data->WriteByteSeq(guidBytes[1]);
    data->WriteByteSeq(guidBytes[7]);
    data->WriteByteSeq(guidBytes[5]);

    uint32 count = 0;
    BattlegroundBracketId bracketId = bracketEntry->GetBracketId();
    BattlegroundClientIdsContainer& clientIds = it->second.m_ClientBattlegroundIds[bracketId];
    for (BattlegroundClientIdsContainer::const_iterator itr = clientIds.begin(); itr != clientIds.end(); ++itr)
    {
        *data << uint32(*itr);
        ++count;
    }
    data->WriteBitsAt(count, 24, countPos, countBit);

    data->WriteByteSeq(guidBytes[0]);
    data->WriteByteSeq(guidBytes[2]);
    data->WriteByteSeq(guidBytes[4]);
    data->WriteByteSeq(guidBytes[3]);
}

void BattlegroundMgr::SendToBattleground(Player* player, uint32 instanceId, BattlegroundTypeId bgTypeId)
{
    if (Battleground* bg = GetBattleground(instanceId, bgTypeId))
    {
        float x, y, z, O;
        uint32 mapid = bg->GetMapId();
        uint32 team = player->GetBGTeam();

        bg->GetTeamStartLoc(team, x, y, z, O);
        sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BattlegroundMgr::SendToBattleground: Sending %s to map %u, X %f, Y %f, Z %f, O %f (bgType %u)", player->GetName(), mapid, x, y, z, O, bgTypeId);
        player->TeleportTo(mapid, x, y, z, O);
    }
    else
        sLog->outError(LOG_FILTER_BATTLEGROUND, "BattlegroundMgr::SendToBattleground: Instance %u (bgType %u) not found while trying to teleport player %s", instanceId, bgTypeId, player->GetName());
}

void BattlegroundMgr::SendAreaSpiritHealerQueryOpcode(Player* player, Battleground* bg, uint64 guid)
{
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
    uint32 time_ = 30000 - bg->GetLastResurrectTime();      // resurrect every 30 seconds
    if (time_ == uint32(-1))
        time_ = 0;
    data << guid << time_;
    player->GetSession()->SendPacket(&data);
}

bool BattlegroundMgr::IsArenaType(BattlegroundTypeId bgTypeId)
{
    return bgTypeId == BATTLEGROUND_AA
        || bgTypeId == BATTLEGROUND_BE
        || bgTypeId == BATTLEGROUND_NA
        || bgTypeId == BATTLEGROUND_DS
        || bgTypeId == BATTLEGROUND_RV
        || bgTypeId == BATTLEGROUND_RL;
}

BattlegroundQueueTypeId BattlegroundMgr::BGQueueTypeId(BattlegroundTypeId bgTypeId, uint8 arenaType)
{
    switch (bgTypeId)
    {
    case BATTLEGROUND_WS:
        return BATTLEGROUND_QUEUE_WS;
    case BATTLEGROUND_AB:
        return BATTLEGROUND_QUEUE_AB;
    case BATTLEGROUND_AV:
        return BATTLEGROUND_QUEUE_AV;
    case BATTLEGROUND_EY:
        return BATTLEGROUND_QUEUE_EY;
    case BATTLEGROUND_SA:
        return BATTLEGROUND_QUEUE_SA;
    case BATTLEGROUND_IC:
        return BATTLEGROUND_QUEUE_IC;
    case BATTLEGROUND_TP:
        return BATTLEGROUND_QUEUE_TP;
    case BATTLEGROUND_BFG:
        return BATTLEGROUND_QUEUE_BFG;
    case BATTLEGROUND_RB:
        return BATTLEGROUND_QUEUE_RB;
    case BATTLEGROUND_RATED_10_VS_10:
        return BATTLEGROUND_QUEUE_RBG;
    case BATTLEGROUND_AA:
    case BATTLEGROUND_NA:
    case BATTLEGROUND_RL:
    case BATTLEGROUND_BE:
    case BATTLEGROUND_DS:
    case BATTLEGROUND_RV:
        switch (arenaType)
        {
        case ARENA_TYPE_2v2:
            return BATTLEGROUND_QUEUE_2v2;
        case ARENA_TYPE_3v3:
            return BATTLEGROUND_QUEUE_3v3;
        case ARENA_TYPE_5v5:
            return BATTLEGROUND_QUEUE_5v5;
        default:
            return BATTLEGROUND_QUEUE_NONE;
        }
    default:
        return BATTLEGROUND_QUEUE_NONE;
    }
}

BattlegroundTypeId BattlegroundMgr::BGTemplateId(BattlegroundQueueTypeId bgQueueTypeId)
{
    switch (bgQueueTypeId)
    {
    case BATTLEGROUND_QUEUE_WS:
        return BATTLEGROUND_WS;
    case BATTLEGROUND_QUEUE_AB:
        return BATTLEGROUND_AB;
    case BATTLEGROUND_QUEUE_AV:
        return BATTLEGROUND_AV;
    case BATTLEGROUND_QUEUE_EY:
        return BATTLEGROUND_EY;
    case BATTLEGROUND_QUEUE_SA:
        return BATTLEGROUND_SA;
    case BATTLEGROUND_QUEUE_IC:
        return BATTLEGROUND_IC;
    case BATTLEGROUND_QUEUE_TP:
        return BATTLEGROUND_TP;
    case BATTLEGROUND_QUEUE_BFG:
        return BATTLEGROUND_BFG;
    case BATTLEGROUND_QUEUE_RB:
        return BATTLEGROUND_RB;
    case BATTLEGROUND_QUEUE_2v2:
    case BATTLEGROUND_QUEUE_3v3:
    case BATTLEGROUND_QUEUE_5v5:
        return BATTLEGROUND_AA;
    default:
        return BattlegroundTypeId(0);                   // used for unknown template (it existed and do nothing)
    }
}

uint8 BattlegroundMgr::BGArenaType(BattlegroundQueueTypeId bgQueueTypeId)
{
    switch (bgQueueTypeId)
    {
    case BATTLEGROUND_QUEUE_2v2:
        return ARENA_TYPE_2v2;
    case BATTLEGROUND_QUEUE_3v3:
        return ARENA_TYPE_3v3;
    case BATTLEGROUND_QUEUE_5v5:
        return ARENA_TYPE_5v5;
    default:
        return 0;
    }
}

void BattlegroundMgr::ToggleTesting()
{
    m_Testing = !m_Testing;
    sWorld->SendWorldText(m_Testing ? LANG_DEBUG_BG_ON : LANG_DEBUG_BG_OFF);
}

void BattlegroundMgr::ToggleArenaTesting()
{
    m_ArenaTesting = !m_ArenaTesting;
    sWorld->SendWorldText(m_ArenaTesting ? LANG_DEBUG_ARENA_ON : LANG_DEBUG_ARENA_OFF);
}

void BattlegroundMgr::SetHolidayWeekends(uint32 mask)
{
    for (uint32 bgtype = 1; bgtype < MAX_BATTLEGROUND_TYPE_ID; ++bgtype)
    {
        if (Battleground* bg = GetBattlegroundTemplate(BattlegroundTypeId(bgtype)))
        {
            bg->SetHoliday(mask & (1 << bgtype));
        }
    }
}

void BattlegroundMgr::ScheduleQueueUpdate(uint32 arenaMatchmakerRating, uint8 arenaType, BattlegroundQueueTypeId bgQueueTypeId, BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id)
{
    //This method must be atomic, TODO add mutex
    //we will use only 1 number created of bgTypeId and bracket_id
    uint64 const scheduleId = ((uint64)arenaMatchmakerRating << 32) | (arenaType << 24) | (bgQueueTypeId << 16) | (bgTypeId << 8) | bracket_id;
    if (std::find(m_QueueUpdateScheduler.begin(), m_QueueUpdateScheduler.end(), scheduleId) == m_QueueUpdateScheduler.end())
        m_QueueUpdateScheduler.push_back(scheduleId);
}

uint32 BattlegroundMgr::GetMaxRatingDifference() const
{
    // this is for stupid people who can't use brain and set max rating difference to 0
    uint32 diff = sWorld->getIntConfig(CONFIG_ARENA_MAX_RATING_DIFFERENCE);
    if (diff == 0)
        diff = 5000;
    return diff;
}

uint32 BattlegroundMgr::GetRatingDiscardTimer() const
{
    return sWorld->getIntConfig(CONFIG_ARENA_RATING_DISCARD_TIMER);
}

uint32 BattlegroundMgr::GetPrematureFinishTime() const
{
    return sWorld->getIntConfig(CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER);
}

void BattlegroundMgr::LoadBattleMastersEntry()
{
    uint32 oldMSTime = getMSTime();

    mBattleMastersMap.clear();                                  // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, bg_template FROM battlemaster_entry");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 battlemaster entries. DB table `battlemaster_entry` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint32 bgTypeId  = fields[1].GetUInt32();
        if (!sBattlemasterListStore.LookupEntry(bgTypeId))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `battlemaster_entry` contain entry %u for not existed battleground type %u, ignored.", entry, bgTypeId);
            continue;
        }

        mBattleMastersMap[entry] = BattlegroundTypeId(bgTypeId);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u battlemaster entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

HolidayIds BattlegroundMgr::BGTypeToWeekendHolidayId(BattlegroundTypeId bgTypeId)
{
    switch (bgTypeId)
    {
    case BATTLEGROUND_AV: return HOLIDAY_CALL_TO_ARMS_AV;
    case BATTLEGROUND_EY: return HOLIDAY_CALL_TO_ARMS_EY;
    case BATTLEGROUND_WS: return HOLIDAY_CALL_TO_ARMS_WS;
    case BATTLEGROUND_SA: return HOLIDAY_CALL_TO_ARMS_SA;
    case BATTLEGROUND_AB: return HOLIDAY_CALL_TO_ARMS_AB;
    case BATTLEGROUND_IC: return HOLIDAY_CALL_TO_ARMS_IC;
    case BATTLEGROUND_TP: return HOLIDAY_CALL_TO_ARMS_TP;
    case BATTLEGROUND_BFG: return HOLIDAY_CALL_TO_ARMS_BFG;
    default: return HOLIDAY_NONE;
    }
}

BattlegroundTypeId BattlegroundMgr::WeekendHolidayIdToBGType(HolidayIds holiday)
{
    switch (holiday)
    {
    case HOLIDAY_CALL_TO_ARMS_AV: return BATTLEGROUND_AV;
    case HOLIDAY_CALL_TO_ARMS_EY: return BATTLEGROUND_EY;
    case HOLIDAY_CALL_TO_ARMS_WS: return BATTLEGROUND_WS;
    case HOLIDAY_CALL_TO_ARMS_SA: return BATTLEGROUND_SA;
    case HOLIDAY_CALL_TO_ARMS_AB: return BATTLEGROUND_AB;
    case HOLIDAY_CALL_TO_ARMS_IC: return BATTLEGROUND_IC;
    case HOLIDAY_CALL_TO_ARMS_TP: return BATTLEGROUND_TP;
    case HOLIDAY_CALL_TO_ARMS_BFG: return BATTLEGROUND_BFG;
    default: return BATTLEGROUND_TYPE_NONE;
    }
}

bool BattlegroundMgr::IsBGWeekend(BattlegroundTypeId bgTypeId)
{
    return IsHolidayActive(BGTypeToWeekendHolidayId(bgTypeId));
}


uint32 BattlegroundMgr::GetBgQuestId(BattlegroundTypeId bgTypeId, uint32 faction)
{
    uint32 questId = 0;

    switch (bgTypeId)
    {
    case BATTLEGROUND_AB:
        questId = (faction == ALLIANCE) ? QUEST_CALL_TO_ARMS_AB_A : QUEST_CALL_TO_ARMS_AB_H;
        break;
    case BATTLEGROUND_AV:
        questId = (faction == ALLIANCE) ? QUEST_CALL_TO_ARMS_AV_A : QUEST_CALL_TO_ARMS_AV_H;
        break;
    case BATTLEGROUND_EY: 
        questId = (faction == ALLIANCE) ? QUEST_CALL_TO_ARMS_EY_A : QUEST_CALL_TO_ARMS_EY_H;
        break;
    case BATTLEGROUND_WS:
        questId = (faction == ALLIANCE) ? QUEST_CALL_TO_ARMS_WS_A : QUEST_CALL_TO_ARMS_WS_H;
        break;
    default:
        return 0;
    }

    return questId;
}


BattlegroundTypeId BattlegroundMgr::GetRandomBG(BattlegroundTypeId bgTypeId)
{
    uint32 weight = 0;
    BattlegroundTypeId returnBgTypeId = BATTLEGROUND_TYPE_NONE;
    BattlegroundSelectionWeightMap selectionWeights;

    if (bgTypeId == BATTLEGROUND_RATED_10_VS_10)
        switch (urand(0, 2))
    {
        case 0: return BATTLEGROUND_WS;
        case 1: return BATTLEGROUND_TP;
        case 2: return BATTLEGROUND_BFG;
    }

    if (bgTypeId == BATTLEGROUND_AA)
    {
        for (BattlegroundSelectionWeightMap::const_iterator it = m_ArenaSelectionWeights.begin(); it != m_ArenaSelectionWeights.end(); ++it)
        {
            if (it->second)
            {
                weight += it->second;
                selectionWeights[it->first] = it->second;
            }
        }
    }
    else if (bgTypeId == BATTLEGROUND_RB)
    {
        for (BattlegroundSelectionWeightMap::const_iterator it = m_BGSelectionWeights.begin(); it != m_BGSelectionWeights.end(); ++it)
        {
            if (it->second)
            {
                weight += it->second;
                selectionWeights[it->first] = it->second;
            }
        }
    }

    if (weight)
    {
        // Select a random value
        uint32 selectedWeight = urand(0, weight - 1);
        // Select the correct bg (if we have in DB A(10), B(20), C(10), D(15) --> [0---A---9|10---B---29|30---C---39|40---D---54])
        weight = 0;
        for (BattlegroundSelectionWeightMap::const_iterator it = selectionWeights.begin(); it != selectionWeights.end(); ++it)
        {
            weight += it->second;
            if (selectedWeight < weight)
            {
                returnBgTypeId = it->first;
                break;
            }
        }
    }

    return returnBgTypeId;
}

BGFreeSlotQueueContainer& BattlegroundMgr::GetBGFreeSlotQueueStore(BattlegroundTypeId bgTypeId)
{
    return bgDataStore[bgTypeId].BGFreeSlotQueue;
}

void BattlegroundMgr::AddToBGFreeSlotQueue(BattlegroundTypeId bgTypeId, Battleground* bg)
{
    bgDataStore[bgTypeId].BGFreeSlotQueue.push_front(bg);
}

void BattlegroundMgr::RemoveFromBGFreeSlotQueue(BattlegroundTypeId bgTypeId, uint32 instanceId)
{
    BGFreeSlotQueueContainer& queues = bgDataStore[bgTypeId].BGFreeSlotQueue;
    for (BGFreeSlotQueueContainer::iterator itr = queues.begin(); itr != queues.end(); ++itr)
        if ((*itr)->GetInstanceID() == instanceId)
        {
            queues.erase(itr);
            return;
        }
}

void BattlegroundMgr::AddBattleground(Battleground* bg)
{
    if (bg)
        bgDataStore[bg->GetTypeID()].m_Battlegrounds[bg->GetInstanceID()] = bg;
}

void BattlegroundMgr::RemoveBattleground(BattlegroundTypeId bgTypeId, uint32 instanceId)
{
    bgDataStore[bgTypeId].m_Battlegrounds.erase(instanceId);
}


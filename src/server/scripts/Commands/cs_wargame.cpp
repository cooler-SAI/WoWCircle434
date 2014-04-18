/*
* Copyright (C) 2013-2014 Cerber Team
* made by : Saqirmdev, L30m4nc3r, moJIto-ice, Xiledra
*/

#include "ScriptPCH.h"
#include "BattlegroundMgr.h"
#include "DisableMgr.h"
#include "Group.h"

class wargame_commands : public CommandScript
{
public:
    wargame_commands() : CommandScript("wargame_commands") {}

    ChatCommand* GetCommands() const
    {
        static ChatCommand WarGameCommandTable[] =
        {
            {   "",     SEC_PLAYER,         false,      &HandlewargameCommand,          "",         NULL},
            {   NULL,   0,                  false,      NULL,                           "",         NULL}
        };


        static ChatCommand commandTable[] =
        {
            {   "wargame",    SEC_PLAYER, false,      NULL,                             "",         WarGameCommandTable},
            {   NULL,           0,          false,      NULL,                           "",         NULL}
        };
        return commandTable;
    }

    static bool HandlewargameCommand(ChatHandler* handler, const char* args)
    {
        Player* target = NULL;
        Player* initiator = NULL;
        Battleground *arena = NULL;
        Group* group = NULL;
        Group* grouptarget = NULL;
        BattlegroundTypeId bg_typeid = BATTLEGROUND_AA;;
        uint8 arenatype = 0;

        if (!handler->getSelectedPlayer())
        {
            handler->PSendSysMessage("Cant find player.");
            handler->SetSentErrorMessage(true);
            return false;
        }
        else
            target = handler->getSelectedPlayer();

        if (target == handler->GetSession()->GetPlayer()) // check if player selected other player
        {
            handler->PSendSysMessage("You must select player.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (initiator = handler->GetSession()->GetPlayer()) // now check how big is player group and if target have group of same size
        {
            int members = 0;
            int targetmembers = 0;

            if (group = initiator->GetGroup()) // get initiator group
                members = group->GetMembersCount();

            if (grouptarget = target->GetGroup())
                targetmembers = grouptarget->GetMembersCount();

            if (targetmembers > 1 && !grouptarget->IsLeader(target->GetGUID()))
            {
                handler->PSendSysMessage("Selected target is not leader of party.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            // this is workaround before we get correct packets from client select gui
            std::string argstr = (char*) args;
            if (argstr == "ruins")
            {
                bg_typeid = BATTLEGROUND_RL;
                arenatype = ARENA_TYPE_5v5;
            }
            else if (argstr == "dalaran")
            {
                bg_typeid = BATTLEGROUND_DS;
                arenatype = ARENA_TYPE_5v5;
            }
            else if (argstr == "nagrand")
            {
                bg_typeid = BATTLEGROUND_NA;
                arenatype = ARENA_TYPE_5v5;
            }
            else if (argstr == "blades")
            {
                bg_typeid = BATTLEGROUND_BE;
                arenatype = ARENA_TYPE_5v5;
            }
            else if (argstr == "valor")
            {
                bg_typeid = BATTLEGROUND_RV;
                arenatype = ARENA_TYPE_5v5;
            }
            else if (argstr == "wsg")   bg_typeid = BATTLEGROUND_WS;
            else if (argstr == "eots")  bg_typeid = BATTLEGROUND_EY;
            else if (argstr == "twin")  bg_typeid = BATTLEGROUND_TP;
            else if (argstr == "ab")    bg_typeid = BATTLEGROUND_AB;
            else if (argstr == "bfg")   bg_typeid = BATTLEGROUND_BFG;
            else if (argstr == "")
            {
                bg_typeid = BATTLEGROUND_AA;
                arenatype = ARENA_TYPE_5v5;
            }
            else
            {
                handler->PSendSysMessage("Wrong parameter cant join");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        // target join arena
        if (!(arena = JoinPlayer(target, arena, arenatype, bg_typeid)))
        {
            handler->PSendSysMessage("Cant join wargame");
            handler->SetSentErrorMessage(true);
            return false;
        }
        // player join arena
        if (!JoinPlayer(initiator, arena, arenatype, bg_typeid))
        {
            if (target->WarGameData)
            {
                target->WarGameData->removeEvent->Execute(0, 0);
                delete (target->WarGameData->removeEvent);
                delete target->WarGameData;
                target->WarGameData = NULL;
            }

            handler->PSendSysMessage("Cant join wargame.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }
private:
    static Battleground* JoinPlayer(Player *player, Battleground *arena, uint8 arenatype, BattlegroundTypeId bg_typeid)
    {
        // ignore if we already in BG or BG queue
        if (player->InBattleground())
            return NULL;

        uint32 matchmakerRating = 0;

        //check existance
        Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bg_typeid);
        if (!bg)
        {
            sLog->outError(LOG_FILTER_BATTLEGROUND, "Battleground: template bg (all arenas) not found");
            return NULL;
        }

        BattlegroundTypeId bgTypeId = bg->GetTypeID();
        BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, arenatype);
        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), player->getLevel());
        if (!bracketEntry)
            return NULL;

        // check if already in queue
        if (player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return NULL;
        // check if has free queue slots
        if (!player->HasFreeBattlegroundQueueId())
            return NULL;

        if (Group* group = player->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* member = itr->getSource();

                if (!member)
                    continue;   // this should never happen

                uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);
                WorldPacket data;
                // send status packet (in queue)
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype);
                member->GetSession()->SendPacket(&data);
            }
        }
        else
        {
            uint32 queueSlot = player->AddBattlegroundQueueId(bgQueueTypeId);
            WorldPacket data;
            // send status packet (in queue)
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype);
            player->GetSession()->SendPacket(&data);
            sLog->outDebug(LOG_FILTER_BATTLEGROUND, "Battleground: player joined queue for arena, skirmish, bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, player->GetGUIDLow(), player->GetName());
        }

        sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, arenatype, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());

        GroupQueueInfo* ginfo = new GroupQueueInfo;
        ginfo->BgTypeId = bgTypeId;
        ginfo->ArenaType = arenatype;
        ginfo->ArenaTeamId = 0;
        ginfo->IsRated = 0;
        ginfo->IsInvitedToBGInstanceGUID = 0;
        ginfo->JoinTime = getMSTime();
        ginfo->RemoveInviteTime = 0;
        ginfo->Team = player->GetTeam();
        ginfo->ArenaTeamRating = 0;
        ginfo->ArenaMatchmakerRating = 0;
        ginfo->OpponentsTeamRating = 0;
        ginfo->OpponentsMatchmakerRating = 0;
        ginfo->Players.clear();

        //add players from group to ginfo
        if (player->GetGroup() && player->GetGroup()->GetMembersCount() > 1)
        {
            Group* group = player->GetGroup();
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* member = itr->getSource();

                if (!member)
                    continue;   // this should never happen

                PlayerQueueInfo* info = new PlayerQueueInfo;
                info->GroupInfo = ginfo;
                info->LastOnlineTime = getMSTime();
                // add the pinfo to ginfo's list
                ginfo->Players[member->GetGUID()] = info;
            }
        }
        else // just one player
        {
            PlayerQueueInfo* info = new PlayerQueueInfo;
            info->GroupInfo = ginfo;
            info->LastOnlineTime = getMSTime();
            ginfo->Players[player->GetGUID()] = info;
        }

        if (!arena) // if theres no arena create one
        {
            arena = sBattlegroundMgr->CreateNewBattleground(bgTypeId, bracketEntry, arenatype, true);
            arena->SetRated(false);
            arena->SetWarGame(true);

            if (Group* group = player->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* member = itr->getSource();

                    if (!member)
                        continue;   // this should never happen

                    member->WarGameData = new WarGameData;
                    member->WarGameData->bg = arena;
                    member->WarGameData->ginfo = ginfo;
                }
            }
            else // just one player
            {
                player->WarGameData = new WarGameData;
                player->WarGameData->bg = arena;
                player->WarGameData->ginfo = ginfo;
            }

            InviteGroupToBG(ginfo, arena, ALLIANCE);
        }
        else // if there is arena add other players into it
        {
            if (Group* group = player->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* member = itr->getSource();

                    if (!member)
                        continue;   // this should never happen

                    member->WarGameData = new WarGameData;
                    member->WarGameData->bg = arena;
                    member->WarGameData->ginfo = ginfo;
                }
            }
            else // just one player
            {
                player->WarGameData = new WarGameData;
                player->WarGameData->bg = arena;
                player->WarGameData->ginfo = ginfo;
            }

            InviteGroupToBG(ginfo, arena, HORDE);
            arena->StartBattleground();

            if (!sBattlegroundMgr->HasBattleground(arena))
                sBattlegroundMgr->AddBattleground(bg);
        }
        return arena;
    }

    static bool InviteGroupToBG(GroupQueueInfo* ginfo, Battleground* bg, uint32 side)
    {
        // set side if needed
        if (side)
            ginfo->Team = side;

        if (!ginfo->IsInvitedToBGInstanceGUID)
        {
            // not yet invited
            // set invitation
            ginfo->IsInvitedToBGInstanceGUID = bg->GetInstanceID();
            BattlegroundTypeId bgTypeId = bg->GetTypeID();
            BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, bg->GetArenaType());

            // set ArenaTeamId for rated matches
            if (bg->isArena() && bg->isRated())
                bg->SetArenaTeamIdForTeam(ginfo->Team, ginfo->ArenaTeamId);

            ginfo->RemoveInviteTime = getMSTime() + INVITE_ACCEPT_WAIT_TIME;

            // loop through the players
            for (std::map<uint64, PlayerQueueInfo*>::iterator itr = ginfo->Players.begin(); itr != ginfo->Players.end(); ++itr)
            {
                // get the player
                Player* player = ObjectAccessor::FindPlayer(itr->first);
                // if offline, skip him, this should not happen - player is removed from queue when he logs out
                if (!player)
                    continue;

                // set invited player counters
                bg->IncreaseInvitedCount(ginfo->Team);

                player->SetInviteForBattlegroundQueueType(bgQueueTypeId, ginfo->IsInvitedToBGInstanceGUID);

                BGQueueRemoveEvent* removeEvent = new BGQueueRemoveEvent(player->GetGUID(), ginfo->IsInvitedToBGInstanceGUID, bgTypeId, bgQueueTypeId, ginfo->RemoveInviteTime);
                player->WarGameData->removeEvent = removeEvent;

                WorldPacket data;

                uint32 queueSlot = player->GetBattlegroundQueueIndex(bgQueueTypeId);

                sLog->outDebug(LOG_FILTER_BATTLEGROUND, "Battleground: invited player %s (%u) to BG instance %u queueindex %u bgtype %u, I can't help it if they don't press the enter battle button.", player->GetName(), player->GetGUIDLow(), bg->GetInstanceID(), queueSlot, bg->GetTypeID());

                // send status packet
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME, 0, ginfo->ArenaType);
                player->GetSession()->SendPacket(&data);
            }
            return true;
        }
        return false;
    }
};

void AddSC_wargame_script()
{
    new wargame_commands();
}
/* Custom Shadowhack spectator */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "BattlegroundMgr.h"
#include "Log.h"
#include "Player.h"
#include "ArenaTeamMgr.h"

class spectator : public CreatureScript
{
    public: 
        spectator() : CreatureScript("arena_spectator") { }
        typedef std::list<std::string> StringList;
        std::vector<StringList*> twovtwo;
        std::vector<StringList*> threevthree;
    void CleanUp(Player * player)
    {
        for(std::vector<StringList*>::iterator itr = player->twovtwo.begin(); itr != player->twovtwo.end();)
        {
            (*itr)->clear();
            player->twovtwo.erase(itr);
            itr = player->twovtwo.begin();
        }
    }
    int8 Doscan()
    {
        const uint32 arenas[] = {BATTLEGROUND_DS, BATTLEGROUND_RV, BATTLEGROUND_RL, BATTLEGROUND_NA, BATTLEGROUND_BE, BATTLEGROUND_AA};
        twovtwo.clear();
        threevthree.clear();
        for (uint32 i = BATTLEGROUND_NA; i < BATTLEGROUND_BFG+1; ++i)
        {
            bool found = false;
            for (uint32 j = 0; j<6; ++j)
                if (arenas[j] == i)
                {
                    found = true;
                    break;
                }
            if (!found)
                continue;
            
            for (BattlegroundContainer::iterator itr = sBattlegroundMgr->bgDataStore[(BattlegroundTypeId)i].m_Battlegrounds.begin(); 
                itr != sBattlegroundMgr->bgDataStore[(BattlegroundTypeId)i].m_Battlegrounds.end();++itr)
                if (Battleground * bg = itr->second)
                {
                    uint32 team1 = bg->GetArenaTeamIdByIndex(0);
                    uint32 team2 = bg->GetArenaTeamIdByIndex(1);
                    StringList* team1s = new StringList();
                    StringList* team2s = new StringList();
                    typedef std::map<uint64, BattlegroundPlayer> BattlegroundPlayerMap;
                    ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(team1);
                    if(!at)
                        at = sArenaTeamMgr->GetArenaTeamById(team2);
                    if(!at)
                    {
                        // wtf disband teamz!!!
                        delete team1s;
                        delete team2s;
                        continue;
                    }
                    for (BattlegroundPlayerMap::iterator jtr = bg->m_Players.begin(); jtr != bg->m_Players.end(); ++jtr)
                    {
                        Player *plr = sObjectMgr->GetPlayerByLowGUID(jtr->first);
                        if (!plr)
                            continue;
                        int32 teamid = plr->GetArenaTeamId(at->GetSlot());
                        std::string name = plr->GetName();
                        if (team1 == teamid)
                        {
                            team1s->push_back(name);
                        }
                        else if (team2 == teamid)
                        {
                            team2s->push_back(name);
                        }
                    }
                    if (at->GetType() == 2) 
                    {
                        if (!team1s->empty())
                            twovtwo.push_back(team1s);
                        if (!team2s->empty())
                            twovtwo.push_back(team2s);
                    }
                    else if (at->GetType() == 3)
                    {
                        if (!team1s->empty())
                            threevthree.push_back(team1s);
                        if (!team2s->empty())
                        threevthree.push_back(team2s);
                    }
                }
        }
        return (!twovtwo.empty()) + (!threevthree.empty())*2;
    }

    void TeleportTo(Player * player, Player * pl)
    {
        if (!pl->InArena() || player->InBattlegroundQueue())
        {
            player->CLOSE_GOSSIP_MENU();         
            return;
        }
        player->KillPlayer();
        player->SetHealth(1);
        player->CastSpell(player, 8326, true);
        player->SetBattlegroundId(pl->GetBattlegroundId(), pl->GetBattlegroundTypeId());
        player->SetBattlegroundEntryPoint();

        // set and clear other
        player->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
        float x,y,z;
        x = pl->GetPositionX();
        y = pl->GetPositionY();
        z = pl->GetPositionZ()+0.2f;

        player->TeleportTo(pl->GetMapId(), x, y, z, player->GetAngle(pl), TELE_TO_GM_MODE);
        player->CastSpell(player, 8326, true);
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        CleanUp(player);
    }

    bool OnGossipHello(Player* player, Creature* pCreature)
    {
        bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;
 
        if (player->InBattlegroundQueue())
        {
            pCreature->MonsterWhisper(ru ? "Выйти с очередей на арены/бг!" : "Come up with queues at the arena/BG!", player->GetGUID(), true);
            player->CLOSE_GOSSIP_MENU();
            return false;
        }
        
        int8 var = Doscan();
        player->PlayerTalkClass->GetGossipMenu().ClearMenu();
        switch(var)
        {
            case 0:
            {
                //"Никого нет на арене, идите спать";
                player->ADD_GOSSIP_ITEM( 7, ru ? "Арена пуста." : "Arena is empty.", GOSSIP_SENDER_MAIN, 1224);
                break;
            }
            default:
            {
                //"Ввести никнейм"
                player->ADD_GOSSIP_ITEM_EXTENDED(0, ru ? "Введите ник для просмотра." : "Enter a nickname for view.", GOSSIP_SENDER_MAIN, 1001, "", 0, true);
                if (var & 1)
                {
                    player->ADD_GOSSIP_ITEM(0, "2x2", GOSSIP_SENDER_MAIN, 1002);
                }
                if (var & 2)
                {
                    player->ADD_GOSSIP_ITEM(0, "3x3", GOSSIP_SENDER_MAIN, 1003);
                }
                player->twovtwo = twovtwo;
            }
        }
        player->ADD_GOSSIP_ITEM( 4,  ru ? "Счастливого пути!" : "Fare you well!", GOSSIP_SENDER_MAIN, 1224);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
        return true;
    }
                
    bool OnGossipSelect(Player* player, Creature* pCreature, uint32 sender, uint32 action)
    {
        bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

        player->PlayerTalkClass->GetGossipMenu().ClearMenu();
        if (player->InBattlegroundQueue())
            return false;
        switch (action)
        {
            case 1224:
                player->CLOSE_GOSSIP_MENU();
                CleanUp(player);
                break;
            case 1225:
                CleanUp(player);
                OnGossipHello(player, pCreature);
                break;
            case 1003:
                player->twovtwo = threevthree;
            default:
            {
                if (action < 1000)
                {
                    //--string_iter;
                    if(player->twovtwo.size() && player->twovtwo[action]->size())
                        if (Player* pl = sObjectMgr->GetPlayerByLowGUID(sObjectMgr->GetPlayerGUIDByName(*player->twovtwo[action]->begin())))
                            TeleportTo(player, pl);
                }
                else
                {
                    int i = 0;
                    std::vector<StringList*>::iterator string_iter = player->twovtwo.begin();
                    if (action > 1300)
                        for (;action!=1300; --action)
                        {
                            ++string_iter;
                            ++i;
                        }
                    int j = 0;
                    bool moar = false;
                    for(; string_iter != player->twovtwo.end(); ++string_iter)
                    {
                        if (j == 9)
                        {
                            moar = true;
                            break;
                        }
                        if(!(*string_iter)->size())
                            continue;
                        StringList::iterator str_str_iter= (*string_iter)->begin();
                        std::string str = *str_str_iter;
                        while(++str_str_iter != (*string_iter)->end())
                        {
                            str+= ", ";
                            str+= *str_str_iter;
                        }
                        player->ADD_GOSSIP_ITEM(11, str, GOSSIP_SENDER_MAIN, (i++));
                        ++j;
                    }
                    if (moar)
                        player->ADD_GOSSIP_ITEM( 7, ru ? "Еще..." : "More...", GOSSIP_SENDER_MAIN, 1300+i);
                    player->ADD_GOSSIP_ITEM( 4, ru ? "Return" : "Return", GOSSIP_SENDER_MAIN, 1225);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
                    break;
                }

            }
        }
        return true;
    }
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            if (Player* pl = sObjectMgr->GetPlayerByLowGUID(sObjectMgr->GetPlayerGUIDByName(code)))
                TeleportTo(player, pl);
        }
        return true;
    }

    bool GossipSelect_spectator(Player *player, Creature *_Creature, uint32 sender, uint32 action )
    {
        // Main menu
        if (sender == GOSSIP_SENDER_MAIN)
        OnGossipSelect(player, _Creature, 0, action);

        return true;
    }
};

void AddSC_spectator()
{
    new spectator;
}
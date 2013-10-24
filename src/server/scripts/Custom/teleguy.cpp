#include "ScriptPCH.h"


class Teleguy : public CreatureScript
{
public:
    Teleguy() : CreatureScript("teleguy") {}
    
    long long int money;
    int costo;
    
    bool OnGossipHello(Player* player, Creature* _Creature)
	{
        if( player->GetTeam() == ALLIANCE )
        {
            player->ADD_GOSSIP_ITEM( 5, "Darnassus."                     , GOSSIP_SENDER_MAIN, 1203);
            player->ADD_GOSSIP_ITEM( 5, "Exodar."                        , GOSSIP_SENDER_MAIN, 1216);
            player->ADD_GOSSIP_ITEM( 5, "Stormwind."                     , GOSSIP_SENDER_MAIN, 1206);
            player->ADD_GOSSIP_ITEM( 5, "Ironforge."                     , GOSSIP_SENDER_MAIN, 1224);
            player->ADD_GOSSIP_ITEM( 5, "Shattrath City."                , GOSSIP_SENDER_MAIN, 1287);
            player->ADD_GOSSIP_ITEM( 5, "Dalaran."                       , GOSSIP_SENDER_MAIN, 1205);
            player->ADD_GOSSIP_ITEM( 5, "Nagrand."                       , GOSSIP_SENDER_MAIN, 1207);
            player->ADD_GOSSIP_ITEM( 7, "[Outdoor PvP] ->"               , GOSSIP_SENDER_MAIN, 5550);
        }
        else
        {
            player->ADD_GOSSIP_ITEM( 5, "Orgrimmar."                     , GOSSIP_SENDER_MAIN, 1215);
            player->ADD_GOSSIP_ITEM( 5, "Silvermoon."                    , GOSSIP_SENDER_MAIN, 1217);
            player->ADD_GOSSIP_ITEM( 5, "Undercity."                     , GOSSIP_SENDER_MAIN, 1213);
            player->ADD_GOSSIP_ITEM( 5, "Thunder Bluff."                 , GOSSIP_SENDER_MAIN, 1225);
            player->ADD_GOSSIP_ITEM( 5, "Shattrath City."                , GOSSIP_SENDER_MAIN, 1287);
            player->ADD_GOSSIP_ITEM( 5, "Dalaran."                       , GOSSIP_SENDER_MAIN, 1205);
            player->ADD_GOSSIP_ITEM( 5, "Nagrand."                       , GOSSIP_SENDER_MAIN, 1207);
            player->ADD_GOSSIP_ITEM( 7, "[Outdoor PvP] ->"               , GOSSIP_SENDER_MAIN, 5550);
        }
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
        return true;
    }
    
    bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
	{
        player->PlayerTalkClass->ClearMenus();
        if (sender == GOSSIP_SENDER_MAIN)
            SendDefaultMenu_teleguy(player, _Creature, action);
        return true;
    }
    
    void SendDefaultMenu_teleguy(Player *player, Creature *_Creature, uint32 action )
    {
        if(!player->getAttackers().empty())
        {
            player->CLOSE_GOSSIP_MENU();
            _Creature->MonsterSay("You are in combat!", LANG_UNIVERSAL, NULL);
            return;
        }

        switch(action)
        {
        case 5550: //Outdoor PvP
            player->ADD_GOSSIP_ITEM( 5, "Roing of Trials."              , GOSSIP_SENDER_MAIN, 1248);
            player->ADD_GOSSIP_ITEM( 5, "Circle of Blood."              , GOSSIP_SENDER_MAIN, 1249);
            player->ADD_GOSSIP_ITEM( 5, "The mauls."                    , GOSSIP_SENDER_MAIN, 1250);
            player->ADD_GOSSIP_ITEM( 5, "Gurubashi arena."              , GOSSIP_SENDER_MAIN, 1252);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
            break;
        case 1203: // Teleport to Darnassus
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(1, 9947.52f, 2482.73f, 1316.21f, 0.0f);
            break;
        case 1204: // Teleport to Transmogrification
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, -540.401f, 6875.22f, 163.15f, 0.0f);
            break;
        case 1206: // Teleport to Stormwind
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(0, -8960.14f, 516.266f, 96.3568f, 0.0f);
            break;
        case 1205: // Teleport to Dalaran
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(571, 5804.14f, 624.770f, 647.7670f, 1.64f);
            break;
        case 1207: // Teleport to Nagrand
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, -2504.31f, 6445.08f, 200.43f, 1.64f);
            break;
        case 1213: // Teleport to Undercity
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(0, 1819.71f, 238.79f, 60.5321f, 0.0f);
            break;
        case 1215: // Teleport to Orgrimmar
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(1, 1552.5f, -4420.66f, 8.94802f, 0.0f);
            break;
        case 1216: // Teleport to Exodar
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, -4170.330f, -12491.03f,  44.21f, 0.0f);
            break;
        case 1217: // Teleport to Silvermoon
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, 9338.74f, -7277.27f, 13.7895f, 0.0f);
            break;
        case 1222: //Teleport to Gnomeregan
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(0, -5163.43f,660.40f,348.28f,4.65f);
            break;
        case 1224: // Teleport to Ironforge
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(0, -4924.07f, -951.95f, 501.55f, 5.40f);
            break;
        case 1225: // Teleport to Thunder Bluff
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(1, -1586.59f, 172.33f, -7.32f, 0.0f);
            break;
            // Shattrath City
        case 1287:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, -1850.209961f, 5435.821777f, -10.961435f, 0.0f);
            break;
            // Roing of Trials
        case 1248:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, -2049.26f, 6662.82f, 13.06f, 0.0f);
            break;
            // Circle of Blood
        case 1249:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(530, 2839.43f, 5930.16f, 11.20f, 0.0f);
            break;
            // The mauls
        case 1250:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(1, -3761.24f, 1131.89f, 132.96f, 0.0f);
            break;
            // Duel Zone
        case 1251:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(1, 5465.16f, -3724.59f, 1593.44f, 0.0f);
            break;
            // Gurubashi arena
        case 1252:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(0, -13312.44f, 61.878f,  22.193f, 0.0f);
            break;
            // Exotic pets
        case 1253:
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(571, 8516.043945f, 791.895935f,  557.714844f, 0.0f);
            break;
        }
    }
};

void AddSC_teleguy()
{
new Teleguy();
}

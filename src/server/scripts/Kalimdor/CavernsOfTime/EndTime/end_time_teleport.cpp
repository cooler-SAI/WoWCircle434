#include "ScriptPCH.h"
#include "end_time.h"
#include "GameObjectAI.h"

enum InstanceTeleporeter
{
    START_TELEPORT          = 1,
    JAINA_TELEPORT          = 2,
	SYLVANAS_TELEPORT       = 3,
	TYRANDE_TELEPORT        = 4,
	BAINE_TELEPORT          = 5,
	MUROZOND_TELEPORT       = 6,
};

class go_end_time_teleport : public GameObjectScript
{
    public:
        go_end_time_teleport() : GameObjectScript("go_end_time_teleport") { }

        bool OnGossipHello(Player* pPlayer, GameObject* pGo)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Start.", GOSSIP_SENDER_MAIN, START_TELEPORT);
            
            if (InstanceScript* pInstance = pGo->GetInstanceScript())
            {
                /*std::list<uint32> echo_list;
                uint32 echo1 = pInstance->GetData(DATA_ECHO_1);
                uint32 echo2 = pInstance->GetData(DATA_ECHO_2);

                switch (echo1)
                {
                    case DATA_ECHO_OF_JAINA:
                        pInstance->SetData(DATA_JAINA_PICKED_STATE, IN_PROGRESS);
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Jaina.", GOSSIP_SENDER_MAIN, JAINA_TELEPORT);
                        break;
                    case DATA_ECHO_OF_BAINE:
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Baine.", GOSSIP_SENDER_MAIN, BAINE_TELEPORT);
                        break;
                    case DATA_ECHO_OF_TYRANDE:
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Tyrande.", GOSSIP_SENDER_MAIN, TYRANDE_TELEPORT);
                        break;
                    case DATA_ECHO_OF_SYLVANAS:
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Sylvanas.", GOSSIP_SENDER_MAIN, SYLVANAS_TELEPORT);
                        break;
                }

                if (pInstance->GetData(DATA_FIRST_ENCOUNTER) == DONE)
                {
                    switch (echo2)
                    {
                        case DATA_ECHO_OF_JAINA:
                            pInstance->SetData(DATA_JAINA_PICKED_STATE, IN_PROGRESS);
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Jaina.", GOSSIP_SENDER_MAIN, JAINA_TELEPORT);
                            break;
                        case DATA_ECHO_OF_BAINE:
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Baine.", GOSSIP_SENDER_MAIN, BAINE_TELEPORT);
                            break;
                        case DATA_ECHO_OF_TYRANDE:
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Tyrande.", GOSSIP_SENDER_MAIN, TYRANDE_TELEPORT);
                            break;
                        case DATA_ECHO_OF_SYLVANAS:
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Sylvanas.", GOSSIP_SENDER_MAIN, SYLVANAS_TELEPORT);
                            break;
                    }
                }

                if (pInstance->GetData(DATA_SECOND_ENCOUNTER) == DONE)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Murozond.", GOSSIP_SENDER_MAIN, MUROZOND_TELEPORT);
            */

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Tyrande.", GOSSIP_SENDER_MAIN, TYRANDE_TELEPORT);
                                       
                if (pInstance->GetBossState(DATA_ECHO_OF_TYRANDE) == DONE)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Sylvanas.", GOSSIP_SENDER_MAIN, SYLVANAS_TELEPORT);
                if (pInstance->GetBossState(DATA_ECHO_OF_SYLVANAS) == DONE)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Murozond.", GOSSIP_SENDER_MAIN, MUROZOND_TELEPORT);
            }
        
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo), pGo->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) 
		{
            //player->PlayerTalkClass->ClearMenus();
            if (!player->getAttackers().empty())
                return false;
            
            switch (action) 
		    {
                case START_TELEPORT:
                    player->TeleportTo(938, 3711.591064f, -375.388763f, 113.064049f, 2.235886f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            
                case JAINA_TELEPORT:
                    player->TeleportTo(938, 2997.370850f, 570.221863f, 25.307350f, 5.543086f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            
                case SYLVANAS_TELEPORT:
                    player->TeleportTo(938, 3833.332520f, 1101.351929f, 83.412987f, 3.786126f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            
                case TYRANDE_TELEPORT:
                    player->TeleportTo(938, 2948.708740f, 68.523956f, 9.076958f, 3.289821f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            
                case BAINE_TELEPORT:
                    player->TeleportTo(938, 4343.328613f, 1294.588135f, 147.503693f, 0.765022f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            
                case MUROZOND_TELEPORT:
                    player->TeleportTo(938, 4042.709717f, -351.774353f, 122.215546f, 4.328253f);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
            
            return true;
        }    
};

void AddSC_end_time_teleport()
{
    new go_end_time_teleport();
}
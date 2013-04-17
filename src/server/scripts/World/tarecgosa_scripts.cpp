#include "ScriptPCH.h"

/*######
## npc_anachronos
## entry 15192
######*/

#define GOSSIP_A_WRINKIE_IN_TIME "Yes, Anachronos. I am ready to witness your vision of the future."
#define ACTION_A_WRINKIE_IN_TIME 1001

#define GOSSIP_ON_A_WING_AND_A_PRAYER "Take the Timeless Eye."
#define ACTION_ON_A_WING_AND_A_PRAYER 1002

class npc_anachronos_15192 : public CreatureScript
{
    public:
        npc_anachronos_15192() : CreatureScript("npc_anachronos_15192") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->isQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(29134) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_A_WRINKIE_IN_TIME, GOSSIP_SENDER_MAIN, ACTION_A_WRINKIE_IN_TIME);
            else if (player->GetQuestStatus(29193) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ON_A_WING_AND_A_PRAYER, GOSSIP_SENDER_MAIN, ACTION_ON_A_WING_AND_A_PRAYER);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* pPlayer, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            if (action == ACTION_A_WRINKIE_IN_TIME)
            {
                pPlayer->KilledMonsterCredit(52605, 0);
            }
            else if (action == ACTION_ON_A_WING_AND_A_PRAYER)
            {
                uint32 curItemCount = pPlayer->GetItemCount(69238, false);
                if (curItemCount >= 1)
                    return false;
                
                ItemPosCountVec dest;
                uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49661, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = pPlayer->StoreNewItem(dest, 69238, true);
                    pPlayer->SendNewItem(item, 1, true, false);
                }
                else
                    return false;
            }

            return true;
        }
};

class npc_tarecgosa_52835 : public CreatureScript
{
    public:
        npc_tarecgosa_52835() : CreatureScript("npc_tarecgosa_52835") { }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetGUID());

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 action)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            return false;
        }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_tarecgosa_52835AI(pCreature);
        }

        struct npc_tarecgosa_52835AI : ScriptedAI
        {
            npc_tarecgosa_52835AI(Creature* pCreature) : ScriptedAI(pCreature) 
            {
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (me->GetDistance(who) >= 20.0f)
                    return;

                if (who->ToPlayer()->GetQuestStatus(29193) == QUEST_STATUS_INCOMPLETE)
                    who->ToPlayer()->KilledMonsterCredit(52832, 0);
            }
        };
};

void AddSC_tarecgosa_scripts()
{
    new npc_anachronos_15192();
    new npc_tarecgosa_52835();
};
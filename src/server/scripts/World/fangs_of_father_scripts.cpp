#include "ScriptPCH.h"

enum Spells
{
    // A Hidden Message
    SPELL_CHARGE_DECODER_RING       = 104480,
    SPELL_GIVE_PLAYER_SIGNED_CIPHER = 104569,
};

enum QuestItems
{
    ITEM_CHARGING_DECORDER_RING = 74749,
    ITEM_CHARGED_DECORDER_RING  = 74748,
};

enum Adds
{
    NPC_THAUMATURGE_RAFIR   = 57800,
};

class item_charging_decorder_ring : public ItemScript
{
    public:
        item_charging_decorder_ring () : ItemScript("item_charging_decorder_ring") { }

        bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/)
        {
            player->AddItem(ITEM_CHARGED_DECORDER_RING, 1);
            return true;
        }
};

void AddSC_fangs_of_father_scripts()
{
    new item_charging_decorder_ring();
}
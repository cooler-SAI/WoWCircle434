#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "Spell.h"
#include "ObjectMgr.h"
#include "PlayerDump.h"

// Options
enum eEnums
{
    CHANGE_ITEMBACK             = 0,
    EFIRALS                     = 38186,
    CHANGE_FACTION              = 1,
    CHANGE_RACE                 = 2,
    CHANGE_GENDER               = 3,
    RECOVERY_CHAR               = 4,
    EFIRALS_TRANS               = 5,

    CHANGE_FACTION_COUNT        = 200,
    CHANGE_RACE_COUNT           = 150,
    CHANGE_GENDER_COUNT         = 100,
    RECOVERY_CHAR_COUNT         = 225,
};

class char_moveto : public CreatureScript
{
public:
    char_moveto() : CreatureScript("char_moveto"){}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE account = '%u' AND guid NOT IN (SELECT guid FROM character_moveto WHERE account = '%u')", player->GetSession()->GetAccountId(), player->GetSession()->GetAccountId());
        if (!result || realmID != 22)
        {
            LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
            player->ADD_GOSSIP_ITEM(0, sObjectMgr->GetTrinityString(20027, loc_idx), GOSSIP_SENDER_MAIN, CHANGE_ITEMBACK);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,creature->GetGUID());
        }
        else
        {
            LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
            player->ADD_GOSSIP_ITEM(0, sObjectMgr->GetTrinityString(20028, loc_idx), GOSSIP_SENDER_MAIN, CHANGE_ITEMBACK);
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                std::string name = fields[1].GetString();

                player->ADD_GOSSIP_ITEM(0, name, GOSSIP_SENDER_MAIN, guid);
            }while (result->NextRow());
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        if(!player || !creature || sender != GOSSIP_SENDER_MAIN || !player->getAttackers().empty())
            return true;

        ChatHandler chH = ChatHandler(player);

        if(action > 0)
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE guid = '%u' AND account = '%u' AND guid NOT IN (SELECT guid FROM character_moveto WHERE guid = '%u')", action, player->GetSession()->GetAccountId(), action);
            if(!result)
            {
                player->CLOSE_GOSSIP_MENU(); return true;
            }

            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            std::string name = fields[1].GetString();
            uint32 account_id = player->GetSession()->GetAccountId();

            char fileName[50];
            char file[100];
            snprintf(fileName, 50, "%d_%d.log", account_id, guid);

            switch (PlayerDumpWriter().WriteDump(fileName, uint32(guid)))
            {
                case DUMP_SUCCESS:
                    chH.PSendSysMessage(LANG_COMMAND_EXPORT_SUCCESS);
                    break;
                default:
                    chH.PSendSysMessage(LANG_COMMAND_EXPORT_FAILED);
                    chH.SetSentErrorMessage(true);
                    return false;
            }

            char buffer[50];
            sprintf(buffer,"mv %s ../moveto/",fileName);
            system(buffer);
            snprintf(file, 100, "../moveto/%s", fileName);

            CharacterDatabase.PQuery("INSERT INTO character_moveto SET `account`='%u',`guid`='%u'", account_id, guid);
            CharacterDatabase.PQuery("INSERT INTO `trinx175characterskata`.custom_command SET `filename`='%s',`account`='%u'",file, account_id);
            CharacterDatabase.PQuery("UPDATE characters SET deleteInfos_Name = name, deleteInfos_Account = account, deleteDate = UNIX_TIMESTAMP(), name = '', account = 0 WHERE guid = %u", guid);
            if(player->GetGUIDLow() == guid)
                player->GetSession()->KickPlayer();
        }

        player->CLOSE_GOSSIP_MENU();
        return true;
    }
};

 void AddSC_perenos_charov()
 {
    new char_moveto();
 }
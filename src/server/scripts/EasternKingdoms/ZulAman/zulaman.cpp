#include "ScriptPCH.h"
#include "zulaman.h"

/*######
## npc_forest_frog
######*/

#define SPELL_REMOVE_AMANI_CURSE    43732
#define SPELL_PUSH_MOJO             43923
#define ENTRY_FOREST_FROG           24396

enum Spells
{
    // Amani'shi Tempest
    SPELL_THUNDERCLAP       = 44033,
    SPELL_CHAIN_LIGHTNING   = 97496,

    // Amani Eagle
    SPELL_TALON             = 43517,

    // Amani'shi Warrior
    SPELL_CHARGE            = 43519,
    SPELL_KICK              = 43518,
};

enum Events
{
    // Amani'shi Tempest
    EVENT_SUMMON_EAGLES     = 1,
    EVENT_SUMMON_MOBS       = 2,
    EVENT_THUNDERCLAP       = 3,
    EVENT_CHAIN_LIGHTNING   = 4,

    // Amani'shi Warrior
    EVENT_CHARGE1            = 5,
    EVENT_KICK              = 6,

    // Amani Eagle
    SPELL_TALON1             = 7,
};

enum Points
{
    POINT_TURN  = 1,
    POINT_DOWN  = 2,
};

enum Actions
{
    ACTION_START_EVENT  = 1,
};

const Position posTurnPoint = {232.83f, 1367.78f, 48.58f, 1.79f};
const Position posDownPoint = {227.75f, 1460.83f, 25.98f, 4.75f};
const Position posUpPoint   = {280.12f, 1380.63f, 49.35f, 3.46f}; 

#define GOSSIP_HOSTAGE1        "I am glad to help you."

static uint32 HostageEntry[] = {23790, 23999, 24024, 24001};
static uint32 ChestEntry[] = {186648, 187021, 186672, 186667};

class npc_zulaman_hostage : public CreatureScript
{
    public:
        npc_zulaman_hostage() : CreatureScript("npc_zulaman_hostage") { }
        
        struct npc_zulaman_hostageAI : public ScriptedAI
        {
            npc_zulaman_hostageAI(Creature* creature) : ScriptedAI(creature)
            {
                IsLoot = false;
            }

            bool IsLoot;
            uint64 PlayerGUID;

            void Reset() {}

            void EnterCombat(Unit* /*who*/) {}

            void JustDied(Unit* /*killer*/)
            {
                Player* player = Unit::GetPlayer(*me, PlayerGUID);
                if (player)
                    player->SendLoot(me->GetGUID(), LOOT_CORPSE);
            }

            void UpdateAI(const uint32 /*diff*/)
            {
                if (IsLoot)
                    DoCast(me, 7, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_zulaman_hostageAI(creature);
        }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->CLOSE_GOSSIP_MENU();

            if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return true;

            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            InstanceScript* instance = creature->GetInstanceScript();
            if (instance)
            {
                //uint8 progress = instance->GetData(DATA_CHESTLOOTED);
                float x, y, z;
                creature->GetPosition(x, y, z);
                uint32 entry = creature->GetEntry();
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (HostageEntry[i] == entry)
                    {
                        creature->SummonGameObject(ChestEntry[i], x-2, y, z, 0, 0, 0, 0, 0, 0);
                        break;
                    }
                }
            }
            return true;
        }
};

class go_strange_gong : public GameObjectScript
{
    public:
        go_strange_gong() : GameObjectScript("go_strange_gong") {}
        
        bool OnGossipHello(Player* pPlayer, GameObject* pGo)
        {
            if (InstanceScript* pInstance = pGo->GetInstanceScript())
            {
                pInstance->SetData(DATA_MAIN_GATE, 1);
            }
            return false;
        }
};

class npc_amanishi_tempest : public CreatureScript
{
    public:
        npc_amanishi_tempest() : CreatureScript("npc_amanishi_tempest") { }
        
        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_amanishi_tempestAI(pCreature);
        }

		struct npc_amanishi_tempestAI : public ScriptedAI
		{
			npc_amanishi_tempestAI(Creature* pCreature) : ScriptedAI(pCreature)
			{
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
			}

			EventMap events;

			void Reset()
			{
				events.Reset();
			}

			void EnterCombat(Unit* attacker)
			{
                events.ScheduleEvent(EVENT_THUNDERCLAP, urand(5000, 10000));
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(6000, 12000));
			}

			void UpdateAI(const uint32 diff)
			{
				if (!UpdateVictim())
					return;

				events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
                        case EVENT_THUNDERCLAP:
                            DoCastAOE(SPELL_THUNDERCLAP);
                            events.ScheduleEvent(EVENT_THUNDERCLAP, urand(14000, 18000));
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(pTarget, SPELL_CHAIN_LIGHTNING);
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 12000));
                            break;
					}
				}

				DoMeleeAttackIfReady();
			}
	 };
};

void AddSC_zulaman()
{
    new npc_zulaman_hostage();
    new go_strange_gong();
    new npc_amanishi_tempest();
    //new npc_amanishi_lookout();
    //new npc_amani_eagle();
    //new npc_amanishi_warrior();
}
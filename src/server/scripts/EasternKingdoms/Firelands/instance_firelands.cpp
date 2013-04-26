#include "ScriptPCH.h"
#include "firelands.h"
// areatrigger
// 6929 - quest1
// 6861 - near miniboss 1

#define MAX_ENCOUNTER 7

static const DoorData doordata[] = 
{
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_FIRE_WALL_BALEROC,   DATA_BALEROC,   DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_RAID_BRIDGE_FORMING, DATA_BALEROC,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_STICKY_WEB,          DATA_BETHTILAC, DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {0, 0, DOOR_TYPE_ROOM, BOUNDARY_NONE},
};

class instance_firelands : public InstanceMapScript
{
    public:
        instance_firelands() : InstanceMapScript("instance_firelands", 720) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_firelands_InstanceMapScript(map);
        }

        struct instance_firelands_InstanceMapScript : public InstanceScript
        {
            instance_firelands_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);
                uiShannoxGUID = 0;
                uiRiplimbGUID = 0;
                uiRagefaceGUID = 0;
                uiRhyolithHealth = 0;
            }

            void OnPlayerEnter(Player* pPlayer)
            {
                if (!uiTeamInInstance)
				    uiTeamInInstance = pPlayer->GetTeam();
            }

            void OnCreatureCreate(Creature* pCreature)
            {
                switch (pCreature->GetEntry())
                {
                    case NPC_SHANNOX:
                        uiShannoxGUID = pCreature->GetGUID();
                        break;
                    case NPC_RIPLIMB:
                        uiRiplimbGUID = pCreature->GetGUID();
                        break;
                    case NPC_RAGEFACE:
                        uiRagefaceGUID = pCreature->GetGUID();
                        break;
                    default:
                        break;
                }
		    }

            void OnGameObjectCreate(GameObject* pGo)
            {
                switch (pGo->GetEntry())
                {
                    case GO_FIRE_WALL_BALEROC:
                    case GO_STICKY_WEB:
                    case GO_RAID_BRIDGE_FORMING:
                    case GO_BRIDGE_OF_RHYOLITH:
                        AddDoor(pGo, true);
                        break;
                }
		    }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_RHYOLITH_HEALTH_SHARED)
                    uiRhyolithHealth = data;
		    }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_RHYOLITH_HEALTH_SHARED)
                    return uiRhyolithHealth;
			    return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_SHANNOX: return uiShannoxGUID;
                    case DATA_RIPLIMB: return uiRiplimbGUID;
                    case DATA_RAGEFACE: return uiRagefaceGUID;
                    default: return 0;
                }
                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
			    if (!InstanceScript::SetBossState(type, state))
				    return false;

			    return true;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "F L " << GetBossSaveData();

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in)
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'F' && dataHead2 == 'L')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
				    {
					    uint32 tmpState;
					    loadStream >> tmpState;
					    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
						    tmpState = NOT_STARTED;
					    SetBossState(i, EncounterState(tmpState));
				    }} else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;
                uint32 uiRhyolithHealth;
                uint64 uiShannoxGUID;
                uint64 uiRiplimbGUID;
                uint64 uiRagefaceGUID;
        };
};

void AddSC_instance_firelands()
{
    new instance_firelands();
}

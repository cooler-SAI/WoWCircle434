#include "ScriptPCH.h"
#include "well_of_eternity.h"

#define MAX_ENCOUNTER 3

static const DoorData doordata[] = 
{
    {0, 0, DOOR_TYPE_ROOM, BOUNDARY_NONE},
};

class instance_well_of_eternity : public InstanceMapScript
{
public:
    instance_well_of_eternity() : InstanceMapScript("instance_well_of_eternity", 939) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_well_of_eternity_InstanceMapScript(map);
    }

    struct instance_well_of_eternity_InstanceMapScript : public InstanceScript
    {
        instance_well_of_eternity_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetBossNumber(MAX_ENCOUNTER);
            LoadDoorData(doordata);
        }

        void OnPlayerEnter(Player* pPlayer)
        {
            if (!uiTeamInInstance)
				uiTeamInInstance = pPlayer->GetTeam();
        }

        void OnCreatureCreate(Creature* pCreature)
        {
		}

        void OnGameObjectCreate(GameObject* pGo)
        {
		}

        void OnGameObjectRemove(GameObject* pGo)
		{
		}

        void SetData(uint32 type, uint32 data)
        {
		}

        uint32 GetData(uint32 type)
        {
			return 0;
        }

        uint64 GetData64(uint32 type)
        {
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
            saveStream << "W o T " << GetBossSaveData();

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

            char dataHead1, dataHead2, dataHead3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> dataHead3;

            if (dataHead1 == 'W' && dataHead2 == 'o' && dataHead3 == 'T')
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
           
    };
};

void AddSC_instance_well_of_eternity()
{
	   new instance_well_of_eternity();
}
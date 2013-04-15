#include "ScriptPCH.h"
#include "end_time.h"

#define MAX_ENCOUNTER 5

static const DoorData doordata[] = 
{
    {0, 0, DOOR_TYPE_ROOM, BOUNDARY_NONE},
};

class instance_end_time : public InstanceMapScript
{
    public:
        instance_end_time() : InstanceMapScript("instance_end_time", 938) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_end_time_InstanceMapScript(map);
        }

        struct instance_end_time_InstanceMapScript : public InstanceScript
        {
            instance_end_time_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);
                uiTeamInInstance = 0;
                uiFragmentsCollected = 0;
                uiJainaPickedState = NOT_STARTED;
                RandEchos();
                first_encounter = 0;
                second_encounter = 0;
                uiMurozondCacheGUID = 0;
            }

            void RandEchos()
            {
                std::list<uint32> echo_list;
                for (uint8 i = 0; i < 4; ++i)
                    echo_list.push_back(i);
                first_echo = Trinity::Containers::SelectRandomContainerElement(echo_list);
                echo_list.remove(first_echo);
                second_echo = Trinity::Containers::SelectRandomContainerElement(echo_list);
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
                switch (pGo->GetEntry())
                {
                    case MUROZOND_CACHE:
                        uiMurozondCacheGUID = pGo->GetGUID();
                        break;
                    default:
                        break;
                }
		    }

            void SetData(uint32 type,uint32 data)
            {
                switch(type)
                {
                    case DATA_JAINA_PICKED_STATE:
                        switch (data)
                        {
                            case IN_PROGRESS:
                                DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, 1);
                                break;
                            case DONE:
                                DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, 0);
                                break;
                        }
                        uiJainaPickedState = data;
			            break;
                }
             
                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type)
            {
                switch(type)
                {
                    case DATA_JAINA_PICKED_STATE:
                        return uiJainaPickedState;
                    case DATA_ECHO_1:
                        return first_echo;
                    case DATA_ECHO_2:
                        return second_echo;
                    case DATA_FIRST_ENCOUNTER:
                        return first_encounter;
                    case DATA_SECOND_ENCOUNTER:
                        return second_encounter;
                }
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

                switch (type)
                {
                    case DATA_MUROZOND:
                        if (state == DONE)
                            DoRespawnGameObject(uiMurozondCacheGUID, DAY);
                        break;
                    case DATA_ECHO_OF_JAINA:
                    case DATA_ECHO_OF_BAINE:
                    case DATA_ECHO_OF_TYRANDE:
                    case DATA_ECHO_OF_SYLVANAS:
                        if (state == DONE)
                        {
                            if (first_encounter != DONE)
                                first_encounter = DONE;
                            else
                                second_encounter = DONE;

                            SaveToDB();
                        }
                        break;
                }

			    return true;
            }

            void FillInitialWorldStates(WorldPacket& data)
            {
                data << uint32(WORLDSTATE_SHOW_FRAGMENTS) << uint32(uiJainaPickedState == IN_PROGRESS);
                data << uint32(WORLDSTATE_FRAGMENTS_COLLECTED) << uint32(uiFragmentsCollected);
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "E T " << GetBossSaveData() 
                    << first_echo  << ' ' << second_echo << ' ' 
                    << first_encounter << ' ' << second_encounter << ' '
                    << uiJainaPickedState << ' ';

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

                if (dataHead1 == 'E' && dataHead2 == 'T')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
				    {
					    uint32 tmpState;
					    loadStream >> tmpState;
					    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
						    tmpState = NOT_STARTED;
					    SetBossState(i, EncounterState(tmpState));
				    }

                    uint32 temp_echo1 = 0;
                    loadStream >> temp_echo1;
                    first_echo = temp_echo1;

                    uint32 temp_echo2 = 0;
                    loadStream >> temp_echo2;
                    second_echo = temp_echo2;

                    uint32 temp_enc1 = 0;
                    loadStream >> temp_enc1;
                    first_encounter = temp_enc1;

                    uint32 temp_enc2 = 0;
                    loadStream >> temp_enc2;
                    second_encounter = temp_enc2;

                    uint32 temp = 0;
                    loadStream >> temp;
                    uiJainaPickedState = temp ? DONE : NOT_STARTED;

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;
                uint32 uiFragmentsCollected;
                uint32 uiJainaPickedState;
                uint32 first_echo;
                uint32 second_echo;
                uint32 first_encounter;
                uint32 second_encounter;
                uint64 uiMurozondCacheGUID;
               
        };
};

void AddSC_instance_end_time()
{
	   new instance_end_time();
}
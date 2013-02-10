#include "ScriptPCH.h"
#include "shadowfang_keep.h"

#define MAX_ENCOUNTER              5

class instance_shadowfang_keep : public InstanceMapScript
{
public:
    instance_shadowfang_keep() : InstanceMapScript("instance_shadowfang_keep", 33) { }

    InstanceScript* GetInstanceScript(InstanceMap* pMap) const
    {
        return new instance_shadowfang_keep_InstanceMapScript(pMap);
    }

    struct instance_shadowfang_keep_InstanceMapScript : public InstanceScript
    {
        instance_shadowfang_keep_InstanceMapScript(Map* pMap) : InstanceScript(pMap) {Initialize();};

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string str_data;

        uint64 uiAshburyGUID;
        uint64 uiSilverlaineGUID;
        uint64 uiSpringvaleGUID;
        uint64 uiValdenGUID;
        uint64 uiGodfreyGUID;

        uint64 DoorCourtyardGUID;
        uint64 DoorSorcererGUID;
        uint64 DoorArugalGUID;

        uint32 teamInInstance;

        void Initialize()
        {
            memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

            uiAshburyGUID = 0;

            DoorCourtyardGUID = 0;
            DoorSorcererGUID = 0;
            DoorArugalGUID = 0;

            teamInInstance = 0;
        }

        void OnPlayerEnter(Player* player)
        {
            if (!teamInInstance)
                    teamInInstance = player->GetTeam();
        }

        void OnCreatureCreate(Creature* pCreature)
        {
            switch(pCreature->GetEntry())
            {
            case NPC_BELMONT:
                if (teamInInstance == ALLIANCE)
                    pCreature->UpdateEntry(NPC_IVAR, ALLIANCE);
                break;
            case NPC_GUARD_HORDE1:
                if (teamInInstance == ALLIANCE)
                    pCreature->UpdateEntry(NPC_GUARD_ALLY, ALLIANCE);
                break;
            case NPC_GUARD_HORDE2:
                if (teamInInstance == ALLIANCE)
                    pCreature->UpdateEntry(NPC_GUARD_ALLY, ALLIANCE);
                break;
            case NPC_ASHBURY:
                uiAshburyGUID = pCreature->GetGUID();
                break;
            case NPC_SILVERLAINE:
                uiSilverlaineGUID = pCreature->GetGUID();
                break;
            case NPC_SPRINGVALE:
                uiSpringvaleGUID = pCreature->GetGUID();
                break;
            case NPC_VALDEN:
                uiValdenGUID = pCreature->GetGUID();
                break;
            case NPC_GODFREY:
                uiGodfreyGUID = pCreature->GetGUID();
                break;
            }
        }

        void OnGameObjectCreate(GameObject* pGo)
        {
            switch(pGo->GetEntry())
            {
                case GO_COURTYARD_DOOR:
                    DoorCourtyardGUID = pGo->GetGUID();
                    if (m_auiEncounter[0] == DONE)
                        HandleGameObject(DoorCourtyardGUID, true, pGo);
                    break;
                case GO_SORCERER_DOOR:
                    DoorSorcererGUID = pGo->GetGUID();
                    if (m_auiEncounter[3] == DONE)
                        HandleGameObject(DoorSorcererGUID, true, pGo);
                    break;
                case GO_ARUGAL_DOOR:
                    DoorArugalGUID = pGo->GetGUID();
                    if (m_auiEncounter[3] == DONE)
                        HandleGameObject(DoorArugalGUID, true, pGo);
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case DATA_ASHBURY:
                    m_auiEncounter[0] = data;
                    if (data == DONE)
                        if (GameObject* courtyarddoor = instance->GetGameObject(DoorCourtyardGUID))
                            HandleGameObject(DoorCourtyardGUID, true, courtyarddoor);
                    break;
                case DATA_SILVERLAINE:
                    m_auiEncounter[1] = data;
                    break;
                case DATA_SPRINGVALE:
                    m_auiEncounter[2] = data;
                    break;
                case DATA_VALDEN:
                    m_auiEncounter[3] = data;
                    if (data == DONE)
                    {
                        if (GameObject* sorcererdoor = instance->GetGameObject(DoorSorcererGUID))
                            HandleGameObject(DoorSorcererGUID, true, sorcererdoor);
                        if (GameObject* arugaldoor = instance->GetGameObject(DoorArugalGUID))
                            HandleGameObject(DoorArugalGUID, true, arugaldoor);
                    }
                    break;
                case DATA_GODFREY:
                    m_auiEncounter[4] = data;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4];

                str_data = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
            case DATA_TEAM:
                return teamInInstance;
            case DATA_ASHBURY:
                return m_auiEncounter[0];
            case DATA_SILVERLAINE:
                return m_auiEncounter[1];
            case DATA_SPRINGVALE:
                return m_auiEncounter[2];
            case DATA_VALDEN:
                return m_auiEncounter[3];
            case DATA_GODFREY:
                return m_auiEncounter[4];
            }
            return 0;
        }

        uint64 GetData64(uint32 type)
        {
            switch (type)
            {
            case DATA_ASHBURY: return uiAshburyGUID;
            case DATA_SILVERLAINE: return uiSilverlaineGUID;
            case DATA_SPRINGVALE: return uiSpringvaleGUID;
            case DATA_VALDEN: return uiValdenGUID;
            case DATA_GODFREY: return uiGodfreyGUID;
            case DATA_COURTYARD_DOOR: return DoorCourtyardGUID;
            case DATA_SORCERER_DOOR: return DoorSorcererGUID;
            case DATA_ARUGAL_DOOR: return DoorArugalGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
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

            std::istringstream loadStream(in);
            loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4];

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            {
                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;
            }

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

};


void AddSC_instance_shadowfang_keep()
{
    new instance_shadowfang_keep();
}
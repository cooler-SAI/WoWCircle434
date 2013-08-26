#include "ScriptPCH.h"
#include "dragon_soul.h"

#define MAX_ENCOUNTER 8

class instance_dragon_soul : public InstanceMapScript
{
    public:
        instance_dragon_soul() : InstanceMapScript("instance_dragon_soul", 967) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_dragon_soul_InstanceMapScript(map);
        }

        struct instance_dragon_soul_InstanceMapScript : public InstanceScript
        {
            instance_dragon_soul_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);

                uiMorchokGUID = 0;
                uiKohcromGUID = 0;
                uiZonozzGUID = 0;
                uiHagaraGUID = 0;
                uiUltraxionGUID = 0;

                uiAllianceShipGUID = 0;
                uiSwayzeGUID = 0;
                uiReevsGUID = 0;

                memset(uiLesserCacheofTheAspects, 0, sizeof(uiLesserCacheofTheAspects));

                bHagaraEvent = 0;
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
                    case NPC_MORCHOK:
                        uiMorchokGUID = pCreature->GetGUID();
                        break;
                    case NPC_KOHCROM:
                        uiKohcromGUID = pCreature->GetGUID();
                        break;
                    case NPC_ZONOZZ:
                        uiZonozzGUID = pCreature->GetGUID();
                        break;
                    case NPC_HAGARA:
                        uiHagaraGUID = pCreature->GetGUID();
                        break;
                    case NPC_ULTRAXION:
                        uiUltraxionGUID = pCreature->GetGUID();
                        break;
                    case NPC_SKY_CAPTAIN_SWAYZE:
                        if (pCreature->GetPositionZ() > 200.0f)
                        {
                            uiSwayzeGUID = pCreature->GetGUID();
                            if (GetBossState(DATA_ULTRAXION) == DONE)
                                pCreature->SetVisible(true);
                            else
                                pCreature->SetVisible(false);
                        }
                        break;
                    case NPC_KAANU_REEVS:
                        if (pCreature->GetPositionZ() > 200.0f)
                        {
                            uiReevsGUID = pCreature->GetGUID();
                            if (GetBossState(DATA_ULTRAXION) == DONE)
                                pCreature->SetVisible(true);
                            else
                                pCreature->SetVisible(false);
                        }
                        break;
                    case NPC_TRAVEL_TO_WYRMREST_TEMPLE:
                    case NPC_TRAVEL_TO_EYE_OF_ETERNITY:
                    case NPC_TRAVEL_TO_WYRMREST_BASE:
                    case NPC_TRAVEL_TO_WYRMREST_SUMMIT:
                        teleportGUIDs.push_back(pCreature->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* pCreature)
            {
                if (pCreature->GetEntry() == NPC_ULTRAXION)
                    uiUltraxionGUID = 0;
            }

            void OnGameObjectCreate(GameObject* pGo)
            {
                switch (pGo->GetEntry())
                {
                    case GO_LESSER_CACHE_OF_THE_ASPECTS_10N:
                        uiLesserCacheofTheAspects[0] = pGo->GetGUID();
                        break;
                    case GO_LESSER_CACHE_OF_THE_ASPECTS_25N:
                        uiLesserCacheofTheAspects[1] = pGo->GetGUID();
                        break;
                    case GO_LESSER_CACHE_OF_THE_ASPECTS_10H:
                        uiLesserCacheofTheAspects[2] = pGo->GetGUID();
                        break;
                    case GO_LESSER_CACHE_OF_THE_ASPECTS_25H:
                        uiLesserCacheofTheAspects[3] = pGo->GetGUID();
                        break;
                    case GO_ALLIANCE_SHIP:
                        uiAllianceShipGUID = pGo->GetGUID();
                        if (GetBossState(DATA_ULTRAXION) == DONE)
                            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                        pGo->UpdateObjectVisibility();
                        break;
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_MORCHOK: return uiMorchokGUID;
                    case DATA_KOHCROM: return uiKohcromGUID;
                    case DATA_ZONOZZ: return uiZonozzGUID;
                    case DATA_HAGARA: return uiHagaraGUID;
                    case DATA_ULTRAXION: return uiUltraxionGUID;
                    case DATA_LESSER_CACHE_10N: return uiLesserCacheofTheAspects[0];
                    case DATA_LESSER_CACHE_25N: return uiLesserCacheofTheAspects[1];
                    case DATA_LESSER_CACHE_10H: return uiLesserCacheofTheAspects[2];
                    case DATA_LESSER_CACHE_25H: return uiLesserCacheofTheAspects[3];
                    case DATA_SWAYZE: return uiSwayzeGUID;
                    case DATA_REEVS: return uiReevsGUID;
                    case DATA_ALLIANCE_SHIP: return uiAllianceShipGUID;
                    default: return 0;
                }
                return 0;
            }

            void SetData(uint32 type, uint32 data)
            {   
                if (type == DATA_HAGARA_EVENT)
                {
                    bHagaraEvent = data;
                    SaveToDB();
                }
            }
            
            uint32 GetData(uint32 type)
            {
                if (type == DATA_HAGARA_EVENT)
                    return bHagaraEvent;

                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state == IN_PROGRESS)
                {
                    if (!teleportGUIDs.empty())
                        for (std::vector<uint64>::const_iterator itr = teleportGUIDs.begin(); itr != teleportGUIDs.end(); ++itr)
                            if (Creature* pTeleport = instance->GetCreature((*itr)))
                            {
                                pTeleport->RemoveAura(SPELL_TELEPORT_VISUAL_ACTIVE);
                                pTeleport->CastSpell(pTeleport, SPELL_TELEPORT_VISUAL_DISABLED, true);
                            }
                }
                else
                {
                    if (!teleportGUIDs.empty())
                        for (std::vector<uint64>::const_iterator itr = teleportGUIDs.begin(); itr != teleportGUIDs.end(); ++itr)
                            if (Creature* pTeleport = instance->GetCreature((*itr)))
                            {
                                pTeleport->RemoveAura(SPELL_TELEPORT_VISUAL_DISABLED);
                                pTeleport->CastSpell(pTeleport, SPELL_TELEPORT_VISUAL_ACTIVE, true);
                            }
                }


                return true;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "D S " << GetBossSaveData() << bHagaraEvent << " ";

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

                if (dataHead1 == 'D' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                    
                    uint32 tmpEvent;
                    loadStream >> tmpEvent;
                    if (tmpEvent != DONE) 
                        tmpEvent = NOT_STARTED;
                    bHagaraEvent = tmpEvent;

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;

                uint64 uiMorchokGUID;
                uint64 uiKohcromGUID;
                uint64 uiZonozzGUID;
                uint64 uiHagaraGUID;
                uint64 uiUltraxionGUID;
                uint64 uiAllianceShipGUID;
                uint64 uiSwayzeGUID;
                uint64 uiReevsGUID;

                uint64 uiLesserCacheofTheAspects[4];

                std::vector<uint64> teleportGUIDs;

                uint32 bHagaraEvent;
               
        };
};

void AddSC_instance_dragon_soul()
{
    new instance_dragon_soul();
}
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "blackwing_lair.h"

class instance_blackwing_lair : public InstanceMapScript
{
    public:
        instance_blackwing_lair() : InstanceMapScript("instance_blackwing_lair", 469) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_blackwing_lair_InstanceMapScript(map);
        }

        struct instance_blackwing_lair_InstanceMapScript : public InstanceScript
        {
            instance_blackwing_lair_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                uiNefarian = 0;
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_NEFARIAN)
                {
                    uiNefarian = data;
                    if (data == DONE)
                        SaveToDB();
                }
		    }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_NEFARIAN)
                    return uiNefarian;
			    return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "B L " << uiNefarian << ' ';

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

                if (dataHead1 == 'B' && dataHead2 == 'L')
                {
                    uint32 tempEvent = 0;
                    loadStream >> tempEvent;
                    uiNefarian = (tempEvent != DONE ? 0 : DONE);

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiNefarian;
        };
};

void AddSC_instance_blackwing_lair()
{
    new instance_blackwing_lair();
}
#include "ScriptPCH.h"
#include "blackrock_caverns.h"

class blackrock_caverns_teleport : public CreatureScript
{
public:
    blackrock_caverns_teleport() : CreatureScript("blackrock_caverns_teleport") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new blackrock_caverns_teleport_AI(creature);
    }

    struct blackrock_caverns_teleport_AI : public CreatureAI
    {
        blackrock_caverns_teleport_AI(Creature* creature) : CreatureAI(creature) { }

        void OnSpellClick(Unit* clicker)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
            if (instance->GetData(DATA_KARSH) != DONE)
                return;

            if (me->GetDBTableGUIDLow() == 327777)
                clicker->NearTeleportTo(284.838f, 840.364f, 95.920f, 3.881242f, false);
            else if (me->GetDBTableGUIDLow() == 327776)
                clicker->NearTeleportTo(233.555f, 1128.875f, 205.569f, 3.551373f, false);
        }

        void UpdateAI(uint32 const diff) { }
    };
};

void AddSC_blackrock_caverns_teleport()
{
    new blackrock_caverns_teleport();
}
#include "BattlefieldTB.h"
#include "SpellAuras.h"
#include "GroupMgr.h"

bool BattlefieldTB::SetupBattlefield()
{
    m_TypeId                     = BATTLEFIELD_TB;    //View enum BattlefieldTypes
    m_BattleId                   = BATTLEFIELD_BATTLEID_TB;
    m_MaxPlayer                  = sWorld->getIntConfig(CONFIG_TOLBARAD_PLR_MAX);
    m_IsEnabled                  = sWorld->getBoolConfig(CONFIG_TOLBARAD_ENABLE);
    m_MinPlayer                  = sWorld->getIntConfig(CONFIG_TOLBARAD_PLR_MIN);
    m_MinLevel                   = sWorld->getIntConfig(CONFIG_TOLBARAD_PLR_MIN_LVL);
    m_BattleTime                 = sWorld->getIntConfig(CONFIG_TOLBARAD_BATTLETIME) * 60 * 1000;   // Time of battle (in ms)
    m_NoWarBattleTime            = sWorld->getIntConfig(CONFIG_TOLBARAD_NOBATTLETIME) * 60 * 1000; // Time between to battle (in ms)


    return true;
}

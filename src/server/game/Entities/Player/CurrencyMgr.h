#ifndef _CURRENCYMGR_H
#define _CURRENCYMGR_H

#include "Player.h"
#include "EventProcessor.h"

#define DEFAULT_RATED_BATTLEGROUND_CAP 1650
#define DEFAULT_ARENA_CAP 1350
#define DEFAULT_BATTLEGROUND_CAP 1650 // need rated bg to use it
#define MAXIMUM_ARENA_CAP 2700
#define MAXIMUM_BATTLEGROUND_CAP 3200 // (it is arena cap * 0.22)

struct CurrencyCap
{
    uint16 highestArenaRating;
    uint16 highestRBgRating;
    uint16 currentArenaCap;
    uint16 currentRBgCap;
    uint8 requireReset;
};

typedef std::map<uint32, CurrencyCap> PlayerCurrencyCapMap;

class CurrencyMgr
{
    friend class ACE_Singleton<CurrencyMgr, ACE_Null_Mutex>;
    friend class CapResetEvent;
    CurrencyMgr();
    ~CurrencyMgr();

public:
    void LoadPlayersCurrencyCap();

    bool IsHaveCap(uint32 lowGuid) const
    {
        PlayerCurrencyCapMap::const_iterator itr = _capValuesStorage.find(lowGuid);
        return itr != _capValuesStorage.end();
    }

    void UpdateCurrencyCap(uint32 lowGuid, uint16 arenaRating = 0, uint16 RBgRating = 0, uint16 arenaCap = DEFAULT_ARENA_CAP, uint16 rbgCap = 0, uint8 reset = 0);
    void UpdateCurrencyCap(CurrencyCap capData, uint32 lowGuid);
    void CalculatingCurrencyCap(uint16 &rating, bool ratedBattleground = false);
    
    float GetPrecision(CurrencyTypesEntry const* currency) const;
    
    CurrencyCap* GetCurrencyCapData(uint32 lowGuid);

    PlayerCurrencyCapMap& getCapMap() { return _capValuesStorage; }
    PlayerCurrencyCapMap::iterator getCapBegin() { return _capValuesStorage.begin(); }
    PlayerCurrencyCapMap::iterator getCapEnd() { return _capValuesStorage.end(); }
    PlayerCurrencyCapMap::iterator getLastItr() { return lastItr; }

    uint32 getFirstGuid() { return _capValuesStorage.begin()->first; }
    uint32 getLastGuid() { return lastGuid; }

    bool capMapIsEmpty() { return _capValuesStorage.empty(); }
    void UpdateEvents(uint32 diff);
    void ResetCurrencyCapToAllPlayers();

    void RestoreResettingCap();
    bool CheckIfNeedRestore();

    void setLastItr(PlayerCurrencyCapMap::iterator itr) { lastItr = itr; }
    void setLastGuid(uint32 lowGuid, bool save = false);
    void DeleteRestoreData();

    bool IsPrecisionAreConsidered(uint32 currencyId) const;

protected:
    PlayerCurrencyCapMap _capValuesStorage; 

private:
    EventProcessor m_events;
    PlayerCurrencyCapMap::iterator lastItr;
    uint32 lastGuid;
};

/*
    This class is used to reset currency week cap in parts
*/

class CapResetEvent : public BasicEvent
{
public:
    CapResetEvent(uint32 lastGuid, uint16 part): m_lastGuid(lastGuid), m_part(part) {}
    virtual ~CapResetEvent() {}

    virtual bool Execute(uint64 e_time, uint32 p_time);
    virtual void Abort(uint64 e_time);

private:
    uint32 m_lastGuid;
    uint16 m_part;
};

#define sCurrencyMgr ACE_Singleton<CurrencyMgr, ACE_Null_Mutex>::instance()

#endif
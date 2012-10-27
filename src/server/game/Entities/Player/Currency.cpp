#include "Player.h"

void Player::SendNewCurrency(uint32 id) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
        return;

    ByteBuffer currencyData;
    WorldPacket packet(SMSG_INIT_CURRENCY, 4 + (5*4 + 1));
    packet.WriteBits(1, 23);

    CurrencyTypesEntry const* entry = sCurrencyTypesStore.LookupEntry(id);
    if (!entry) // should never happen
        return;

    uint32 weekCount = itr->second.weekCount / entry->GetPrecision();
    uint32 weekCap = _GetCurrencyWeekCap(entry) / entry->GetPrecision();

    packet.WriteBit(weekCount);
    packet.WriteBits(itr->second.flags, 4);
    packet.WriteBit(weekCap);
    packet.WriteBit(0);     // season total earned

    currencyData << uint32(itr->second.totalCount / entry->GetPrecision());
    if (weekCap)
        currencyData << uint32(weekCap);

    //if (seasonTotal)
    //    currencyData << uint32(seasonTotal);

    currencyData << uint32(entry->ID);
    if (weekCount)
        currencyData << uint32(weekCount);

    packet.FlushBits();
    packet.append(currencyData);
    GetSession()->SendPacket(&packet);
}

void Player::SendCurrencies() const
{
    ByteBuffer currencyData;

    int32 size = int32(_currencyStorage.size());
    WorldPacket packet(SMSG_INIT_CURRENCY, 4 + (1 + 4*4) * size);
    packet.WriteBits(size, 23);

    for (PlayerCurrenciesMap::const_iterator itr = _currencyStorage.begin(); itr != _currencyStorage.end(); ++itr)
    {
        CurrencyTypesEntry const* entry = sCurrencyTypesStore.LookupEntry(itr->first);
        if (!entry) // should never happen
            continue;

        uint32 weekCount = itr->second.weekCount / entry->GetPrecision();
        uint32 weekCap = _GetCurrencyWeekCap(entry) / entry->GetPrecision();

        packet.WriteBit(weekCount);
        packet.WriteBits(itr->second.flags, 4);
        packet.WriteBit(weekCap);
        packet.WriteBit(0);     // season total earned

        currencyData << uint32(itr->second.totalCount / entry->GetPrecision());
        if (weekCap)
            currencyData << uint32(weekCap);

        //if (seasonTotal)
        //    currencyData << uint32(seasonTotal);

        currencyData << uint32(entry->ID);
        if (weekCount)
            currencyData << uint32(weekCount);
    }

    packet.FlushBits();
    packet.append(currencyData);
    GetSession()->SendPacket(&packet);
}

uint32 Player::GetCurrency(uint32 id) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    return itr != _currencyStorage.end() ? itr->second.totalCount : 0;
}

bool Player::HasCurrency(uint32 id, uint32 count) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    return itr != _currencyStorage.end() && itr->second.totalCount >= count;
}

void Player::ModifyCurrencyFlag(uint32 id, uint8 flag)
{
    if (!id)
        return;

    if (_currencyStorage.find(id) == _currencyStorage.end())
        return;

    _currencyStorage[id].flags = flag;
    if (_currencyStorage[id].state != PLAYERCURRENCY_NEW)
        _currencyStorage[id].state = PLAYERCURRENCY_CHANGED;
}

void Player::ModifyCurrency(uint32 id, int32 count, bool printLog/* = true*/, bool ignoreMultipliers/* = false*/)
{
    if (!count)
        return;

    CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id);
    ASSERT(currency);

    if (!ignoreMultipliers)
        count *= GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_CURRENCY_GAIN, id);

    uint32 oldTotalCount = 0;
    uint32 oldWeekCount = 0;
    PlayerCurrenciesMap::iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
    {
        PlayerCurrency cur;
        cur.state = PLAYERCURRENCY_NEW;
        cur.totalCount = 0;
        cur.weekCount = 0;
        cur.flags = 0;
        _currencyStorage[id] = cur;
        itr = _currencyStorage.find(id);
    }
    else
    {
        oldTotalCount = itr->second.totalCount;
        oldWeekCount = itr->second.weekCount;
    }

    // count can't be more then weekCap.
    uint32 weekCap = _GetCurrencyWeekCap(currency);
    if (count > int32(weekCap))
        count = weekCap;

    int32 newTotalCount = int32(oldTotalCount) + count;
    if (newTotalCount < 0)
        newTotalCount = 0;

    int32 newWeekCount = int32(oldWeekCount) + (count > 0 ? count : 0);
    if (newWeekCount < 0)
        newWeekCount = 0;

    ASSERT(weekCap >= oldWeekCount);

    // TODO: fix conquest points
    // if we get more then weekCap just set to limit
    if (weekCap && int32(weekCap) < newWeekCount)
    {
        newWeekCount = int32(weekCap);
        // weekCap - oldWeekCount alwayt >= 0 as we set limit before!
        newTotalCount = oldTotalCount + (weekCap - oldWeekCount);
    }

    // if we get more then totalCap set to maximum;
    if (currency->TotalCap && int32(currency->TotalCap) < newTotalCount)
    {
        newTotalCount = int32(currency->TotalCap);
        newWeekCount = weekCap;
    }

    if (uint32(newTotalCount) != oldTotalCount)
    {
        if (itr->second.state != PLAYERCURRENCY_NEW)
            itr->second.state = PLAYERCURRENCY_CHANGED;

        itr->second.totalCount = newTotalCount;
        itr->second.weekCount = newWeekCount;

        // probably excessive checks
        if (IsInWorld() && !GetSession()->PlayerLoading())
        {
            if (count > 0)
                UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CURRENCY, id, count);

            // on new case just set init.
            if(itr->second.state == PLAYERCURRENCY_NEW)
            {
                SendNewCurrency(id);
                return;
            }

            WorldPacket packet(SMSG_UPDATE_CURRENCY, 4 + (weekCap != 0) ? 4 : 0);

            packet.WriteBit(weekCap != 0);
            packet.WriteBit(0); // hasSeasonCount
            packet.WriteBit(printLog); // print in log

            // if hasSeasonCount packet << uint32(seasontotalearned); TODO: save this in character DB and use it

            packet << uint32(newTotalCount / currency->GetPrecision());
            packet << uint32(id);
            if (weekCap)
                packet << uint32(newWeekCount / currency->GetPrecision());

            GetSession()->SendPacket(&packet);
        }
    }
}

void Player::SetCurrency(uint32 id, uint32 count, bool printLog /*= true*/)
{
   ModifyCurrency(id, int32(count) - GetCurrency(id), printLog);
}

uint32 Player::_GetCurrencyWeekCap(const CurrencyTypesEntry* currency) const
{
   uint32 cap = currency->WeekCap;
   switch (currency->ID)
   {
       case CURRENCY_TYPE_CONQUEST_POINTS:
       {
           // TODO: implement
           cap = 1500 * currency->GetPrecision();
           break;
       }
       case CURRENCY_TYPE_HONOR_POINTS:
       {
           uint32 honorcap = sWorld->getIntConfig(CONFIG_MAX_HONOR_POINTS);
           if (honorcap > 0)
               cap = honorcap;
           break;
       }
       case CURRENCY_TYPE_JUSTICE_POINTS:
       {
           uint32 justicecap = sWorld->getIntConfig(CONFIG_MAX_JUSTICE_POINTS);
           if (justicecap > 0)
               cap = justicecap;
           break;
       }
   }

   SendCurrencyWeekCap(currency);

   return cap;
}

void Player::SendCurrencyWeekCap(uint32 id) const
{
    SendCurrencyWeekCap(sCurrencyTypesStore.LookupEntry(id));
}

void Player::SendCurrencyWeekCap(const CurrencyTypesEntry* currency) const
{
    if (!currency || !IsInWorld() || GetSession()->PlayerLoading())
        return;

    uint32 cap = _GetCurrencyWeekCap(currency);
    if (!cap)
        return;

    WorldPacket packet(SMSG_UPDATE_CURRENCY_WEEK_LIMIT, 8);
    packet << uint32(floor(cap / currency->GetPrecision()));
    packet << uint32(currency->ID);
    GetSession()->SendPacket(&packet);
}
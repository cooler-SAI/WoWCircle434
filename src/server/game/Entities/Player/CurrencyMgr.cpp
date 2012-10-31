#include "gamePCH.h"
#include "Player.h"
#include "Formulas.h"
#include "ArenaTeamMgr.h"

void Player::SendNewCurrency(uint32 id) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
        return;

    CurrencyTypesEntry const* entry = sCurrencyTypesStore.LookupEntry(id);
    // not send init meta currencies.
    if (!entry || entry->Category == CURRENCY_CATEGORY_META_CONQUEST)
        return;

    ByteBuffer currencyData;
    WorldPacket packet(SMSG_INIT_CURRENCY, 4 + (5*4 + 1));
    packet.WriteBits(1, 23);

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
    size_t count_pos = packet.bitwpos();
    packet.WriteBits(size, 23);

    size_t count = 0;
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

        ++count;
    }

    packet.FlushBits();
    packet.append(currencyData);
    packet.PutBits(count_pos, count, 23);
    GetSession()->SendPacket(&packet);
}

uint32 Player::GetCurrency(uint32 id, bool precision) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
         return 0;

    if (!precision)
        return itr->second.totalCount;

    CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id);
    // ASSERT(currency);

    return itr->second.totalCount / currency->GetPrecision();
}

uint32 Player::GetCurrencyOnWeek(uint32 id, bool precision) const
{
    PlayerCurrenciesMap::const_iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
        return 0;

    if (!precision)
        return itr->second.weekCount;

    CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id);
    //ASSERT(currency);

    return itr->second.weekCount / currency->GetPrecision();
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

    // count can't be more then weekCap if used (weekCap > 0)
    uint32 weekCap = _GetCurrencyWeekCap(currency);
    if (weekCap && count > int32(weekCap))
        count = weekCap;

    int32 newTotalCount = int32(oldTotalCount) + count;
    if (newTotalCount < 0)
        newTotalCount = 0;

    int32 newWeekCount = int32(oldWeekCount) + (count > 0 ? count : 0);
    if (newWeekCount < 0)
        newWeekCount = 0;

    ASSERT(weekCap >= oldWeekCount);

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

            if (currency->Category == CURRENCY_CATEGORY_META_CONQUEST)
            {
                // original conquest cap is highest of bg/arena conquest cap.
                if (weekCap > _ConquestCurrencytotalWeekCap)
                    _ConquestCurrencytotalWeekCap = weekCap;
                // count was changed to week limit, now we can modify original points.
                ModifyCurrency(CURRENCY_TYPE_CONQUEST_POINTS, count, printLog);
                return;
            }

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
   PlayerCurrenciesMap::iterator itr = _currencyStorage.find(id);
   if (itr == _currencyStorage.end())
   {
       PlayerCurrency cur;
       cur.state = PLAYERCURRENCY_NEW;
       cur.totalCount = count;
       cur.weekCount = 0;
       _currencyStorage[id] = cur;
   }
}

uint32 Player::GetCurrencyWeekCap(uint32 id, bool usePrecision) const
{
    CurrencyTypesEntry const* entry = sCurrencyTypesStore.LookupEntry(id);
    if (!entry)
        return 0;

    uint32 cap = _GetCurrencyWeekCap(entry);
    if (usePrecision)
        cap /= entry->GetPrecision();

    return cap;
}

uint32 Player::_GetCurrencyWeekCap(const CurrencyTypesEntry* currency) const
{
    uint32 cap = currency->WeekCap;

    switch (currency->ID)
    {
       // original conquest not have week cap
        case CURRENCY_TYPE_CONQUEST_POINTS:
            return _ConquestCurrencytotalWeekCap;
        case CURRENCY_TYPE_CONQUEST_META_ARENA:
            // should add precision mod = 100
            return Trinity::Currency::ConquestRatingCalculator(_maxPersonalArenaRate);
        case CURRENCY_TYPE_CONQUEST_META_BG:
            // should add precision mod = 100
            return Trinity::Currency::BgConquestRatingCalculator(GetRBGPersonalRating());
        case CURRENCY_TYPE_HONOR_POINTS:
        {
            uint32 honorcap = sWorld->getIntConfig(CONFIG_CURRENCY_MAX_HONOR_POINTS);
            if (honorcap > 0)
                cap = honorcap;
            break;
        }
        case CURRENCY_TYPE_JUSTICE_POINTS:
        {
            uint32 justicecap = sWorld->getIntConfig(CONFIG_CURRENCY_MAX_JUSTICE_POINTS);
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

void Player::ResetCurrencyWeekCap()
{
    for (uint32 arenaSlot = 0; arenaSlot < MAX_ARENA_SLOT; arenaSlot++)
    {
        if (uint32 arenaTeamId = GetArenaTeamId(arenaSlot))
        {
            ArenaTeam* arenaTeam = sArenaTeamMgr->GetArenaTeamById(arenaTeamId);
            arenaTeam->FinishWeek();                              // set played this week etc values to 0 in memory, too
            arenaTeam->SaveToDB();                                // save changes
            arenaTeam->NotifyStatsChanged();                      // notify the players of the changes
        }
    }

    for (PlayerCurrenciesMap::iterator itr = _currencyStorage.begin(); itr != _currencyStorage.end(); ++itr)
    {
        itr->second.weekCount = 0;
        itr->second.state = PLAYERCURRENCY_CHANGED;
    }

    WorldPacket data(SMSG_WEEKLY_RESET_CURRENCY, 0);
    SendDirectMessage(&data);
}

void Player::SendPvpRewards() const
{
    WorldPacket packet(SMSG_REQUEST_PVP_REWARDS_RESPONSE, 24);
    packet << GetCurrencyWeekCap(CURRENCY_TYPE_CONQUEST_POINTS, false);
    packet << GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_POINTS, true);
    packet << GetCurrencyWeekCap(CURRENCY_TYPE_CONQUEST_META_ARENA, false);
    packet << GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_META_ARENA, true);
    packet << GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_META_BG, true);
    packet << GetCurrencyWeekCap(CURRENCY_TYPE_CONQUEST_POINTS, false);
    packet << GetCurrencyWeekCap(CURRENCY_TYPE_CONQUEST_META_BG, false);
    GetSession()->SendPacket(&packet);
}
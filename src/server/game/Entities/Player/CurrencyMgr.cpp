
#include "CurrencyMgr.h"
#include "Formulas.h"
#include "ArenaTeamMgr.h"
#include "World.h"

CurrencyMgr::CurrencyMgr() 
{
    lastGuid = 0;
}

CurrencyMgr::~CurrencyMgr() {}

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

    float precision = sCurrencyMgr->GetPrecision(entry);

    uint32 weekCount = itr->second.weekCount / precision;
    uint32 weekCap = _GetCurrencyWeekCap(entry) / precision;
    uint32 seasonCount = itr->second.seasonCount / precision;

    packet.WriteBit(weekCount);
    packet.WriteBits(itr->second.flags, 4);
    packet.WriteBit(weekCap);
    packet.WriteBit(seasonCount > 0);     // season total earned

    currencyData << uint32(itr->second.totalCount / precision);
    if (weekCap)
        currencyData << uint32(weekCap);

    if (seasonCount)
        currencyData << uint32(seasonCount);

    currencyData << uint32(entry->ID);
    if (weekCount)
        currencyData << uint32(weekCount);

    packet.append(currencyData);
    GetSession()->SendPacket(&packet);
}

void Player::SendCurrencies() const
{
    ByteBuffer currencyData;

    WorldPacket packet(SMSG_INIT_CURRENCY, 4 + (1 + 4*4) * _currencyStorage.size());
    packet.WriteBits<int>(_currencyStorage.size(), 23);

    for (PlayerCurrenciesMap::const_iterator itr = _currencyStorage.begin(); itr != _currencyStorage.end(); ++itr)
    {
        CurrencyTypesEntry const* entry = sCurrencyTypesStore.LookupEntry(itr->first);
        ASSERT(entry && "What the fack?");

        uint32 Id = entry->ID;

        float precision = sCurrencyMgr->GetPrecision(entry);

        uint32 weekCount = itr->second.weekCount;
        uint32 weekCap = _GetCurrencyWeekCap(entry);
        uint32 seasonCount = itr->second.seasonCount;

        weekCount /= precision;
        weekCap /= precision;
        seasonCount /= precision;

        if (Id == CURRENCY_TYPE_HONOR_POINTS)
        {
            weekCount = 0;
            weekCap = 0;
        }

        packet
            .WriteBit(weekCount)
            .WriteBits(itr->second.flags, 4)
            .WriteBit(weekCap)
            .WriteBit(seasonCount > 0);     // seasonTotal

        currencyData << uint32(itr->second.totalCount / precision);
        if (weekCap)
            currencyData << uint32(weekCap);

        if (seasonCount)
            currencyData << uint32(seasonCount);

        currencyData << uint32(entry->ID);
        if (weekCount)
            currencyData << uint32(weekCount);
    }

    packet.append(currencyData);
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

void Player::ModifyCurrency(uint32 id, int32 count, bool printLog /* = true */, bool ignoreMultipliers /* = false */, bool ignoreLimit /* = false */)
{
    if (!count)
        return;

    CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id);
    ASSERT(currency);

    if (!ignoreMultipliers)
        count *= GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_CURRENCY_GAIN, id);

    if (sCurrencyMgr->IsPrecisionAreConsidered(id))
        count /= currency->GetPrecision();

    uint32 oldTotalCount = 0;
    uint32 oldWeekCount = 0;
    uint32 oldSeasonCount = 0;
    PlayerCurrenciesMap::iterator itr = _currencyStorage.find(id);
    if (itr == _currencyStorage.end())
    {
        PlayerCurrency cur;
        cur.state = PLAYERCURRENCY_NEW;
        cur.totalCount = 0;
        cur.weekCount = 0;
        cur.seasonCount = 0;
        cur.flags = 0;
        _currencyStorage[id] = cur;
        itr = _currencyStorage.find(id);
    }
    else
    {
        oldTotalCount = itr->second.totalCount;
        oldWeekCount = itr->second.weekCount;
        oldSeasonCount = itr->second.seasonCount;
    }

    // count can't be more then weekCap if used (weekCap > 0)
    uint32 weekCap = _GetCurrencyWeekCap(currency);
    if (!ignoreLimit && weekCap && count > int32(weekCap))
        count = weekCap;

    int32 newTotalCount = int32(oldTotalCount) + count;
    int32 newWeekCount = 0;
    uint32 newSeasonCount = 0;

    if (!ignoreLimit && count > 0)
    {
        newWeekCount = int32(oldWeekCount) + count;
        newSeasonCount = oldSeasonCount + count;
    }
    else
        newWeekCount = int32(oldWeekCount);

    //ASSERT(weekCap >= oldWeekCount);
    
    if ( !ignoreLimit )
    {
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
    }

    if (newWeekCount < 0)
        newWeekCount = 0;
    if (newTotalCount < 0)
        newTotalCount = 0;

    if (id == CURRENCY_TYPE_HONOR_POINTS || id == CURRENCY_TYPE_JUSTICE_POINTS)
    {
        newWeekCount = newTotalCount;
        weekCap = 0;
    }

    if (uint32(newTotalCount) != oldTotalCount)
    {
        if (itr->second.state != PLAYERCURRENCY_NEW)
            itr->second.state = PLAYERCURRENCY_CHANGED;

        itr->second.totalCount = newTotalCount;
        itr->second.weekCount = newWeekCount;
        itr->second.seasonCount = newSeasonCount;

        // probably excessive checks
        if (IsInWorld() && !GetSession()->PlayerLoading())
        {
            if (!ignoreLimit && count > 0)
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
            if (itr->second.state == PLAYERCURRENCY_NEW)
            {
                SendNewCurrency(id);
                return;
            }

            WorldPacket packet(SMSG_UPDATE_CURRENCY, 4 + (weekCap != 0 ? 4 : 0));

            packet
                .WriteBit(weekCap != 0)
                .WriteBit(newSeasonCount > 0) // hasSeasonCount
                .WriteBit(printLog); // print in log

            if (!sCurrencyMgr->IsPrecisionAreConsidered(id))
                newSeasonCount /= currency->GetPrecision();

            if (newSeasonCount)
                packet << uint32(newSeasonCount);

            packet << uint32(newTotalCount / currency->GetPrecision());
            packet << uint32(id);
            if (weekCap)
                packet << uint32(newWeekCount / currency->GetPrecision());

            GetSession()->SendPacket(&packet);
        }
    }
}

void Player::SetCurrency(uint32 id, uint32 count)
{
   PlayerCurrenciesMap::iterator itr = _currencyStorage.find(id);
   if (itr == _currencyStorage.end())
   {
       PlayerCurrency cur;
       cur.state = PLAYERCURRENCY_NEW;
       cur.totalCount = count;
       cur.weekCount = 0;
       cur.seasonCount = 0;
       cur.flags = 0;
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
            return const_cast<Player*>(this)->GetArenaCap();
        case CURRENCY_TYPE_CONQUEST_META_BG:
            return const_cast<Player*>(this)->GetBattlegroundCap();
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

    cap /= sCurrencyMgr->GetPrecision(currency);

    WorldPacket packet(SMSG_UPDATE_CURRENCY_WEEK_LIMIT, 8);
    packet << uint32(floor(float(cap)));
    packet << uint32(currency->ID);
    GetSession()->SendPacket(&packet);
}

void Player::FinishWeek()
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    for (PlayerCurrenciesMap::iterator itr = _currencyStorage.begin(); itr != _currencyStorage.end(); ++itr)
    {
        itr->second.weekCount = 0;
        itr->second.state = PLAYERCURRENCY_CHANGED;
    }

    _SaveCurrency(trans);
    CharacterDatabase.CommitTransaction(trans);

    WorldPacket data(SMSG_WEEKLY_RESET_CURRENCY, 0);
    SendDirectMessage(&data);
}

void Player::ResetCurrencyWeekCap(SQLTransaction* trans /* = NULL */)
{
    if (!IsHaveCap())
        return;

    FinishWeek();

    sCurrencyMgr->CalculatingCurrencyCap(m_currencyCap->currentArenaCap);
    // we need it after rated bg implementation
    //sCurrencyMgr->CalculatingCurrencyCap(m_currencyCap->currentRBgCap, true);

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<6>(CHAR_UPD_CURRENCY_WEEK_CAP);
    stmt->setUInt16(0, m_currencyCap->currentArenaCap); // currentArenaCap
    stmt->setUInt16(1, 0);                              // currentRBgCap
    stmt->setUInt8(2, 0);                               // requireReset
    stmt->setUInt16(3, 0);                              // highestArenaRating
    stmt->setUInt16(4, 0);                              // highestRBgRating
    stmt->setUInt32(5, GetGUIDLow());                   // guid

    if (trans && !trans->null())
        (*trans)->Append(stmt);
}

void Player::SendPvpRewards()
{
    WorldPacket packet(SMSG_REQUEST_PVP_REWARDS_RESPONSE, 28);
    packet << GetBattlegroundCap();
    packet << GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_META_ARENA, false);;
    packet << GetArenaCap();
    packet << uint32(0); // Rated battleground current cap, still don't need
    packet << GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_META_ARENA, false);
    packet << GetMaximumCap();
    packet << GetMaximumCap();
    GetSession()->SendPacket(&packet);
}

float CurrencyMgr::GetPrecision(CurrencyTypesEntry const* currency) const
{
    float precision = currency->GetPrecision();

    if (IsPrecisionAreConsidered(currency->ID))
        precision = 1.0f;

    return precision;
}

void CurrencyMgr::AddCurrencyCapData(uint32 lowGuid, uint16 arenaRating /* = 0 */, uint16 RBgRating /* = 0 */, uint16 cap /* = 1350 */, uint16 rbgCap /* = 0 */, uint8 reset /* = 0 */)
{
    CurrencyCap &data = _capValuesStorage[lowGuid];
    data.highestArenaRating = arenaRating;
    data.highestRBgRating = RBgRating;
    data.currentArenaCap = cap;
    data.currentRBgCap = cap;
    data.requireReset = reset;
}

void CurrencyMgr::LoadPlayersCurrencyCap()
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<0>(CHAR_SEL_CURRENCY_CAP);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 players who have any currency cap data. DB table `character_currency_cap` is empty!");
        return;
    }

    do 
    {
        Field* fields = result->Fetch();
        uint32 lowGuid = fields[0].GetUInt32();

        CurrencyCap cap;
        cap.highestArenaRating  = fields[1].GetUInt16();
        cap.highestRBgRating    = fields[2].GetUInt16();
        cap.currentArenaCap     = fields[3].GetUInt16();
        cap.currentRBgCap       = fields[4].GetUInt16();
        cap.requireReset        = fields[5].GetUInt8();

        _capValuesStorage[lowGuid] = cap;

    } while (result->NextRow());

    lastItr = getCapBegin();

    // if server crashed when cap was not been reset we must restore it
    if (CheckIfNeedRestore())
        RestoreResettingCap();
}

bool CurrencyMgr::CheckIfNeedRestore()
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<1>(CHAR_SEL_CURRENCY_RESTORE_DATA);
    stmt->setUInt8(0, 0);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return false;

    Field* fields = result->Fetch();
    uint32 lowGuid = fields[0].GetUInt32();

    if (lowGuid != getCapEnd()->first)
    {
        lastGuid = lowGuid;
        return true;
    }

    return false;
}

void CurrencyMgr::RestoreResettingCap()
{
    PlayerCurrencyCapMap::iterator itr = _capValuesStorage.find(lastGuid);
    if (itr == _capValuesStorage.end())
        return;

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    for (; itr != sCurrencyMgr->getCapEnd(); ++itr)
    {
        CurrencyCap second = itr->second;
        if (!second.requireReset)
            continue;

        sCurrencyMgr->CalculatingCurrencyCap(second.highestArenaRating);

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<6>(CHAR_UPD_CURRENCY_WEEK_CAP);
        stmt->setUInt16(0, second.highestArenaRating); // currentArenaCap
        stmt->setUInt16(1, 0);                         // currentRBgCap
        stmt->setUInt8(2, 0);                          // requireReset
        stmt->setUInt16(3, 0);                         // highestArenaRating
        stmt->setUInt16(4, 0);                         // highestRBgRating
        stmt->setUInt32(5, itr->first);                // guid

        trans->Append(stmt);
    }

    CharacterDatabase.CommitTransaction(trans);
    DeleteRestoreData();
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Server crashed on resetting arena cap but cap restore now!");
}

void CurrencyMgr::CalculatingCurrencyCap(uint16 &rating, bool ratedBattleground /* = false */)
{
    Trinity::Currency::ConquestRatingCalculator(rating, ratedBattleground);
}

CurrencyCap* CurrencyMgr::getCurrencyCapData(uint32 lowGuid)
{
    std::map<uint32, CurrencyCap>::iterator itr = _capValuesStorage.find(lowGuid);
    if (itr != _capValuesStorage.end())
        return &itr->second;
    else
        return NULL;
}

void CurrencyMgr::ResetCurrencyCapToAllPlayers()
{
    if (capMapIsEmpty())
        return;

    lastItr = getCapBegin();

    CapResetEvent* resetEvent = new CapResetEvent(getLastGuid(), 0);
    // players need to know in advance about cap reset
    m_events.AddEvent(resetEvent, m_events.CalculateTime(20000));
    sWorld->SendWorldText(LANG_CURRENCY_CAP_RESET_STARTED);
    sArenaTeamMgr->FinishWeek();
}

void CurrencyMgr::setLastGuid(uint32 lowGuid, bool save)
{
    lastGuid = lowGuid;
    if (save)
    {
        PreparedStatement *stmt = CharacterDatabase.GetPreparedStatement<2>(CHAR_INS_PLAYER_CURRENCY_CAP_PROGRESS);
        stmt->setUInt8(0, 1);
        stmt->setUInt32(1, lastGuid);
        CharacterDatabase.Execute(stmt);
    }
}

void CurrencyMgr::DeleteRestoreData()
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<1>(CHAR_DEL_PLAYER_CURRENCY_CAP_PROGRESS);
    stmt->setUInt8(0, 1);
    CharacterDatabase.Execute(stmt);
}

void CurrencyMgr::UpdateEvents(uint32 diff)
{
    m_events.Update(diff);
}

bool CurrencyMgr::IsPrecisionAreConsidered(uint32 currencyId) const
{
    switch (currencyId)
    {
        case CURRENCY_TYPE_CONQUEST_POINTS:
        case CURRENCY_TYPE_CONQUEST_META_ARENA:
            return true;
    }

    return false;
}


bool CapResetEvent::Execute(uint64 e_time, uint32 p_time)
{
    // check table if empty
    if (!sCurrencyMgr->getLastItr()->first)
    {
        sLog->outError(LOG_FILTER_PLAYER, "There no players to reset cap!");
        return false;
    }

    uint32 count = 0;
    uint16 numberInPart = sWorld->getIntConfig(CONFIG_CURRENCY_RESET_CAP_PLAYERS_COUNT);
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PlayerCurrencyCapMap::iterator itr = sCurrencyMgr->getLastItr();
    for (; itr != sCurrencyMgr->getCapEnd(); ++itr)
    {
        ++count;

        CurrencyCap second = itr->second;
        if (!second.requireReset)
            continue;

        if (Player* player = HashMapHolder<Player>::Find(itr->first))
            player->ResetCurrencyWeekCap(&trans);
        else
        {
            sCurrencyMgr->CalculatingCurrencyCap(second.highestArenaRating);
            // We need it after rated bg implementation
            //sCurrencyMgr->CalculatingCurrencyCap(itr->second.highestRBgRating, true);

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement<6>(CHAR_UPD_CURRENCY_WEEK_CAP);
            stmt->setUInt16(0, second.highestArenaRating); // currentArenaCap
            stmt->setUInt16(1, 0);                         // currentRBgCap
            stmt->setUInt8(2, 0);                          // requireReset
            stmt->setUInt16(3, 0);                         // highestArenaRating
            stmt->setUInt16(4, 0);                         // highestRBgRating
            stmt->setUInt32(5, itr->first);                // guid

            trans->Append(stmt);
        }

        if (count > numberInPart)
        {
            ++m_part;
            CapResetEvent* resetEvent = new CapResetEvent(sCurrencyMgr->getLastGuid(), m_part);
            sCurrencyMgr->m_events.AddEvent(resetEvent, sCurrencyMgr->m_events.CalculateTime(sWorld->getIntConfig(CONFIG_CURRENCY_RESET_CAP_TIMEOUT)));
            sWorld->SendWorldText(LANG_CURRENCY_CAP_RESET_PART, m_part);
            break;
        }
    }

    CharacterDatabase.CommitTransaction(trans);
    sCurrencyMgr->setLastItr(itr);
    // for crash restore
    sCurrencyMgr->setLastGuid(itr->first, true);

    if (itr == sCurrencyMgr->getCapEnd())
    {
        sCurrencyMgr->DeleteRestoreData();
        sWorld->SendWorldText(LANG_CURRENCY_CAP_RESET_END);
        return true;
    }

    return false;
}
void CapResetEvent::Abort(uint64 e_time) {}
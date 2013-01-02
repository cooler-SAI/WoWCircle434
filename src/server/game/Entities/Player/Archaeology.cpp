
#include "Player.h"

#define MAX_RESEARCH_SITES                      16
#define MAX_RESEARCH_PROJECTS                   9
#define ARCHAEOLOGY_DIG_SITE_FAR_DIST           40
#define ARCHAEOLOGY_DIG_SITE_MED_DIST           20
#define ARCHAEOLOGY_DIG_SITE_CLOSE_DIST         8
#define ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT      206590
#define ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT   206589
#define ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT    204272

const static uint16 _mapIds[4] = { 0, 1, 530, 571};

const static uint8 _races[9] = {1, 2, 3, 4, 5, 6, 7, 8, 27};

const static int q_patt[2][2]= { {0,1}, {3,2} };

bool Player::GenerateDigitLoot(uint16 zoneid, DigitSite &site)
{
    ResearchLootVector const& loot = sObjectMgr->GetResearchLoot();
    if (loot.empty())
        return false;

    ResearchLootVector lootList;
    for (ResearchLootVector::const_iterator itr = loot.begin(); itr != loot.end(); ++itr)
    {
        ResearchLootEntry entry = (*itr);
        if (entry.id != zoneid)
            continue;

        if (site.loot_id == 0)
        {
            switch (entry.race)
            {
            case 1: site.loot_id = 204282; break;
            case 2: site.loot_id = 207188; break;
            case 3: site.loot_id = 206836; break;
            case 4: site.loot_id = 203071; break;
            case 5: site.loot_id = 203078; break;
            case 6: site.loot_id = 207187; break;
            case 7: site.loot_id = 207190; break;
            case 8: site.loot_id = 202655; break;
            case 27: site.loot_id = 207189; break;
            default: site.loot_id = 0; break;
            }
        }

        lootList.push_back(entry);
    }

    if (lootList.empty())
        return false;

    ResearchLootVector::const_iterator entry = lootList.begin();
    std::advance(entry, urand(0, lootList.size() - 1));

    site.loot_x = entry->x;
    site.loot_y = entry->y;
    site.loot_z = entry->z;

    return true;
}

uint32 Player::GetSurveyBotEntry(float &orientation)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    uint16 zoneid = GetResearchSiteID();
    if (!zoneid)
        return 0;

    uint32 at_pos = 0xFFFF;

    for(uint8 i = 0; i < MAX_RESEARCH_SITES / 2; ++i)
    {
        //Replace by GetUInt16Value
        uint32 site_now_1 = GetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + i) & 0xFFFF;
        uint32 site_now_2 = GetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + i) >> 16;

        if (zoneid == site_now_1)
        {
            at_pos = i * 2;
            break;
        }
        if (zoneid == site_now_2)
        {
            at_pos = i * 2 + 1;
            break;
        }
    }

    if (at_pos == 0xFFFF)
        return 0;

    ASSERT(at_pos < 16);

    DigitSite &site = _digSites[at_pos];
    if (site.site_id != zoneid)
    {
        if (!GenerateDigitLoot(zoneid, site))
            return 0;

        site.site_id = zoneid;
    }

    orientation = GetAngle(site.loot_x, site.loot_y);
    float dist_now = GetDistance2d(site.loot_x, site.loot_y);

    if (dist_now >= ARCHAEOLOGY_DIG_SITE_FAR_DIST)
        return ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_MED_DIST)
        return ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_CLOSE_DIST)
        return ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT;

    if (skill_now < 50)
        UpdateSkill(SKILL_ARCHAEOLOGY, 1);

    uint32 curr_id = 0;
    switch (site.loot_id)
    {
        case 204282: curr_id = 384; break;
        case 207188: curr_id = 398; break;
        case 206836: curr_id = 393; break;
        case 203071: curr_id = 394; break;
        case 203078: curr_id = 400; break;
        case 207187: curr_id = 397; break;
        case 207190: curr_id = 401; break;
        case 202655: curr_id = 385; break;
        case 207189: curr_id = 399; break;
    }

    if (curr_id)
    {
        uint32 curr_min = 0;
        uint32 curr_max = 0;
        if (skill_now == 525) { curr_min = 6; curr_max = 7; }
        else if (skill_now >= 450) { curr_min = 5; curr_max = 6; }
        else if (skill_now >= 375) { curr_min = 4; curr_max = 6; }
        else if (skill_now >= 300) { curr_min = 4; curr_max = 5; }
        else if (skill_now >= 225) { curr_min = 3; curr_max = 5; }
        else if (skill_now >= 150) { curr_min = 3; curr_max = 4; }
        else if (skill_now >= 75) { curr_min = 2; curr_max = 4; }
        else if (skill_now >= 1) { curr_min = 2; curr_max = 3; }

        ModifyCurrency(curr_id, urand(curr_min, curr_max));
    }

    // We cannot implement special race fragments, so don't  spawn go
    /*SummonGameObject(
        m_digSites[at_pos].loot_id,
        m_digSites[at_pos].loot_x,
        m_digSites[at_pos].loot_y,
        m_digSites[at_pos].loot_z,
        0, 0, 0, 0, 0,
        time(NULL) + 30000);*/

    if (site.count < 2)
    {
        site.count++;
        if (!GenerateDigitLoot(zoneid, site))
            return 0;
    }
    else
    {
        site.clear();
        UseResearchSite(zoneid);
    }

    _archaeologyChanged = true;

    return 0;
}

uint16 Player::GetResearchSiteID()
{
    ResearchPOIPointMap DigZonesMap;

    ResearchZoneVector const& zone = sObjectMgr->GetResearchZones();
    for (ResearchZoneVector::const_iterator itr = zone.begin(); itr != zone.end(); ++itr)
    {
        ResearchZoneEntry entry = (*itr);
        if (entry.map == GetMapId() && entry.zone == GetZoneId())
        {
            ResearchPOIPoint pt;
            pt.x = entry.x;
            pt.y = entry.y;
            DigZonesMap[entry.POIid].push_back(pt);
        }
    }

    if (DigZonesMap.empty())
        return 0;

    for (ResearchPOIPointMap::const_iterator itr = DigZonesMap.begin(); itr != DigZonesMap.end(); ++itr)
    {
        ResearchPOIPoint pt;
        pt.x = int32(GetPositionX());
        pt.y = int32(GetPositionY());
        if (!IsPointInZone(pt, itr->second))
            continue;

        uint32 hashId = itr->first;
        for (uint32 i = 0; i < sResearchSiteStore.GetNumRows(); ++i)
        {
            if (ResearchSiteEntry const* rs = sResearchSiteStore.LookupEntry(i))
            {
                if (rs->POIid == hashId)
                    return rs->ID;
            }
        }
    }

    return 0;
}

bool Player::IsPointInZone(const ResearchPOIPoint &test, const ResearchPOIPoints &polygon)
{
    if (polygon.size() < 3)
        return false;

    ResearchPOIPoints::const_iterator end = polygon.end();
    ResearchPOIPoint pred_pt = polygon.back();
    pred_pt.x -= test.x;
    pred_pt.y -= test.y;

    int pred_q = q_patt[pred_pt.y < 0][pred_pt.x<  0];

    int w = 0;

    for (std::vector<ResearchPOIPoint>::const_iterator iter = polygon.begin(); iter != end; ++iter)
    {
        ResearchPOIPoint cur_pt = *iter;

        cur_pt.x -= test.x;
        cur_pt.y -= test.y;

        int q = q_patt[cur_pt.y < 0][cur_pt.x < 0];

        switch (q - pred_q)
        {
            case -3:
                ++w;
                break;
            case 3:
                --w;
                break;
            case -2:
                if (pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x)
                    ++w;
                break;
            case 2:
                if (!(pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x))
                    --w;
                break;
        }

        pred_pt = cur_pt;
        pred_q = q;
    }
    return w != 0;
}

void Player::UseResearchSite(uint32 id)
{
    _researchSites.erase(id);
    GenerateResearchSiteInMap(GetMapId());
}

void Player::ShowResearchSites()
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (Uint32Set::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
    {
        uint32 id = (*itr);
        ResearchSiteEntry const* rs = GetResearchSiteEntryById(id);

        if (!rs || CanResearchWithSkillLevel(rs->POIid) == 2)
            id = 0;

        if (count % 2 == 1)
        {
            newvalue |= id;
            SetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + count / 2, newvalue);
        }
        else
            newvalue = id << 16;

        ++count;
    }
}

void Player::ShowResearchProjects()
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (std::set<uint32>::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
    {
        if (count % 2 == 1)
        {
            newvalue |= (*itr);
            SetUInt32Value(PLAYER_FIELD_RESEARCHING_1 + count / 2, newvalue);
        }
        else if (count == 8)
        {
            SetUInt32Value(PLAYER_FIELD_RESEARCHING_1 + count / 2, (*itr));
            break;
        }
        else
            newvalue = (*itr) << 16;

        ++count;
    }
}

bool Player::CanResearchWithLevel(uint32 POIid)
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return false;

    ResearchZoneVector const& zone = sObjectMgr->GetResearchZones();
    for (ResearchZoneVector::const_iterator itr = zone.begin(); itr != zone.end(); ++itr)
    {
        if ((*itr).POIid == POIid)
            return getLevel() + 19 >= (*itr).level;
    }
    return true;
}

uint8 Player::CanResearchWithSkillLevel(uint32 POIid)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    ResearchZoneVector const& zone = sObjectMgr->GetResearchZones();
    for (ResearchZoneVector::const_iterator itr = zone.begin(); itr != zone.end(); ++itr)
    {
        ResearchZoneEntry entry = (*itr);
        if (entry.POIid != POIid)
            continue;

        uint16 skill_cap = 0;
        switch (entry.map)
        {
        case 0:
            if (entry.zone == 4922) // Twilight Hightlands
                skill_cap = 450;
            break;
        case 1:
            if (entry.zone == 616) // Hidjal
                skill_cap = 450;
            else if (entry.zone == 5034) // Uldum
                skill_cap = 450;
            break;
        case 530:
            skill_cap = 275; // Outland
            break;
        case 571:
            skill_cap = 350; // Northrend
            break;
        }

        if (skill_now >= skill_cap)
            return 1;
        else if (entry.map == 530 || entry.map == 571)
            return 2;
        else
            return 0;
    }
    return 0;
}

ResearchSiteEntry const* Player::GetResearchSiteEntryById(uint32 id)
{
    if (sResearchSiteSet.empty())
        return NULL;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
        if ((*itr)->ID == id)
            return (*itr);

    return NULL;
}

void Player::GenerateResearchSiteInMap(uint32 mapId)
{
    if (sResearchSiteSet.empty())
        return;

    Uint32Set tempSites;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        ResearchSiteEntry const* entry = (*itr);
        if (!HasResearchSite(entry->ID)
            && entry->mapId == mapId
            && CanResearchWithLevel(entry->POIid)
            && CanResearchWithSkillLevel(entry->POIid))
            tempSites.insert(entry->ID);
    }

    if (tempSites.empty())
        return;

    Uint32Set::const_iterator entry = tempSites.begin();
    std::advance(entry, urand(0, tempSites.size() - 1));

    _researchSites.insert((*entry));
    _archaeologyChanged = true;

    ShowResearchSites();
}

void Player::GenerateResearchSites()
{
    if (sResearchSiteSet.empty())
        return;

    _researchSites.clear();

    Sites tempSites;
    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        ResearchSiteEntry const* entry = (*itr);
        if (CanResearchWithLevel(entry->POIid) && CanResearchWithSkillLevel(entry->POIid))
            tempSites[entry->mapId].insert(entry->ID);
    }

    uint16 const* map = _mapIds;
    do
    {
        uint8 mapMax = std::min<int>(tempSites[*map].size(), 4);

        for (uint8 i = 0; i < mapMax;)
        {
            Uint32Set::const_iterator itr = tempSites[*map].begin();
            std::advance(itr, urand(0, tempSites[*map].size() - 1));
            if (!HasResearchSite((*itr)))
            {
                _researchSites.insert((*itr));
                ++i;
            }
        }
    }
    while(*++map);

    _archaeologyChanged = true;

    ShowResearchSites();
}

void Player::GenerateResearchProjects()
{
    if (sResearchProjectSet.empty())
        return;

    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return;

    _researchProjects.clear();

    Sites tempProjects;
    uint32 chance_mod = skill_now / 50;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        ResearchProjectEntry const* entry = (*itr);
        if (entry->rare && urand(0, 100) > chance_mod || IsCompletedProject(entry->ID))
            continue;

        tempProjects[entry->branchId].insert(entry->ID);
    }

    uint8 const* race = _races;
    Uint32Set::const_iterator itr;
    do
    {
        itr = tempProjects[*race].begin();
        std::advance(itr, urand(0, tempProjects[*race].size() - 1));
        _researchProjects.insert((*itr));
    }
    while(*++race);

    _archaeologyChanged = true;

    ShowResearchProjects();
}

bool Player::SolveResearchProject(uint32 spellId)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return false;

    ResearchProjectEntry const* entry = NULL;
    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        if ((*itr)->spellId != spellId)
            continue;

        entry = (*itr);
        break;
    }

    if (!entry || !HasResearchProject(entry->ID))
        return false;

    for (uint32 i = 0; i < sResearchBranchStore.GetNumRows(); ++i)
    {
        ResearchBranchEntry const* branch = sResearchBranchStore.LookupEntry(i);
        if (!branch)
            continue;

        if (branch->ID != entry->branchId)
            continue;

        if (!HasCurrency(branch->currency, entry->req_currency))
            return false;

        ModifyCurrency(branch->currency, -int32(entry->req_currency));
        break;
    }

    _researchProjects.erase(entry->ID);

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS, entry->ID, 1);
    UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS, entry->ID, 1);

    if (entry->rare)
        _completedProjects.insert(entry->ID);

    Uint32Set tempProjects;
    uint32 chance_mod = skill_now / 50;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        ResearchProjectEntry const* project = (*itr);
        if (project->branchId == entry->branchId)
        {
            if (project->rare && urand(0, 100) > chance_mod || IsCompletedProject(project->ID))
                continue;

            tempProjects.insert(project->ID);
        }
    }

    Uint32Set::const_iterator itr = tempProjects.begin();
    std::advance(itr, urand(0, tempProjects.size() - 1));

    _researchProjects.insert((*itr));
    _archaeologyChanged = true;

    ShowResearchProjects();
    return true;
}

bool Player::IsCompletedProject(uint32 id)
{
    if (_completedProjects.empty())
        return false;

    for (std::set<uint32>::const_iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
    {
        if (id == (*itr))
            return true;
    }

    return false;
}

void Player::SaveArchaeology(SQLTransaction& trans)
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    if (!_archaeologyChanged)
        return;

    trans->PAppend("DELETE FROM character_archaeology WHERE guid = '%u'", GUID_LOPART(GetGUID()));

    std::ostringstream ss;

    ss << "INSERT INTO character_archaeology  (guid, sites, counts, projects, completed) VALUES (";

    ss << GUID_LOPART(GetGUID()) << ", '";

    for (std::set<uint32>::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (uint8 j = 0; j < MAX_RESEARCH_SITES; ++j)
        ss << (_digSites[j].count) << " ";

    ss << "', '";

    for (std::set<uint32>::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (std::set<uint32>::const_iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "')";
    trans->Append(ss.str().c_str());
    _archaeologyChanged = false;
}

void Player::LoadArchaeology(PreparedQueryResult result)
{
    for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
        _digSites[i].count = 0;

    if (!result)
        return;

    Field* fields = result->Fetch();

    // Loading current zones
    if (fields[0].GetCString())
    {
        Tokenizer tokens(fields[0].GetCString(), ' ', MAX_RESEARCH_SITES);
        if (tokens.size() != 0 && tokens.size() <= MAX_RESEARCH_SITES)
        {
            _researchSites.clear();

            for (uint8 i = 0; i < tokens.size(); ++i)
            {
                _researchSites.insert(uint32(atoi(tokens[i])));
            }
        }
        else
            GenerateResearchSites();
    }
    else
        GenerateResearchSites();

    // Loading current zone info
    if (fields[1].GetCString())
    {
        Tokenizer tokens(fields[1].GetCString(), ' ', MAX_RESEARCH_SITES);
        if (tokens.size() == MAX_RESEARCH_SITES)
        {
            for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
                _digSites[i].count = uint32(atoi(tokens[i]));
        }
    }

    // Loading current projects
    if (fields[2].GetCString())
    {
        Tokenizer tokens(fields[2].GetCString(), ' ', MAX_RESEARCH_PROJECTS);
        if (tokens.size() == MAX_RESEARCH_PROJECTS)
        {
            _researchProjects.clear();

            for (uint8 i = 0; i < MAX_RESEARCH_PROJECTS; ++i)
            {
                _researchProjects.insert(uint32(atoi(tokens[i])));
            }
        }
        else
            GenerateResearchProjects();
    }
    else
        GenerateResearchProjects();

    // Loading completed projects
    if (fields[3].GetCString())
    {
        Tokenizer tokens(fields[3].GetCString(), ' ');
        if (!tokens.size())
            return;

        for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
            _completedProjects.insert(atoi((*itr)));
    }
}
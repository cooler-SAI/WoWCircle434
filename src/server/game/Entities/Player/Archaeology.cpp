#include "gamePCH.h"
#include "Player.h"

#define MAX_RESEARCH_SITES                      16
#define MAX_RESEARCH_PROJECTS                   9
#define ARCHAEOLOGY_DIG_SITE_FAR_DIST           40
#define ARCHAEOLOGY_DIG_SITE_MED_DIST           20
#define ARCHAEOLOGY_DIG_SITE_CLOSE_DIST         8
#define ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT      206590
#define ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT   206589
#define ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT    204272

bool Player::GenerateDigitLoot(uint32 zoneid, float &x, float &y, float &z, uint32 &loot_id)
{
    if (sObjectMgr->mResearchLoot.empty())
        return false;

    ObjectMgr::ResearchLootVector lootList;
    for (ObjectMgr::ResearchLootVector::const_iterator itr = sObjectMgr->mResearchLoot.begin(); itr != sObjectMgr->mResearchLoot.end(); ++itr)
        if ((*itr)->id == zoneid)
            lootList.push_back((*itr));

    if (lootList.empty())
        return false;

    ObjectMgr::ResearchLootVector::const_iterator itr1 = lootList.begin();
    std::advance(itr1, urand(0, lootList.size() - 1));

    x = (*itr1)->x;
    y = (*itr1)->y;
    z = (*itr1)->z;

    for (ObjectMgr::ResearchLootVector::const_iterator itr = sObjectMgr->mResearchLoot.begin(); itr != sObjectMgr->mResearchLoot.end(); ++itr)
    {
        if ((*itr)->id == zoneid)
        {
            switch ((*itr)->race)
            {
                case 1: loot_id = 204282; break;
                case 2: loot_id = 207188; break;
                case 3: loot_id = 206836; break;
                case 4: loot_id = 203071; break;
                case 5: loot_id = 203078; break;
                case 6: loot_id = 207187; break;
                case 7: loot_id = 207190; break;
                case 8: loot_id = 202655; break;
                case 27: loot_id = 207189; break;
                default: loot_id = 0; break;
            }
            break;
        }
    }

    return true;
}

uint32 Player::GetSurveyBotEntry(float &orientation)
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    uint32 zoneid = GetResearchSiteID();
    if (!zoneid)
        return 0;

    uint32 at_pos = 0xFFFF;

    for(uint32 i = 0; i < MAX_RESEARCH_SITES / 2; ++i)
    {
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

    if (m_digSites[at_pos].site_id != zoneid)
    {
        if (!GenerateDigitLoot(zoneid, m_digSites[at_pos].loot_x, m_digSites[at_pos].loot_y, m_digSites[at_pos].loot_z, m_digSites[at_pos].loot_id))
            return 0;
        m_digSites[at_pos].site_id = zoneid;
    }

    orientation = GetAngle(m_digSites[at_pos].loot_x, m_digSites[at_pos].loot_y);
    float dist_now = GetDistance2d(m_digSites[at_pos].loot_x, m_digSites[at_pos].loot_y);

    if (dist_now >= ARCHAEOLOGY_DIG_SITE_FAR_DIST)
        return ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_MED_DIST)
        return ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_CLOSE_DIST)
        return ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT;

    if (skill_now < 50)
        UpdateSkill(SKILL_ARCHAEOLOGY, 1);

    uint32 curr_id = 0;

    switch (m_digSites[at_pos].loot_id)
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

        ModifyCurrency(curr_id, urand(curr_min, curr_max) * 100);
    }

    // We cannot implement special race fragments, so don't  spawn go
    /*SummonGameObject(
        m_digSites[at_pos].loot_id,
        m_digSites[at_pos].loot_x,
        m_digSites[at_pos].loot_y,
        m_digSites[at_pos].loot_z,
        0, 0, 0, 0, 0,
        time(NULL) + 30000);*/

    if (m_digSites[at_pos].count < 2)
    {
        m_digSites[at_pos].count++;
        if (!GenerateDigitLoot(zoneid, m_digSites[at_pos].loot_x, m_digSites[at_pos].loot_y, m_digSites[at_pos].loot_z, m_digSites[at_pos].loot_id))
            return 0;
    }
    else
    {
        m_digSites[at_pos].clear();
        UseResearchSite(zoneid);
    }

    m_archaeology_changed = true;

    return 0;
}

uint32 Player::GetResearchSiteID()
{
    typedef std::vector<ResearchPOIPoint> digzoneType;
    std::map<uint32, digzoneType> DigZonesMap;

    for (ObjectMgr::ResearchZoneVector::const_iterator itr = sObjectMgr->mResearchZones.begin(); itr != sObjectMgr->mResearchZones.end(); ++itr)
    {
        if ((*itr)->map == GetMapId() && (*itr)->zone == GetZoneId())
        {
            ResearchPOIPoint pt;
            pt.x = (*itr)->x;
            pt.y = (*itr)->y;
            DigZonesMap[(*itr)->POIid].push_back(pt);
        }
    }

    if (DigZonesMap.empty())
        return 0;

    for (std::map<uint32, digzoneType>::const_iterator itr = DigZonesMap.begin(); itr != DigZonesMap.end(); ++itr)
    {
        ResearchPOIPoint pt;
        pt.x = int32(GetPositionX());
        pt.y = int32(GetPositionY());
        if (IsPointInZone(pt, itr->second))
        {
            uint32 hashId = itr->first;
            for (uint32 i = 0; i < sResearchSiteStore.GetNumRows(); ++i)
            {
                ResearchSiteEntry const* rs = sResearchSiteStore.LookupEntry(i);
                if (!rs)
                    continue;

                if (rs->POIid == hashId)
                    return rs->ID;
            }
        }
    }
    return 0;
}

bool Player::IsPointInZone(const ResearchPOIPoint &test, const std::vector<ResearchPOIPoint> &polygon)
{
    static const int q_patt[2][2]= { {0,1}, {3,2} };

    if (polygon.size() < 3)
        return false;

    std::vector<ResearchPOIPoint>::const_iterator end = polygon.end();
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
    m_ResearchSites.erase(id);
    GenerateResearchSiteInMap(GetMapId());
}

void Player::ShowResearchSites()
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (std::set<uint32>::const_iterator itr = m_ResearchSites.begin(); itr != m_ResearchSites.end(); ++itr)
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
        {
            newvalue = id << 16;
        }
        ++count;
    }
}

void Player::ShowResearchProjects()
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (std::set<uint32>::const_iterator itr = m_ResearchProjects.begin(); itr != m_ResearchProjects.end(); ++itr)
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
        {
            newvalue = (*itr) << 16;
        }
        ++count;
    }
}

bool Player::CanResearchWithLevel(uint32 POIid)
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return false;

    for (ObjectMgr::ResearchZoneVector::const_iterator itr = sObjectMgr->mResearchZones.begin(); itr != sObjectMgr->mResearchZones.end(); ++itr)
        if ((*itr)->POIid == POIid)
            return getLevel() + 19 >= (*itr)->level;
    return true;
}

uint8 Player::CanResearchWithSkillLevel(uint32 POIid)
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    for (ObjectMgr::ResearchZoneVector::const_iterator itr = sObjectMgr->mResearchZones.begin(); itr != sObjectMgr->mResearchZones.end(); ++itr)
        if ((*itr)->POIid == POIid)
        {
            uint32 skill_cap = 0;
            switch ((*itr)->map)
            {
            case 0:
                if ((*itr)->zone == 4922) skill_cap = 450; break; // Twilight Hightlands
            case 1:
                if ((*itr)->zone == 616) skill_cap = 450; // Hidjal
                else if ((*itr)->zone == 5034) skill_cap = 450; // Uldum
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
            else if ((*itr)->map == 530 || (*itr)->map == 571)
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

    std::set<uint32> tempSites;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        if (!HasResearchSite((*itr)->ID)
            && (*itr)->mapId == mapId
            && CanResearchWithLevel((*itr)->POIid)
            && CanResearchWithSkillLevel((*itr)->POIid))
            tempSites.insert((*itr)->ID);
    }

    if (tempSites.empty())
        return;

    std::set<uint32>::const_iterator itr = tempSites.begin();
    std::advance(itr, urand(0, tempSites.size() - 1));
    m_ResearchSites.insert((*itr));

    m_archaeology_changed = true;

    ShowResearchSites();
}

void Player::GenerateResearchSites()
{
    if (sResearchSiteSet.empty())
        return;

    m_ResearchSites.clear();

    typedef std::set<uint32> sites;
    std::map<uint32, sites> tempSites;
    uint8 mapMax = 4;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        if (CanResearchWithLevel((*itr)->POIid) && CanResearchWithSkillLevel((*itr)->POIid))
        {
            tempSites[(*itr)->mapId].insert((*itr)->ID);
        }
    }

    mapMax = tempSites[0].size() < 4? tempSites[0].size(): 4;

    for (uint8 i = 0; i < mapMax;)
    {
        std::set<uint32>::const_iterator itr = tempSites[0].begin();
        std::advance(itr, urand(0, tempSites[0].size() - 1));
        if (!HasResearchSite((*itr)))
        {
            m_ResearchSites.insert((*itr));
            ++i;
        }
    }

    mapMax = tempSites[1].size() < 4? tempSites[1].size(): 4;
    for (uint8 i = 0; i < mapMax;)
    {
        std::set<uint32>::const_iterator itr = tempSites[1].begin();
        std::advance(itr, urand(0, tempSites[1].size() - 1));
        if (!HasResearchSite((*itr)))
        {
            m_ResearchSites.insert((*itr));
            ++i;
        }
    }

    mapMax = tempSites[530].size() < 4? tempSites[530].size(): 4;
    for (uint8 i = 0; i < mapMax;)
    {
        std::set<uint32>::const_iterator itr = tempSites[530].begin();
        std::advance(itr, urand(0, tempSites[530].size() - 1));
        if (!HasResearchSite((*itr)))
        {
            m_ResearchSites.insert((*itr));
            ++i;
        }
    }

    mapMax = tempSites[571].size() < 4? tempSites[571].size(): 4;
    for (uint8 i = 0; i < mapMax;)
    {
        std::set<uint32>::const_iterator itr = tempSites[571].begin();
        std::advance(itr, urand(0, tempSites[571].size() - 1));
        if (!HasResearchSite((*itr)))
        {
            m_ResearchSites.insert((*itr));
            ++i;
        }
    }

    m_archaeology_changed = true;

    ShowResearchSites();
}

bool Player::HasResearchSite(uint32 id)
{
    if (m_ResearchSites.find(id) != m_ResearchSites.end())
        return true;
    return false;
}

bool Player::HasResearchProject(uint32 id)
{
    if (m_ResearchProjects.find(id) != m_ResearchProjects.end())
        return true;
    return false;
}

void Player::GenerateResearchProjects()
{
    if (sResearchProjectSet.empty())
        return;

    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return;

    m_ResearchProjects.clear();

    typedef std::set<uint32> projects;
    std::map<uint32, projects> tempProjects;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        uint32 chance_mod =  skill_now / 50;
        if ((*itr)->rare && urand(0, 100) > chance_mod || IsCompletedProject((*itr)->ID))
            continue;
        tempProjects[(*itr)->branchId].insert((*itr)->ID);
    }

    std::set<uint32>::const_iterator itr = tempProjects[1].begin();
    std::advance(itr, urand(0, tempProjects[1].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[2].begin();
    std::advance(itr, urand(0, tempProjects[2].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[3].begin();
    std::advance(itr, urand(0, tempProjects[3].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[4].begin();
    std::advance(itr, urand(0, tempProjects[4].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[5].begin();
    std::advance(itr, urand(0, tempProjects[5].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[6].begin();
    std::advance(itr, urand(0, tempProjects[6].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[7].begin();
    std::advance(itr, urand(0, tempProjects[7].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[8].begin();
    std::advance(itr, urand(0, tempProjects[8].size() - 1));
    m_ResearchProjects.insert((*itr));
    itr = tempProjects[27].begin();
    std::advance(itr, urand(0, tempProjects[27].size() - 1));
    m_ResearchProjects.insert((*itr));

    m_archaeology_changed = true;

    ShowResearchProjects();
}

bool Player::SolveResearchProject(uint32 spellId)
{
    uint32 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return false;

    ResearchProjectEntry const* rp = NULL;
    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        if ((*itr)->spellId == spellId)
        {
            rp = (*itr);
            break;
        }
    }

    if (!rp)
        return false;

    if (!HasResearchProject(rp->ID))
        return false;

    for (uint32 i = 0; i < sResearchBranchStore.GetNumRows(); ++i)
    {
        ResearchBranchEntry const* rb = sResearchBranchStore.LookupEntry(i);
        if (!rb)
            continue;

        if (rb->ID == rp->branchId)
        {
            if (!HasCurrency(rb->currency, rp->req_currency))
                return false;
            ModifyCurrency(rb->currency, -(int32)rp->req_currency);
            break;
        }
    }

    m_ResearchProjects.erase(rp->ID);

    //GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ARCHAEOLOGY_PROJECT, rp->ID, 1);
    //UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ARCHAEOLOGY_PROJECT, rp->ID, 1);

    if (rp->rare)
        m_CompletedProjects.insert(rp->ID);

    std::set<uint32> tempProjects;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        if ((*itr)->branchId == rp->branchId)
        {
            uint32 chance_mod =  skill_now / 50;
            if ((*itr)->rare && urand(0, 100) > chance_mod || IsCompletedProject((*itr)->ID))
                continue;
            tempProjects.insert((*itr)->ID);
        }
    }

    std::set<uint32>::const_iterator itr = tempProjects.begin();
    std::advance(itr, urand(0, tempProjects.size() - 1));
    m_ResearchProjects.insert((*itr));

    m_archaeology_changed = true;

    ShowResearchProjects();

    return true;
}

bool Player::IsCompletedProject(uint32 id)
{
    if (!m_CompletedProjects.size())
        return false;

    for (std::set<uint32>::const_iterator itr = m_CompletedProjects.begin(); itr != m_CompletedProjects.end(); ++itr)
        if (id == (*itr))
            return true;

    return false;
}

void Player::SaveArchaeology(SQLTransaction& trans)
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    if (!m_archaeology_changed)
        return;

    trans->PAppend("DELETE FROM character_archaeology WHERE guid = '%u'", GUID_LOPART(GetGUID()));

    std::ostringstream ss;

    ss << "INSERT INTO character_archaeology  (guid, sites, counts, projects, completed) VALUES (";

    ss << GUID_LOPART(GetGUID()) << ", '";

    for (std::set<uint32>::const_iterator itr = m_ResearchSites.begin(); itr != m_ResearchSites.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (uint8 j = 0; j < MAX_RESEARCH_SITES; ++j)
        ss << (m_digSites[j].count) << " ";

    ss << "', '";

    for (std::set<uint32>::const_iterator itr = m_ResearchProjects.begin(); itr != m_ResearchProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (std::set<uint32>::const_iterator itr = m_CompletedProjects.begin(); itr != m_CompletedProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "')";
    trans->Append(ss.str().c_str());
    m_archaeology_changed = false;
}

void Player::LoadArchaeology(PreparedQueryResult result)
{
    for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
        m_digSites[i].count = 0;

    if (!result)
        return;

    Field* fields = result->Fetch();

    // Loading current zones
    if (fields[0].GetCString())
    {
        Tokenizer tokens(fields[0].GetCString(), ' ', MAX_RESEARCH_SITES);
        if (tokens.size() != 0 && tokens.size() <= MAX_RESEARCH_SITES)
        {
            m_ResearchSites.clear();

            for (uint8 i = 0; i < tokens.size(); ++i)
            {
                m_ResearchSites.insert(uint32(atoi(tokens[i])));
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
                m_digSites[i].count = uint32(atoi(tokens[i]));
        }
    }

    // Loading current projects
    if (fields[2].GetCString())
    {
        Tokenizer tokens(fields[2].GetCString(), ' ', MAX_RESEARCH_PROJECTS);
        if (tokens.size() == MAX_RESEARCH_PROJECTS)
        {
            m_ResearchProjects.clear();

            for (uint8 i = 0; i < MAX_RESEARCH_PROJECTS; ++i)
            {
                m_ResearchProjects.insert(uint32(atoi(tokens[i])));
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
            m_CompletedProjects.insert(atoi((*itr)));
    }
}
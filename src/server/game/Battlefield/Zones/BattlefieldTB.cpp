/* Tol Barad Battlefield*/
#include "gamePCH.h"
#include "BattlefieldTB.h"
#include "SpellAuras.h"

bool BattlefieldTB::SetupBattlefield()
{
    m_TypeId                    = BATTLEFIELD_TB;
    m_BattleId                  = BATTLEFIELD_BATTLEID_TB;
    m_ZoneId                    = BATTLEFIELD_TB_ZONEID;
    m_MapId                     = BATTLEFIELD_TB_MAPID;

    m_MaxPlayer                 = sWorld->getIntConfig(CONFIG_TOL_BARAD_PLR_MAX);
    m_IsEnabled                 = sWorld->getBoolConfig(CONFIG_TOL_BARAD_ENABLE);
    m_MinPlayer                 = sWorld->getIntConfig(CONFIG_TOL_BARAD_PLR_MIN);
    m_MinLevel                  = sWorld->getIntConfig(CONFIG_TOL_BARAD_PLR_MIN_LVL);
    m_BattleTime                = sWorld->getIntConfig(CONFIG_TOL_BARAD_BATTLETIME) * MINUTE * IN_MILLISECONDS;
    m_NoWarBattleTime           = sWorld->getIntConfig(CONFIG_TOL_BARAD_NOBATTLETIME) * MINUTE * IN_MILLISECONDS;
    m_RestartAfterCrash         = sWorld->getIntConfig(CONFIG_TOL_BARAD_RESTART_AFTER_CRASH) * MINUTE * IN_MILLISECONDS;

    m_TimeForAcceptInvite       = 20;
    m_StartGroupingTimer        = 15 * MINUTE * IN_MILLISECONDS;
    m_StartGrouping             = false;

    KickPosition.Relocate(-630.460f, 1184.504f, 95.766f, 0);
    KickPosition.m_mapId        = m_MapId;

    RegisterZone(m_ZoneId);

    m_Data32.resize(BATTLEFIELD_TB_DATA_MAX);

    m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] = 0;

    m_saveTimer                 = 60000;

    SetGraveyardNumber(BATTLEFIELD_TB_GY_MAX);

    if ((sWorld->getWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED) == 0) && (sWorld->getWorldState(WS_TB_HORDE_DEFENCE) == 0) && (sWorld->getWorldState(ClockBTWorldState[0]) == 0))
    {
        sWorld->setWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED, uint64(false));
        sWorld->setWorldState(WS_TB_HORDE_DEFENCE, uint64(urand(0, 1)));
        sWorld->setWorldState(ClockBTWorldState[0], uint64(m_NoWarBattleTime));
    }

    if (sWorld->getWorldState(WS_TB_UNK) == 0)
        sWorld->setWorldState(WS_TB_UNK, urand(1, 3));

    if (sWorld->getWorldState(WS_TB_UNK_2) == 0)
        sWorld->setWorldState(WS_TB_UNK_2, time(NULL) + 86400);

    m_isActive = bool(sWorld->getWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED));
    m_DefenderTeam = (TeamId)sWorld->getWorldState(WS_TB_HORDE_DEFENCE);

    m_Timer = sWorld->getWorldState(ClockBTWorldState[0]);

    if (m_isActive)
    {
        m_isActive = false;
        m_Timer = m_RestartAfterCrash;
    }

    for (uint8 i = 0; i < BATTLEFIELD_TB_GY_MAX; i++)
    {
        BfGraveyardTB* graveyard = new BfGraveyardTB(this);
        graveyard->Initialize(TBGraveYard[i].entryh, TBGraveYard[i].entrya, TBGraveYard[i].x, TBGraveYard[i].y,  TBGraveYard[i].z, TBGraveYard[i].o, m_DefenderTeam, TBGraveYard[i].gyid);
        m_GraveyardList[i] = graveyard;
    }

    for (uint8 i = 0; i < TB_MAX_WORKSHOP; i++)
    {
        BfTBWorkShopData* workshop = new BfTBWorkShopData(this);

        workshop->Init(TBWorkShopDataBase[i].worldstate, TBWorkShopDataBase[i].type, TBWorkShopDataBase[i].nameid);
        workshop->ChangeControl(GetDefenderTeam(),true);

        BfCapturePointTB* point = new BfCapturePointTB(this, GetDefenderTeam());

        point->SetCapturePointData(GetDefenderTeam() == TEAM_ALLIANCE ? TBWorkShopDataBase[i].CapturePoint.entrya : TBWorkShopDataBase[i].CapturePoint.entryh, BATTLEFIELD_TB_MAPID,
            TBWorkShopDataBase[i].CapturePoint.x, TBWorkShopDataBase[i].CapturePoint.y, TBWorkShopDataBase[i].CapturePoint.z, 0);
        point->LinkToWorkShop(workshop);
        AddCapturePoint(point);

        CapturePoints.insert(point);

        WorkShopList.insert(workshop);
    }

    for (uint8 i = 0; i < TB_MAX_DESTROY_MACHINE_NPC; i++)
    {
        if (Creature* creature = SpawnCreature(TBDestroyMachineNPC[i].entrya, TBDestroyMachineNPC[i].x, TBDestroyMachineNPC[i].y, TBDestroyMachineNPC[i].z, TBDestroyMachineNPC[i].o, GetAttackerTeam()))
        {
            HideNpc(creature);
            Vehicles.insert(creature->GetGUID());
        }
    }

    for (uint8 i = 0; i < TB_MAX_OBJ; i++)
    {
        GameObject* go = SpawnGameObject(TBGameObjectBuillding[i].entry, BATTLEFIELD_TB_MAPID, TBGameObjectBuillding[i].x, TBGameObjectBuillding[i].y, TBGameObjectBuillding[i].z, TBGameObjectBuillding[i].o);
        BfTBGameObjectBuilding* b = new BfTBGameObjectBuilding(this);
        b->Init(go,TBGameObjectBuillding[i].type,TBGameObjectBuillding[i].WorldState,TBGameObjectBuillding[i].nameid);
        b->m_State = BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT - (GetDefenderTeam() * 3);
        BuildingsInZone.insert(b);
    }

    for (uint8 i = 0; i < 4; i++)
    {
        if (Creature* creature = SpawnCreature(QuestGivers[i].entrya, QuestGivers[i].x, QuestGivers[i].y, QuestGivers[i].z, QuestGivers[i].o, TEAM_ALLIANCE))
        {
            HideNpc(creature);
            creature->setFaction(TolBaradFaction[TEAM_ALLIANCE]);
            questgiversA.insert(creature->GetGUID());
        }
    }

    for (uint8 i = 0; i < 4; i++)
    {
        if (Creature* creature = SpawnCreature(QuestGivers[i].entryh, QuestGivers[i].x, QuestGivers[i].y, QuestGivers[i].z, QuestGivers[i].o, TEAM_HORDE))
        {
            HideNpc(creature);
            creature->setFaction(TolBaradFaction[TEAM_HORDE]);
            questgiversH.insert(creature->GetGUID());
        }
    }

    {
        int i = 0;
        GuidSet questgivers = (GetDefenderTeam() == TEAM_ALLIANCE ? questgiversA : questgiversH);
        for (GuidSet::const_iterator itr = questgivers.begin(); itr != questgivers.end(); ++itr)
        {
            if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            {
                if (Creature* creature = unit->ToCreature())
                {
                    if (!i || i == sWorld->getWorldState(WS_TB_UNK))
                        if (!m_isActive)
                            ShowNpc(creature, true);
                    i++;
                }
            }
        }
    }

    for (uint8 i = 0; i < 23; i++)
    {
        if (Creature* creature = SpawnCreature(AllianceSpawnNPC[i].entrya, AllianceSpawnNPC[i].x, AllianceSpawnNPC[i].y, AllianceSpawnNPC[i].z, AllianceSpawnNPC[i].o, TEAM_ALLIANCE))
        {
            creature->setFaction(TolBaradFaction[TEAM_ALLIANCE]);
            npcAlliance.insert(creature->GetGUID());
        }
    }

    for (uint8 i = 0; i < 23; i++)
    {
        if (Creature* creature = SpawnCreature(HordeSpawnNPC[i].entrya, HordeSpawnNPC[i].x, HordeSpawnNPC[i].y, HordeSpawnNPC[i].z, HordeSpawnNPC[i].o, TEAM_HORDE))
        {
            creature->setFaction(TolBaradFaction[TEAM_HORDE]);
            npcHorde.insert(creature->GetGUID());
        }
    }

    for (uint8 i = 0; i < 130; i++)
    {
        if (Creature* creature = SpawnCreature(TbOldNpcStructure[i].entrya, TbOldNpcStructure[i].x, TbOldNpcStructure[i].y, TbOldNpcStructure[i].z, TbOldNpcStructure[i].o, TEAM_NEUTRAL))
        {
            creature->SetRespawnRadius(15.0f);
            creature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
            creature->GetMotionMaster()->Initialize();
            npcOld.insert(creature->GetGUID());
        }
    }

    for (uint8 i = 0; i < 4; i++)
    {
        if (GameObject* go = SpawnGameObject(TBGameobjectsDoor[i].entrya, BATTLEFIELD_TB_MAPID, TBGameobjectsDoor[i].x, TBGameobjectsDoor[i].y, TBGameobjectsDoor[i].z, TBGameobjectsDoor[i].o))
        {
            go->ToGameObject()->SetLootState(GO_READY);
            go->ToGameObject()->UseDoorOrButton();
            goDoors.insert(go);
        }
    }
    return true;
}

bool BattlefieldTB::Update(uint32 diff)
{
    bool m_return = Battlefield::Update(diff);
    if (m_saveTimer <= diff)
    {
        sWorld->setWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED, m_isActive);
        sWorld->setWorldState(WS_TB_HORDE_DEFENCE, m_DefenderTeam);
        sWorld->setWorldState(ClockBTWorldState[0], m_Timer);
        m_saveTimer = 60 * IN_MILLISECONDS;
    } 
    else 
        m_saveTimer -= diff;

    for (GuidSet::const_iterator itr = m_PlayersIsSpellImu.begin(); itr != m_PlayersIsSpellImu.end(); ++itr)
    {
        if (Player* plr = sObjectMgr->GetPlayerByLowGUID((*itr)))
        {
            if (plr->HasAura(SPELL_TB_SPIRITUAL_IMMUNITY))
            {
                const WorldSafeLocsEntry* graveyard = GetClosestGraveYard(plr);
                if (graveyard)
                {
                    if (plr->GetDistance2d(graveyard->x, graveyard->y) > 10.0f)
                    {
                        plr->RemoveAurasDueToSpell(SPELL_TB_SPIRITUAL_IMMUNITY);
                        m_PlayersIsSpellImu.erase(plr->GetGUID());
                    }
                }
            }
        }
    }

    if (m_isActive)
        if (m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] == 3)
            EndBattle(false);

    return m_return;
}

void BattlefieldTB::OnPlayerEnterZone(Player* plr)
{
    if (!m_WarTime)
    {
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_VETERAN);
        plr->RemoveAurasDueToSpell(SPELL_TB_SPIRITUAL_IMMUNITY);
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_TOWER_BONUS);
    }

    SendInitWorldStatesTo(plr);
}

void BattlefieldTB::OnPlayerLeaveZone(Player* plr)
{
    if (!m_WarTime)
    {
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_VETERAN);
        plr->RemoveAurasDueToSpell(SPELL_TB_SPIRITUAL_IMMUNITY);
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_TOWER_BONUS);
    }
}

void BattlefieldTB::OnPlayerLeaveWar(Player* plr)
{
    if (!plr->GetSession()->PlayerLogout())
    {
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_VETERAN);
        plr->RemoveAurasDueToSpell(SPELL_TB_SPIRITUAL_IMMUNITY);
        plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_TOWER_BONUS);
    }
}

void BattlefieldTB::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
    Battlefield::AddPlayerToResurrectQueue(npc_guid, player_guid);
    if (IsWarTime())
    {
        if (Player* player = sObjectMgr->GetPlayerByLowGUID(player_guid))
        {
            if (!player->HasAura(SPELL_TB_SPIRITUAL_IMMUNITY))
            {
                player->CastSpell(player, SPELL_TB_SPIRITUAL_IMMUNITY, true);
                m_PlayersIsSpellImu.insert(player->GetGUID());
            }
        }
    }
}

void BattlefieldTB::OnBattleStart()
{
    m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] = 0;
    m_Data32[BATTLEFIELD_TB_DATA_DESTROYED] = 0;

    for (TbGameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
        if ((*itr))
            (*itr)->Rebuild();

    for (TbWorkShop::const_iterator itr = WorkShopList.begin(); itr != WorkShopList.end(); ++itr)
        if ((*itr))
            (*itr)->UpdateWorkshop();

    for (uint8 i = 0; i < BATTLEFIELD_TB_GY_MAX; i++)
    {
        BfGraveyardTB* graveyard = new BfGraveyardTB(this);

        if (i == BATTLEFIELD_TB_GY_BARADIN_HOLD)
            m_GraveyardList[i]->GiveControlTo(GetDefenderTeam());
        else
            m_GraveyardList[i]->GiveControlTo(GetAttackerTeam());

        m_GraveyardList[i] = graveyard;
    }

    for (GuidSet::const_iterator itr = npcAlliance.begin(); itr != npcAlliance.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GuidSet::const_iterator itr = npcHorde.begin(); itr != npcHorde.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GuidSet::const_iterator itr = npcOld.begin(); itr != npcOld.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GameobjectSet::const_iterator itr = goDoors.begin(); itr != goDoors.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->ToGameObject()->SetLootState(GO_READY);
            (*itr)->ToGameObject()->UseDoorOrButton();
        }
    }

    for (GuidSet::const_iterator itr = Vehicles.begin(); itr != Vehicles.end(); ++itr)
    {
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
        {
            if (Creature* creature = unit->ToCreature())
            {
                ShowNpc(creature, false);
                creature->setFaction(TolBaradFaction[GetAttackerTeam()]);
            }
        }
    }

    for (GuidSet::const_iterator itr = questgiversA.begin(); itr != questgiversA.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GuidSet::const_iterator itr = questgiversH.begin(); itr != questgiversH.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    HashMapHolder<Player>::MapType const& plist = sObjectAccessor->GetPlayers();
    for (HashMapHolder<Player>::MapType::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
    {
        if (Player* plr = itr->second)
        {
            plr->SendUpdateWorldState(WS_TB_BATTLE_TIMER_ENABLED, 1);
            plr->SendUpdateWorldState(WS_TB_BATTLE_TIMER, uint32(time(NULL) + GetTimer() / 1000));
            plr->SendUpdateWorldState(WS_TB_COUNTER_BUILDINGS, 0);
            plr->SendUpdateWorldState(WS_TB_COUNTER_BUILDINGS_ENABLED, 1);
            plr->SendUpdateWorldState(WS_TB_HORDE_DEFENCE, GetDefenderTeam() == TEAM_HORDE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_ALLIANCE_DEFENCE, GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED, 0);
            plr->SendUpdateWorldState(WS_TB_KEEP_HORDE_DEFENCE, GetDefenderTeam() == TEAM_HORDE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_KEEP_ALLIANCE_DEFENCE, GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_ALLIANCE_ATTACK, GetDefenderTeam() == TEAM_HORDE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_HORDE_ATTACK, GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);

            switch (GetDefenderTeam())
            {
            case TEAM_ALLIANCE:
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 0);
                }
                break;

            case TEAM_HORDE:
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 0);
                }
                break;
            }
        }
    }

    for (uint8 team = 0; team < 2; ++team)
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
        {
            if (Player* plr = sObjectMgr->GetPlayerByLowGUID((*itr)))
            {
                if (plr->GetTeamId() == GetDefenderTeam())
                {
                    uint32 k = urand(0, 3);
                    plr->TeleportTo(BATTLEFIELD_TB_MAPID, TbDefencerStartPosition[k].m_positionX, TbDefencerStartPosition[k].m_positionY, TbDefencerStartPosition[k].m_positionZ, TbDefencerStartPosition[k].m_orientation);
                    plr->CastSpell(plr, SPELL_TB_TOL_BARAD_SLOW_FALL, true);
                }
                else
                    plr->TeleportTo(BATTLEFIELD_TB_MAPID, TbDefencerStartPosition[4].m_positionX, TbDefencerStartPosition[4].m_positionY, TbDefencerStartPosition[4].m_positionZ, TbDefencerStartPosition[4].m_orientation);
            }
        }
    }

    DoPlaySoundToAll(BATTLEFIELD_TB_START);
    SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_START);
}

void BattlefieldTB::OnBattleEnd(bool endbytimer)
{
    if (sWorld->getWorldState(WS_TB_UNK_2) > (uint64)time(NULL))
    {
        if (sWorld->getWorldState(WS_TB_UNK) > 2)
            sWorld->setWorldState(WS_TB_UNK, 1);
        else
            sWorld->setWorldState(WS_TB_UNK, (sWorld->getWorldState(WS_TB_UNK) + 1));
        sWorld->setWorldState(WS_TB_UNK_2, time(NULL) + 86400);
    }

    for (uint8 team = 0; team < 2; ++team)
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
        {
            if (Player* plr = sObjectMgr->GetPlayerByLowGUID((*itr)))
            {
                if (endbytimer)
                {
                    if (plr->GetTeamId() == GetDefenderTeam())
                    {
                        switch (plr->GetTeamId())
                        {
                        case TEAM_ALLIANCE:
                            plr->CastSpell(plr, SPELL_TB_VICTORY_REWARD_ALLIANCE, true);
                            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_VETERAN, true);
                            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_BONUS, true);
                            IncrementQuest(plr, QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_A, true);
                            break;
                        case TEAM_HORDE:
                            plr->CastSpell(plr, SPELL_TB_VICTORY_REWARD_HORDE, true);
                            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_VETERAN, true);
                            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_BONUS, true);
                            IncrementQuest(plr, QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_H, true);
                            break;
                        }

                        DoCompleteOrIncrementAchievement(ACHIEVEMENT_TOL_BARAD_VICTORY, plr, 1);

                        if (m_Data32[BATTLEFIELD_TB_DATA_DESTROYED] == 0)
                            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_DEFENDED, true);
                    }
                    else
                    {
                        switch (plr->GetTeamId())
                        {
                        case TEAM_ALLIANCE:
                            IncrementQuest(plr, QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_A, true);
                            break;
                        case TEAM_HORDE:
                            IncrementQuest(plr, QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_H, true);
                            break;
                        }
                        plr->CastSpell(plr, SPELL_TB_LOOSER_REWARD, true);
                        plr->RepopAtGraveyard();
                    }
                }
                else
                {
                    if (plr->GetTeamId() == GetAttackerTeam())
                    {
                        switch (team)
                        {
                        case TEAM_ALLIANCE:
                            plr->CastSpell(plr, SPELL_TB_VICTORY_REWARD_ALLIANCE, true);
                            break;
                        case TEAM_HORDE:
                            plr->CastSpell(plr, SPELL_TB_VICTORY_REWARD_HORDE, true);
                            break;
                        }
                    }
                    else
                    {
                        plr->CastSpell(plr, SPELL_TB_LOOSER_REWARD, true);
                        plr->RepopAtGraveyard();
                    }
                }
            }
        }
        m_PlayersInWar[team].clear();
    }

    for (TbGameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
        if ((*itr))
            (*itr)->Rebuild();

    for (TbWorkShop::const_iterator itr = WorkShopList.begin(); itr != WorkShopList.end(); ++itr)
        if ((*itr))
            (*itr)->UpdateWorkshop();

    for (BfCapturePointSet::const_iterator itr = CapturePoints.begin(); itr != CapturePoints.end(); ++itr)
        if ((*itr))
            (*itr)->UpdateCapturePointValue();

    for (uint8 i = 0; i < BATTLEFIELD_TB_GY_MAX; i++)
        m_GraveyardList[i]->GiveControlTo(GetDefenderTeam());

    if (GetDefenderTeam() == TEAM_ALLIANCE)
    {
        for (GuidSet::const_iterator itr = npcAlliance.begin(); itr != npcAlliance.end(); ++itr)
            if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
                if (Creature* creature = unit->ToCreature())
                    ShowNpc(creature, true);

        for (GuidSet::const_iterator itr = npcHorde.begin(); itr != npcHorde.end(); ++itr)
            if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
                if (Creature* creature = unit->ToCreature())
                    HideNpc(creature);
    }
    else if (GetDefenderTeam() == TEAM_HORDE)
    {
        for (GuidSet::const_iterator itr = npcAlliance.begin(); itr != npcAlliance.end(); ++itr)
            if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
                if (Creature* creature = unit->ToCreature())
                    ShowNpc(creature, true);

        for (GuidSet::const_iterator itr = npcHorde.begin(); itr != npcHorde.end(); ++itr)
            if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
                if (Creature* creature = unit->ToCreature())
                    HideNpc(creature);
    }

    for (GuidSet::const_iterator itr = npcOld.begin(); itr != npcOld.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                ShowNpc(creature, true);

    for (GameobjectSet::const_iterator itr = goDoors.begin(); itr != goDoors.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->ToGameObject()->SetLootState(GO_READY);
            (*itr)->ToGameObject()->UseDoorOrButton();
        }
    }

    for (GuidSet::const_iterator itr = Vehicles.begin(); itr != Vehicles.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GuidSet::const_iterator itr = questgiversA.begin(); itr != questgiversA.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    for (GuidSet::const_iterator itr = questgiversH.begin(); itr != questgiversH.end(); ++itr)
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
            if (Creature* creature = unit->ToCreature())
                HideNpc(creature);

    int i = 0;
    GuidSet questgivers = GetDefenderTeam() == TEAM_ALLIANCE ? questgiversA : questgiversH;
    for (GuidSet::const_iterator itr = questgivers.begin(); itr != questgivers.end(); ++itr)
    {
        if (Unit* unit = sObjectAccessor->FindUnit((*itr)))
        {
            if (Creature* creature = unit->ToCreature())
            {
                if (!i || i == sWorld->getWorldState(WS_TB_UNK))
                    ShowNpc(creature, true);
                i++;
            }
        }
    }

    HashMapHolder<Player>::MapType const& plist = sObjectAccessor->GetPlayers();
    for (HashMapHolder<Player>::MapType::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
    {
        if (Player* plr = itr->second)
        {
            plr->SendUpdateWorldState(WS_TB_BATTLE_TIMER_ENABLED, 0);
            plr->SendUpdateWorldState(WS_TB_BATTLE_TIMER, 0);
            plr->SendUpdateWorldState(WS_TB_COUNTER_BUILDINGS, 0);
            plr->SendUpdateWorldState(WS_TB_COUNTER_BUILDINGS_ENABLED, 0);
            plr->SendUpdateWorldState(WS_TB_HORDE_DEFENCE, 0);
            plr->SendUpdateWorldState(WS_TB_ALLIANCE_DEFENCE, 0);
            plr->SendUpdateWorldState(WS_TB_NEXT_BATTLE_TIMER_ENABLED, 1);
            plr->SendUpdateWorldState(WS_TB_KEEP_HORDE_DEFENCE, GetDefenderTeam() == TEAM_HORDE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_KEEP_ALLIANCE_DEFENCE, GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);
            plr->SendUpdateWorldState(WS_TB_ALLIANCE_ATTACK, 0);
            plr->SendUpdateWorldState(WS_TB_HORDE_ATTACK, 0);

            switch (GetDefenderTeam())
            {
            case TEAM_ALLIANCE:
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_ALLIANCE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 0);
                }
                break;

            case TEAM_HORDE:
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_CAPTURE_POINT + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_EAST_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_SOUTH_SPIRE + i, 0);
                }
                for (int i = 0; i < BUILDING_MAX_DIFF; i++)
                {
                    if (i == BUILDING_HORDE_DEFENCE)
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 1);
                    else
                        plr->SendUpdateWorldState(WS_TB_WEST_SPIRE + i, 0);
                }
                break;
            }
        }
    }

    m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] = 0;
    m_Data32[BATTLEFIELD_TB_DATA_DESTROYED] = 0;
}

void BattlefieldTB::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(WS_TB_BATTLE_TIMER_ENABLED) << uint32(IsWarTime() ? 1 : 0);
    data << uint32(WS_TB_BATTLE_TIMER) << uint32(IsWarTime() ? uint32(time(NULL) + GetTimer() / 1000) : 0);
    data << uint32(WS_TB_COUNTER_BUILDINGS) << uint32(0);
    data << uint32(WS_TB_COUNTER_BUILDINGS_ENABLED) << uint32(IsWarTime() ? 1 : 0);
    data << uint32(WS_TB_HORDE_DEFENCE) << uint32(IsWarTime() ? (GetDefenderTeam() == TEAM_HORDE ? 1 : 0) : 0);
    data << uint32(WS_TB_ALLIANCE_DEFENCE) << uint32(IsWarTime() ? (GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0) : 0);
    data << uint32(WS_TB_NEXT_BATTLE_TIMER_ENABLED) << uint32(IsWarTime() ? 0 : 1);
    data << uint32(WS_TB_NEXT_BATTLE_TIMER) << uint32(!IsWarTime() ? uint32(time(NULL) + GetTimer() / 1000) : 0);
    data << uint32(WS_TB_ALLIANCE_ATTACK) << uint32(IsWarTime() ? (GetAttackerTeam() == TEAM_ALLIANCE ? 1 : 0) : 0);
    data << uint32(WS_TB_HORDE_ATTACK) << uint32(IsWarTime() ? (GetAttackerTeam() == TEAM_HORDE ? 1 : 0) : 0);

    if (!IsWarTime())
        data << uint32(WS_TB_NEXT_BATTLE_TIMER) << uint32(time(NULL) + (GetTimer() / 1000));
    else
        data << uint32(WS_TB_NEXT_BATTLE_TIMER) << uint32(0);
    data << uint32(WS_TB_KEEP_HORDE_DEFENCE) << uint32(GetDefenderTeam() == TEAM_HORDE ? 1 : 0);
    data << uint32(WS_TB_KEEP_ALLIANCE_DEFENCE) << uint32(GetDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);

    for (TbWorkShop::const_iterator itr = WorkShopList.begin(); itr != WorkShopList.end(); ++itr)
    {
        for (int i = 0; i < MAX_CP_DIFF; i++)
        {
            switch (i)
            {
            case ALLIANCE_DEFENCE:
                if ((*itr)->m_State == BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT)
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case HORDE_DEFENCE:
                if ((*itr)->m_State == BATTLEFIELD_TB_OBJECTSTATE_HORDE_INTACT)
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case ALLIANCE_ATTACK:
                if ((*itr)->m_State == BF_CAPTUREPOINT_OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE || (*itr)->m_State == BF_CAPTUREPOINT_OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE)
                    data << (uint32)((*itr)->m_WorldState + ALLIANCE_ATTACK) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + ALLIANCE_ATTACK) << uint32(0);
                break;
            case HORDE_ATTACK:
                if ((*itr)->m_State == BF_CAPTUREPOINT_OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE || (*itr)->m_State == BF_CAPTUREPOINT_OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE)
                    data << (uint32)((*itr)->m_WorldState + HORDE_ATTACK) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + HORDE_ATTACK) << uint32(0);
                break;
            default:
                data << (uint32)((*itr)->m_WorldState + NEUTRAL) << uint32(1);
            }
        }
    }

    for (TbGameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        for (int i = 0; i < BUILDING_MAX_DIFF; i++)
        {
            switch (i)
            {
            case BUILDING_HORDE_DEFENCE:
                if ((*itr)->m_State == (BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT - TEAM_HORDE * 3))
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case BUILDING_HORDE_DEFENCE_DAMAGED:
                if ((*itr)->m_State == (BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DAMAGE - TEAM_HORDE * 3))
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case BUILDING_DESTROYED:
                if ((*itr)->m_State == BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DESTROY - TEAM_HORDE * 3)
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case BUILDING_ALLIANCE_DEFENCE:
                if ((*itr)->m_State == (BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT - TEAM_ALLIANCE * 3))
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            case BUILDING_ALLIANCE_DEFENCE_DAMAGED:
                if ((*itr)->m_State == (BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DAMAGE - TEAM_ALLIANCE * 3))
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(1);
                else
                    data << (uint32)((*itr)->m_WorldState + i) << uint32(0);
                break;
            default:
                data << (uint32)((*itr)->m_WorldState + BUILDING_DESTROYED) << uint32(1);
            }
        }
    }
}

void BattlefieldTB::CapturePoint(uint32 team)
{
    if (team == BATTLEFIELD_TB_TEAM_NEUTRAL)
        return;

    if (team == GetDefenderTeam())
        m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]--;
    else
        m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]++;

    if (m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] < 0)
        m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] = 0;

    SendUpdateWorldState(WS_TB_COUNTER_BUILDINGS, m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]);
}

void BattlefieldTB::OnDestroyed()
{
    for (uint8 team = 0; team < 2; team++)
    {
        for (GuidSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if (Player *plr = sObjectMgr->GetPlayerByLowGUID((*itr)))
                if (plr->GetTeam() == GetAttackerTeam())
                {
                    plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_DESTROYED, true);
                    plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_BONUS, true);
                }
    }
    m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]++;
}

void BattlefieldTB::OnDamaged()
{
    for(uint8 team = 0; team < 2; team++)
        for (GuidSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if (Player *plr = sObjectMgr->GetPlayerByLowGUID((*itr)))
                if (plr->GetTeam() == GetAttackerTeam())
                    plr->CastSpell(plr, SPELL_TB_TOL_BARAD_TOWER_DAMAGED, true);
}

void BattlefieldTB::ProcessEvent(GameObject* obj, uint32 eventId)
{
    if (!obj || !IsWarTime())
        return;

    for (TbGameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (obj->GetEntry()==(*itr)->m_Build->GetEntry())
        {
            if ((*itr)->m_Build->GetGOInfo()->building.damagedEvent == eventId)
                (*itr)->Damaged();

            if ((*itr)->m_Build->GetGOInfo()->building.destroyedEvent == eventId)
                (*itr)->Destroyed();

            break;
        }
    }
}

void BattlefieldTB::SendInitWorldStatesTo(Player* player)
{
    WorldPacket data(SMSG_INIT_WORLD_STATES, (4 + 4 + 4 + 2 + (BuildingsInZone.size() * 8)));

    data << uint32(m_MapId);
    data << uint32(m_ZoneId);
    data << uint32(0);
    data << uint16(10 + BuildingsInZone.size());

    FillInitialWorldStates(data);

    player->GetSession()->SendPacket(&data);
}

void BattlefieldTB::SendInitWorldStatesToAll()
{
    for (uint8 team = 0; team < 2; team++)
        for (GuidSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                SendInitWorldStatesTo(player);
}

void BattlefieldTB::AddBrokenTower(TeamId team)
{
    m_BattleTime += 5 * MINUTE * IN_MILLISECONDS;
    m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]++;

    if (team == GetDefenderTeam())
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer((*itr)))
                player->RemoveAuraFromStack(SPELL_TB_TOL_BARAD_TOWER_BONUS);

        for (GuidSet::const_iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer((*itr)))
                player->CastSpell(player, SPELL_TB_TOL_BARAD_TOWER_BONUS, true);

        if (m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] == 3)
        {
            if (int32(m_Timer - 600000) < 0)
                m_Timer = 0;
            else
                m_Timer -= 600000;

            SendInitWorldStatesToAll();
        }
    }
}

void BattlefieldTB::OnPlayerJoinWar(Player* plr)
{
    plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_VETERAN);
    plr->RemoveAurasDueToSpell(SPELL_TB_SPIRITUAL_IMMUNITY);
    plr->RemoveAurasDueToSpell(SPELL_TB_TOL_BARAD_TOWER_BONUS);

    if (plr->GetZoneId() != m_ZoneId)
    {
        if (plr->GetTeamId() == GetDefenderTeam())
        {
            plr->TeleportTo(BATTLEFIELD_TB_MAPID, -1244.58f, 981.233f, 155.426f, 0);
            plr->CastSpell(plr, SPELL_TB_TOL_BARAD_SLOW_FALL, true);
        }
        else
            plr->TeleportTo(BATTLEFIELD_TB_MAPID, -827.212646f, 1187.948608f, 112.81f, 3.092834f);
    }

    if (plr->GetTeamId() == GetAttackerTeam())
    {
        plr->CastSpell(plr, SPELL_TB_TOL_BARAD_VETERAN, true);
        if (3 - m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] > 0)
            plr->SetAuraStack(SPELL_TB_TOL_BARAD_TOWER_BONUS, plr, 3 - m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]);
    }
    else
    {
        if (m_Data32[BATTLEFIELD_TB_DATA_CAPTURED] > 0)
            plr->SetAuraStack(SPELL_TB_TOL_BARAD_TOWER_BONUS, plr, m_Data32[BATTLEFIELD_TB_DATA_CAPTURED]);
    }

    SendInitWorldStatesTo(plr);
}

BfCapturePointTB::BfCapturePointTB(BattlefieldTB* battlefield, TeamId control) : BfCapturePoint(battlefield)
{
    m_Bf = battlefield;
    m_team = control;
    m_WorkShop = NULL;
}

void BfCapturePointTB::ChangeTeam(TeamId /*oldTeam*/)
{
    ASSERT(m_WorkShop);
    m_WorkShop->ChangeControl(m_team, false);
}

BfGraveyardTB::BfGraveyardTB(BattlefieldTB* battlefield) : BfGraveyard(battlefield)
{
    m_Bf = battlefield;
}

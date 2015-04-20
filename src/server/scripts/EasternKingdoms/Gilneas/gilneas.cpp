/*
*
* Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
*
*/

#include "gilneas.h"

class npc_panicked_citizen_gate : public CreatureScript
{
public:
    npc_panicked_citizen_gate() : CreatureScript("npc_panicked_citizen_gate") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_panicked_citizen_gateAI(creature);
    }

    struct npc_panicked_citizen_gateAI : public ScriptedAI
    {
        npc_panicked_citizen_gateAI(Creature* creature) : ScriptedAI(creature)
        {
            uiRandomEmoteTimer = urand(4000, 8000);
        }

        uint32 uiRandomEmoteTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiRandomEmoteTimer <= diff)
            {
                uiRandomEmoteTimer = urand(2000, 5000);
                uint8 roll = urand(0, 5);
                me->HandleEmoteCommand(PanickedCitizenRandomEmote[roll]);
            } else
                uiRandomEmoteTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_gilneas_city_guard_gate : public CreatureScript
{
public:
    npc_gilneas_city_guard_gate() : CreatureScript("npc_gilneas_city_guard_gate") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_city_guard_gateAI(creature);
    }

    struct npc_gilneas_city_guard_gateAI : public ScriptedAI
    {
        npc_gilneas_city_guard_gateAI(Creature* creature) : ScriptedAI(creature)
        {
            uiEventTimer = urand(10000, 40000);
            uiEventPhase = 0;

            if (me->GetDistance2d(-1430.47f, 1345.55f) < 10.0f)
                Event = true;
            else
                Event = false;
        }

        uint32 uiEventTimer;
        uint8 uiEventPhase;
        bool Event;

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    switch (uiEventPhase)
                    {
                    case 0:
                        {
                            std::list<Creature*> lCitizen;
                            me->GetCreatureListWithEntryInGrid(lCitizen, 44086, 35.0f);

                            if (!lCitizen.empty())
                            {
                                uint8 roll = urand(0, lCitizen.size() - 1);
                                std::list<Creature*>::iterator itr = lCitizen.begin();
                                std::advance(itr, roll);
                                Creature* citizen = *itr;

                                if (citizen)
                                {
                                    ++uiEventPhase;
                                    uiEventTimer = urand(5000, 10000);
                                    uint8 roll = urand(0, 2);
                                    DoScriptText(PANICKED_CITIZEN_RANDOM_SAY - roll, citizen);
                                    return;
                                }
                            }

                            uiEventTimer = urand(10000, 40000);
                        }
                        break;
                    case 1:
                        --uiEventPhase;
                        uiEventTimer = urand(10000, 40000);
                        uint8 roll = urand(0, 2);
                        DoScriptText(GILNEAS_CITY_GUARD_RANDOM_SAY - roll, me);
                        break;
                    }
                } else
                    uiEventTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_gilnean_crow : public CreatureScript
{
public:
    npc_gilnean_crow() : CreatureScript("npc_gilnean_crow") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilnean_crowAI(creature);
    }

    struct npc_gilnean_crowAI : public npc_vehicle_escortAI
    {
        npc_gilnean_crowAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 2.0f, true);
            CheckFly();
            Flying = false;
        }

        uint8 point_id;
        bool Flying;

        bool CheckFly()
        {
            float dist = 100500;
            float check_dist;
            int i = -1;

            for (int j = 0; j < 12; ++j)
            {
                check_dist = me->GetDistance2d(CrowFlyPos[j].FirstCoord.x, CrowFlyPos[j].FirstCoord.y);

                if (dist > check_dist && check_dist < 50.0f)
                {
                    dist = check_dist;
                    i = j;
                }
            }

            if (i == -1)
                return false;

            point_id = i;
            return true;
        }

        void FlyAway()
        {
            Flying = true;
            me->SetCanFly(true);
            AddWaypoint(0,
                CrowFlyPos[point_id].FirstCoord.x + irand(-4, 4),
                CrowFlyPos[point_id].FirstCoord.y + irand(-4, 4),
                CrowFlyPos[point_id].FirstCoord.z + irand(-4, 4));
            AddWaypoint(0,
                CrowFlyPos[point_id].SecondCoord.x + irand(-4, 4),
                CrowFlyPos[point_id].SecondCoord.y + irand(-4, 4),
                CrowFlyPos[point_id].SecondCoord.z + irand(-4, 4));
            Start();
        }

        void JustRespawned()
        {
            me->RemoveAura(SPELL_EXPLOSION);
            me->SetCanFly(true);
        }

        void FinishEscort()
        {
            Flying = false;
            me->AddAura(SPELL_EXPLOSION, me);
            me->SetCanFly(false);
            me->DespawnOrUnsummon();
        }

        void WaypointReached(uint32 point)
        { }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                if (me->IsWithinDistInMap(who, 15.0f))
                    if (!Flying)
                        FlyAway();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class npc_prince_liam_greymane_intro : public CreatureScript
{
public:
    npc_prince_liam_greymane_intro() : CreatureScript("npc_prince_liam_greymane_intro") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymane_introAI(creature);
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_LOCKDOWN)
            if (Creature* citizen = creature->FindNearestCreature(NPC_PANCKED_CITIZEN, 20.0f))
                citizen->DoPersonalScriptText(CITIZEN_SAY_WHAT_AT_THE_ROOF, player);

        return true;
    }

    struct npc_prince_liam_greymane_introAI : public ScriptedAI
    {
        npc_prince_liam_greymane_introAI(Creature* c) : ScriptedAI(c) { }

        uint32 uiSayTimer;
        uint8 uiSayCount;

        void Reset()
        {
            uiSayTimer = 1000;
            uiSayCount = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiSayTimer <= diff)
            {
                ++uiSayCount;

                switch (uiSayCount)
                {
                case 1:
                    DoScriptText(LIAM_INTRO_1, me);
                    uiSayTimer = 12000;
                    break;
                case 2:
                    DoScriptText(LIAM_INTRO_2, me);
                    uiSayTimer = 12000;
                    break;
                case 3:
                    DoScriptText(LIAM_INTRO_3, me);
                    uiSayTimer = 18000;
                    uiSayCount = 0;
                    break;
                }
            } else
                uiSayTimer -= diff;
        }
    };
};

///////////
// Quest Lockdown! 14078
///////////

class npc_lieutenant_walden : public CreatureScript
{
public:
    npc_lieutenant_walden() : CreatureScript("npc_lieutenant_walden") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_LOCKDOWN)
        {
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_1, false);
            player->CastSpell(player, SPELL_QUEST_INVISIBILITY_DETECTION_3, false);

            player->SaveToDB();
        }

        return true;
    }
};

///////////
// Quest Royal Orders 14099
///////////

class npc_prince_liam_greymane_phase_1 : public CreatureScript
{
public:
    npc_prince_liam_greymane_phase_1() : CreatureScript("npc_prince_liam_greymane_phase_1") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() ==  QUEST_ROYAL_ORDERS)
        {
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_3);
            player->CastSpell(player, SPELL_QUEST_INVISIBILITY_DETECTION_2, false);
            player->SaveToDB();
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymane_phase_1AI(creature);
    }

    struct npc_prince_liam_greymane_phase_1AI : public Scripted_NoMovementAI
    {
        npc_prince_liam_greymane_phase_1AI(Creature* c) : Scripted_NoMovementAI(c)
        {
            SetCombatMovement(false);
            me->_ReactDistance = 10.0f;
        }

        uint32 uiSayTimer;
        uint32 uiShootTimer;
        bool miss;

        void Reset()
        {
            uiSayTimer = urand(5000, 10000);
            uiShootTimer = 1500;
            miss = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;
                Unit* target = NULL;

                if (target = me->FindNearestCreature(NPC_RAMPAGING_WORGEN, 40.0f))
                    if (target != me->getVictim())
                    {
                        me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                        me->CombatStart(target);
                        me->AddThreat(target, 1000);
                    }

                    if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                    {
                        me->Attack(me->getVictim(), false);
                        me->CastSpell(me->getVictim(), SPELL_SHOOT, false);
                    } else
                        me->Attack(me->getVictim(), true);
            } else
                uiShootTimer -= diff;

            if (uiSayTimer <= diff)
            {
                uiSayTimer = urand(30000, 120000);
                uint8 id = urand(0, 4);
                DoScriptText(LIAM_RANDOM_YELL - id, me);
            } else
                uiSayTimer -= diff;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_rampaging_worgen : public CreatureScript
{
public:
    npc_rampaging_worgen() : CreatureScript("npc_rampaging_worgen") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rampaging_worgenAI(creature);
    }

    struct npc_rampaging_worgenAI : public ScriptedAI
    {
        npc_rampaging_worgenAI(Creature* c) : ScriptedAI(c) { }

        uint32 uiEnemyEntry;
        bool enrage;
        bool miss;

        void Reset()
        {
            enrage = false;
            miss = false;

            if (me->GetEntry() == NPC_AFFLICTED_GILNEAN)
                me->_ReactDistance = 3.0f;
            else
                me->_ReactDistance = 10.0f;

            switch (me->GetEntry())
            {
            case NPC_RAMPAGING_WORGEN:
                uiEnemyEntry = NPC_GILNEAS_CITY_GUARD;
                break;
            case NPC_BLOODFANG_WORGEN:
                uiEnemyEntry = NPC_GILNEAN_ROYAL_GUARD;
                break;
            case NPC_FRENZIED_STALKER:
                uiEnemyEntry = NPC_NORTHGATE_REBEL;
                break;
            default:
                uiEnemyEntry = 0;
                break;
            }
        }

        void DamageTaken(Unit* attacker, uint32 &/*damage*/)
        {
            if (attacker->GetTypeId() != TYPEID_PLAYER)
                return;

            Unit* victim = NULL;

            if (victim = me->getVictim())
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    return;

            if (victim)
                me->getThreatManager().modifyThreatPercent(victim, -100);

            AttackStart(attacker);
            me->AddThreat(attacker, 10005000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!enrage && me->HealthBelowPct(30))
            {
                DoCast(me, SPELL_ENRAGE);
                enrage = true;
            }

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_gilneas_city_guard : public CreatureScript
{
public:
    npc_gilneas_city_guard() : CreatureScript("npc_gilneas_city_guard") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_city_guardAI(creature);
    }

    struct npc_gilneas_city_guardAI : public Scripted_NoMovementAI
    {
        npc_gilneas_city_guardAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            uiEmoteState = me->GetUInt32Value(UNIT_NPC_EMOTESTATE);
        }

        uint32 uiYellTimer;
        uint32 uiEnemyEntry;
        uint32 uiShootTimer;
        uint32 uiEmoteState;
        bool miss;
        bool CanSay;

        void Reset()
        {
            uiYellTimer = urand(25000, 75000);
            uiShootTimer = 1000;
            miss = false;
            CanSay = false;
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, uiEmoteState);

            switch (me->GetEntry())
            {
            case NPC_GILNEAS_CITY_GUARD:
                uiEnemyEntry = NPC_RAMPAGING_WORGEN;
                break;
            case NPC_GILNEAN_ROYAL_GUARD:
                uiEnemyEntry = NPC_BLOODFANG_WORGEN;
                break;
            case NPC_GILNEAS_CITY_GUARD_PHASE_4:
                CanSay = true;
                uiEnemyEntry = NPC_AFFLICTED_GILNEAN;
                break;
            case NPC_NORTHGATE_REBEL_PHASE_5:
                uiEnemyEntry = NPC_BLOODFANG_STALKER_PHASE_5;
                break;
            case NPC_NORTHGATE_REBEL:
                uiEnemyEntry = NPC_FRENZIED_STALKER;
                break;
            default:
                uiEnemyEntry = 0;
                break;
            }

            if (me->GetEntry() == NPC_GILNEAS_CITY_GUARD_PHASE_4)
            {
                me->_ReactDistance = 3.0f;
                SetCombatMovement(false);
            } else
                me->_ReactDistance = 10.0f;

            if (me->GetDistance2d(-1770.2f, 1343.78f) < 25.0f)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            if (Unit* target = me->FindNearestCreature(uiEnemyEntry, 40.0f))
                AttackStart(target);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CanSay)
                if (uiYellTimer <= diff)
                {
                    uiYellTimer = urand(50000, 100000);
                    uint8 roll = urand(0, 2);
                    DoScriptText(GUARD_RANDOM_YELL - roll, me);
                } else
                    uiYellTimer -= diff;

                if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                    if (me->getVictim()->GetHealthPct() < 90)
                        miss = true;
                    else
                        miss = false;

                if (uiShootTimer <= diff)
                {
                    uiShootTimer = 2500;
                    Unit* target = NULL;

                    if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                        if (uiEnemyEntry)
                            if (target = me->FindNearestCreature(uiEnemyEntry, 40.0f))
                                if (target != me->getVictim())
                                {
                                    me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                                    me->CombatStart(target);
                                    me->AddThreat(target, 1000);
                                }

                                if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                                {
                                    if (me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                                    {
                                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                                        me->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                                        me->SendMeleeAttackStop(me->getVictim());
                                    }

                                    me->CastSpell(me->getVictim(), SPELL_SHOOT, false);
                                } else
                                    if (!me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                                    {
                                        me->AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                                        me->SendMeleeAttackStart(me->getVictim());
                                    }
                } else
                    uiShootTimer -= diff;

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Evacuate the Merchant Square 14098
///////////

class npc_qems_citizen : public CreatureScript
{
public:
    npc_qems_citizen() : CreatureScript("npc_qems_citizen") { }

    struct npc_qems_citizenAI : public npc_escortAI
    {
        npc_qems_citizenAI(Creature* c) : npc_escortAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
            uiEventTimer = 1000;
            pause = false;
        }

        uint32 uiEventTimer;
        bool pause;

        void WaypointReached(uint32 point)
        {
            if (point == 1)
            {
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                            player->KilledMonsterCredit(NPC_QEMS_KILL_CREDIT, 0);

                SetEscortPaused(true);
                pause = true;

                if (me->GetEntry() == NPC_J_CITIZEN)
                {
                    uint8 roll = urand(0, 7);
                    DoScriptText(NPC_J_CITIZEN_RANDOM_SAY - roll, me);
                } else
                {
                    uint8 roll = urand(0, 6);
                    DoScriptText(NPC_CW_CITIZEN_RANDOM_SAY - roll, me);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_COWER);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (pause)
                if (uiEventTimer <= diff)
                {
                    pause = false;
                    SetEscortPaused(false);
                } else
                    uiEventTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qems_citizenAI(creature);
    }
};

class npc_qems_worgen : public CreatureScript
{
public:
    npc_qems_worgen() : CreatureScript("npc_qems_worgen") { }

    struct npc_qems_worgenAI : public npc_escortAI
    {
        npc_qems_worgenAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            uiPauseTimer = 4000;
            enrage = false;
            pause = true;
        }

        uint32 uiPauseTimer;
        bool pause;
        bool enrage;

        void WaypointReached(uint32 point)
        {
            if (point == 1)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        AttackStart(summoner);
                    else
                        me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (pause)
                if (uiPauseTimer <= diff)
                {
                    pause = false;
                    SetEscortPaused(false);
                } else
                    uiPauseTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (!enrage && me->HealthBelowPct(30))
                {
                    DoCast(SPELL_CW_WORGEN_ENRAGE);
                    enrage = true;
                }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qems_worgenAI(creature);
    }
};

class go_merchant_square_door : public GameObjectScript
{
public:
    go_merchant_square_door() : GameObjectScript("go_merchant_square_door") { }

    void SummonQuestCreature(uint32 entry, bool IsCitizen, GameObject* go, Player* player)
    {
        if (!player)
            return;

        float x, y;
        go->GetNearPoint2D(x, y, 2.8f, go->GetOrientation() + M_PI * 0.75f);

        if (Creature* creature = player->SummonCreature(entry, x, y, go->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
        {
            creature->SetPhaseMask(PHASE_ONE, true);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            npc_escortAI* npc_escort = NULL;

            if (IsCitizen)
                npc_escort = CAST_AI(npc_qems_citizen::npc_qems_citizenAI, creature->AI());
            else
                npc_escort = CAST_AI(npc_qems_worgen::npc_qems_worgenAI, creature->AI());

            if (!npc_escort)
                return;

            float x, y, z = creature->GetPositionZ();
            go->GetNearPoint2D(x, y, 1.0f, go->GetOrientation() + M_PI);
            npc_escort->AddWaypoint(0, x, y, z);
            go->GetNearPoint2D(x, y, 3.0f, go->GetOrientation());
            npc_escort->AddWaypoint(1, x, y, z);

            if (IsCitizen)
            {
                float x, y;
                go->GetNearPoint2D(x, y, 10.0f, go->GetOrientation());
                const Position src ={x, y, go->GetPositionZ(), 0};
                Position dst;
                go->GetRandomPoint(src, 5.0f, dst);
                npc_escort->AddWaypoint(2, dst.GetPositionX(), dst.GetPositionY(), z);
            }

            npc_escort->Start(false, true);
            npc_escort->SetDespawnAtEnd(IsCitizen);
            npc_escort->SetEscortPaused(!IsCitizen);
        }
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        uint8 roll = urand(ACTION_JUST_CITIZEN, ACTION_CITIZEN_AND_WORGEN);

        switch (roll)
        {
        case ACTION_JUST_CITIZEN:
            SummonQuestCreature(NPC_J_CITIZEN, true, go, player);
            break;
        case ACTION_CITIZEN_AND_WORGEN:
            SummonQuestCreature(NPC_CW_CITIZEN, true, go, player);
            SummonQuestCreature(NPC_CW_WORGEN, false, go, player);
            break;
        }

        return false;
    }
};

///////////
// Quest Old Divisions 14157
///////////

class npc_king_genn_greymane_qod : public CreatureScript
{
public:
    npc_king_genn_greymane_qod() : CreatureScript("npc_king_genn_greymane_qod") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_genn_greymane_qodAI(creature);
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_OLD_DIVISIONS)
            if (Creature* godfrey = creature->FindNearestCreature(NPC_LORD_GODFREY_QOD, 20.0f))
            {
                godfrey->DoPersonalScriptText(NPC_LORD_GODFREY_QOD_SAY, player);
                Psc new_psc;
                new_psc.uiPersonalTimer = 5000;
                new_psc.uiPlayerGUID = player->GetGUID();
                CAST_AI(npc_king_genn_greymane_qodAI, creature->AI())->lPlayerList.push_back(new_psc);
            }

            return true;
    }

    struct npc_king_genn_greymane_qodAI : public ScriptedAI
    {
        npc_king_genn_greymane_qodAI(Creature* creature) : ScriptedAI(creature) { }

        std::list<Psc> lPlayerList;

        void UpdateAI(const uint32 diff)
        {
            if (lPlayerList.empty())
                return;

            for (std::list<Psc>::iterator itr = lPlayerList.begin(); itr != lPlayerList.end();)
                if ((*itr).uiPersonalTimer <= diff)
                {
                    if (Player* player = Unit::GetPlayer(*me, (*itr).uiPlayerGUID))
                        me->DoPersonalScriptText(NPC_KING_GENN_GREYMANE_QOD_SAY, player);

                    itr = lPlayerList.erase(itr);
                } else
                {
                    (*itr).uiPersonalTimer -= diff;
                    ++itr;
                }
        }
    };
};

///////////
// Quest By the Skin of His Teeth 14154
///////////

class npc_worgen_attacker : public CreatureScript
{
public:
    npc_worgen_attacker() : CreatureScript("npc_worgen_attacker") { }

    struct npc_worgen_attackerAI : public npc_escortAI
    {
        npc_worgen_attackerAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            enrage = false;
            uiHorrifyingHowlTimer = 3000;
        }

        bool enrage;
        uint32 uiHorrifyingHowlTimer;

        void StartAttack()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    me->CombatStart(summoner);
                    me->AddThreat(summoner, 100500);
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 2)
                if (me->GetEntry() == NPC_WORGEN_ALPHA_SW || NPC_WORGEN_RUNT_SW)
                    StartAttack();

            if (point == 5)
                StartAttack();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (!enrage && me->HealthBelowPct(30))
            {
                enrage = true;
                DoCast(SPELL_ENRAGE);
            }

            if (uiHorrifyingHowlTimer <= diff)
            {
                uiHorrifyingHowlTimer = 15000;
                DoCast(SPELL_HORRIFYING_HOWL);
            } else
                uiHorrifyingHowlTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_worgen_attackerAI(creature);
    }
};

class npc_lord_darius_crowley : public CreatureScript
{
public:
    npc_lord_darius_crowley() : CreatureScript("npc_lord_darius_crowley") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BY_THE_SKIN_OF_HIS_TEETH)
        {
            creature->CastSpell(player, SPELL_GILNEAS_PRISON_PERIODIC_FORCECAST, true);
            creature->AI()->DoAction(ACTION_START_EVENT);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_darius_crowleyAI(creature);
    }

    struct npc_lord_darius_crowleyAI : public Scripted_NoMovementAI
    {
        npc_lord_darius_crowleyAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            Event = false;
            uiEventTimer = 120000;
            uiSummonTimer = 3000;
            uiDShoutTimer = 15000;
            uiType = TYPE_FIRST_WAVE;
            SetCombatMovement(false);
        }

        uint32 uiDShoutTimer;
        uint32 uiSummonTimer;
        uint32 uiEventTimer;
        uint8 uiType;
        bool Event;

        void DoAction(const int32 action)
        {
            if (action == ACTION_START_EVENT)
            {
                Event = true;
                uiEventTimer = 120000;
            }
        }

        void GetNearPoint2D(float src_x, float src_y, float &dst_x, float &dst_y, float distance2d, float absAngle) const
        {
            dst_x = src_x + distance2d * cos(absAngle);
            dst_y = src_y + distance2d * sin(absAngle);

            Trinity::NormalizeMapCoord(dst_x);
            Trinity::NormalizeMapCoord(dst_y);
        }

        void SummonWorgen(uint32 entry, uint8 id)
        {
            float x, y;
            GetNearPoint2D(WorgenPosT2[id][0], WorgenPosT2[id][1], x, y, 3.0f + rand()%90, WorgenPosT2[id][3]);

            if (Creature* worgen = me->SummonCreature(entry, x, y, WorgenPosT2[id][2] + 2.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
                if (npc_escortAI* npc_escort = CAST_AI(npc_worgen_attacker::npc_worgen_attackerAI, worgen->AI()))
                {
                    npc_escort->AddWaypoint(0, WorgenPosT2[id][0], WorgenPosT2[id][1], WorgenPosT2[id][2], 0, true);
                    const Position src ={WorgenPosT2[7][0], WorgenPosT2[7][1], WorgenPosT2[7][2], WorgenPosT2[7][3]};
                    Position dst;
                    worgen->GetRandomPoint(src, 5.0f, dst);
                    npc_escort->AddWaypoint(1, dst.GetPositionX(), dst.GetPositionY(), dst.GetPositionZ());
                    npc_escort->AddWaypoint(2, -1679.73f, 1442.12f, 52.3705f);
                    npc_escort->Start(true, true);
                    npc_escort->SetDespawnAtEnd(false);
                }
        }

        void SummonFirstWave()
        {
            if (Creature* worgen = me->SummonCreature(NPC_WORGEN_ALPHA_FW, WorgenPosT1[0][0], WorgenPosT1[0][1], WorgenPosT1[0][2], WorgenPosT1[0][3], TEMPSUMMON_DEAD_DESPAWN, 500))
                for (int i = 0; i < 2; ++i)
                {
                    float x, y;
                    worgen->GetNearPoint2D(x, y, urand(1, 4), WorgenPosT1[0][3] + M_PI / 2);
                    me->SummonCreature(NPC_WORGEN_RUNT_FW, x, y, WorgenPosT1[0][2], WorgenPosT1[0][3], TEMPSUMMON_DEAD_DESPAWN, 500);
                    worgen->GetNearPoint2D(x, y, urand(1, 5), WorgenPosT1[0][3] - M_PI / 2);
                    me->SummonCreature(NPC_WORGEN_RUNT_FW, x, y, WorgenPosT1[0][2], WorgenPosT1[0][3], TEMPSUMMON_DEAD_DESPAWN, 500);
                }
        }

        void SummonSecondWave()
        {
            SummonWorgen(NPC_WORGEN_ALPHA_SW, 6);

            for (int i = 0; i < 4; ++i)
            {
                uint8 roll = urand(0, 5);
                SummonWorgen(NPC_WORGEN_RUNT_SW, roll);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_WORGEN_ALPHA_FW || summoned->GetEntry() == NPC_WORGEN_RUNT_FW)
                if (npc_escortAI* worgen = CAST_AI(npc_worgen_attacker::npc_worgen_attackerAI, summoned->AI()))
                {
                    worgen->AddWaypoint(1, WorgenPosT1[0][0], WorgenPosT1[0][1], WorgenPosT1[0][2]);
                    worgen->AddWaypoint(2, WorgenPosT1[1][0], WorgenPosT1[1][1], WorgenPosT1[1][2]);
                    worgen->AddWaypoint(3, WorgenPosT1[2][0], WorgenPosT1[2][1], WorgenPosT1[2][2], 0, true);
                    worgen->AddWaypoint(4, WorgenPosT1[3][0], WorgenPosT1[3][1], WorgenPosT1[3][2]);
                    worgen->AddWaypoint(5, -1679.73f, 1442.12f, 52.3705f);
                    worgen->Start(true, true);
                    worgen->SetDespawnAtEnd(false);
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
            {
                if (uiSummonTimer <= diff)
                {
                    uiSummonTimer = 15000;

                    switch (uiType)
                    {
                    case TYPE_FIRST_WAVE:
                        uiType = TYPE_SECOND_WAVE;
                        SummonFirstWave();
                        break;
                    case TYPE_SECOND_WAVE:
                        uiType = TYPE_FIRST_WAVE;
                        SummonSecondWave();
                        break;
                    }
                } else
                    uiSummonTimer -= diff;

                if (uiEventTimer <= diff)
                    Event = false;
                else
                    uiEventTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (uiDShoutTimer <= diff)
            {
                uiDShoutTimer = 15000;
                DoCast(SPELL_DEMORALIZING_SHOUT);
            } else
                uiDShoutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest The Rebel Lord's Arsenal 14159
///////////

class npc_josiah_avery_worgen : public CreatureScript
{
public:
    npc_josiah_avery_worgen() : CreatureScript("npc_josiah_avery_worgen") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_josiah_avery_worgenAI(creature);
    }

    struct npc_josiah_avery_worgenAI : public ScriptedAI
    {
        npc_josiah_avery_worgenAI(Creature* creature) : ScriptedAI(creature)
        {
            uiEventTimer = 500;
            uiPase = 1;
            uiPlayerGUID = 0;
            me->SetReactState(REACT_PASSIVE);

            if (me->isSummon())
                Event = true;
            else
                Event = false;
        }

        uint64 uiPlayerGUID;
        uint32 uiEventTimer;
        uint8 uiPase;
        bool Event;

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FAKE_SHOT)
                me->Kill(me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    if (Creature* lorna = me->FindNearestCreature(NPC_LORNA_CROWLEY, 30.0f))
                        switch (uiPase)
                    {
                        case 1:
                            ++uiPase;
                            uiEventTimer = 1000;
                            if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                            {
                                float x, y, z;
                                lorna->GetPosition(x, y, z);
                                me->CastSpell(player, 91074, false);
                                player->GetMotionMaster()->MoveJump(x, y, z, 25.0f, 5.0f);
                                lorna->Whisper(SAY_WORGEN_BITE, player->GetGUID(), true);
                            }
                            break;
                        case 2:
                            Event = false;
                            uiEventTimer = 1000;
                            float x, y, z;
                            lorna->GetPosition(x, y, z);
                            me->GetMotionMaster()->MoveJump(x, y, z, 25.0f, 10.0f);
                            lorna->CastSpell(me, SPELL_FAKE_SHOT, false);
                            break;
                    }
                } else
                    uiEventTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_josiah_avery_human : public CreatureScript
{
public:
    npc_josiah_avery_human() : CreatureScript("npc_josiah_avery_human") { }

    bool OnQuestComplete(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_REBEL_LORD_ARSENAL)
        {
            player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_1);
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_2);
            player->CastSpell(player, SPELL_WORGEN_BITE, true);
            float p_x, p_y;
            player->GetPosition(p_x, p_y);
            float x, y, z, o = creature->GetAngle(p_x, p_y);
            creature->GetPosition(x, y, z);
            player->SaveToDB();

            if (Creature* josiah = player->SummonCreature(NPC_JOSIAH_AVERY_WORGEN, x, y, z, o, TEMPSUMMON_CORPSE_DESPAWN, 10000))
            {
                CAST_AI(npc_josiah_avery_worgen::npc_josiah_avery_worgenAI, josiah->AI())->uiPlayerGUID = player->GetGUID();
                josiah->SetSeerGUID(player->GetGUID());
                josiah->SetVisible(false);
            }
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_josiah_avery_humanAI(creature);
    }

    struct npc_josiah_avery_humanAI : public ScriptedAI
    {
        npc_josiah_avery_humanAI(Creature* creature) : ScriptedAI(creature)
        {
            uiRandomSpeachTimer = urand(5000, 15000);
        }

        uint32 uiRandomSpeachTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiRandomSpeachTimer <= diff)
            {
                uiRandomSpeachTimer = urand(5000, 15000);
                uint8 roll = urand(0, 5);
                Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

                if (PlList.isEmpty())
                    return;

                float z = me->GetPositionZ();
                uint32 uiPhase = me->GetPhaseMask();

                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                    if (Player* player = i->getSource())
                        if (uiPhase == player->GetPhaseMask())
                            if (me->GetDistance(player) < 35.0f)
                                if (abs(z - player->GetPositionZ()) < 5.0f)
                                    me->DoPersonalScriptText(RANDOM_JOSIAH_YELL - roll, player);
            } else
                uiRandomSpeachTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest From the Shadows 14204
///////////

class npc_lorna_crowley_qfs : public CreatureScript
{
public:
    npc_lorna_crowley_qfs() : CreatureScript("npc_lorna_crowley_qfs") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_FROM_THE_SHADOWS)
        {
            player->CastSpell(player, SPELL_SUMMON_MASTIFF, false);
            creature->DoPersonalScriptText(SAY_MASTIFF, player);
        }
        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_FROM_THE_SHADOWS)
            if (Unit* charm = Unit::GetCreature(*creature, player->GetMinionGUID()))
                if (charm->GetEntry() == NPC_GILNEAS_MASTIFF)
                    if (Creature* mastiff = charm->ToCreature())
                        mastiff->DespawnOrUnsummon();

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lorna_crowley_qfsAI(creature);
    }

    struct npc_lorna_crowley_qfsAI : public ScriptedAI
    {
        npc_lorna_crowley_qfsAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
        }
    };
};

class npc_gilneas_mastiff : public CreatureScript
{
public:
    npc_gilneas_mastiff() : CreatureScript("npc_gilneas_mastiff") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_mastiffAI(creature);
    }

    struct npc_gilneas_mastiffAI : public ScriptedAI
    {
        npc_gilneas_mastiffAI(Creature* creature) : ScriptedAI(creature)
        {
            if (CharmInfo* charm = me->GetCharmInfo())
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ATTACK_LURKER);
                charm->InitEmptyActionBar();
                charm->AddSpellToActionBar(spellInfo);
                me->SetReactState(REACT_DEFENSIVE);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_bloodfang_lurker : public CreatureScript
{
public:
    npc_bloodfang_lurker() : CreatureScript("npc_bloodfang_lurker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodfang_lurkerAI(creature);
    }

    struct npc_bloodfang_lurkerAI : public ScriptedAI
    {
        npc_bloodfang_lurkerAI(Creature* creature) : ScriptedAI(creature) { }

        bool enrage;
        bool frenzy;

        void Reset()
        {
            enrage = false;
            frenzy = false;
            DoCast(SPELL_SHADOWSTALKER_STEALTH);
            me->SetReactState(REACT_PASSIVE);
        }

        void StartAttack(Unit* who)
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);
            me->AddThreat(who, 100500);
        }

        void DamageTaken(Unit* attacker, uint32 &damage)
        {
            if (me->HasReactState(REACT_PASSIVE))
                StartAttack(attacker);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ATTACK_LURKER)
                StartAttack(caster);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!frenzy && me->HealthBelowPct(45))
            {
                frenzy = true;
                DoCast(SPELL_UNDYING_FRENZY);
            }

            if (!enrage && me->HealthBelowPct(30))
            {
                enrage = true;
                DoCast(SPELL_ENRAGE);
            }

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Save Krennan Aranas 14293
///////////

class npc_saved_aranas : public CreatureScript
{
public:
    npc_saved_aranas() : CreatureScript("npc_saved_aranas") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saved_aranasAI(creature);
    }

    struct npc_saved_aranasAI : public ScriptedAI
    {
        npc_saved_aranasAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Vehicle* vehicle = summoner->GetVehicle())
                        if (vehicle->HasEmptySeat(1))
                            me->EnterVehicle(vehicle->GetBase(), 1);
                        else
                            me->DespawnOrUnsummon();
        }

        void UpdateAI(const uint32 diff)
        { }
    };
};

class npc_vehicle_genn_horse : public CreatureScript
{
public:
    npc_vehicle_genn_horse() : CreatureScript("npc_vehicle_genn_horse") { }

    struct npc_vehicle_genn_horseAI : public npc_vehicle_escortAI
    {
        npc_vehicle_genn_horseAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            AranasIsSave = false;
        }

        bool AranasIsSave;

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!apply)
                return;

            if (seatId != 1)
                return;

            if (Vehicle* vehicle = me->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(0))
                    if (Player* player = passenger->ToPlayer())
                        who->DoPersonalScriptText(ARANAS_SAY_GENN_HORSE, player);

            SetEscortPaused(false);
            AranasIsSave = true;
            me->SetSpeed(MOVE_WALK, 1.0f, true);
            me->SetSpeed(MOVE_RUN, 1.3f, true);
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 17:
                if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
                {
                    if (Creature* aranas = passenger->FindNearestCreature(NPC_KRENNAN_ARANAS, 50.0f))
                        if (Vehicle* vehicle = me->GetVehicleKit())
                            if (Unit* passenger = vehicle->GetPassenger(0))
                                if (Player* player = passenger->ToPlayer())
                                    aranas->DoPersonalScriptText(ARANAS_SAY_SAVE_ME, player);

                    me->Whisper(HORSE_SAY_SAVE_ARANAS, passenger->GetGUID(), true);
                }
                break;
            case 18:
                if (!AranasIsSave)
                {
                    SetEscortPaused(true);
                    me->SetSpeed(MOVE_WALK, 0, true);
                    me->SetSpeed(MOVE_RUN, 0, true);
                }
                break;
            case 40:
                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Player* player = passenger->ToPlayer())
                        {
                            std::list<Creature*> lGuards;
                            me->GetCreatureListWithEntryInGrid(lGuards, NPC_GUARD_QSKA, 90.0f);

                            if (!lGuards.empty())
                                for (std::list<Creature*>::const_iterator itr = lGuards.begin(); itr != lGuards.end(); ++itr)
                                    if ((*itr)->isAlive())
                                        if (Creature* worgen = (*itr)->FindNearestCreature(NPC_WORGEN_QSKA, 90.0f))
                                            (*itr)->CastSpell(worgen, SPELL_SHOOT_QSKA, false);
                        }
                        break;
            case 41:
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->CombatStop();
                break;
            case 42:
                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Player* player = passenger->ToPlayer())
                        {
                            player->KilledMonsterCredit(NPC_QSKA_KILL_CREDIT, 0);

                            if (Unit* passenger_2 = vehicle->GetPassenger(1))
                                if (Creature* aranas = passenger_2->ToCreature())
                                    aranas->DoPersonalScriptText(ARANAS_THANK, player);
                        }
                        break;
            case 43:
                if (Vehicle* vehicle = me->GetVehicleKit())
                {
                    if (Unit* passenger = vehicle->GetPassenger(1))
                        if (Creature* aranas = passenger->ToCreature())
                            aranas->DespawnOrUnsummon();

                    vehicle->RemoveAllPassengers();
                }

                me->DespawnOrUnsummon();
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vehicle_genn_horseAI(creature);
    }
};

class npc_king_genn_greymane : public CreatureScript
{
public:
    npc_king_genn_greymane() : CreatureScript("npc_king_genn_greymane") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_SAVE_KRENNAN_ARANAS)
        {
            float x, y;
            creature->GetNearPoint2D(x, y, 2.0f, player->GetOrientation() + M_PI / 2);

            if (Creature* horse = player->SummonCreature(NPC_GENN_HORSE, x, y, creature->GetPositionZ(), creature->GetOrientation()))
                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, horse->AI()))
                {
                    escort->AddWaypoint(1, -1799.37f, 1400.21f, 19.8951f);
                    escort->AddWaypoint(2, -1798.23f, 1396.9f, 19.8993f);
                    escort->AddWaypoint(3, -1795.03f, 1388.01f, 19.8898f);
                    escort->AddWaypoint(4, -1790.16f, 1378.7f, 19.8016f);
                    escort->AddWaypoint(5, -1786.41f, 1372.97f, 19.8406f);
                    escort->AddWaypoint(6, -1779.72f, 1364.88f, 19.8131f);
                    escort->AddWaypoint(7, -1774.43f, 1359.87f, 19.7021f);
                    escort->AddWaypoint(8, -1769.0f, 1356.76f, 19.7439f);
                    escort->AddWaypoint(9, -1762.64f, 1356.02f, 19.7979f);
                    escort->AddWaypoint(10, -1758.91f, 1356.08f, 19.8635f);
                    escort->AddWaypoint(11, -1751.95f, 1356.8f, 19.8273f);
                    escort->AddWaypoint(12, -1745.66f, 1357.21f, 19.7993f);
                    escort->AddWaypoint(13, -1738.7f, 1356.64f, 19.7822f);
                    escort->AddWaypoint(14, -1731.79f, 1355.51f, 19.7149f);
                    escort->AddWaypoint(15, -1724.89f, 1354.29f, 19.8661f);
                    escort->AddWaypoint(16, -1718.03f, 1352.93f, 19.7824f);
                    escort->AddWaypoint(17, -1707.68f, 1351.16f, 19.7811f);
                    escort->AddWaypoint(18, -1673.04f, 1344.91f, 15.1353f, 0, true);
                    escort->AddWaypoint(19, -1673.04f, 1344.91f, 15.1353f);
                    escort->AddWaypoint(20, -1669.32f, 1346.55f, 15.1353f);
                    escort->AddWaypoint(21, -1666.45f, 1349.89f, 15.1353f);
                    escort->AddWaypoint(22, -1665.61f, 1353.85f, 15.1353f);
                    escort->AddWaypoint(23, -1666.04f, 1358.01f, 15.1353f);
                    escort->AddWaypoint(24, -1669.79f, 1360.71f, 15.1353f);
                    escort->AddWaypoint(25, -1673.1f, 1362.11f, 15.1353f);
                    escort->AddWaypoint(26, -1677.12f, 1361.57f, 15.1353f);
                    escort->AddWaypoint(27, -1679.9f, 1360.1f, 15.1353f);
                    escort->AddWaypoint(28, -1682.79f, 1357.56f, 15.1353f);
                    escort->AddWaypoint(29, -1682.79f, 1357.56f, 15.1353f);
                    escort->AddWaypoint(30, -1689.07f, 1352.39f, 15.1353f);
                    escort->AddWaypoint(31, -1691.91f, 1351.83f, 15.1353f);
                    escort->AddWaypoint(32, -1703.81f, 1351.82f, 19.7604f);
                    escort->AddWaypoint(33, -1707.26f, 1352.38f, 19.7826f);
                    escort->AddWaypoint(34, -1712.25f, 1353.55f, 19.7826f);
                    escort->AddWaypoint(35, -1718.2f, 1356.51f, 19.7164f);
                    escort->AddWaypoint(36, -1741.5f, 1372.04f, 19.9569f);
                    escort->AddWaypoint(37, -1746.23f, 1375.8f, 19.9817f);
                    escort->AddWaypoint(38, -1751.06f, 1380.53f, 19.8424f);
                    escort->AddWaypoint(39, -1754.97f, 1386.34f, 19.8474f);
                    escort->AddWaypoint(40, -1760.77f, 1394.37f, 19.9282f);
                    escort->AddWaypoint(41, -1765.11f, 1402.07f, 19.8816f);
                    escort->AddWaypoint(42, -1768.24f, 1410.2f, 19.7833f);
                    escort->AddWaypoint(43, -1772.26f, 1420.48f, 19.9029f);
                    escort->Start(true);
                    player->CastSpell(player, SPELL_QUEST_INVISIBILITY_DETECTION_3, false);
                    player->EnterVehicle(horse, 0);
                    horse->EnableAI();
                }
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_genn_greymaneAI(creature);
    }

    struct npc_king_genn_greymaneAI : public ScriptedAI
    {
        npc_king_genn_greymaneAI(Creature* creature) : ScriptedAI(creature)
        {
            uiRandomYellTimer = urand(15000, 35000);
        }

        uint32 uiRandomYellTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiRandomYellTimer <= diff)
            {
                uiRandomYellTimer = urand(15000, 35000);
                uint8 roll = urand(0, 2);
                DoScriptText(GREYMANE_RANDOM_YELL - roll, me);
            } else
                uiRandomYellTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_cannon_camera : public CreatureScript
{
public:
    npc_cannon_camera() : CreatureScript("npc_cannon_camera") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cannon_cameraAI(creature);
    }

    struct npc_cannon_cameraAI : public ScriptedAI
    {
        npc_cannon_cameraAI(Creature* creature) : ScriptedAI(creature)
        {
            uiEventTimer = 1000;
            uiPhase = 0;
            Event = false;
            lSummons.clear();
        }

        std::list<Creature*> lSummons;
        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;

        void PassengerBoarded(Unit* /*who*/, int8 /*seatId*/, bool apply)
        {
            if (apply)
                Event = true;
            else
            {
                if (Creature* cannon = me->FindNearestCreature(NPC_COMMANDEERED_CANNON, 40.0f))
                    cannon->CastSpell(cannon, SPELL_CANNON_FIRE, false);

                for (std::list<Creature*>::iterator itr = lSummons.begin(); itr != lSummons.end(); ++itr)
                    if ((*itr)->isAlive())
                        (*itr)->Kill(*itr);

                lSummons.clear();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    ++uiPhase;

                    switch (uiPhase)
                    {
                    case 1:
                        uiEventTimer = 1500;

                        for (int i = 0; i < 13; ++i)
                            if (Creature* worgen = me->SummonCreature(NPC_BLOODFANG_RIPPER_QSKA, WorgenSummonPos[i][0], WorgenSummonPos[i][1], WorgenSummonPos[i][2], WorgenSummonPos[i][3]))
                            {
                                lSummons.push_back(worgen);
                                worgen->GetMotionMaster()->MovePoint(0, -1751.874f + irand(-3, 3), 1377.457f + irand(-3, 3), 19.930f);
                            }
                            break;
                    case 2:
                        uiEventTimer = 2000;
                        if (Creature* cannon = me->FindNearestCreature(NPC_COMMANDEERED_CANNON, 40.0f))
                            cannon->CastSpell(cannon, SPELL_CANNON_FIRE, false);
                        break;
                    case 3:
                        Event = false;
                        if (Creature* cannon = me->FindNearestCreature(NPC_COMMANDEERED_CANNON, 40.0f))
                            cannon->CastSpell(cannon, SPELL_CANNON_FIRE, false);
                        break;
                    }
                } else
                    uiEventTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_lord_godfrey_qska : public CreatureScript
{
public:
    npc_lord_godfrey_qska() : CreatureScript("npc_lord_godfrey_qska") { }

    bool OnQuestComplete(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_SAVE_KRENNAN_ARANAS)
        {
            creature->DoPersonalScriptText(GOLDFREY_SAY_ARANAS_WITH_US, player);
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_3, false);
            player->CastSpell(player, SPELL_CANNON_CAMERA, false);
            player->CastSpell(player, SPELL_QUEST_INVISIBILITY_DETECTION_2, false);
            player->SaveToDB();
        }

        return true;
    }
};

///////////
// Quest Time to Regroup 14294
///////////

class npc_king_genn_greymane_qtr : public CreatureScript
{
public:
    npc_king_genn_greymane_qtr() : CreatureScript("npc_king_genn_greymane_qtr") { }

    bool OnQuestComplete(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TIME_TO_REGROUP)
        {
            creature->DoPersonalScriptText(GENN_SAY_IF, player);
            Psc_qtr new_psc;
            new_psc.uiSpeachId = 0;
            new_psc.uiPersonalTimer = 7000;
            new_psc.uiPlayerGUID = player->GetGUID();
            CAST_AI(npc_king_genn_greymane_qtrAI, creature->AI())->lPlayerList.push_back(new_psc);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_genn_greymane_qtrAI(creature);
    }

    struct npc_king_genn_greymane_qtrAI : public ScriptedAI
    {
        npc_king_genn_greymane_qtrAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayerList.clear();
        }

        std::list<Psc_qtr> lPlayerList;

        void DeletePlayer(uint64 uiPlayerGUID)
        {
            for (std::list<Psc_qtr>::iterator itr = lPlayerList.begin(); itr != lPlayerList.end();)
                if (itr->uiPlayerGUID == uiPlayerGUID)
                    itr = lPlayerList.erase(itr);
                else
                    ++itr;
        }

        void UpdateAI(const uint32 diff)
        {
            if (lPlayerList.empty())
                return;

            for (std::list<Psc_qtr>::iterator itr = lPlayerList.begin(); itr != lPlayerList.end();)
                if ((*itr).uiPersonalTimer <= diff)
                {
                    ++(*itr).uiSpeachId;

                    if (Player* player = Unit::GetPlayer(*me, (*itr).uiPlayerGUID))
                    {
                        switch ((*itr).uiSpeachId)
                        {
                        case 1:
                            (*itr).uiPersonalTimer = 7000;
                            if (Creature* crowley = me->FindNearestCreature(NPC_LORD_CROWLEY_QTR, 30.0f))
                                crowley->DoPersonalScriptText(CROWLEY_SAY_NEED, player);
                            break;
                        case 2:
                            (*itr).uiPersonalTimer = 4000;
                            if (Creature* liam = me->FindNearestCreature(NPC_PRINCE_LIAM_QTR, 30.0f))
                                liam->DoPersonalScriptText(LIAM_SAY_STAY, player);
                            break;
                        case 3:
                            (*itr).uiPersonalTimer = 9000;
                            if (Creature* crowley = me->FindNearestCreature(NPC_LORD_CROWLEY_QTR, 30.0f))
                                crowley->DoPersonalScriptText(CROWLEY_SAY_NO_CHANCE, player);
                            break;
                        case 4:
                            (*itr).uiPersonalTimer = 8000;
                            if (Creature* crowley = me->FindNearestCreature(NPC_LORD_CROWLEY_QTR, 30.0f))
                                crowley->DoPersonalScriptText(CROWLEY_SAY_MY_MEN, player);
                            break;
                        case 5:
                            me->DoPersonalScriptText(GENN_SAY_WE_FOOLS, player);
                            itr = lPlayerList.erase(itr);
                            break;
                        }
                    }
                } else
                {
                    (*itr).uiPersonalTimer -= diff;
                    ++itr;
                }
        }
    };
};

///////////
// Quest Sacrifices 14212
///////////

class npc_crowley_horse : public CreatureScript
{
public:
    npc_crowley_horse() : CreatureScript("npc_crowley_horse") { }

    struct npc_crowley_horseAI : public npc_vehicle_escortAI
    {
        npc_crowley_horseAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            uiRandomSpeachTimer = urand(15000, 25000);
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 uiRandomSpeachTimer;

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 1:
                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* crowley = vehicle->GetPassenger(1))
                        if (Unit* passenger = vehicle->GetPassenger(0))
                            if (Player* player = passenger->ToPlayer())
                                crowley->DoPersonalScriptText(CROWLEY_SAY_START, player);
                break;
            case 26:
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case 29:
                if (Vehicle* vehicle = me->GetVehicleKit())
                {
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        passenger->ExitVehicle();

                    if (Unit* passenger = vehicle->GetPassenger(1))
                        passenger->ToCreature()->DespawnOrUnsummon();
                }
                me->DespawnOrUnsummon();
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);

            if (uiRandomSpeachTimer <= diff)
            {
                uiRandomSpeachTimer = urand(10000, 20000);
                uint8 roll = urand(0, 2);

                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* crowley = vehicle->GetPassenger(1))
                        if (Unit* passenger = vehicle->GetPassenger(0))
                            if (Player* player = passenger->ToPlayer())
                                crowley->DoPersonalScriptText(CROWLEY_RANDOM_SAY - roll, player);
            } else
                uiRandomSpeachTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crowley_horseAI(creature);
    }
};

class npc_bloodfang_stalker : public CreatureScript
{
public:
    npc_bloodfang_stalker() : CreatureScript("npc_bloodfang_stalker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodfang_stalkerAI(creature);
    }

    struct npc_bloodfang_stalkerAI : public ScriptedAI
    {
        npc_bloodfang_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

        Player* player;
        uint32 tEnrage;
        uint32 tAnimate;
        uint32 BurningReset;
        bool Miss, willCastEnrage, Burning;

        void Reset()
        {
            tEnrage    = 0;
            tAnimate   = DELAY_ANIMATE;
            Miss  = false;
            willCastEnrage = urand(0, 1);
            BurningReset = 3000;
            Burning = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->HasAura(SPELL_THROW_TORCH))
            {
                Burning = true;
            } else
                Burning = false;

            if (Burning && BurningReset <=diff)
            {
                me->RemoveAllAuras();
                BurningReset = 5000;
                Burning = false;
            } else
                BurningReset -= diff;

            if (!UpdateVictim())
            {
                return;
            }

            if (tEnrage <= diff && willCastEnrage)
            {
                if (me->GetHealthPct() <= 30)
                {
                    //me->MonsterTextEmote(-106, 0);
                    DoCast(me, SPELL_ENRAGE);
                    tEnrage = CD_ENRAGE;
                }
            } else tEnrage -= diff;

            if (me->getVictim()->GetTypeId() == TYPEID_PLAYER)
            {
                Miss = false;
            } else if (me->getVictim()->isPet())
            {
                Miss = false;
            } else if (me->getVictim()->GetEntry() == NPC_NORTHGATE_REBEL_PHASE_5)
            {
                if (me->getVictim()->GetHealthPct() < 90)
                {
                    Miss = true;
                }
            }

            if (Miss && tAnimate <= diff)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
                me->PlayDistanceSound(SOUND_WORGEN_ATTACK);
                tAnimate = DELAY_ANIMATE;
            } else
                tAnimate -= diff;

            if (!Miss)
            {
                DoMeleeAttackIfReady();
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            Creature *horse = me->FindNearestCreature(NPC_CROWLEYS_HORSE, 100, true);
            if (horse && spell->Id == SPELL_THROW_TORCH)
            {
                Burning = true;

                if (me->getVictim()->GetTypeId() == TYPEID_PLAYER)
                {
                    me->getThreatManager().resetAllAggro();
                    horse->AddThreat(me, 1.0f);
                    me->AddThreat(horse, 1.0f);
                    me->AI()->AttackStart(horse);
                }

                if (caster->GetTypeId() == TYPEID_PLAYER && caster->ToPlayer()->GetQuestStatus(QUEST_SACRIFICES) == QUEST_STATUS_INCOMPLETE)
                {
                    caster->ToPlayer()->KilledMonsterCredit(NPC_BLOODFANG_STALKER_CREDIT, 0);
                }
            }
        }
    };
};

class npc_crowley_horse_second_round : public CreatureScript
{
public:
    npc_crowley_horse_second_round() : CreatureScript("npc_crowley_horse_second_round") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crowley_horse_second_roundAI(creature);
    }

    struct npc_crowley_horse_second_roundAI : public npc_vehicle_escortAI
    {
        npc_crowley_horse_second_roundAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void FinishEscort()
        {
            me->DespawnOrUnsummon();
        }

        void WaypointReached(uint32 point)
        {
            if (point == 84)
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class spell_summon_crowley_horse : public SpellScriptLoader
{
public:
    spell_summon_crowley_horse() : SpellScriptLoader("spell_summon_crowley_horse") { }

    class spell_summon_crowley_horse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_crowley_horse_SpellScript)

            void OnSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            Creature* horse = NULL;
            horse = player->SummonCreature(NPC_CROWLEYS_HORSE, -1737.68f, 1655.11f, 20.56283f, 1.64061f);

            if (!horse)
                return;

            if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, horse->AI()))
            {
                if (Creature* greymane = player->FindNearestCreature(NPC_KING_GENN_GREYMANE_TTR, 30.0f))
                    CAST_AI(npc_king_genn_greymane_qtr::npc_king_genn_greymane_qtrAI, greymane->AI())->DeletePlayer(player->GetGUID());

                player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_2);
                player->EnterVehicle(horse, 0);
                horse->EnableAI();
                escort->AddWaypoint(0, -1737.09f, 1663.36f, 20.4791f);
                escort->AddWaypoint(1, -1722.70f, 1677.69f, 20.3930f, 0, true);
                escort->AddWaypoint(2, -1700.89f, 1653.31f, 20.4907f);
                escort->AddWaypoint(3, -1672.69f, 1664.77f, 20.4884f);
                escort->AddWaypoint(4, -1667.30f, 1651.55f, 20.4884f);
                escort->AddWaypoint(5, -1666.10f, 1626.02f, 20.4884f);
                escort->AddWaypoint(6, -1680.09f, 1601.27f, 20.4869f);
                escort->AddWaypoint(7, -1688.33f, 1591.46f, 20.4869f);
                escort->AddWaypoint(8, -1733.32f, 1592.11f, 20.4869f);
                escort->AddWaypoint(9, -1713.33f, 1633.47f, 20.4869f);
                escort->AddWaypoint(10, -1709.09f, 1690.63f, 20.1549f);
                escort->AddWaypoint(11, -1678.91f, 1704.73f, 20.4792f);
                escort->AddWaypoint(12, -1631.35f, 1710.25f, 20.4881f);
                escort->AddWaypoint(13, -1613.85f, 1710.41f, 22.6797f);
                escort->AddWaypoint(14, -1590.98f, 1710.63f, 20.4849f);
                escort->AddWaypoint(15, -1569.10f, 1709.55f, 20.4848f, 0, true);
                escort->AddWaypoint(16, -1553.84f, 1701.86f, 20.4862f);
                escort->AddWaypoint(17, -1542.33f, 1639.14f, 21.1955f);
                escort->AddWaypoint(18, -1518.09f, 1615.94f, 20.4865f);
                escort->AddWaypoint(19, -1475.35f, 1626.09f, 20.4865f);
                escort->AddWaypoint(20, -1439.23f, 1625.94f, 20.4865f);
                escort->AddWaypoint(21, -1424.39f, 1608.56f, 20.4867f);
                escort->AddWaypoint(22, -1415.99f, 1581.91f, 20.4835f);
                escort->AddWaypoint(23, -1430.42f, 1549.91f, 20.7399f);
                escort->AddWaypoint(24, -1440.54f, 1538.35f, 20.4858f);
                escort->AddWaypoint(25, -1473.94f, 1536.39f, 20.4857f);
                escort->AddWaypoint(26, -1505.65f, 1567.46f, 20.4857f);
                escort->AddWaypoint(27, -1519.51f, 1571.06f, 26.5095f);
                escort->AddWaypoint(28, -1531.39f, 1581.60f, 26.5376f);
                escort->AddWaypoint(29, -1540.11f, 1575.89f, 29.209f);
                escort->SetSpeedZ(18.0f);
                escort->Start(true);
                horse->SetInCombatWith(player);
                horse->AddThreat(player, 100500.0f);
                float x, y, z;
                horse->GetPosition(x, y, z);

                if (Creature* crowley = player->SummonCreature(NPC_LORD_DARIYS_CROWLEY, x, y, z))
                    crowley->EnterVehicle(horse, 1);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_summon_crowley_horse_SpellScript::OnSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_summon_crowley_horse_SpellScript();
    }
};

class spell_summon_crowley_horse_second_round : public SpellScriptLoader
{
public:
    spell_summon_crowley_horse_second_round() : SpellScriptLoader("spell_summon_crowley_horse_second_round") { }

    class spell_summon_crowley_horse_second_round_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_crowley_horse_second_round_SpellScript)

            void OnSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            Creature* horse = NULL;
            horse = player->SummonCreature(NPC_CROWLEYS_HORSE_S_R, -1539.03f, 1570.91f, 29.28873f);

            if (!horse)
                return;

            if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, horse->AI()))
            {
                player->EnterVehicle(horse, 0);
                horse->EnableAI();

                for (int i = 0; i < 90; ++i)
                    escort->AddWaypoint(i, SecondRoundWP[i][0], SecondRoundWP[i][1], SecondRoundWP[i][2]);

                escort->Start(true);
                horse->SetInCombatWith(player);
                horse->AddThreat(player, 100500.0f);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_summon_crowley_horse_second_round_SpellScript::OnSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_summon_crowley_horse_second_round_SpellScript();
    }
};

///////////
// Quest Last Stand 14222 && Last Chance at Humanity 14375
///////////

class npc_lord_darius_crowley_qls : public CreatureScript
{
public:
    npc_lord_darius_crowley_qls() : CreatureScript("npc_lord_darius_crowley_qls") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_LAST_STAND)
        {
            //player->setInWorgenForm(UNIT_FLAG2_WORGEN_TRANSFORM3);
            player->SendMovieStart(MOVIE_WORGEN_FORM);
            player->CastSpell(player, SPELL_PLAY_MOVIE, true);
            player->RemoveAura(SPELL_WORGEN_BITE);
            player->CastSpell(player, SPELL_HIDEOUS_BITE_WOUND, true);
            player->CastSpell(player, SPELL_CURSE_OF_THE_WORGEN, true);
            player->CastSpell(player, SPELL_LAST_STAND_COMPLETE, true);
            WorldLocation loc;
            loc.m_mapId       = Gilneas2;
            loc.m_positionX   = -1818.4f;
            loc.m_positionY   = 2294.25f;
            loc.m_positionZ   = 42.2135f;
            player->SetHomebind(loc, 4786);
            player->SaveToDB();
        }
        return true;
    }
};

class npc_king_genn_greymane_qls : public CreatureScript
{
public:
    npc_king_genn_greymane_qls() : CreatureScript("npc_king_genn_greymane_qls") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_LAST_CHANCE_AT_HUMANITY)
        {
            player->RemoveAura(SPELL_CURSE_OF_THE_WORGEN);
            player->RemoveAura(SPELL_HIDEOUS_BITE_WOUND);
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_6, true);
            player->CastSpell(player, SPELL_FADE_TO_BLACK_2, true);
            player->SaveToDB();

            if (creature->isSummon())
                if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
                    if (Creature* krennan = summoner->ToCreature())
                        krennan->AI()->DoAction(ACTION_EVENT_DONE);
        }
        return true;
    }
};

class npc_krennan_aranas_qls : public CreatureScript
{
public:
    npc_krennan_aranas_qls() : CreatureScript("npc_krennan_aranas_qls") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_krennan_aranas_qlsAI(creature);
    }

    struct npc_krennan_aranas_qlsAI : public ScriptedAI
    {
        npc_krennan_aranas_qlsAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
        {
            Event = false;

            if (!creature->isSummon())
                creature->DespawnOrUnsummon();
            else
            {
                lSummons.DespawnAll();
                Event = true;
                uiPhase = 1;
                uiGodfreyGUID = 0;
                uiGreymaneGUID = 0;
                uiEventTimer = 5000;
                creature->setActive(true);
            }
        }

        SummonList lSummons;
        uint64 uiGodfreyGUID;
        uint64 uiGreymaneGUID;
        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;

        void JustSummoned(Creature* summoned)
        {
            lSummons.Summon(summoned);
            summoned->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_EVENT_DONE)
            {
                lSummons.DespawnAll();
                me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
            {
                if (!me->ToTempSummon()->GetSummoner())
                {
                    lSummons.DespawnAll();
                    me->DespawnOrUnsummon();
                }

                if (uiEventTimer <= diff)
                {
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                            switch (uiPhase)
                        {
                            case 1:
                                uiEventTimer = 2000;
                                if (!(player->GetExtraFlags() & PLAYER_EXTRA_WATCHING_MOVIE))
                                    ++uiPhase;
                                break;
                            case 2:
                                ++uiPhase;
                                uiEventTimer = 500;
                                me->DoPersonalScriptText(ARANAS_SAY_YOU_CAN_CONTROL, player);
                                if (Creature* godfrey = me->SummonCreature(NPC_LORD_GODFREY_QLS, -1844.92f, 2291.69f, 42.2967f))
                                {
                                    godfrey->SetSeerGUID(player->GetGUID());
                                    godfrey->SetVisible(false);
                                    uiGodfreyGUID = godfrey->GetGUID();
                                }
                                break;
                            case 3:
                                ++uiPhase;
                                uiEventTimer = 4500;
                                if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                    godfrey->GetMotionMaster()->MovePoint(0, -1822.22f, 2296.55f, 42.1670f);
                                break;
                            case 4:
                                ++uiPhase;
                                uiEventTimer = 5000;
                                if (Creature* greymane = me->SummonCreature(NPC_KING_GENN_GREYMANE_QLS, -1844.51f, 2289.50f, 42.3237f))
                                {
                                    greymane->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                                    greymane->SetSeerGUID(player->GetGUID());
                                    greymane->SetVisible(false);
                                    greymane->GetMotionMaster()->MovePoint(0, -1821.82f, 2293.69f, 42.0869f);
                                    uiGreymaneGUID = greymane->GetGUID();
                                }
                                break;
                            case 5:
                                ++uiPhase;
                                uiEventTimer = 7000;
                                if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                {
                                    godfrey->SetFacingToObject(player);
                                    godfrey->DoPersonalScriptText(GODFREY_SAY_PUT_DOWN, player);
                                }
                                break;
                            case 6:
                                ++uiPhase;
                                uiEventTimer = 9000;
                                if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                {
                                    if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                        greymane->SetFacingToObject(godfrey);

                                    greymane->DoPersonalScriptText(GREYMANE_SAY_TELL_ME_GODFREY, player);
                                    greymane->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                                }
                                break;
                            case 7:
                                ++uiPhase;
                                uiEventTimer = 8000;
                                if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                {
                                    greymane->SetFacingToObject(me);
                                    greymane->DoPersonalScriptText(GREYMANE_SAY_DIDNT_THINK_SO, player);
                                    greymane->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                                }
                                break;
                            case 8:
                                Event = false;
                                if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                {
                                    greymane->SetFacingToObject(player);
                                    greymane->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                                }
                                break;
                        }
                } else
                    uiEventTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class spell_curse_of_the_worgen_summon : public SpellScriptLoader
{
public:
    spell_curse_of_the_worgen_summon() : SpellScriptLoader("spell_curse_of_the_worgen_summon") { }

    class spell_curse_of_the_worgen_summon_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_curse_of_the_worgen_summon_AuraScript);

        void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* player = GetTarget()->ToPlayer();

            if (!player)
                return;

            if (Creature* krennan = player->SummonCreature(NPC_KRENNAN_ARANAS_QLS, -1819.01f, 2291.30f, 42.1981f, 1.99f))
            {
                krennan->SetSeerGUID(player->GetGUID());
                krennan->SetVisible(false);
                krennan->SetPhaseMask(1024, true);
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_curse_of_the_worgen_summon_AuraScript::ApplyEffect, EFFECT_1, SPELL_AURA_LINKED, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_curse_of_the_worgen_summon_AuraScript();
    }
};

class spell_curse_of_the_worgen_invis : public SpellScriptLoader
{
public:
    spell_curse_of_the_worgen_invis() : SpellScriptLoader("spell_curse_of_the_worgen_invis") { }

    class spell_curse_of_the_worgen_invis_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_curse_of_the_worgen_invis_AuraScript);

        void ApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            PreventDefaultAction();

            if (Player* player = GetTarget()->ToPlayer())
                player->SetVisible(false);
        }

        void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            PreventDefaultAction();

            if (Player* player = GetTarget()->ToPlayer())
                player->SetVisible(true);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_curse_of_the_worgen_invis_AuraScript::ApplyEffect, EFFECT_0, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_curse_of_the_worgen_invis_AuraScript::RemoveEffect, EFFECT_0, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_curse_of_the_worgen_invis_AuraScript();
    }
};

///////////
// Quest In Need of Ingredients 14320
///////////

class go_crate_of_mandrake_essence : public GameObjectScript
{
public:
    go_crate_of_mandrake_essence() : GameObjectScript("go_crate_of_mandrake_essence") { }

    bool OnQuestComplete(Player* player, GameObject* go, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_IN_NEED_OF_INGR)
            player->SendCinematicStart(CINEMATIC_FORSAKEN);

        return true;
    }
};

///////////
// Quest Invasion 14321
///////////

class npc_slain_watchman : public CreatureScript
{
public:
    npc_slain_watchman() : CreatureScript("npc_slain_watchman") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_INVASION)
        {
            float x, y;
            player->GetNearPoint2D(x, y, 2.0f, player->GetOrientation() + M_PI);

            if (Creature* forsaken = player->SummonCreature(NPC_FORSAKEN_ASSASSIN, x, y, player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6000))
            {
                forsaken->AI()->AttackStart(player);
                forsaken->CastSpell(player, SPELL_BACKSTAB, false);
                forsaken->DoPersonalScriptText(NPC_FORSAKEN_ASSASSIN_SAY, player);
            }
        }

        return true;
    }
};

class npc_gwen_armstead_qi : public CreatureScript
{
public:
    npc_gwen_armstead_qi() : CreatureScript("npc_gwen_armstead_qi") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_INVASION)
        {
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_7, false);
            player->SaveToDB();
        }

        return true;
    }
};

///////////
// Quest You Can't Take 'Em Alone 14348
///////////

class npc_horrid_abomination : public CreatureScript
{
public:
    npc_horrid_abomination() : CreatureScript("npc_horrid_abomination") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horrid_abominationAI(creature);
    }

    struct npc_horrid_abominationAI : public ScriptedAI
    {
        npc_horrid_abominationAI(Creature* creature) : ScriptedAI(creature)
        {
            uiRestitchingTimer = 3000;
            uiShootTimer = 3000;
            uiPlayerGUID = 0;
            shoot = false;
            miss = false;
            me->_ReactDistance = 10.0f;
        }

        uint64 uiPlayerGUID;
        uint32 uiShootTimer;
        uint32 uiRestitchingTimer;
        bool shoot;
        bool miss;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_KEG_PLACED)
            {
                uint8 roll = urand(0, 5);
                DoScriptText(NPC_HORRID_ABOMINATION_RANDOM_SAY - roll, me);
                shoot = true;
                uiPlayerGUID = caster->GetGUID();
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveRandom(5.0f);
                me->CombatStop();

            }

            if (spell->Id == SPELL_SHOOT_QYCTEA)
                ShootEvent();
        }

        void ShootEvent()
        {
            me->RemoveAura(SPELL_KEG_PLACED);

            for (int i = 0; i < 11; ++i)
                DoCast(SPELL_EXPLOSION_POISON);

            for (int i = 0; i < 6; ++i)
                DoCast(SPELL_EXPLOSION_BONE_TYPE_ONE);

            for (int i = 0; i < 4; ++i)
                DoCast(SPELL_EXPLOSION_BONE_TYPE_TWO);

            DoCast(SPELL_EXPLOSION);

            if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                player->KilledMonsterCredit(NPC_HORRID_ABOMINATION_KILL_CREDIT, 0);

            me->DespawnOrUnsummon(1000);
        }

        void DamageTaken(Unit* attacker, uint32 &/*damage*/)
        {
            if (attacker->GetTypeId() != TYPEID_PLAYER)
                return;

            Unit* victim = NULL;

            if (victim = me->getVictim())
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    return;

            if (victim)
                me->getThreatManager().modifyThreatPercent(victim, -100);

            AttackStart(attacker);
            me->AddThreat(attacker, 100500);
        }

        void UpdateAI(const uint32 diff)
        {
            if (shoot)
                if (uiShootTimer <= diff)
                {
                    shoot = false;
                    uiShootTimer = 3000;
                    std::list<Creature*> liamList;
                    GetCreatureListWithEntryInGrid(liamList, me, NPC_PRINCE_LIAM_GREYMANE_QYCTEA, 50.0f);

                    if (liamList.empty())
                        ShootEvent();
                    else
                        (*liamList.begin())->CastSpell(me, SPELL_SHOOT_QYCTEA, false);
                } else
                    uiShootTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (me->getVictim() && me->getVictim()->GetTypeId() == TYPEID_UNIT)
                {
                    if (me->getVictim()->GetHealthPct() < 90)
                        miss = true;
                    else
                        miss = false;
                }

                if (uiRestitchingTimer <= diff)
                {
                    uiRestitchingTimer = 8000;
                    DoCast(SPELL_RESTITCHING);
                } else
                    uiRestitchingTimer -= diff;

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Two By Sea 14382
///////////

class npc_forsaken_catapult_qtbs : public CreatureScript
{
public:
    npc_forsaken_catapult_qtbs() : CreatureScript("npc_forsaken_catapult_qtbs") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_catapult_qtbsAI(creature);
    }

    struct npc_forsaken_catapult_qtbsAI : public ScriptedAI
    {
        npc_forsaken_catapult_qtbsAI(Creature* creature) : ScriptedAI(creature)
        {
            me->setActive(true);
            speedXY = 10.0f;
            speedZ = 10.0f;
            creature->GetPosition(x, y, z);
            uiJumpTimer = 2000;
            uiCastTimer = urand(1000, 5000);
            uiRespawnTimer = 10000;
            CanCast = true;
            jump = false;
            respawn = false;
        }

        float speedXY, speedZ, x, y, z;
        uint32 uiCastTimer;
        uint32 uiJumpTimer;
        uint32 uiRespawnTimer;
        bool CanCast;
        bool jump;
        bool respawn;

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!apply)
            {
                respawn = true;
                CanCast = false;

                if (seatId == 2)
                    me->setFaction(35);
            } else
            {
                respawn = false;

                if (seatId == 2)
                {
                    who->ClearUnitState(UNIT_STATE_ONVEHICLE);
                    CanCast = true;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (respawn)
                if (uiRespawnTimer <= diff)
                {
                    respawn = false;
                    uiRespawnTimer = 10000;
                    me->DespawnOrUnsummon();
                } else
                    uiRespawnTimer -= diff;

                if (CanCast)
                    if (uiCastTimer <= diff)
                    {
                        uiCastTimer = urand(5000, 20000);
                        float x, y, z;
                        me->GetNearPoint2D(x, y, urand(100, 150), me->GetOrientation());
                        z = me->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);
                        me->CastSpell(x, y, z, SPELL_FIERY_BOULDER, false);
                    } else
                        uiCastTimer -= diff;

                    if (jump)
                        if (uiJumpTimer <= diff)
                        {
                            jump = false;
                            uiJumpTimer = 2000;

                            if (Vehicle* vehicle = me->GetVehicleKit())
                                if (Unit* passenger = vehicle->GetPassenger(1))
                                    if (Player* player = passenger->ToPlayer())
                                    {
                                        me->AddAura(SPELL_LAUNCH, player);
                                        player->ExitVehicle();
                                        player->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
                                    }
                        } else
                            uiJumpTimer -= diff;

                        if (!UpdateVictim())
                            return;

                        DoMeleeAttackIfReady();
        }
    };
};

class spell_launch_qtbs : public SpellScriptLoader
{
public:
    spell_launch_qtbs() : SpellScriptLoader("spell_launch_qtbs") { }

    class spell_launch_qtbs_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_launch_qtbs_SpellScript)

            void Function(SpellEffIndex effIndex)
        {
            Unit* caster = GetCaster();
            bool CheckCast = false;
            float x, y, z;
            x = GetExplTargetDest()->GetPositionX();
            y = GetExplTargetDest()->GetPositionY();
            z = GetExplTargetDest()->GetPositionZ();

            for (int i = 0; i < 16; ++i)
                if (sqrt(pow(x - CheckSpellPos[i][0], 2) + pow(y - CheckSpellPos[i][1], 2) + pow(z - CheckSpellPos[i][2], 2)) < 10.0f)
                {
                    CheckCast = true;
                    break;
                }

                if (!CheckCast)
                    return;

                if (Vehicle* vehicle = caster->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (passenger->GetTypeId() == TYPEID_PLAYER)
                        {
                            passenger->ChangeSeat(1);
                            caster->EnableAI();
                            float speedXY = 10.0f, speedZ = 10.0f;
                            GetTargetSpeedXYZ(speedXY, speedZ);
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->speedXY = speedXY;
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->speedZ = speedZ;
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->x = x;
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->y = y;
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->z = z;
                            CAST_AI(npc_forsaken_catapult_qtbs::npc_forsaken_catapult_qtbsAI, caster->ToCreature()->AI())->jump = true;
                        }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_launch_qtbs_SpellScript::Function, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_launch_qtbs_SpellScript();
    }
};

///////////
// Quest Save the Children! 14368
///////////

class npc_james : public CreatureScript
{
public:
    npc_james() : CreatureScript("npc_james") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jamesAI(creature);
    }

    struct npc_jamesAI : public npc_escortAI
    {
        npc_jamesAI(Creature* creature) : npc_escortAI(creature)
        {
            uiEventTimer = 3500;
            uiPlayerGUID = 0;
            Event = false;
        }

        uint64 uiPlayerGUID;
        uint32 uiEventTimer;
        bool Event;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SAVE_JAMES)
                if (Player* player = caster->ToPlayer())
                {
                    Event = true;
                    uiPlayerGUID = player->GetGUID();
                    player->Say(GossipMenu[26], LANG_UNIVERSAL);
                    player->KilledMonsterCredit(36289, 0);
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 7)
                if (GameObject* door = me->FindNearestGameObject(GO_DOOR_TO_THE_BASEMENT, 10.0f))
                    door->UseDoorOrButton();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    uiEventTimer = 3500;

                    if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                    {
                        Start(false, true);
                        DoScriptText(NPC_JAMES_SAY, me, player);
                    }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class npc_ashley : public CreatureScript
{
public:
    npc_ashley() : CreatureScript("npc_ashley") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashleyAI(creature);
    }

    struct npc_ashleyAI : public npc_escortAI
    {
        npc_ashleyAI(Creature* creature) : npc_escortAI(creature)
        {
            uiEventTimer = 3500;
            uiPlayerGUID = 0;
            Event = false;
        }

        uint64 uiPlayerGUID;
        uint32 uiEventTimer;
        bool Event;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SAVE_ASHLEY)
                if (Player* player = caster->ToPlayer())
                {
                    Event = true;
                    uiPlayerGUID = player->GetGUID();
                    player->Say(GossipMenu[25], LANG_UNIVERSAL);
                    player->KilledMonsterCredit(36288, 0);
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 16)
                if (GameObject* door = me->FindNearestGameObject(GO_DOOR_TO_THE_BASEMENT, 10.0f))
                    door->UseDoorOrButton();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    uiEventTimer = 3500;

                    if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                    {
                        Start(false, true);
                        DoScriptText(NPC_ASHLEY_SAY, me, player);
                    }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class npc_cynthia : public CreatureScript
{
public:
    npc_cynthia() : CreatureScript("npc_cynthia") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cynthiaAI(creature);
    }

    struct npc_cynthiaAI : public npc_escortAI
    {
        npc_cynthiaAI(Creature* creature) : npc_escortAI(creature)
        {
            uiEventTimer = 3500;
            uiPlayerGUID = 0;
            Event = false;
        }

        uint64 uiPlayerGUID;
        uint32 uiEventTimer;
        bool Event;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SAVE_CYNTHIA)
                if (Player* player = caster->ToPlayer())
                {
                    Event = true;
                    uiPlayerGUID = player->GetGUID();
                    player->Say(GossipMenu[24], LANG_UNIVERSAL);
                    player->KilledMonsterCredit(36287, 0);
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 8)
                if (GameObject* door = me->FindNearestGameObject(GO_DOOR_TO_THE_BASEMENT, 10.0f))
                    door->UseDoorOrButton();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    uiEventTimer = 3500;

                    if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                    {
                        Start(false, true);
                        DoScriptText(NPC_CYNTHIA_SAY, me, player);
                    }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class npc_lord_godfrey_qatls : public CreatureScript
{
public:
    npc_lord_godfrey_qatls() : CreatureScript("npc_lord_godfrey_qatls") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_EVACUATION)
        {
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_8, false);
            player->SaveToDB();
        }

        return false;
    }
};

///////////
// Quest Gasping for Breath 14395
///////////

class npc_drowning_watchman : public CreatureScript
{
public:
    npc_drowning_watchman() : CreatureScript("npc_drowning_watchman") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drowning_watchmanAI(creature);
    }

    struct npc_drowning_watchmanAI : public ScriptedAI
    {
        npc_drowning_watchmanAI(Creature* creature) : ScriptedAI(creature)
        {
            reset = true;
            despawn = false;
            exit = false;
            uiDespawnTimer = 10000;
        }

        uint32 uiDespawnTimer;
        bool reset;
        bool despawn;
        bool exit;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_RESCUE_DROWNING_WATCHMAN)
            {
                despawn = false;
                uiDespawnTimer = 10000;
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveAura(SPELL_DROWNING);
                me->EnterVehicle(caster);

                if (GameObject* go = me->FindNearestGameObject(GO_SPARKLES, 10.0f))
                    go->Delete();
            }
        }

        void OnExitVehicle(Unit* /*vehicle*/, uint32 /*seatId*/)
        {
            if (!exit)
            {
                float x, y, z, o;
                me->GetPosition(x, y, z, o);
                me->SetHomePosition(x, y, z, o);
                me->Relocate(x, y, z, o);
                reset = true;
                despawn = true;
                Reset();
            }
        }

        void Reset()
        {
            exit = false;

            if (reset)
            {
                DoCast(SPELL_DROWNING);
                DoCast(SPELL_SUMMON_SPARKLES);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                reset = false;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (despawn)
                if (uiDespawnTimer <= diff)
                {
                    if (GameObject* go = me->FindNearestGameObject(GO_SPARKLES, 10.0f))
                        go->Delete();

                    reset = true;
                    despawn = false;
                    uiDespawnTimer = 10000;
                    me->DespawnOrUnsummon();
                } else
                    uiDespawnTimer -= diff;
        }
    };
};

class npc_prince_liam_greymane : public CreatureScript
{
public:
    npc_prince_liam_greymane() : CreatureScript("npc_prince_liam_greymane") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymaneAI(creature);
    }

    struct npc_prince_liam_greymaneAI : public ScriptedAI
    {
        npc_prince_liam_greymaneAI(Creature* creature) : ScriptedAI(creature) { }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetEntry() == NPC_DROWNING_WATCHMAN)
            {
                if (who->IsInWater() || !who->GetVehicle())
                    return;

                if (who->GetDistance(-1897.0f, 2519.97f, 1.50667f) < 5.0f)
                    if (Unit* unit = who->GetVehicleBase())
                    {
                        if (Creature* watchman = who->ToCreature())
                        {
                            uint8 roll = urand(0, 2);
                            DoScriptText(DROWNING_WATCHMAN_RANDOM_SAY - roll, watchman);
                            watchman->DespawnOrUnsummon(15000);
                            watchman->SetStandState(UNIT_STAND_STATE_KNEEL);
                            CAST_AI(npc_drowning_watchman::npc_drowning_watchmanAI, watchman->AI())->exit = true;
                            CAST_AI(npc_drowning_watchman::npc_drowning_watchmanAI, watchman->AI())->reset = true;
                            who->ExitVehicle();
                            unit->RemoveAura(SPELL_RESCUE_DROWNING_WATCHMAN);
                        }

                        if (Player* player = unit->ToPlayer())
                            player->KilledMonsterCredit(NPC_QGFB_KILL_CREDIT, 0);
                    }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Grandma's Cat 14401
///////////

class npc_wahl : public CreatureScript
{
public:
    npc_wahl() : CreatureScript("npc_wahl") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wahlAI(creature);
    }

    struct npc_wahlAI : public npc_escortAI
    {
        npc_wahlAI(Creature* creature) : npc_escortAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void WaypointReached(uint32 point)
        {
            if (point == 1)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        me->UpdateEntry(NPC_WAHL_WORGEN);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        AttackStart(summoner);
                    }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_lucius_the_cruel : public CreatureScript
{
public:
    npc_lucius_the_cruel() : CreatureScript("npc_lucius_the_cruel") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lucius_the_cruelAI(creature);
    }

    struct npc_lucius_the_cruelAI : public Scripted_NoMovementAI
    {
        npc_lucius_the_cruelAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
            Catch = false;
            Summon = false;
            uiCatchTimer = 1000;
            uiShootTimer = 500;
            uiPlayerGUID = 0;
            uiSummonTimer = 1500;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        uint64 uiPlayerGUID;
        uint32 uiCatchTimer;
        uint32 uiShootTimer;
        uint32 uiSummonTimer;
        bool Catch;
        bool Summon;

        void EnterEvadeMode()
        {
            me->DespawnOrUnsummon();
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == POINT_CATCH_CHANCE)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* chance = summoner->ToCreature())
                        {
                            Catch = true;
                            Summon = true;
                            chance->AI()->DoAction(ACTION_CHANCE_DESPAWN);
                        }
            }
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_SUMMON_LUCIUS)
            {
                me->GetMotionMaster()->MovePoint(POINT_CATCH_CHANCE, -2106.372f, 2331.106f, 7.360674f);
                DoScriptText(SAY_THIS_CAT_IS_MINE, me);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Catch)
                if (uiCatchTimer <= diff)
                {
                    Catch = false;
                    uiCatchTimer = 1000;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                        AttackStart(player);
                } else
                    uiCatchTimer -= diff;

                if (Summon)
                    if (uiSummonTimer <= diff)
                    {
                        Summon = false;
                        uiSummonTimer = 1500;

                        if (Creature* wahl = me->SummonCreature(NPC_WAHL, -2098.366f, 2352.075f, 7.160643f))
                        {
                            DoScriptText(YELL_DONT_MESS, wahl);

                            if (npc_escortAI* npc_escort = CAST_AI(npc_wahl::npc_wahlAI, wahl->AI()))
                            {
                                npc_escort->AddWaypoint(0, -2106.54f, 2342.69f, 6.93668f);
                                npc_escort->AddWaypoint(1, -2106.12f, 2334.90f, 7.36691f);
                                npc_escort->AddWaypoint(2, -2117.80f, 2357.15f, 5.88139f);
                                npc_escort->AddWaypoint(3, -2111.46f, 2366.22f, 7.17151f);
                                npc_escort->Start(false, true);
                            }
                        }

                    } else
                        uiSummonTimer -= diff;

                    if (!UpdateVictim())
                        return;

                    if (uiShootTimer <= diff)
                    {
                        uiShootTimer = 1000;

                        if (me->GetDistance(me->getVictim()) > 2.0f)
                            DoCast(me->getVictim(), SPELL_SHOOT_QGC);
                    } else
                        uiShootTimer -= diff;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_chance_the_cat : public CreatureScript
{
public:
    npc_chance_the_cat() : CreatureScript("npc_chance_the_cat") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chance_the_catAI(creature);
    }

    struct npc_chance_the_catAI : public ScriptedAI
    {
        npc_chance_the_catAI(Creature* creature) : ScriptedAI(creature)
        {
            Despawn = false;
            uiDespawnTimer = 500;
        }

        uint32 uiDespawnTimer;
        bool Despawn;

        void DoAction(const int32 action)
        {
            if (action == ACTION_CHANCE_DESPAWN)
                Despawn = true;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CATCH_CAT && caster->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                if (Creature* lucius = me->SummonCreature(NPC_LUCIUS_THE_CRUEL, -2111.533f, 2329.95f, 7.390349f))
                {
                    lucius->AI()->DoAction(ACTION_SUMMON_LUCIUS);
                    CAST_AI(npc_lucius_the_cruel::npc_lucius_the_cruelAI, lucius->AI())->uiPlayerGUID = caster->GetGUID();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Despawn)
                if (uiDespawnTimer <= diff)
                {
                    uiDespawnTimer = 500;
                    Despawn = false;
                    me->DespawnOrUnsummon();
                } else
                    uiDespawnTimer -= diff;
        }
    };
};

///////////
// Quest The Hungry Ettin 14416
///////////

class npc_mountain_horse_vehicle : public CreatureScript
{
public:
    npc_mountain_horse_vehicle() : CreatureScript("npc_mountain_horse_vehicle") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mountain_horse_vehicleAI(creature);
    }

    struct npc_mountain_horse_vehicleAI : public ScriptedAI
    {
        npc_mountain_horse_vehicleAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            lSummons.clear();
            uiDespawnTimer = 10000;
            despawn = false;
        }

        std::vector<Creature*> lSummons;
        uint32 uiDespawnTimer;
        bool despawn;

        void OnCharmed(bool /*charm*/)
        {
            me->EnableAI();
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ROPE_IN_HORSE)
                me->DespawnOrUnsummon();
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (apply)
            {
                despawn = false;

                if (lSummons.empty())
                    return;

                for (std::vector<Creature*>::const_iterator itr = lSummons.begin(); itr != lSummons.end(); ++itr)
                    if (*itr)
                        (*itr)->DespawnOrUnsummon();

                lSummons.clear();
            } else
            {
                uiDespawnTimer = 10000;
                despawn = true;

                if (Creature* lorna = me->FindNearestCreature(NPC_LORNA_CROWLEY_QTHE, 30.0f))
                {
                    if (lSummons.empty())
                        return;

                    Player* player = who->ToPlayer();

                    if (!player)
                        return;

                    for (std::vector<Creature*>::const_iterator itr = lSummons.begin(); itr != lSummons.end(); ++itr)
                        if (*itr)
                        {
                            player->KilledMonsterCredit(NPC_MOUNTAIN_HORSE_KILL_CREDIT, 0);
                            (*itr)->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
                            (*itr)->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
                            (*itr)->DespawnOrUnsummon();
                        }

                        lSummons.clear();
                } else
                {
                    if (lSummons.empty())
                        return;

                    for (std::vector<Creature*>::const_iterator itr = lSummons.begin(); itr != lSummons.end(); ++itr)
                        if (*itr)
                            (*itr)->DespawnOrUnsummon();

                    lSummons.clear();
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_MOUNTAIN_HORSE_FOLLOWER)
                lSummons.push_back(summoned);
        }

        void UpdateAI(const uint32 diff)
        {
            if (despawn)
                if (uiDespawnTimer <= diff)
                {
                    despawn = false;
                    uiDespawnTimer = 10000;
                    me->DespawnOrUnsummon();
                } else
                    uiDespawnTimer -= diff;
        }
    };
};

class npc_mountain_horse_follower : public CreatureScript
{
public:
    npc_mountain_horse_follower() : CreatureScript("npc_mountain_horse_follower") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mountain_horse_followerAI(creature);
    }

    struct npc_mountain_horse_followerAI : public FollowerAI
    {
        npc_mountain_horse_followerAI(Creature* creature) : FollowerAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Player* player = summoner->ToPlayer())
                    {
                        StartFollow(player);
                        me->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, player->GetGUID());
                        me->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_ROPE_CHANNEL);

                        if (Creature* horse = player->GetVehicleCreatureBase())
                            horse->AI()->JustSummoned(me);
                    }
        }
    };
};

class spell_round_up_horse : public SpellScriptLoader
{
public:
    spell_round_up_horse() : SpellScriptLoader("spell_round_up_horse") { }

    class spell_round_up_horse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_round_up_horse_SpellScript)

            void Trigger(SpellEffIndex effIndex)
        {
            Unit* target = GetExplTargetUnit();

            if (Creature* horse = target->ToCreature())
                if (horse->GetEntry() == NPC_MOUNTAIN_HORSE_VEHICLE)
                    if (Vehicle* vehicle = target->GetVehicleKit())
                        if (vehicle->HasEmptySeat(0))
                            return;

            PreventHitDefaultEffect(effIndex);
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_round_up_horse_SpellScript::Trigger, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_round_up_horse_SpellScript();
    }
};

///////////
// Quest To Greymane Manor 14465
///////////

class npc_swift_mountain_horse : public CreatureScript
{
public:
    npc_swift_mountain_horse() : CreatureScript("npc_swift_mountain_horse") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swift_mountain_horseAI(creature);
    }

    struct npc_swift_mountain_horseAI : public npc_vehicle_escortAI
    {
        npc_swift_mountain_horseAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 3:
                if (GameObject* go = me->FindNearestGameObject(GO_FIRST_GATE, 30.0f))
                    go->UseDoorOrButton();
                break;
            case 11:
                if (GameObject* go = me->FindNearestGameObject(GO_SECOND_GATE, 30.0f))
                    go->UseDoorOrButton();
                break;
            case 12:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            WorldLocation loc ;
                            loc.m_mapId       = Gilneas2;
                            loc.m_positionX   = -1586.57f;
                            loc.m_positionY   = 2551.24f;
                            loc.m_positionZ   = 130.218f;
                            player->SetHomebind(loc, 817);
                            player->CompleteQuest(QUEST_TO_GREYMANE_MANOR);
                            player->SaveToDB();
                        }
                        me->DespawnOrUnsummon();
                        break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class npc_gwen_armstead : public CreatureScript
{
public:
    npc_gwen_armstead() : CreatureScript("npc_gwen_armstead") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_TO_GREYMANE_MANOR)
        {
            float x, y;
            player->GetNearPoint2D(x, y, 2.0f, player->GetOrientation() + M_PI / 4);

            if (Creature* horse = player->SummonCreature(NPC_SWIFT_MOUNTAIN_HORSE, x, y, player->GetPositionZ(), player->GetOrientation()))
                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, horse->AI()))
                {
                    player->EnterVehicle(horse, 0);
                    horse->EnableAI();
                    escort->AddWaypoint(0, -1902.86f, 2263.62f, 42.3231f);
                    escort->AddWaypoint(1, -1875.48f, 2292.34f, 42.2551f);
                    escort->AddWaypoint(2, -1852.71f, 2302.98f, 41.8658f);
                    escort->AddWaypoint(3, -1832.20f, 2321.67f, 37.9518f, 2000);
                    escort->AddWaypoint(4, -1799.26f, 2345.02f, 35.7393f);
                    escort->AddWaypoint(5, -1784.89f, 2365.27f, 39.1875f);
                    escort->AddWaypoint(6, -1777.29f, 2413.63f, 52.9219f);
                    escort->AddWaypoint(7, -1767.03f, 2449.39f, 63.1745f);
                    escort->AddWaypoint(8, -1751.81f, 2463.86f, 69.9014f);
                    escort->AddWaypoint(9, -1718.04f, 2465.80f, 80.5825f);
                    escort->AddWaypoint(10, -1705.98f, 2469.33f, 84.9378f);
                    escort->AddWaypoint(11, -1691.09f, 2494.77f, 95.5378f, 2000);
                    escort->AddWaypoint(12, -1672.92f, 2515.75f, 97.8955f);
                    escort->Start(true);
                }
        }
        return true;
    }
};

///////////
// Quest The King's Observatory 14466, Alas, Gilneas! 14467
///////////

class npc_king_genn_greymane_c3 : public CreatureScript
{
public:
    npc_king_genn_greymane_c3() : CreatureScript("npc_king_genn_greymane_c3") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_KINGS_OBSERVATORY)
        {
            player->CastSpell(player, SPELL_CATACLYSM_TYPE_3, false);
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_11, false);
            player->SaveToDB();
        }

        if (quest->GetQuestId() == QUEST_ALAS_GILNEAS)
            player->SendCinematicStart(CINEMATIC_TELESCOPE);

        return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_EXODUS)
        {
            player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_19);
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_11, false);
            player->SaveToDB();
        }

        return true;
    }
};

///////////
// Quest Exodus 24438
///////////

class npc_stagecoach_harness_summoner : public CreatureScript
{
public:
    npc_stagecoach_harness_summoner() : CreatureScript("npc_stagecoach_harness_summoner") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stagecoach_harness_summonerAI(creature);
    }

    struct npc_stagecoach_harness_summonerAI : public ScriptedAI
    {
        npc_stagecoach_harness_summonerAI(Creature* creature) : ScriptedAI(creature)
        {
            me->setActive(true);
            uiEventTimer = 1000;
            Event = true;
        }

        uint32 uiEventTimer;
        bool Event;

        void Reset()
        { }

        void OnCharmed(bool /*charm*/)
        {
            me->EnableAI();
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;

                    if (Unit* carriage = me->GetVehicleKit()->GetPassenger(2))
                    {
                        carriage->SetPhaseMask(EXODUS_PHASE_MASK, false);
                        float x, y, z, o;
                        me->GetPosition(x, y, z, o);

                        if (Creature* marie = me->SummonCreature(NPC_QE_MARIE_ALLEN, x, y, z, o))
                            marie->EnterVehicle(carriage, 1);

                        if (Creature* gwen = me->SummonCreature(NPC_QE_GWEN_ARMSTEAD, x, y, z, o))
                        {
                            gwen->EnterVehicle(carriage, 4);
                            gwen->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* krennan = me->SummonCreature(NPC_QE_KRENNAN_ARANAS, x, y, z, o))
                        {
                            krennan->EnterVehicle(carriage, 3);
                            krennan->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* watchman = me->SummonCreature(NPC_QE_DUSKHAVEN_WATCHMAN, x, y, z, o))
                        {
                            watchman->EnterVehicle(carriage, 2);
                            watchman->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* watchman = me->SummonCreature(NPC_QE_DUSKHAVEN_WATCHMAN_GUN, x, y, z, o))
                            watchman->EnterVehicle(carriage, 5);

                        if (Creature* lorna = me->SummonCreature(NPC_QE_LORNA_CROWLEY, x, y, z, o))
                            lorna->EnterVehicle(carriage, 6);
                    }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class npc_stagecoach_harness : public CreatureScript
{
public:
    npc_stagecoach_harness() : CreatureScript("npc_stagecoach_harness") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stagecoach_harnessAI(creature);
    }

    struct npc_stagecoach_harnessAI : public npc_vehicle_escortAI
    {
        npc_stagecoach_harnessAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            Event = false;
            uiCarriageGUID = 0;
            uiEventPhase = 0;
            uiEventTimer = 1500;
            SummonList.clear();

            if (creature->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Player* player = summoner->ToPlayer())
                    {
                        me->SetPhaseMask(EXODUS_PHASE_MASK, false);
                        me->SetSeerGUID(player->GetGUID());
                        me->SetVisible(false);
                        player->CompleteQuest(QUEST_EXODUS);
                        Event = true;
                    }
        }

        uint64 uiCarriageGUID;
        uint32 uiEventTimer;
        uint8 uiEventPhase;
        std::list<Creature*> SummonList;
        std::list<Creature*> OrgeList;
        bool Event;

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 93:
                if (GameObject* go = me->FindNearestGameObject(GO_KINGS_GATE, 30.0f))
                    go->UseDoorOrButton();
                break;
            case 155:
                Event = true;
                GetCreatureListWithEntryInGrid(OrgeList, me, NPC_QE_OGRE_AMBUSHER, 50.0f);
                if (Unit* carriage = me->GetVehicleKit()->GetPassenger(2))
                    if (Unit* lorna = carriage->GetVehicleKit()->GetPassenger(6))
                        if (Unit* passenger = carriage->GetVehicleKit()->GetPassenger(0))
                            if (Player* player = passenger->ToPlayer())
                                lorna->DoPersonalScriptText(LORNA_YELL_CARRIAGE, player);
                break;
            case 180:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        summoner->ExitVehicle();
                        summoner->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_19);
                        summoner->CastSpell(summoner, PHASE_QUEST_ZONE_SPECIFIC_11, false);

                        if (Player* player = summoner->ToPlayer())
                            player->SaveToDB();
                    }
                    break;
            case 197:
                for (std::list<Creature*>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                    if (*itr)
                    {
                        (*itr)->ExitVehicle();
                        (*itr)->DespawnOrUnsummon();
                    }

                    SummonList.clear();
                    me->DespawnOrUnsummon();
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            SummonList.push_back(summoned);
        }

        void OgreCast()
        {
            if (OrgeList.empty())
                return;

            for (std::list<Creature*>::const_iterator itr = OrgeList.begin(); itr != OrgeList.end(); ++itr)
            {
                if (!(*itr)->isAlive())
                    (*itr)->setDeathState(ALIVE);

                float x, y, z;
                me->GetPosition(x, y, z);
                (*itr)->CastSpell(x, y, z, SPELL_THROW_BOULDER, false);
            }
        }

        void StartEvent()
        {
            if (!me->IsVehicle())
                return;

            if (Unit* horse = me->GetVehicleKit()->GetPassenger(0))
                horse->SetPhaseMask(EXODUS_PHASE_MASK, false);

            if (Unit* horse = me->GetVehicleKit()->GetPassenger(1))
                horse->SetPhaseMask(EXODUS_PHASE_MASK, false);

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Unit* carriage = me->GetVehicleKit()->GetPassenger(2))
                    {
                        carriage->SetPhaseMask(EXODUS_PHASE_MASK, false);
                        float x, y, z, o;
                        me->GetPosition(x, y, z, o);

                        if (Creature* marie = me->SummonCreature(NPC_QE_MARIE_ALLEN, x, y, z, o))
                        {
                            marie->SetSeerGUID(summoner->GetGUID());
                            marie->SetVisible(false);
                            marie->EnterVehicle(carriage, 1);
                        }

                        if (Creature* gwen = me->SummonCreature(NPC_QE_GWEN_ARMSTEAD, x, y, z, o))
                        {
                            gwen->SetSeerGUID(summoner->GetGUID());
                            gwen->SetVisible(false);
                            gwen->EnterVehicle(carriage, 4);
                            gwen->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* krennan = me->SummonCreature(NPC_QE_KRENNAN_ARANAS, x, y, z, o))
                        {
                            krennan->SetSeerGUID(summoner->GetGUID());
                            krennan->SetVisible(false);
                            krennan->EnterVehicle(carriage, 3);
                            krennan->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* watchman = me->SummonCreature(NPC_QE_DUSKHAVEN_WATCHMAN, x, y, z, o))
                        {
                            watchman->SetSeerGUID(summoner->GetGUID());
                            watchman->SetVisible(false);
                            watchman->EnterVehicle(carriage, 2);
                            watchman->m_movementInfo.t_pos.m_orientation = M_PI;
                        }

                        if (Creature* watchman = me->SummonCreature(NPC_QE_DUSKHAVEN_WATCHMAN_GUN, x, y, z, o))
                        {
                            watchman->SetSeerGUID(summoner->GetGUID());
                            watchman->SetVisible(false);
                            watchman->EnterVehicle(carriage, 5);
                        }

                        if (Creature* lorna = me->SummonCreature(NPC_QE_LORNA_CROWLEY, x, y, z, o))
                        {
                            lorna->SetSeerGUID(summoner->GetGUID());
                            lorna->SetVisible(false);
                            lorna->EnterVehicle(carriage, 6);
                        }

                        summoner->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_11);
                        summoner->CastSpell(summoner, PHASE_QUEST_ZONE_SPECIFIC_19, false);
                        summoner->EnterVehicle(carriage, 0);

                        if (Creature* mastiff = me->FindNearestCreature(NPC_QE_YOUNG_MASTIFF, 30.0f))
                        {
                            float x, y, z, o;
                            mastiff->GetPosition(x, y, z, o);

                            if (Creature* summon_mastiff = me->SummonCreature(NPC_QE_YOUNG_MASTIFF, x, y, z, o))
                            {
                                summon_mastiff->SetSeerGUID(summoner->GetGUID());
                                summon_mastiff->SetVisible(false);
                                summon_mastiff->GetMotionMaster()->Clear();
                                summon_mastiff->GetMotionMaster()->MoveFollow(me, 1.0f, M_PI / 2, MOTION_SLOT_CONTROLLED);
                            }
                        }

                        for (int i = 0; i < 198; ++i)
                            AddWaypoint(i, HarnessWP[i][0], HarnessWP[i][1], HarnessWP[i][2]);

                        Start(true);

                        if (GameObject* go = me->FindNearestGameObject(GO_GREYMANE_GATE, 30.0f))
                            go->UseDoorOrButton();
                    }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    switch (uiEventPhase)
                    {
                    case 0:
                        Event = false;
                        StartEvent();
                        break;
                    case 1:
                        uiEventTimer = 1000;
                        OgreCast();
                        break;
                    case 2:
                        OgreCast();
                        Event = false;
                        OrgeList.clear();
                        break;
                    }
                    ++uiEventPhase;
                } else
                    uiEventTimer -= diff;

                npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class spell_summon_carriage : public SpellScriptLoader
{
public:
    spell_summon_carriage() : SpellScriptLoader("spell_summon_carriage") { }

    class spell_summon_carriage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_carriage_SpellScript)

            void SummonHarness(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (Unit* caster = GetCaster())
                caster->SummonCreature(NPC_STAGECOACH_HARNESS, -1665.08f, 2524.78f, 97.9892f, 4.5123f);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_summon_carriage_SpellScript::SummonHarness, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_summon_carriage_SpellScript();
    }
};

///////////
// Quest Stranded at the Marsh 24468
///////////

class npc_crash_survivor : public CreatureScript
{
public:
    npc_crash_survivor() : CreatureScript("npc_crash_survivor") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crash_survivorAI(creature);
    }

    struct npc_crash_survivorAI : public ScriptedAI
    {
        npc_crash_survivorAI(Creature* creature) : ScriptedAI(creature)
        {
            summon = false;
            despawn = false;
            uiDespawnTimer= 2000;
        }

        uint32 uiDespawnTimer;
        bool miss;
        bool summon;
        bool despawn;

        void Reset()
        {
            miss = false;

            if (!summon)
            {
                summon = true;
                float x, y;
                me->GetNearPoint2D(x, y, 3.0f, me->GetOrientation());
                me->SummonCreature(NPC_SWAMP_CROCOLISK, x, y, me->GetPositionZ());
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (despawn)
                if (uiDespawnTimer <= diff)
                {
                    despawn = false;
                    summon = false;
                    uiDespawnTimer = 2000;
                    me->DespawnOrUnsummon();
                } else
                    uiDespawnTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                    if (me->getVictim()->GetHealthPct() < 90)
                        miss = true;
                    else
                        miss = false;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_swamp_crocolisk : public CreatureScript
{
public:
    npc_swamp_crocolisk() : CreatureScript("npc_swamp_crocolisk") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swamp_crocoliskAI(creature);
    }

    struct npc_swamp_crocoliskAI : public ScriptedAI
    {
        npc_swamp_crocoliskAI(Creature* creature) : ScriptedAI(creature) { }

        bool miss;

        void Reset()
        {
            miss = false;

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    summoner->getThreatManager().resetAllAggro();
                    me->getThreatManager().resetAllAggro();
                    me->CombatStart(summoner);
                    summoner->CombatStart(me);
                }
        }

        void DamageTaken(Unit* attacker, uint32 &/*damage*/)
        {
            if (attacker->GetTypeId() != TYPEID_PLAYER)
                return;

            Unit* victim = NULL;

            if (victim = me->getVictim())
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    return;

            if (victim)
                me->getThreatManager().modifyThreatPercent(victim, -100);

            AttackStart(attacker);
            me->AddThreat(attacker, 100500);
        }

        void JustDied(Unit* /*who*/)
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    uint8 roll = urand(0, 3);
                    DoScriptText(SURVIVOR_RANDOM_SAY - roll, summoner);

                    if (Creature* surv = summoner->ToCreature())
                        CAST_AI(npc_crash_survivor::npc_crash_survivorAI, surv->AI())->despawn = true;
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Introductions Are in Order 24472
///////////

class npc_koroth_the_hillbreaker_qiao_friend : public CreatureScript
{
public:
    npc_koroth_the_hillbreaker_qiao_friend() : CreatureScript("npc_koroth_the_hillbreaker_qiao_friend") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_koroth_the_hillbreaker_qiao_friendAI(creature);
    }

    struct npc_koroth_the_hillbreaker_qiao_friendAI : public npc_vehicle_escortAI
    {
        npc_koroth_the_hillbreaker_qiao_friendAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            uiCleaveTimer = 250;
            uiDemoralizingShoutTimer = 500;

            if (me->isSummon())
            {
                for (int i = 0; i < 14; ++i)
                    AddWaypoint(i, KorothWP[i][0], KorothWP[i][1], KorothWP[i][2]);

                Start();
                SetRun(true);
            }
        }

        uint32 uiCleaveTimer;
        uint32 uiDemoralizingShoutTimer;

        void FinishEscort()
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if (Creature* capitan = summoner->ToCreature())
                        capitan->AI()->DoAction(ACTION_KOROTH_ATTACK);

                    if (summoner->isSummon())
                        if (Unit* _summoner = summoner->ToTempSummon()->GetSummoner())
                            if (Player* player = _summoner->ToPlayer())
                                me->DoPersonalScriptText(KOROTH_YELL_MY_BANNER, player, true);
                }
        }

        void WaypointReached(uint32 point)
        { }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiCleaveTimer <= diff)
            {
                uiCleaveTimer = urand(2500, 15000);
                DoCast(me->getVictim(), SPELL_CLEAVE);
            } else
                uiCleaveTimer -= diff;

            if (uiDemoralizingShoutTimer <= diff)
            {
                uiDemoralizingShoutTimer = 5000;
                DoCast(SPELL_DEMORALIZING_SHOUT_QIAO);
            } else
                uiDemoralizingShoutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_captain_asther_qiao : public CreatureScript
{
public:
    npc_captain_asther_qiao() : CreatureScript("npc_captain_asther_qiao") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captain_asther_qiaoAI(creature);
    }

    struct npc_captain_asther_qiaoAI : public npc_vehicle_escortAI
    {
        npc_captain_asther_qiaoAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->setActive(true);
            uiKorothGUID = 0;
            lSoldiers.clear();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (me->isSummon())
                StartEvent();
        }

        std::list<sSoldier> lSoldiers;
        uint64 uiKorothGUID;

        void JustDied(Unit* who)
        {
            me->DespawnOrUnsummon(15000);

            if (Creature* koroth = Unit::GetCreature(*me, uiKorothGUID))
                koroth->DespawnOrUnsummon(15000);

            if (lSoldiers.empty())
                return;

            for (std::list<sSoldier>::iterator itr = lSoldiers.begin(); itr != lSoldiers.end(); ++itr)
            {
                Creature* soldier = itr->soldier;
                soldier->DespawnOrUnsummon(15000);
            }
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_KOROTH_ATTACK)
                if (Creature* koroth = Unit::GetCreature(*me, uiKorothGUID))
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    AttackStart(koroth);
                    me->AddThreat(koroth, 100500);
                    koroth->AddThreat(me, 100500);
                    float x, y, z;
                    koroth->GetNearPoint(koroth, x, y, z, 0.0f, 1.0f, koroth->GetOrientation() + M_PI);
                    me->GetMotionMaster()->MoveCharge(x, y, z);

                    if (lSoldiers.empty())
                        return;

                    for (std::list<sSoldier>::iterator itr = lSoldiers.begin(); itr != lSoldiers.end(); ++itr)
                    {
                        Creature* soldier = itr->soldier;
                        soldier->SetReactState(REACT_AGGRESSIVE);
                        soldier->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        soldier->AI()->AttackStart(koroth);
                        soldier->AddThreat(koroth, 100500);

                        if (soldier->GetEntry() == NPC_FORSAKEN_CATAPULT_QIAO)
                        {
                            koroth->AddThreat(soldier, 100500);
                            koroth->AI()->AttackStart(soldier);
                            continue;
                        }

                        koroth->AddThreat(soldier, 10000);
                        soldier->GetMotionMaster()->MoveCharge(x, y, z);
                    }
                }
        }

        void StartMoveTo(float x, float y, float z)
        {
            if (lSoldiers.empty())
                return;

            float pathangle = atan2(me->GetPositionY() - y, me->GetPositionX() - x);

            for (std::list<sSoldier>::iterator itr = lSoldiers.begin(); itr != lSoldiers.end(); ++itr)
            {
                Creature* member = itr->soldier;

                if (!member->isAlive() || member->getVictim())
                    continue;

                float angle = itr->follow_angle;
                float dist = itr->follow_dist;

                float dx = x - cos(angle + pathangle) * dist;
                float dy = y - sin(angle + pathangle) * dist;
                float dz = z;

                Trinity::NormalizeMapCoord(dx);
                Trinity::NormalizeMapCoord(dy);

                member->UpdateGroundPositionZ(dx, dy, dz);

                if (member->IsWithinDist(me, dist + 5.0f))
                    member->SetUnitMovementFlags(me->GetUnitMovementFlags());
                else
                    member->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

                member->GetMotionMaster()->MovePoint(0, dx, dy, dz);
                member->SetHomePosition(dx, dy, dz, pathangle);
            }
        }

        void SummonSoldier(uint64 guid, uint32 SoldierEntry, float dist, float angle)
        {
            float x, y;
            me->GetNearPoint2D(x, y, dist, me->GetOrientation() + angle);
            float z = me->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);

            if (Creature* soldier = me->SummonCreature(SoldierEntry, x, y, z))
            {
                soldier->SetReactState(REACT_PASSIVE);
                soldier->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                soldier->SetSeerGUID(guid);
                soldier->SetVisible(false);
                soldier->setActive(true);
                sSoldier new_soldier;
                new_soldier.soldier = soldier;
                new_soldier.follow_angle = angle;
                new_soldier.follow_dist = dist;
                lSoldiers.push_back(new_soldier);
            }
        }

        void StartEvent()
        {
            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                if (Player* player = summoner->ToPlayer())
                {
                    uint64 uiPlayerGUID = player->GetGUID();

                    for (int i = 1; i < 4; ++i)
                    {
                        SummonSoldier(uiPlayerGUID, NPC_FORSAKEN_SOLDIER_QIAO, i * 2, M_PI);
                        SummonSoldier(uiPlayerGUID, NPC_FORSAKEN_SOLDIER_QIAO, i * 2, M_PI -  M_PI / (2 * i));
                        SummonSoldier(uiPlayerGUID, NPC_FORSAKEN_SOLDIER_QIAO, i * 2, M_PI +  M_PI / (2 * i));
                    }

                    SummonSoldier(uiPlayerGUID, NPC_FORSAKEN_CATAPULT_QIAO, 8.0f, M_PI);

                    for (int i = 0; i < 18; ++i)
                        AddWaypoint(i, AstherWP[i][0], AstherWP[i][1], AstherWP[i][2]);

                    Start();
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 15)
                if (Creature* koroth = me->SummonCreature(NPC_KOROTH_THE_HILLBREAKER_QIAO_FRIEND, -2213.64f, 1863.51f, 15.404f))
                    uiKorothGUID = koroth->GetGUID();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_prince_liam_greymane_qiao : public CreatureScript
{
public:
    npc_prince_liam_greymane_qiao() : CreatureScript("npc_prince_liam_greymane_qiao") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_INTRODUCTIONS_ARE_IN_ORDER)
            CAST_AI(npc_prince_liam_greymane_qiaoAI, creature->AI())->StartEvent(player);

        if (quest->GetQuestId() == QUEST_EXODUS)
        {
            WorldLocation loc ;
            loc.m_mapId       = Gilneas2;
            loc.m_positionX   = -245.84f;
            loc.m_positionY   = 1550.56f;
            loc.m_positionZ   = 18.6917f;
            player->SetHomebind(loc, 4731);
            player->SaveToDB();
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymane_qiaoAI(creature);
    }

    struct npc_prince_liam_greymane_qiaoAI : public ScriptedAI
    {
        npc_prince_liam_greymane_qiaoAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayerList.clear();
        }

        std::list<Psc_qiao> lPlayerList;

        void StartEvent(Player* player)
        {
            if (!player)
                return;

            uint64 uiPlayerGUID = player->GetGUID();
            Psc_qiao new_player;
            new_player.uiPlayerGUID = uiPlayerGUID;
            new_player.uiCapitanGUID = 0;
            new_player.uiEventTimer = 0;
            new_player.uiPhase = 0;
            Creature* capitan = NULL;

            if (capitan = player->SummonCreature(NPC_CAPTAIN_ASTHER_QIAO, -2120.19f, 1833.06f, 30.1510f, 3.87363f))
            {
                capitan->SetSeerGUID(player->GetGUID());
                capitan->SetVisible(false);
                new_player.uiCapitanGUID = capitan->GetGUID();
            }

            if (!capitan)
                return;

            lPlayerList.push_back(new_player);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!lPlayerList.empty())
                for (std::list<Psc_qiao>::iterator itr = lPlayerList.begin(); itr != lPlayerList.end();)
                    if (itr->uiEventTimer <= diff)
                    {
                        ++itr->uiPhase;

                        if (Player* player = Unit::GetPlayer(*me, itr->uiPlayerGUID))
                            switch (itr->uiPhase)
                        {
                            case 1:
                                itr->uiEventTimer = 8000;
                                me->DoPersonalScriptText(LIAN_SAY_HERE_FORSAKEN, player, true);
                                break;
                            case 2:
                                itr->uiEventTimer = 5000;
                                me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                                me->DoPersonalScriptText(LIAM_YELL_YOU_CANT, player, true);
                                break;
                            case 3:
                                itr->uiEventTimer = 3000;
                                me->CastSpell(me, SPELL_PUSH_BANNER, false);
                                break;
                            case 4:
                                if (Creature* capitan = Unit::GetCreature(*me, itr->uiCapitanGUID))
                                    capitan->DoPersonalScriptText(CAPITAN_YELL_WILL_ORDER, player, true);
                                itr = lPlayerList.erase(itr);
                                break;
                        }
                    } else
                    {
                        itr->uiEventTimer -= diff;
                        ++itr;
                    }

                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_koroth_the_hillbreaker_qiao : public CreatureScript
{
public:
    npc_koroth_the_hillbreaker_qiao() : CreatureScript("npc_koroth_the_hillbreaker_qiao") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_koroth_the_hillbreaker_qiaoAI(creature);
    }

    struct npc_koroth_the_hillbreaker_qiaoAI : public ScriptedAI
    {
        npc_koroth_the_hillbreaker_qiaoAI(Creature* creature) : ScriptedAI(creature)
        {
            uiEventTimer = 5000;
            Event = false;
        }

        uint32 uiEventTimer;
        bool Event;

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == POINT_BANNER)
                me->GetMotionMaster()->MoveTargetedHome();
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <=  diff)
                {
                    Event = false;
                    uiEventTimer = 5000;
                    DoScriptText(KOROTH_YELL_FIND_YOU, me);
                } else
                    uiEventTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class go_koroth_banner : public GameObjectScript
{
public:
    go_koroth_banner() : GameObjectScript("go_koroth_banner") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_INTRODUCTIONS_ARE_IN_ORDER) == QUEST_STATUS_INCOMPLETE)
            if (Creature* koroth = go->FindNearestCreature(NPC_KOROTH_THE_HILLBREAKER_QIAO, 30.0f))
            {
                DoScriptText(KOROTH_YELL_WHO_STEAL_BANNER, koroth);
                float x, y;
                go->GetNearPoint2D(x, y, 5.0f, go->GetOrientation() + M_PI / 2);
                koroth->GetMotionMaster()->MovePoint(POINT_BANNER, x, y, go->GetPositionZ());
                CAST_AI(npc_koroth_the_hillbreaker_qiao::npc_koroth_the_hillbreaker_qiaoAI, koroth->AI())->Event = true;
            }

            return false;
    }
};

///////////
// Quest Losing Your Tail 24616
///////////

class npc_dark_scout_summoner : public CreatureScript
{
public:
    npc_dark_scout_summoner() : CreatureScript("npc_dark_scout_summoner") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_scout_summonerAI(creature);
    }

    struct npc_dark_scout_summonerAI : public ScriptedAI
    {
        npc_dark_scout_summonerAI(Creature* creature) : ScriptedAI(creature) { }

        void MoveInLineOfSight(Unit* who)
        {
            if (Player* player = who->ToPlayer())
                if (me->IsWithinDistInMap(player, 20.0f) && player->GetQuestStatus(QUEST_LOSING_YOUR_TAIL) == QUEST_STATUS_INCOMPLETE)
                {
                    if (player->isInCombat())
                        return;

                    if (Creature* scout = me->SummonCreature(NPC_DARK_SCOUT, -2239.28f, 1429.67f, -22.86f))
                    {
                        scout->AddAura(SPELL_FREEZING_TRAP, player);
                        DoScriptText(NPC_DARK_SCOUT_SAY_CATCH, scout);
                        scout->Whisper(NPC_DARK_SCOUT_SAY_FREE, player->GetGUID(), true);
                        scout->AI()->AttackStart(player);
                        scout->CastSpell(who, SPELL_AIMED_SHOT, false);
                    }
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
        }
    };
};

class npc_dark_scout : public CreatureScript
{
public:
    npc_dark_scout() : CreatureScript("npc_dark_scout") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_scoutAI(creature);
    }

    struct npc_dark_scoutAI : public ScriptedAI
    {
        npc_dark_scoutAI(Creature* creature) : ScriptedAI(creature)
        {
            me->AddUnitState(UNIT_STATE_ROOT);
            me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
        }

        void Reset()
        {
            shot = true;
            uiShotTimer = 2000;
        }

        uint32 uiShotTimer;
        bool shot;

        void EnterEvadeMode()
        {
            me->DespawnOrUnsummon();
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_PLAYER_IS_FREE)
            {
                DoScriptText(NPC_DARK_SCOUT_SAY_HOW, me);
                shot = false;
                me->CastStop();
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (shot)
                if (uiShotTimer <= diff)
                {
                    uiShotTimer = 2000;
                    DoCast(me->getVictim(), SPELL_AIMED_SHOT);
                } else
                    uiShotTimer -= diff;

                DoMeleeAttackIfReady();
        }
    };
};

class spell_belysras_talisman : public SpellScriptLoader
{
public:
    spell_belysras_talisman() : SpellScriptLoader("spell_belysras_talisman") { }

    class spell_belysras_talisman_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_belysras_talisman_SpellScript)

            void DispellStun(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Unit* target = GetExplTargetUnit();
            Unit* caster = GetCaster();

            if (!target || !caster)
                return;

            if (Aura* aur = caster->GetAura(SPELL_FREEZING_TRAP))
            {
                if (Unit* owner = aur->GetCaster())
                    if (Creature* scout = owner->ToCreature())
                        scout->AI()->DoAction(ACTION_PLAYER_IS_FREE);

                caster->RemoveAura(SPELL_FREEZING_TRAP);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_belysras_talisman_SpellScript::DispellStun, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_belysras_talisman_SpellScript();
    }
};

///////////
// Quest Take Back What's Ours 24646
///////////

class npc_taldoren_tracker : public CreatureScript
{
public:
    npc_taldoren_tracker() : CreatureScript("npc_taldoren_tracker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_taldoren_trackerAI(creature);
    }

    struct npc_taldoren_trackerAI : public npc_escortAI
    {
        npc_taldoren_trackerAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_BACK)
            {
                DoCast(SPELL_WAR_STOMP);
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop();
                SetEscortPaused(false);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 2)
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_tobias_mistmantle_qtbwo : public CreatureScript
{
public:
    npc_tobias_mistmantle_qtbwo() : CreatureScript("npc_tobias_mistmantle_qtbwo") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tobias_mistmantle_qtbwoAI(creature);
    }

    struct npc_tobias_mistmantle_qtbwoAI : public ScriptedAI
    {
        npc_tobias_mistmantle_qtbwoAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
        {
            uiEventTimer = 25000;
            Event = true;
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (creature->isSummon())
                SummonTrackers();
        }

        SummonList lSummons;
        uint32 uiEventTimer;
        bool Event;

        void SummonTrackers()
        {
            std::list<Creature*> RangerList;
            me->GetCreatureListWithEntryInGrid(RangerList, NPC_VETERAN_DARK_RANGER, 80.0f);

            if (RangerList.empty())
                return;

            DoScriptText(TOBIAS_SAY_DISTRACT_RANGERS, me, me->ToTempSummon()->GetSummoner());

            for (std::list<Creature*>::const_iterator itr = RangerList.begin(); itr != RangerList.end(); ++itr)
            {
                if ((*itr)->isAlive())
                {
                    Position pos;
                    (*itr)->GetNearPosition(pos, 2.0f, (*itr)->GetOrientation());

                    if (Creature* tracker = me->SummonCreature(NPC_TALDOREN_TRACKER, pos))
                    {
                        lSummons.Summon(tracker);

                        if (npc_escortAI* npc_escort = CAST_AI(npc_taldoren_tracker::npc_taldoren_trackerAI, tracker->AI()))
                        {
                            float dist_1 = pos.GetExactDist2d(WorgensBackPos[0][0], WorgensBackPos[0][1]);
                            float dist_2 = pos.GetExactDist2d(WorgensBackPos[4][0], WorgensBackPos[4][1]);

                            int j = (dist_1 < dist_2) ? 0 : 4;

                            npc_escort->AddWaypoint(0, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 0, true);

                            for (int i = 0; i < 4; ++i)
                            {
                                const Position src ={WorgensBackPos[j + i][0], WorgensBackPos[j + i][1], WorgensBackPos[j + i][2], 0};
                                Position dst;
                                me->GetRandomPoint(src, 5.0f, dst);
                                npc_escort->AddWaypoint(i + 1, dst.m_positionX, dst.m_positionY, dst.m_positionZ, 0, (i == 2) ? true : false);
                            }

                            npc_escort->SetEscortPaused(true);
                            npc_escort->Start(true, true);
                            tracker->SetReactState(REACT_AGGRESSIVE);
                            tracker->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            tracker->CombatStart((*itr));
                            (*itr)->CombatStart(tracker);
                        }
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    DoScriptText(TOBIAS_SAY_FALL_BACK, me);
                    lSummons.DoAction(NPC_TALDOREN_TRACKER, ACTION_BACK);
                    me->DespawnOrUnsummon(2000);
                } else
                    uiEventTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class spell_horn_of_taldoren : public SpellScriptLoader
{
public:
    spell_horn_of_taldoren() : SpellScriptLoader("spell_horn_of_taldoren") { }

    class spell_horn_of_taldoren_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_horn_of_taldoren_SpellScript)

            void SummonWorgens(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Unit* caster = GetCaster();
            if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                return;

            float x = -2146.12f, y = 1583.35f;
            float d = caster->GetExactDist2d(x, y);

            if (d > 30.0f)
                return;

            if (caster->FindNearestCreature(NPC_TOBIAS_MISTMANTLE_QTBWO, 40.0f))
                return;

            if (caster->ToPlayer()->GetQuestStatus(QUEST_TAKE_BACK_WHATS_OURS) == QUEST_STATUS_INCOMPLETE)
                caster->SummonCreature(NPC_TOBIAS_MISTMANTLE_QTBWO, -2146.12f, 1583.35f, -48.8068f, 1.239184f);
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_horn_of_taldoren_SpellScript::SummonWorgens, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_horn_of_taldoren_SpellScript();
    }
};

///////////
// Quest Neither Human Nor Beast 24593
///////////

class go_qnhnb_well : public GameObjectScript
{
public:
    go_qnhnb_well() : GameObjectScript("go_qnhnb_well") { }

    void SummonPersonalTrigger(Player* player, GameObject* go)
    {
        float x, y, z;
        go->GetPosition(x, y, z);

        if (Creature* trigger = player->SummonCreature(NPC_TRIGGER, x, y, z, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 15000))
        {
            trigger->SetSeerGUID(player->GetGUID());
            trigger->SetVisible(false);
            trigger->AddAura(SPELL_TALDOREN_WELL, trigger);
        }
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_NEITHER_HUMAN_NOR_BEAST) == QUEST_STATUS_INCOMPLETE)
        {
            QuestStatusMap::iterator itr = player->getQuestStatusMap().find(QUEST_NEITHER_HUMAN_NOR_BEAST);

            switch (go->GetEntry())
            {
            case GO_WELL_OF_FURY:
                if (itr->second.CreatureOrGOCount[0] == 0)
                {
                    player->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), 0);
                    SummonPersonalTrigger(player, go);

                    if (Creature* lyros = go->FindNearestCreature(NPC_LYROS_SWIFTWIND, 30.0f))
                        lyros->DoPersonalScriptText(LYROS_SAY_WELL_OF_FURY, player);
                }
                break;
            case GO_WELL_OF_TRANQUILITY:
                if (itr->second.CreatureOrGOCount[1] == 0)
                {
                    player->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), 0);
                    SummonPersonalTrigger(player, go);

                    if (Creature* vassandra = go->FindNearestCreature(37873, 30.0f))
                        vassandra->DoPersonalScriptText(VASSANDRA_SAY_WELL_OF_TRANQUILITY, player);
                }
                break;
            case GO_WELL_OF_BALANCE:
                if (itr->second.CreatureOrGOCount[2] == 0)
                {
                    player->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), 0);
                    SummonPersonalTrigger(player, go);

                    if (Creature* talran = go->FindNearestCreature(36814, 30.0f))
                        talran->DoPersonalScriptText(TALRAN_SAY_WELL_OF_BALANCE, player);
                }
                break;
            }
        }
        return true;
    }
};

class npc_lord_godfrey_qnhnb : public CreatureScript
{
public:
    npc_lord_godfrey_qnhnb() : CreatureScript("npc_lord_godfrey_qnhnb") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_godfrey_qnhnbAI(creature);
    }

    struct npc_lord_godfrey_qnhnbAI : public npc_escortAI
    {
        npc_lord_godfrey_qnhnbAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 point)
        {
            if (point == 2)
                if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                    me->SetFacingToObject(crowley);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_lorna_crowley_qnhnb : public CreatureScript
{
public:
    npc_lorna_crowley_qnhnb() : CreatureScript("npc_lorna_crowley_qnhnb") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lorna_crowley_qnhnbAI(creature);
    }

    struct npc_lorna_crowley_qnhnbAI : public npc_escortAI
    {
        npc_lorna_crowley_qnhnbAI(Creature* creature) : npc_escortAI(creature)
        {
            uiSayTimer = 1000;
            say = false;
        }

        uint32 uiSayTimer;
        bool say;

        void WaypointReached(uint32 point)
        {
            if (point == 1)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            SetRun(false);
                            say = true;
                            me->DoPersonalScriptText(LORNA_YELL_FATHER, player);
                        }

                        if (point == 2)
                            if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                                me->SetFacingToObject(crowley);
        }

        void UpdateAI(const uint32 diff)
        {
            if (say)
                if (uiSayTimer <= diff)
                {
                    say = false;

                    if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                                crowley->DoPersonalScriptText(CROWLEY_YELL_LORNA, player);
                } else
                    uiSayTimer -= diff;

                npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_king_genn_greymane_qnhnb : public CreatureScript
{
public:
    npc_king_genn_greymane_qnhnb() : CreatureScript("npc_king_genn_greymane_qnhnb") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_genn_greymane_qnhnbAI(creature);
    }

    struct npc_king_genn_greymane_qnhnbAI : public npc_escortAI
    {
        npc_king_genn_greymane_qnhnbAI(Creature* creature) : npc_escortAI(creature)
        {
            uiLornaGUID = 0;
            uiGodfreyGUID = 0;
            uiEventTimer = 0;
            uiPhase = 0;
            Event= false;
            StartEvent();
        }

        uint64 uiLornaGUID;
        uint64 uiGodfreyGUID;
        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;

        void StartEvent()
        {
            if (!me->isSummon())
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                if (Player* player = summoner->ToPlayer())
                {
                    Start(false);
                    SetDespawnAtEnd(false);
                    me->setActive(true);

                    if (Creature* lorna = player->SummonCreature(NPC_LORNA_CROWLEY_QNHNB, -2104.00f, 1282.00f, -83.7271f))
                    {
                        uiLornaGUID = lorna->GetGUID();
                        lorna->SetSeerGUID(player->GetGUID());
                        lorna->SetVisible(false);
                        CAST_AI(npc_lorna_crowley_qnhnb::npc_lorna_crowley_qnhnbAI, lorna->AI())->Start(false, true);
                        CAST_AI(npc_lorna_crowley_qnhnb::npc_lorna_crowley_qnhnbAI, lorna->AI())->SetDespawnAtEnd(false);
                    }

                    if (Creature* godfrey = player->SummonCreature(NPC_LORD_GODFREY_QNHNB, -2110.44f, 1288.52f, -83.5184f))
                    {
                        uiGodfreyGUID = godfrey->GetGUID();
                        godfrey->SetSeerGUID(player->GetGUID());
                        godfrey->SetVisible(false);
                        CAST_AI(npc_lord_godfrey_qnhnb::npc_lord_godfrey_qnhnbAI, godfrey->AI())->Start(false);
                        CAST_AI(npc_lord_godfrey_qnhnb::npc_lord_godfrey_qnhnbAI, godfrey->AI())->SetDespawnAtEnd(false);
                    }
                }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 2)
            {
                Event = true;

                if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                    me->SetFacingToObject(crowley);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (Event)
                if (uiEventTimer <= diff)
                {
                    ++uiPhase;

                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                            switch (uiPhase)
                        {
                            case 1:
                                uiEventTimer= 13000;
                                if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                {
                                    godfrey->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                                    godfrey->DoPersonalScriptText(GODFREY_SAY_LOW, player);
                                }
                                break;
                            case 2:
                                uiEventTimer= 8000;
                                if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                                    crowley->DoPersonalScriptText(CROWLEY_YELL_FROG, player);
                                break;
                            case 3:
                                uiEventTimer= 4000;
                                DoCast(SPELL_WORGEN_COMBAT_TRANSFORM_FX);
                                me->DoPersonalScriptText(GREYMANE_SAY_NO_FRIEND, player);
                                break;
                            case 4:
                                uiEventTimer= 2000;
                                if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                    godfrey->DoPersonalScriptText(GODFREY_YELL_CANT_BE, player);
                                break;
                            case 5:
                                uiEventTimer= 11000;
                                if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QNHNB, 30.0f))
                                {
                                    crowley->HandleEmoteCommand(EMOTE_ONESHOT_YES);
                                    crowley->DoPersonalScriptText(CROWLEY_SAY_YES_GENN, player);
                                }
                                break;
                            case 6:
                                uiEventTimer= 15000;
                                me->DoPersonalScriptText(CREYMANE_SAY_HELD, player);
                                break;
                            case 7:
                                Event = false;
                                if (Creature* godfrey = Unit::GetCreature(*me, uiGodfreyGUID))
                                    godfrey->DespawnOrUnsummon();
                                if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                                    lorna->DespawnOrUnsummon();
                                me->DespawnOrUnsummon();
                                break;
                        }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class spell_worgen_combat_transform_fx : public SpellScriptLoader
{
public:
    spell_worgen_combat_transform_fx() : SpellScriptLoader("spell_worgen_combat_transform_fx") { }

    class spell_worgen_combat_transform_fx_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_worgen_combat_transform_fx_SpellScript)

            void Transform(SpellEffIndex effIndex)
        {
            Unit* target = GetCaster();

            if (!target)
                return;

            if (Creature* greymane = target->ToCreature())
                if (greymane->GetEntry() == NPC_KING_GENN_GREYMANE_HUMAN)
                    greymane->SetDisplayId(NPC_KING_GENN_GREYMANE_WORGEN);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_worgen_combat_transform_fx_SpellScript::Transform, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_worgen_combat_transform_fx_SpellScript();
    }
};

///////////
// Quest At Our Doorstep 24627
///////////

class npc_tobias_mistmantle_qaod : public CreatureScript
{
public:
    npc_tobias_mistmantle_qaod() : CreatureScript("npc_tobias_mistmantle_qaod") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tobias_mistmantle_qaodAI(creature);
    }

    struct npc_tobias_mistmantle_qaodAI : public npc_vehicle_escortAI
    {
        npc_tobias_mistmantle_qaodAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            uiSpeachTimer = 5000;
            uiSpeachId = 0;
            InPosition = false;
        }

        uint32 uiSpeachTimer;
        uint8 uiSpeachId;
        bool InPosition;

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 1:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            me->DoPersonalScriptText(TOBIAS_SAY_FORSAKEN, player);
                            InPosition = true;
                            SetEscortPaused(true);
                        }
                        break;
            case 3:
                me->DespawnOrUnsummon();
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);

            if (InPosition)
                if (uiSpeachTimer <= diff)
                {
                    ++uiSpeachId;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                                switch (uiSpeachId)
                            {
                                case 1:
                                    uiSpeachTimer = 7000;
                                    if (Creature* crowley = me->FindNearestCreature(NPC_LORD_DARIUS_CROWLEY_QAOD, 30.0f))
                                        crowley->DoPersonalScriptText(CROWLEY_SAY_IMMEDIATELY, player);
                                    break;
                                case 2:
                                    me->DoPersonalScriptText(TOBIAS_SAY_OK, player);
                                    uiSpeachTimer = 3000;
                                    break;
                                case 3:
                                    SetEscortPaused(false);
                                    break;
                            }
                } else
                    uiSpeachTimer -= diff;
        }
    };
};

class npc_lord_darius_crowley_qaod : public CreatureScript
{
public:
    npc_lord_darius_crowley_qaod() : CreatureScript("npc_lord_darius_crowley_qaod") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_AT_OUR_DOORSTEP)
            if (Creature* tobias = player->SummonCreature(NPC_TOBIAS_MISTMANTLE_QAOD, -2079.345f, 1327.531f, -83.0644f))
            {
                tobias->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                tobias->SetSeerGUID(player->GetGUID());
                tobias->SetVisible(false);

                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, tobias->AI()))
                {
                    for (int i = 0; i < 4; ++i)
                        escort->AddWaypoint(i, TobiasWP[i][0], TobiasWP[i][1], TobiasWP[i][2]);

                    escort->Start(true);
                }
            }

            if (quest->GetQuestId() == QUEST_NEITHER_HUMAN_NOR_BEAST)
            {
                player->CastSpell(player, SPELL_LEARN_TWO_FORMS, false);

                if (Creature* greymane = player->SummonCreature(NPC_KING_GENN_GREYMANE_HUMAN, -2111.09f, 1286.12f, -83.1804f))
                {
                    greymane->SetSeerGUID(player->GetGUID());
                    greymane->SetVisible(false);
                }
            }

            return true;
    }
};

///////////
// Quest Betrayal at Tempest's Reach 24592
///////////

class npc_krennan_aranas_qbatr : public CreatureScript
{
public:
    npc_krennan_aranas_qbatr() : CreatureScript("npc_krennan_aranas_qbatr") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BETRAYAL_AT_TEMPESTS_REACH)
        {
            player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_2, player);
            player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_1, player);
        }

        return true;
    }
};

class npc_lord_godfrey_qbatr : public CreatureScript
{
public:
    npc_lord_godfrey_qbatr() : CreatureScript("npc_lord_godfrey_qbatr") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_godfrey_qbatrAI(creature);
    }

    struct npc_lord_godfrey_qbatrAI : public npc_escortAI
    {
        npc_lord_godfrey_qbatrAI(Creature* creature) : npc_escortAI(creature)
        {
            AddWaypoint(0, -2055.98f, 937.692f, 69.9569f, 0, true);
            AddWaypoint(1, -2080.73f, 889.893f, -43.8015f);
            uiEventTimer = 4000;
            uiPhase = 0;
            Event = false;
            start = false;
        }

        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;
        bool start;

        void StartEvent()
        {
            if (!(Event && start))
            {
                Event = true;

                if (Creature* greymane = me->FindNearestCreature(NPC_KING_GENN_GREYMANE_QBATR, 30.0f))
                    DoScriptText(GENN_SAY_ALL_IS_DONE, greymane);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 1)
                start = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                    switch (uiPhase)
                {
                    case 0:
                        uiPhase = 1;
                        uiEventTimer = 1500;
                        DoScriptText(GODFREY_SAY_BETTER_DIED, me);
                        break;
                    case 1:
                        uiPhase = 0;
                        start = true;
                        Event = false;
                        uiEventTimer = 4000;
                        Start(false, true, 0, NULL, true);
                        SetSpeedZ(25.0f);
                        break;
                } else
                    uiEventTimer -= diff;

                npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_king_genn_greymane_qbatr : public CreatureScript
{
public:
    npc_king_genn_greymane_qbatr() : CreatureScript("npc_king_genn_greymane_qbatr") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BETRAYAL_AT_TEMPESTS_REACH)
        {
            player->AddAura(SPELL_AI_REACTION, player);
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_2);
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_1);
            player->SaveToDB();

            if (Creature* godfrey = creature->FindNearestCreature(NPC_LORD_GODFREY_QBATR, 30.0f))
                CAST_AI(npc_lord_godfrey_qbatr::npc_lord_godfrey_qbatrAI, godfrey->AI())->StartEvent();
        }

        return true;
    }
};

///////////
// Quest Liberation Day 24575
///////////

class go_ball_and_chain_qld : public GameObjectScript
{
public:
    go_ball_and_chain_qld() : GameObjectScript("go_ball_and_chain_qld") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_LIBERATION_DAY) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* villager = go->FindNearestCreature(NPC_ENSLAVED_VILLAGER_QLD, 5.0f))
            {
                uint8 roll = urand(0, 2);
                DoScriptText(VILLAGER_RANDOM_SAY - roll, villager);
                villager->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                villager->DespawnOrUnsummon(3000);
                player->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), 0);
            }
        }

        return true;
    }
};

///////////
// Quest The Battle for Gilneas City QUEST_THE_BATTLE_FOR_GILNEAS_CITY
///////////

class npc_prince_liam_greymane_tbfgc : public CreatureScript
{
public:
    npc_prince_liam_greymane_tbfgc() : CreatureScript("npc_prince_liam_greymane_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymane_tbfgcAI(creature);
    }

    struct npc_prince_liam_greymane_tbfgcAI : public npc_escortAI
    {
        npc_prince_liam_greymane_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            me->SetPhaseMask(262144, true);
            FirstWave.clear();
            SecondWave.clear();
            ThirdWave.clear();
            CannonList.clear();
            WorgenList.clear();
            GreymaneMilitia.clear();
            creature->setActive(true);
            SetCombatMovement(false);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiMyriamGUID = 0;
            uiAlmyraGUID = 0;
            uiLornaGUID = 0;
            uiCrowleyGUID = 0;
            uiGreymaneGUID = 0;
            uiSylvanasGUID = 0;
            uiEventTimer = 0;
            uiDeadBossCount = 2;
            uiShootTimer = 1500;
            uiMultiShotTimer = 3000;
            uiChallengingShoutTimer = urand(10000, 30000);
            uiRandomSpechTimer = urand(10000, 30000);
            uiFailTimer = 900000;
            uiSubPhase = 0;
            Event = false;
            EscortFinished = false;
            Finish = false;
            Aura = true;
            Fail = false;
            uiBattlePhase = PHASE_NONE;
            DoCast(SPELL_SOLDIER_OF_TBFGC);
            SummonEventCreatures();
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_BANSHEE_QUEENS_WAIL, true);
        }

        std::list<uint64> FirstWave;
        std::list<uint64> SecondWave;
        std::list<uint64> ThirdWave;
        std::list<uint64> CannonList;
        std::list<uint64> WorgenList;
        std::list<uint64> GreymaneMilitia;
        uint64 uiMyriamGUID;
        uint64 uiAlmyraGUID;
        uint64 uiLornaGUID;
        uint64 uiCrowleyGUID;
        uint64 uiGreymaneGUID;
        uint64 uiSylvanasGUID;
        uint32 uiEventTimer;
        uint32 uiShootTimer;
        uint32 uiMultiShotTimer;
        uint32 uiChallengingShoutTimer;
        uint32 uiRandomSpechTimer;
        uint32 uiFailTimer;
        uint8 uiSubPhase;
        uint8 uiBattlePhase;
        uint8 uiDeadBossCount;
        bool Event;
        bool EscortFinished;
        bool Finish;
        bool Aura;
        bool Fail;

        void SetFacingToLiam()
        {
            if (Creature* myriam = Unit::GetCreature(*me, uiMyriamGUID))
                myriam->SetFacingToObject(me);

            if (Creature* almyra = Unit::GetCreature(*me, uiAlmyraGUID))
                almyra->SetFacingToObject(me);

            if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                lorna->SetFacingToObject(me);

            if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                crowley->SetFacingToObject(me);

            if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                greymane->SetFacingToObject(me);

            if (!FirstWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = FirstWave.begin(); itr != FirstWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->SetFacingToObject(me);
                }
            }

            if (!SecondWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = SecondWave.begin(); itr != SecondWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->SetFacingToObject(me);
                }
            }

            if (!ThirdWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = ThirdWave.begin(); itr != ThirdWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->SetFacingToObject(me);
                }
            }

            if (!WorgenList.empty())
            {
                for (std::list<uint64>::const_iterator itr = WorgenList.begin(); itr != WorgenList.end();)
                {
                    Creature* worgen = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (worgen)
                        worgen->SetFacingToObject(me);
                }
            }

            if (!GreymaneMilitia.empty())
            {
                for (std::list<uint64>::const_iterator itr = GreymaneMilitia.begin(); itr != GreymaneMilitia.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->SetFacingToObject(me);
                }
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SHOOT_LIAM)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->CombatStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            }
        }

        void DespawnEvent(uint32 time)
        {
            if (Creature* myriam = Unit::GetCreature(*me, uiMyriamGUID))
                myriam->DespawnOrUnsummon(time);

            if (Creature* almyra = Unit::GetCreature(*me, uiAlmyraGUID))
                almyra->DespawnOrUnsummon(time);

            if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                lorna->DespawnOrUnsummon(time);

            if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                crowley->DespawnOrUnsummon(time);

            if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                greymane->DespawnOrUnsummon(time);

            if (!FirstWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = FirstWave.begin(); itr != FirstWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->DespawnOrUnsummon(time);
                }
            }

            if (!SecondWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = SecondWave.begin(); itr != SecondWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->DespawnOrUnsummon(time);
                }
            }

            if (!ThirdWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = ThirdWave.begin(); itr != ThirdWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->DespawnOrUnsummon(time);
                }
            }

            if (!WorgenList.empty())
            {
                for (std::list<uint64>::const_iterator itr = WorgenList.begin(); itr != WorgenList.end();)
                {
                    Creature* worgen = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (worgen)
                        worgen->DespawnOrUnsummon(time);
                }
            }

            if (!GreymaneMilitia.empty())
            {
                for (std::list<uint64>::const_iterator itr = GreymaneMilitia.begin(); itr != GreymaneMilitia.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        militia->DespawnOrUnsummon(time);
                }
            }

            me->DespawnOrUnsummon(time);
        }

        void SummonKingCreaymane()
        {
            if (Creature* sylvanas = me->SummonCreature(NPC_LADY_SYLVANAS_WINDRUNNER_TBFGC, -1684.79f, 1616.47f, 20.4894f, 2.50855f))
                uiSylvanasGUID = sylvanas->GetGUID();

            if (Creature* greymane = me->SummonCreature(NPC_KING_GENN_GREYMANE_TBFGC, -1797.62f, 1709.17f, 22.1583f))
            {
                uiGreymaneGUID = greymane->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, greymane->AI()))
                {
                    for (int j = 0; j < 12; ++j)
                        escort->AddWaypoint(j, GreymaneWP[j][0], GreymaneWP[j][1], GreymaneWP[j][2]);

                    escort->Start(true, true);
                }
            }

            for (int i = 0; i < 15; ++i)
                if (Creature* militia = me->SummonCreature(NPC_GILNEAN_MILITIA, -1797.62f, 1709.17f, 22.1583f))
                {
                    GreymaneMilitia.push_back(militia->GetGUID());

                    if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                    {
                        int roll_x = irand(-3, 3);
                        int roll_y = irand(-3, 3);

                        for (int j = 0; j < 12; ++j)
                            escort->AddWaypoint(j, GreymaneWP[j][0] + roll_x, GreymaneWP[j][1] + roll_y, GreymaneWP[j][2]);

                        escort->Start(true, true);
                    }
                }
        }

        void SummonCrowley()
        {
            if (Creature* crowley = me->SummonCreature(NPC_LORD_DARIUS_CROWLEY_TBFGC, -1791.19f, 1323.6f, 19.8029f))
            {
                uiCrowleyGUID = crowley->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, crowley->AI()))
                {
                    for (int j = 0; j < 46; ++j)
                        escort->AddWaypoint(j, FWorgenWP[j][0], FWorgenWP[j][1], FWorgenWP[j][2]);

                    escort->Start(true, true);
                }
            }

            for (int i = 0; i < 12; ++i)
                if (Creature* worgen = me->SummonCreature(NPC_WORGEN_WARRIOR, WorgenSummPos[i][0], WorgenSummPos[i][1], WorgenSummPos[i][2]))
                {
                    WorgenList.push_back(worgen->GetGUID());

                    if (npc_escortAI* escort = CAST_AI(npc_escortAI, worgen->AI()))
                    {
                        int roll_x = irand(-3, 3);
                        int roll_y = irand(-3, 3);

                        for (int j = 0; j < 46; ++j)
                            escort->AddWaypoint(j, FWorgenWP[j][0] + roll_x, FWorgenWP[j][1] + roll_y, FWorgenWP[j][2]);

                        escort->Start(true, true);
                    }
                }
        }

        void DoAction(const int32 action)
        {
            switch (action)
            {
            case ACTION_CANNON_AT_POSITION:
                if (uiBattlePhase == PHASE_MILITARY_DISTRICT)
                    Event = true;
                break;
            case ACTION_GOREROT_DIED:
                if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                    crowley->AI()->DoAction(ACTION_GOREROT_DIED);
                uiBattlePhase = PHASE_GREYMANE_COURT;
                BreakEscortPaused();
                break;
            case ACTION_FINISH_TBFGC:
                Event = false;
                Finish = true;
                break;
            }
        }

        uint32 GetData(uint32 type) const
        {
            if (type == DATA_CURRENT_PHASE)
                return uiBattlePhase;

            return 0;
        }

        void SummonLorna()
        {
            if (Creature* lorna = me->SummonCreature(NPC_LORNA_CROWLEY_TBFGC, -1551.90f, 1284.75f, 13.992f))
            {
                uiLornaGUID = lorna->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, lorna->AI()))
                {
                    for (int i = 0; i < 78; ++i)
                        escort->AddWaypoint(i, LornaWP[i][0], LornaWP[i][1], LornaWP[i][2]);

                    escort->Start(false, true);
                }
            }

            for (int i = 0; i < 3; ++i)
                if (Creature* cannon = me->SummonCreature(NPC_EMBERSTONE_CANNON, SummonCannonPos[i][0],
                    SummonCannonPos[i][1], SummonCannonPos[i][2], SummonCannonPos[i][3]))
                {
                    CannonList.push_back(cannon->GetGUID());
                    float x, y, z, o;
                    cannon->GetPosition(x, y, z, o);

                    if (Creature* villager = cannon->SummonCreature(NPC_FREED_EMBERSTONE_VILLAGER, x, y, z, o))
                    {
                        villager->EnterVehicle(cannon);
                        cannon->EnableAI();
                    }

                    if (npc_escortAI* escort = CAST_AI(npc_escortAI, cannon->AI()))
                    {
                        int roll_x = irand(-3, 3);
                        int roll_y = irand(-3, 3);

                        for (int j = 0; j < 11; ++j)
                            escort->AddWaypoint(j, CannonWP[j][0] + roll_x, CannonWP[j][1] + roll_y, CannonWP[j][2]);

                        escort->Start(false, true);
                    }
                }
        }

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;
        }

        void StartEvent()
        {
            if (!Event && uiBattlePhase == PHASE_NONE)
            {
                Event = true;
                Fail = true;
                uiSubPhase= 0;
                uiEventTimer = 10000;
                uiFailTimer = 900000;
                uiBattlePhase = PHASE_INTRO;
                uiDeadBossCount = 2;
            }
        }

        void KilledBoss()
        {
            --uiDeadBossCount;
        }

        void BreakEscortPaused()
        {
            SetEscortPaused(false);

            if (Creature* myriam = Unit::GetCreature(*me, uiMyriamGUID))
                if (npc_escortAI* escort = CAST_AI(npc_escortAI, myriam->AI()))
                    escort->SetEscortPaused(false);

            if (Creature* almyra = Unit::GetCreature(*me, uiAlmyraGUID))
                if (npc_escortAI* escort = CAST_AI(npc_escortAI, almyra->AI()))
                    escort->SetEscortPaused(false);

            if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                if (npc_escortAI* escort = CAST_AI(npc_escortAI, lorna->AI()))
                    escort->SetEscortPaused(false);

            if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                if (npc_escortAI* escort = CAST_AI(npc_escortAI, crowley->AI()))
                    escort->SetEscortPaused(false);

            if (!FirstWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = FirstWave.begin(); itr != FirstWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            escort->SetEscortPaused(false);
                }
            }

            if (!SecondWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = SecondWave.begin(); itr != SecondWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            escort->SetEscortPaused(false);
                }
            }

            if (!ThirdWave.empty())
            {
                for (std::list<uint64>::const_iterator itr = ThirdWave.begin(); itr != ThirdWave.end();)
                {
                    Creature* militia = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (militia)
                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            escort->SetEscortPaused(false);
                }
            }

            if (!WorgenList.empty())
            {
                for (std::list<uint64>::const_iterator itr = WorgenList.begin(); itr != WorgenList.end();)
                {
                    Creature* worgen = Unit::GetCreature(*me, *itr);
                    ++itr;

                    if (worgen)
                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, worgen->AI()))
                            escort->SetEscortPaused(false);
                }
            }
        }

        void StartBattle()
        {
            for (int i = 0; i < 96; ++i)
                AddWaypoint(i, FWaveWP[i][0], FWaveWP[i][1], FWaveWP[i][2]);

            Start(true, true);

            if (Creature* myriam = Unit::GetCreature(*me, uiMyriamGUID))
                if (npc_escortAI* escort = CAST_AI(npc_escortAI, myriam->AI()))
                {
                    for (int i = 0; i < 96; ++i)
                        escort->AddWaypoint(i, SWaveWP[i][0], SWaveWP[i][1], SWaveWP[i][2]);

                    escort->Start(true, true);
                }

                if (Creature* almyra = Unit::GetCreature(*me, uiAlmyraGUID))
                    if (npc_escortAI* escort = CAST_AI(npc_escortAI, almyra->AI()))
                    {
                        for (int i = 0; i < 87; ++i)
                            escort->AddWaypoint(i, TWaveWP[i][0], TWaveWP[i][1], TWaveWP[i][2]);

                        escort->Start(true, true);
                    }

                    for (std::list<uint64>::const_iterator itr = FirstWave.begin(); itr != FirstWave.end();)
                    {
                        Creature* militia = Unit::GetCreature(*me, *itr);
                        ++itr;

                        if (militia)
                        {
                            militia->AI()->SetData(DATA_WP_PAUSE, 30);

                            if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            {
                                int32 roll_x = irand(-3, 3);
                                int32 roll_y = irand(-3, 3);

                                for (int i = 0; i < 96; ++i)
                                    escort->AddWaypoint(i, FWaveWP[i][0] + roll_x, FWaveWP[i][1] + roll_y, FWaveWP[i][2]);

                                escort->Start(true, true);
                            }
                        }
                    }

                    for (std::list<uint64>::const_iterator itr = SecondWave.begin(); itr != SecondWave.end();)
                    {
                        Creature* militia = Unit::GetCreature(*me, *itr);
                        ++itr;

                        if (militia)
                        {
                            militia->AI()->SetData(DATA_WP_PAUSE, 30);

                            if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            {
                                int32 roll_x = irand(-3, 3);
                                int32 roll_y = irand(-3, 3);

                                for (int i = 0; i < 96; ++i)
                                    escort->AddWaypoint(i, SWaveWP[i][0] + roll_x, SWaveWP[i][1] + roll_y, SWaveWP[i][2]);

                                escort->Start(true, true);
                            }
                        }
                    }

                    for (std::list<uint64>::const_iterator itr = ThirdWave.begin(); itr != ThirdWave.end();)
                    {
                        Creature* militia = Unit::GetCreature(*me, *itr);
                        ++itr;

                        if (militia)
                        {
                            militia->AI()->SetData(DATA_WP_PAUSE, 17);

                            if (npc_escortAI* escort = CAST_AI(npc_escortAI, militia->AI()))
                            {
                                int32 roll_x = irand(-3, 3);
                                int32 roll_y = irand(-3, 3);

                                for (int i = 0; i < 87; ++i)
                                    escort->AddWaypoint(i, TWaveWP[i][0] + roll_x, TWaveWP[i][1] + roll_y, TWaveWP[i][2]);

                                escort->Start(true, true);
                            }
                        }
                    }
        }

        void EnterEvadeMode()
        {
            npc_escortAI::EnterEvadeMode();

            if (Aura)
                DoCast(SPELL_SOLDIER_OF_TBFGC);
        }

        void SummonEventCreatures()
        {
            if (Creature* myriam = me->SummonCreature(NPC_MYRIAM_SPELLWAKER, -1412.95f, 1248.7f, 36.5112f, 1.76278f))
                uiMyriamGUID = myriam->GetGUID();

            if (Creature* almyra = me->SummonCreature(NPC_SISTER_ALMYRA, -1401.31f, 1251.91f, 36.5112f, 2.02458f))
                uiAlmyraGUID = almyra->GetGUID();

            for (int i = 0; i < 5; ++i)
            {
                if (Creature* militia = me->SummonCreature(NPC_GILNEAN_MILITIA, MilSumPos[i][0], MilSumPos[i][1], MilSumPos[i][2], MilSumPos[i][3]))
                {
                    FirstWave.push_back(militia->GetGUID());
                    float x, y, z = militia->GetPositionZ(), o = militia->GetOrientation();

                    for (int j = 1; j < 3; ++j)
                    {
                        militia->GetNearPoint2D(x, y, 2 * j, o + M_PI);

                        if (Creature* new_militia = me->SummonCreature(NPC_GILNEAN_MILITIA, x, y, z, o))
                            FirstWave.push_back(new_militia->GetGUID());
                    }

                    for (int j = 3; j < 6; ++j)
                    {
                        militia->GetNearPoint2D(x, y, j * 2, o + M_PI);

                        if (Creature* new_militia = me->SummonCreature(NPC_GILNEAN_MILITIA, x, y, z, o))
                            SecondWave.push_back(new_militia->GetGUID());
                    }

                    for (int j = 6; j < 9; ++j)
                    {
                        militia->GetNearPoint2D(x, y, j * 2, o + M_PI);

                        if (Creature* new_militia = me->SummonCreature(NPC_GILNEAN_MILITIA, x, y, z, o))
                            if (npc_escortAI* npc = CAST_AI(npc_escortAI, new_militia->AI()))
                                ThirdWave.push_back(new_militia->GetGUID());
                    }
                }
            }
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 30:
                SetEscortPaused(true);
                SetFacingToLiam();
                if (uiBattlePhase == PHASE_MERCHANT_SQUARE)
                    if (uiDeadBossCount <= 0)
                        Event = true;
                    else
                        EscortFinished = true;
                break;
            case 44:
                DoScriptText(LIAM_BATTLE_BATTLE_3, me);
                SummonCrowley();
                break;
            case 58:
                Event = true;
                break;
            case 59:
                SetEscortPaused(true);
                break;
            case 61:
                if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                    DoScriptText(CROWLEY_BATTLE_BATTLE_2, crowley);
                break;
            case 80:
                Event = true;
                SummonKingCreaymane();
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!Finish && Fail)
                if (uiFailTimer <= diff)
                {
                    Fail = false;
                    uiFailTimer = 900000;
                    DespawnEvent(0);
                } else
                    uiFailTimer -= diff;

                if (EscortFinished)
                    if (uiDeadBossCount <= 0)
                    {
                        uiSubPhase = 0;
                        uiEventTimer = 0;
                        EscortFinished = false;
                        Event = true;
                    }

                    if (Event)
                        if (uiEventTimer <= diff)
                            switch (uiBattlePhase)
                        {
                            case PHASE_INTRO:
                                ++uiSubPhase;
                                switch (uiSubPhase)
                                {
                                case 1:
                                    uiEventTimer = 7500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_1, me);
                                    break;
                                case 2:
                                    uiEventTimer = 8500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_2, me);
                                    break;
                                case 3:
                                    uiEventTimer = 10500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_3, me);
                                    break;
                                case 4:
                                    uiEventTimer = 8500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_4, me);
                                    break;
                                case 5:
                                    uiEventTimer = 10500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_5, me);
                                    break;
                                case 6:
                                    uiEventTimer = 4500;
                                    DoScriptText(LIAM_BATTLE_SPEACH_6, me);
                                    break;
                                case 7:
                                    Event = false;
                                    StartBattle();
                                    uiSubPhase = 0;
                                    uiBattlePhase = PHASE_MERCHANT_SQUARE;
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                                    DoScriptText(LIAM_BATTLE_FOR_GILNEAS, me);
                                    break;
                                }
                                break;
                            case PHASE_MERCHANT_SQUARE:
                                ++uiSubPhase;
                                switch (uiSubPhase)
                                {
                                case 1:
                                    uiEventTimer = 6500;
                                    SummonLorna();
                                    DoScriptText(LIAM_BATTLE_BATTLE_1, me);
                                    break;
                                case 2:
                                    uiSubPhase = 0;
                                    Event = false;
                                    uiEventTimer = 7000;
                                    BreakEscortPaused();
                                    uiBattlePhase = PHASE_MILITARY_DISTRICT;
                                    break;
                                }
                                break;
                            case PHASE_MILITARY_DISTRICT:
                                ++uiSubPhase;
                                switch (uiSubPhase)
                                {
                                case 1:
                                    uiEventTimer = 8000;
                                    DoScriptText(LIAM_BATTLE_BATTLE_2, me);
                                    break;
                                case 2:
                                    Event = false;
                                    uiEventTimer = 7000;
                                    if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                                        DoScriptText(LORNA_BATTLE_BATTLE_2, lorna);
                                    break;
                                case 3:
                                    uiEventTimer = 25000;
                                    me->SummonCreature(NPC_GOREROT, -1671.78f, 1449.64f, 52.287f);
                                    if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                                        crowley->AI()->DoAction(ACTION_GOREROT_FIGHT);
                                    break;
                                case 4:
                                    Event = false;
                                    uiSubPhase = 0;
                                    uiEventTimer = 10000;
                                    if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                                        DoScriptText(CROWLEY_BATTLE_BATTLE_1, crowley);
                                    break;
                                }
                                break;
                            case PHASE_GREYMANE_COURT:
                                ++uiSubPhase;
                                switch (uiSubPhase)
                                {
                                case 1:
                                    uiEventTimer = 21000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                        DoScriptText(GREYMANE_BATTLE_BATTLE_1, greymane);
                                    break;
                                case 2:
                                    Event = false;
                                    uiSubPhase = 0;
                                    uiEventTimer = 2000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                        greymane->AI()->DoAction(ACTION_CAN_ATTACK);
                                    break;
                                }
                                break;
                        } else
                            uiEventTimer -= diff;

                        if (Finish)
                        {
                            if (uiEventTimer <= diff)
                            {
                                ++uiSubPhase;
                                switch (uiSubPhase)
                                {
                                case 1:
                                    uiEventTimer = 3000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        DoScriptText(SYLVANAS_KILL_LIAM, sylvanas);
                                    Aura = false;
                                    me->Dismount();
                                    me->RemoveAllAuras();
                                    break;
                                case 2:
                                    uiEventTimer = 4000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                    {
                                        SetRun(true);
                                        DoScriptText(LIAM_BATTLE_DEATH_1, me);
                                        float x, y, z = greymane->GetPositionZ();
                                        greymane->GetNearPoint2D(x, y, 0.5f, greymane->GetOrientation());
                                        if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                            DoStartMovement(sylvanas, 1.0f);
                                    }
                                    break;
                                case 3:
                                    uiEventTimer = 3000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                    {
                                        sylvanas->CastSpell(me, SPELL_SHOOT_LIAM, false);
                                        DoCast(SPELL_BFGC_COMPLETE);
                                        if (me->isSummon())
                                            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                                if (Creature* krennan = summoner->ToCreature())
                                                    krennan->AI()->DoAction(ACTION_BATTLE_COMPLETE);
                                        sylvanas->GetMotionMaster()->MovePoint(0, -1628.85f, 1626.61f, 20.4884f);
                                        sylvanas->DespawnOrUnsummon(2000);
                                        if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                            DoScriptText(GREYMANE_YELL_LIAM, greymane);
                                    }
                                    break;
                                case 4:
                                    uiEventTimer = 5000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                        greymane->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    SetFacingToLiam();
                                    break;
                                case 5:
                                    uiEventTimer = 3000;
                                    DoScriptText(LIAM_BATTLE_DEATH_2, me);
                                    break;
                                case 6:
                                    DespawnEvent(30000);
                                    Finish = false;
                                    DoScriptText(LIAM_BATTLE_DEATH_3, me);
                                    break;
                                }
                            } else
                                uiEventTimer -= diff;
                        }

                        if (!UpdateVictim())
                            return;

                        if (uiRandomSpechTimer <= diff)
                        {
                            uiRandomSpechTimer = urand(10000, 30000);
                            uint8 roll = urand(0, 3);
                            DoScriptText(LIAM_RANDOM_BATTLE_SPEACH - roll, me);
                        } else
                            uiRandomSpechTimer -= diff;

                        if (uiShootTimer <= diff)
                        {
                            uiShootTimer = 1500;
                            DoCast(me->getVictim(), SPELL_SHOOT);
                        } else
                            uiShootTimer -= diff;

                        if (uiMultiShotTimer <= diff)
                        {
                            uiMultiShotTimer = 3000;
                            DoCast(me->getVictim(), SPELL_MULTI_SHOT);
                        } else
                            uiMultiShotTimer -= diff;

                        if (uiChallengingShoutTimer <= diff)
                        {
                            uiChallengingShoutTimer = urand(10000, 30000);
                            DoCast(SPELL_CHALLENGING_SHOUT);
                        } else
                            uiChallengingShoutTimer -= diff;
        }
    };
};

class npc_gilnean_militia_tbfgc : public CreatureScript
{
public:
    npc_gilnean_militia_tbfgc() : CreatureScript("npc_gilnean_militia_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilnean_militia_tbfgcAI(creature);
    }

    struct npc_gilnean_militia_tbfgcAI : public npc_escortAI
    {
        npc_gilnean_militia_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetCombatMovement(false);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiShotTimer = 2000;
            uiIWingClipTimer = urand(30000, 60000);
            uiSunderArmorTimer = 12000;
            uiPausePoint = 100500;
        }

        uint32 uiShotTimer;
        uint32 uiIWingClipTimer;
        uint32 uiSunderArmorTimer;
        uint32 uiPausePoint;

        void SetData(uint32 data, uint32 value)
        {
            if (data == DATA_WP_PAUSE)
                uiPausePoint = value;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == uiPausePoint)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                        {
                            if (point == 17)
                            {
                                if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MERCHANT_SQUARE)
                                    SetEscortPaused(true);

                                uiPausePoint = 46;
                            }

                            if (point == 30)
                            {
                                if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MERCHANT_SQUARE)
                                    SetEscortPaused(true);

                                uiPausePoint = 59;
                            }

                            if (point == 46 || point == 59)
                                if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                    SetEscortPaused(true);
                        }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiShotTimer <= diff)
            {
                uiShotTimer = 2000;

                if (me->getVictim() && me->IsInRange(me->getVictim(), 2.0f, 25.0f, false))
                    DoCast(me->getVictim(), SPELL_SHOOT_TBFGC);
            } else
                uiShotTimer -= diff;

            if (uiIWingClipTimer <= diff)
            {
                uiIWingClipTimer = urand(30000, 60000);
                DoCast(me->getVictim(), SPELL_IMPROVED_WING_CLIP);
            } else
                uiIWingClipTimer -= diff;

            if (uiSunderArmorTimer <= diff)
            {
                uiSunderArmorTimer = 12000;
                DoCast(me->getVictim(), SPELL_SUNDER_ARMOR);
            } else
                uiSunderArmorTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_myriam_spellwaker_tbfgc : public CreatureScript
{
public:
    npc_myriam_spellwaker_tbfgc() : CreatureScript("npc_myriam_spellwaker_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_myriam_spellwaker_tbfgcAI(creature);
    }

    struct npc_myriam_spellwaker_tbfgcAI : public npc_escortAI
    {
        npc_myriam_spellwaker_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetCombatMovement(false);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiBlizzardTimer = urand(10000, 25000);
            uiFireBlastTimer = urand(5000, 15000);
            uiFrostboltTimer = urand(5000, 15000);
        }

        uint32 uiBlizzardTimer;
        uint32 uiFireBlastTimer;
        uint32 uiFrostboltTimer;

        void EnterEvadeMode()
        {
            npc_escortAI::EnterEvadeMode();
            me->AddAura(SPELL_ARCANE_INTELLECT, me);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (me->GetHealthPct() < 10)
                damage = 0;
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 30:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MERCHANT_SQUARE)
                                SetEscortPaused(true);
                break;
            case 59:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                SetEscortPaused(true);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiFireBlastTimer <= diff)
            {
                uiFireBlastTimer = urand(5000, 15000);
                DoCast(me->getVictim(), SPELL_FIRE_BLAST);
            } else
                uiFireBlastTimer -= diff;

            if (uiBlizzardTimer <= diff)
            {
                uiBlizzardTimer = urand(10000, 25000);
                DoCast(SPELL_BLIZZARD);
            } else
                uiBlizzardTimer -= diff;

            if (uiFrostboltTimer <= diff)
            {
                uiFrostboltTimer = urand(5000, 15000);
                DoCast(me->getVictim(), SPELL_FROSTBOLT);
            } else
                uiFrostboltTimer -= diff;
        }
    };
};

class npc_sister_almyra_tbfgc : public CreatureScript
{
public:
    npc_sister_almyra_tbfgc() : CreatureScript("npc_sister_almyra_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sister_almyra_tbfgcAI(creature);
    }

    struct npc_sister_almyra_tbfgcAI : public npc_escortAI
    {
        npc_sister_almyra_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetCombatMovement(false);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiFlashHealTimer = urand(1500, 3000);
            uiHolyNovaTimer = urand(3000, 5000);
        }

        uint32 uiFlashHealTimer;
        uint32 uiHolyNovaTimer;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void EnterEvadeMode()
        {
            npc_escortAI::EnterEvadeMode();
            me->AddAura(SPELL_ARCANE_INTELLECT, me);
        }

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (me->GetHealthPct() < 10)
                damage = 0;
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 9:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        if (Creature* ranger = summoner->SummonCreature(NPC_DARK_RANGER_ELITE, -1572.91f, 1319.44f, 35.556f))
                            if (npc_escortAI* escort = CAST_AI(npc_escortAI, ranger->AI()))
                            {
                                for (int i = 0; i < 31; ++i)
                                    escort->AddWaypoint(i, FDarkRangerWP[i][0], FDarkRangerWP[i][1], FDarkRangerWP[i][2]);
                                escort->Start(true, true, 0, NULL, false, true);
                            }

                            if (Creature* ranger = summoner->SummonCreature(NPC_DARK_RANGER_ELITE, -1570.32f, 1318.69f, 35.556f))
                                if (npc_escortAI* escort = CAST_AI(npc_escortAI, ranger->AI()))
                                {
                                    for (int i = 0; i < 30; ++i)
                                        escort->AddWaypoint(i, SDarkRangerWP[i][0], SDarkRangerWP[i][1], SDarkRangerWP[i][2]);
                                    escort->Start(true, true, 0, NULL, false, true);
                                }
                    }
                    break;
            case 17:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MERCHANT_SQUARE)
                                SetEscortPaused(true);
                break;
            case 46:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                SetEscortPaused(true);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiHolyNovaTimer <= diff)
            {
                uiHolyNovaTimer = urand(3000, 5000);
                DoCast(SPELL_HOLY_NOVA);
            } else
                uiHolyNovaTimer -= diff;

            if (uiFlashHealTimer <= diff)
            {
                uiFlashHealTimer = urand(1500, 3000);

                if (me->GetHealthPct() < 30.0f)
                    DoCast(SPELL_HOLY_NOVA);
                else
                    if (Unit* target = DoSelectLowestHpFriendly(35.0f, 1))
                        DoCast(target, SPELL_HOLY_NOVA);
            } else
                uiFlashHealTimer -= diff;
        }
    };
};

class npc_dark_ranger_elite_tbfgc : public CreatureScript
{
public:
    npc_dark_ranger_elite_tbfgc() : CreatureScript("npc_dark_ranger_elite_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_ranger_elite_tbfgcAI(creature);
    }

    struct npc_dark_ranger_elite_tbfgcAI : public npc_escortAI
    {
        npc_dark_ranger_elite_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiKnockbackTimer = urand(5000, 15000);
            uiShootTimer = 1500;
        }

        uint32 uiKnockbackTimer;
        uint32 uiShootTimer;

        void WaypointReached(uint32 point)
        { }

        void JustDied(Unit* /*killer*/)
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Creature* liam = summoner->ToCreature())
                        CAST_AI(npc_prince_liam_greymane_tbfgc::npc_prince_liam_greymane_tbfgcAI, liam->AI())->KilledBoss();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiKnockbackTimer <= diff)
            {
                uiKnockbackTimer = urand(5000, 15000);
                DoCast(SPELL_KNOCKBACK);
            } else
                uiKnockbackTimer -= diff;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;
                DoCast(me->getVictim(), SPELL_SHOOT);
            } else
                uiShootTimer -= diff;
        }
    };
};

class npc_emberstone_cannon : public CreatureScript
{
public:
    npc_emberstone_cannon() : CreatureScript("npc_emberstone_cannon") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emberstone_cannonAI(creature);
    }

    struct npc_emberstone_cannonAI : public npc_escortAI
    {
        npc_emberstone_cannonAI(Creature* creature) : npc_escortAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SOLDIER_OF_TBFGC, true);
            me->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
        }

        void WaypointReached(uint32 point)
        {
            if (point == 10)
                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Creature* villager = passenger->ToCreature())
                        {
                            villager->ExitVehicle();
                            villager->DespawnOrUnsummon(25000);
                        }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_lorna_crowley_tbfgc : public CreatureScript
{
public:
    npc_lorna_crowley_tbfgc() : CreatureScript("npc_lorna_crowley_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lorna_crowley_tbfgcAI(creature);
    }

    struct npc_lorna_crowley_tbfgcAI : public npc_escortAI
    {
        npc_lorna_crowley_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetCombatMovement(false);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiShootTimer = 1500;
        }

        uint32 uiShootTimer;

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (me->GetHealthPct() < 10)
                damage = 0;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 8:
                DoScriptText(LORNA_BATTLE_BATTLE_1, me);
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            liam->AI()->DoAction(ACTION_CANNON_AT_POSITION);
                break;
            case 41:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                SetEscortPaused(true);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;
                DoCast(me->getVictim(), SPELL_SHOOT);
            } else
                uiShootTimer -= diff;
        }
    };
};

class npc_lord_darius_crowley_tbfgc : public CreatureScript
{
public:
    npc_lord_darius_crowley_tbfgc() : CreatureScript("npc_lord_darius_crowley_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_darius_crowley_tbfgcAI(creature);
    }

    struct npc_lord_darius_crowley_tbfgcAI : public npc_escortAI
    {
        npc_lord_darius_crowley_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiSliceAndDiceTimer = 30000;
            uiTauntTimer = 5000;
            uiJumpTimer = 8000;
            uiFeralLeapTimer = 1500;
            uiInterceptTimer = 21000;
            gorerot = false;
            jump = false;
        }

        uint32 uiSliceAndDiceTimer;
        uint32 uiTauntTimer;
        uint32 uiFeralLeapTimer;
        uint32 uiInterceptTimer;
        uint32 uiJumpTimer;
        bool gorerot;
        bool jump;

        void DoAction(const int32 action)
        {
            if (action == ACTION_GOREROT_FIGHT)
                gorerot = true;

            if (action == ACTION_GOREROT_DIED)
            {
                gorerot = false;
                me->GetMotionMaster()->MoveJump(-1715.62f, 1403.78f, 21.6688f, 10.0f, 10.0f);
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 10)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                SetEscortPaused(true);
        }

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (me->GetHealthPct() < 10)
                damage = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (gorerot)
                if (uiJumpTimer <= diff)
                {
                    jump = true;
                    uiJumpTimer = 8000;
                    me->AddUnitState(UNIT_STATE_ROOT);
                    me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                    me->GetMotionMaster()->MoveJump(-1690.97f, 1430.47f, 40.9132f, 10.0f, 15.0f);
                } else
                    uiJumpTimer -= diff;

                if (jump)
                    if (uiFeralLeapTimer <= diff)
                    {
                        jump = false;
                        uiFeralLeapTimer = 1500;
                        float x, y, z;

                        if (Unit* target = me->getVictim())
                        {
                            target->GetPosition(x, y, z);
                            me->CastSpell(x, y, z, SPELL_FERAL_LEAP, false);
                        }

                        me->ClearUnitState(UNIT_STATE_ROOT);
                        me->RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
                    } else
                        uiFeralLeapTimer -= diff;

                    if (!jump)
                    {
                        if (uiSliceAndDiceTimer <= diff)
                        {
                            uiSliceAndDiceTimer = 30000;
                            DoCast(SPELL_SLICE_AND_DICE);
                        } else
                            uiSliceAndDiceTimer -= diff;

                        if (uiTauntTimer <= diff)
                        {
                            uiTauntTimer = 10000;
                            DoCast(me->getVictim(), SPELL_TAUNT);
                        } else
                            uiTauntTimer -= diff;

                        if (uiInterceptTimer <= diff)
                        {
                            uiInterceptTimer = 11000;

                            if (Unit* target = me->getVictim())
                            {
                                if (me->getVictim() && me->IsInRange(me->getVictim(), 8.0f, 25.0f, false))
                                    DoCast(me->getVictim(), SPELL_INTERCEPT);
                            }
                        } else
                            uiInterceptTimer -= diff;
                    }
        }
    };
};

class worgen_warrior_tbfgc : public CreatureScript
{
public:
    worgen_warrior_tbfgc() : CreatureScript("worgen_warrior_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new worgen_warrior_tbfgcAI(creature);
    }

    struct worgen_warrior_tbfgcAI : public npc_escortAI
    {
        worgen_warrior_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SMASH)
            {
                float x, y, z;
                caster->GetPosition(x, y, z);
                me->GetMotionMaster()->MoveJump(x, y, z, 10.0f, 10.0f);
            }

            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 10)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            if (liam->AI()->GetData(DATA_CURRENT_PHASE) == PHASE_MILITARY_DISTRICT)
                                SetEscortPaused(true);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_gorerot : public CreatureScript
{
public:
    npc_gorerot() : CreatureScript("npc_gorerot") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gorerotAI(creature);
    }

    struct npc_gorerotAI : public npc_escortAI
    {
        npc_gorerotAI(Creature* creature) : npc_escortAI(creature)
        {
            me->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiThunderclapTimer = urand(5000, 15000);
            uiSmashTimer = urand(5000, 10000);
            uiGorerotThreatTimer= 3000;
            FreePlace = true;
            StartEscort();
        }

        void StartEscort()
        {
            AddWaypoint(0, -1671.78f, 1449.64f, 52.287f, 0, true);
            AddWaypoint(1, -1710.47f, 1407.91f, 21.752f);
            SetSpeedZ(27.0f);
            Start(true, true);
        }

        uint32 uiThunderclapTimer;
        uint32 uiSmashTimer;
        uint32 uiGorerotThreatTimer;
        bool FreePlace;

        void WaypointReached(uint32 point)
        {
            if (point == 1)
            {
                DoScriptText(GOREROT_YELL_CRUSH, me);

                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        AttackStart(summoner);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Creature* liam = summoner->ToTempSummon())
                        liam->AI()->DoAction(ACTION_GOREROT_DIED);
        }

        Creature* GetFreeWorgen(std::list<Creature*> List) const
        {
            for (std::list<Creature*>::const_iterator itr = List.begin(); itr != List.end();)
            {
                if (!(*itr)->IsOnVehicle(me))
                    return (*itr);

                ++itr;
            }
            return NULL;
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (FreePlace)
            {
                FreePlace = false;
                std::list<Creature*> WorgenList;
                me->GetCreatureListWithEntryInGrid(WorgenList, NPC_WORGEN_WARRIOR, 30.0f);

                if (Vehicle* vehicle = me->GetVehicleKit())
                {
                    for (int i = 0; i < 2; ++i)
                        if (vehicle->HasEmptySeat(i))
                            if (Creature* worgen = GetFreeWorgen(WorgenList))
                            {
                                worgen->EnterVehicle(me, i);
                                worgen->m_movementInfo.t_pos.m_orientation = M_PI;
                            }

                            for (int i = 2; i < 4; ++i)
                                if (vehicle->HasEmptySeat(i))
                                    if (Creature* worgen = GetFreeWorgen(WorgenList))
                                        worgen->EnterVehicle(me, i);
                }
            }

            if (uiThunderclapTimer <= diff)
            {
                uiThunderclapTimer = urand(5000, 15000);

                if (Vehicle* vehicle = me->GetVehicleKit())
                    vehicle->RemoveAllPassengers();

                FreePlace = true;
                DoCast(SPELL_THUNDERCLAP);
            } else
                uiThunderclapTimer -= diff;

            if (uiSmashTimer <= diff)
            {
                uiSmashTimer = urand(5000, 10000);
                uint8 roll = urand(0, 4);

                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(roll))
                    {
                        passenger->ExitVehicle();
                        DoCast(passenger, SPELL_SMASH);
                        FreePlace = true;
                    }
            } else
                uiSmashTimer -= diff;

            if (uiGorerotThreatTimer <= diff)
            {
                uiGorerotThreatTimer = 3000;
                DoCast(SPELL_GOREROT_THREAT);
            } else
                uiGorerotThreatTimer -= diff;
        }
    };
};

class npc_king_genn_greymane_tbfgc : public CreatureScript
{
public:
    npc_king_genn_greymane_tbfgc() : CreatureScript("npc_king_genn_greymane_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_genn_greymane_tbfgcAI(creature);
    }

    struct npc_king_genn_greymane_tbfgcAI : public npc_escortAI
    {
        npc_king_genn_greymane_tbfgcAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->setActive(true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            uiSylvanasTimer = 0;
            uiPhase = 0;
            sylvanas = false;
            can_attack = false;
        }

        void DamageTaken(Unit* /*damager*/, uint32 &damage)
        {
            if (me->GetHealthPct() < 10)
                damage = 0;
        }

        uint32 uiSylvanasTimer;
        uint8 uiPhase;
        bool sylvanas;
        bool can_attack;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BANSHEE_QUEENS_WAIL)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_ATTACK_SYLVANAS)
                sylvanas = true;

            if (action == ACTION_CAN_ATTACK)
                can_attack = true;
        }

        void WaypointReached(uint32 point)
        { }

        void UpdateAI(const uint32 diff)
        {
            if (sylvanas && can_attack)
                if (uiSylvanasTimer <= diff)
                {
                    if (Creature* Sylvanas = me->FindNearestCreature(NPC_LADY_SYLVANAS_WINDRUNNER_TBFGC, 50.0f))
                    {
                        ++uiPhase;
                        switch (uiPhase)
                        {
                        case 1:
                            uiSylvanasTimer = 1000;
                            DoCast(SPELL_GREYMANE_TRANSFORM);
                            Sylvanas->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            AttackStart(Sylvanas);
                            break;
                        case 2:
                            sylvanas = false;
                            can_attack = false;
                            uiSylvanasTimer = 7000;
                            DoScriptText(GREYMANE_BATTLE_BATTLE_2, me);
                            float x, y, z = Sylvanas->GetPositionZ();
                            Sylvanas->GetNearPoint2D(x, y, 3.0f, Sylvanas->GetOrientation());
                            me->GetMotionMaster()->MoveJump(x, y, z, 10.0f, 10.0f);
                            break;
                        }
                    }
                } else
                    uiSylvanasTimer -= diff;

                npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_lady_sylvanas_windrunner_tbfgc : public CreatureScript
{
public:
    npc_lady_sylvanas_windrunner_tbfgc() : CreatureScript("npc_lady_sylvanas_windrunner_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_sylvanas_windrunner_tbfgcAI(creature);
    }

    struct npc_lady_sylvanas_windrunner_tbfgcAI : public ScriptedAI
    {
        npc_lady_sylvanas_windrunner_tbfgcAI(Creature* creature) : ScriptedAI(creature)
        {
            me->AddUnitState(UNIT_STATE_ROOT);
            me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            uiShootTimer = 1500;
            move = false;
            health = false;
            finish = false;
        }

        uint32 uiShootTimer;
        bool move;
        bool health;
        bool finish;

        void MoveInLineOfSight(Unit* who)
        {
            if (!move)
                if (who->GetEntry() == NPC_KING_GENN_GREYMANE_TBFGC && me->IsWithinDistInMap(who, 25.0f))
                {
                    move = true;
                    who->ToCreature()->AI()->DoAction(ACTION_ATTACK_SYLVANAS);
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!health && me->GetHealthPct() < 30)
            {
                health = true;
                DoScriptText(SYLVANAS_YELL_ENOUGH, me);
                DoCast(SPELL_BANSHEE_QUEENS_WAIL);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop();

                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* liam = summoner->ToCreature())
                            liam->AI()->DoAction(ACTION_FINISH_TBFGC);
            }

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;
                DoCast(SPELL_SHOOT);
            } else
                uiShootTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_damaged_catapult_tbfgc : public CreatureScript
{
public:
    npc_damaged_catapult_tbfgc() : CreatureScript("npc_damaged_catapult_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_damaged_catapult_tbfgcAI(creature);
    }

    struct npc_damaged_catapult_tbfgcAI : public ScriptedAI
    {
        npc_damaged_catapult_tbfgcAI(Creature* creature) : ScriptedAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SOLDIER_OF_TBFGC, true);
        }

        void PassengerBoarded(Unit* /*who*/, int8 /*seatId*/, bool apply)
        {
            if (apply)
                me->AddAura(SPELL_BURNING, me);
            else
                me->RemoveAura(SPELL_BURNING);
        }

        void OnCharmed(bool /*charm*/)
        {
            me->EnableAI();
        }

        void UpdateAI(const uint32 diff)
        { }
    };
};

class npc_lorna_crowley : public CreatureScript
{
public:
    npc_lorna_crowley() : CreatureScript("npc_lorna_crowley") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_BATTLE_FOR_GILNEAS_CITY)
        {
            player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_12, false);
            if (Creature* aranas = creature->FindNearestCreature(NPC_KRENNAN_ARANAS_TBFGC, 35.0f))
                aranas->DoPersonalScriptText(TIME_TO_START_BATTLE, player);

            player->SaveToDB();
        }

        return true;
    }
};

class npc_krennan_aranas_tbfgc : public CreatureScript
{
public:
    npc_krennan_aranas_tbfgc() : CreatureScript("npc_krennan_aranas_tbfgc") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_krennan_aranas_tbfgcAI(creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (npc_krennan_aranas_tbfgcAI* aranas = CAST_AI(npc_krennan_aranas_tbfgcAI, creature->AI()))
                aranas->TelepotPlayer(player);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            CAST_AI(npc_krennan_aranas_tbfgcAI, creature->AI())->StartEvent();
            break;
        }

        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        /*
        if (player->GetPhaseMask() != 262144)
        return false;
        */

        if (player->GetQuestStatus(QUEST_THE_BATTLE_FOR_GILNEAS_CITY) == QUEST_STATUS_INCOMPLETE)
            if (npc_krennan_aranas_tbfgcAI* AranasAI = CAST_AI(npc_krennan_aranas_tbfgcAI, creature->AI()))
            {

                if (AranasAI->IsBattle())
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GossipMenu[27], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[28], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                } else
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GossipMenu[29], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[30], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                }
            }

            player->SEND_GOSSIP_MENU(MENU_START_BATTLE, creature->GetGUID());
            return true;
    }

    struct npc_krennan_aranas_tbfgcAI : public ScriptedAI
    {
        npc_krennan_aranas_tbfgcAI(Creature* creature) : ScriptedAI(creature)
        {
            Battle = false;
            creature->setActive(true);

            if (Creature* liam = me->SummonCreature(NPC_PRINCE_LIAM_GREYMANE, -1412.34f, 1274.66f, 36.4277f, 1.80124f))
                uiLiamGUID = liam->GetGUID();
            else
                uiLiamGUID = 0;
        }

        uint64 uiLiamGUID;
        bool Battle;

        void DoAction(const int32 action)
        {
            if (action == ACTION_BATTLE_COMPLETE)
            {
                Battle = false;
                uiLiamGUID = 0;

                if (Creature* liam = me->SummonCreature(NPC_PRINCE_LIAM_GREYMANE, -1412.34f, 1274.66f, 36.4277f, 1.80124f))
                    uiLiamGUID = liam->GetGUID();
            }
        }

        bool IsBattle()
        {
            return Battle;
        }

        void TelepotPlayer(Player* player)
        {
            if (Creature* liam = Unit::GetCreature(*me, uiLiamGUID))
            {
                float x, y, z, o;
                liam->GetPosition(x, y, z, o);
                player->TeleportTo(liam->GetMapId(), x, y, z, o);
            }
        }

        void StartEvent()
        {
            Battle = true;

            if (Creature* liam = Unit::GetCreature(*me, uiLiamGUID))
                CAST_AI(npc_prince_liam_greymane_tbfgc::npc_prince_liam_greymane_tbfgcAI, liam->AI())->StartEvent();
        }
    };
};

///////////
// Quest The Hunt For Sylvanas 24902
///////////

class npc_lorna_crowley_qthfs : public CreatureScript
{
public:
    npc_lorna_crowley_qthfs() : CreatureScript("npc_lorna_crowley_qthfs") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_HUNT_FOR_SYLVANAS)
        {
            CAST_AI(npc_lorna_crowley_qthfsAI, creature->AI())->SummonPersonalTobias(player->GetGUID());
            player->SaveToDB();
        }

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_HUNT_FOR_SYLVANAS)
            CAST_AI(npc_lorna_crowley_qthfsAI, creature->AI())->DespawnPersonalTobias(player->GetGUID());

        if (quest->GetQuestId() == QUEST_THE_BATTLE_FOR_GILNEAS_CITY)
        {
            player->SaveToDB();
            return false;
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lorna_crowley_qthfsAI(creature);
    }

    struct npc_lorna_crowley_qthfsAI : public ScriptedAI
    {
        npc_lorna_crowley_qthfsAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayers.clear();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        std::list<Psc_qthfs> lPlayers;

        void SummonPersonalTobias(uint64 player)
        {
            for (std::list<Psc_qthfs>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
            {
                if ((*itr).uiPlayerGUID == player)
                    if (Unit::GetCreature(*me, (*itr).uiTobiasGUID))
                        return;
                    else
                        itr = lPlayers.erase(itr);
                else
                    ++itr;
            }

            if (Creature* tobias = me->SummonCreature(NPC_TOBIAS_MISTMANTLE_QTHFS, -1655.85f, 1605.59f, 23.1241f))
            {
                tobias->SetSeerGUID(player);
                tobias->SetVisible(false);
                Psc_qthfs new_psc;
                new_psc.uiPlayerGUID = player;
                new_psc.uiTobiasGUID = tobias->GetGUID();
                lPlayers.push_back(new_psc);

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, tobias->AI()))
                {
                    escort->AddWaypoint(1, -1655.85f, 1605.59f, 23.1241f);
                    escort->AddWaypoint(2, -1662.65f, 1588.30f, 23.1234f);
                    escort->AddWaypoint(3, -1656.50f, 1582.69f, 20.4859f);
                    escort->AddWaypoint(4, -1651.79f, 1578.62f, 21.6913f);
                    escort->AddWaypoint(5, -1626.70f, 1601.46f, 21.8459f, 3000, true);
                    escort->AddWaypoint(6, -1599.64f, 1607.60f, 21.5590f, 3000);
                    escort->AddWaypoint(7, -1587.31f, 1607.03f, 21.5981f, 0, true);
                    escort->AddWaypoint(8, -1582.17f, 1626.81f, 20.5150f);
                    escort->AddWaypoint(9, -1561.72f, 1629.94f, 20.7313f, 0, true);
                    escort->AddWaypoint(10, -1539.67f, 1611.00f, 20.6071f);
                    escort->AddWaypoint(11, -1521.61f, 1606.76f, 20.4858f);
                    escort->AddWaypoint(12, -1493.69f, 1574.78f, 20.4858f);
                    escort->AddWaypoint(13, -1491.67f, 1576.35f, 20.4856f, 10000);
                    escort->AddWaypoint(14, -1504.72f, 1577.82f, 20.4858f);
                    escort->AddWaypoint(15, -1516.82f, 1578.70f, 26.1162f);
                    escort->AddWaypoint(16, -1532.91f, 1579.08f, 26.9852f);
                    escort->AddWaypoint(17, -1539.01f, 1575.19f, 29.1926f);
                    escort->AddWaypoint(18, -1550.37f, 1566.61f, 29.2215f);
                    escort->AddWaypoint(19, -1555.80f, 1570.43f, 29.1907f);
                    escort->AddWaypoint(20, -1567.43f, 1559.57f, 29.1914f);
                    escort->AddWaypoint(21, -1565.06f, 1553.10f, 29.2230f);
                    escort->AddWaypoint(22, -1568.97f, 1548.16f, 29.2020f);
                    escort->AddWaypoint(23, -1593.16f, 1525.53f, 29.2358f);
                    escort->AddWaypoint(24, -1609.31f, 1531.94f, 29.2298f, 0, true);
                    escort->AddWaypoint(25, -1617.92f, 1532.31f, 23.6932f);
                    escort->AddWaypoint(26, -1613.88f, 1536.84f, 24.7464f);
                    escort->AddWaypoint(27, -1613.19f, 1536.32f, 27.3053f);
                    escort->SetSpeedZ(15.0f);
                    escort->Start(false, true, player);
                }
            }
        }

        void DespawnPersonalTobias(uint64 player)
        {
            for (std::list<Psc_qthfs>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                if ((*itr).uiPlayerGUID == player)
                    itr = lPlayers.erase(itr);
                else
                    ++itr;
        }
    };
};

class npc_tobias_mistmantle_qthfs : public CreatureScript
{
public:
    npc_tobias_mistmantle_qthfs() : CreatureScript("npc_tobias_mistmantle_qthfs") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tobias_mistmantle_qthfsAI(creature);
    }

    struct npc_tobias_mistmantle_qthfsAI : public npc_escortAI
    {
        npc_tobias_mistmantle_qthfsAI(Creature* creature) : npc_escortAI(creature)
        {
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            uiSylvanasGUID = 0;
            uiWarhowlGUID = 0;
            uiCrenshawGUID = 0;
            uiEventTimer = 0;
            uiFailTimer = 600000;
            uiPhase = 0;
            Event = false;
            CheckSwim = false;
            Ide = false;
        }

        uint64 uiSylvanasGUID;
        uint64 uiWarhowlGUID;
        uint64 uiCrenshawGUID;
        uint32 uiEventTimer;
        uint32 uiFailTimer;
        uint8 uiPhase;
        bool Event;
        bool CheckSwim;
        bool Ide;

        void WaypointReached(uint32 point)
        {
            if (Player* player = GetPlayerForEscort())
                switch (point)
            {
                case 1:
                    me->DoPersonalScriptText(TOBIAS_SAY_GO, player);
                    Event = true;
                    uiEventTimer = 2000;
                    uiPhase = 0;
                    break;
                case 5:
                case 13:
                    Event = true;
                    break;
                case 24:
                    me->DoPersonalScriptText(TOBIAS_SAY_HIDE, player);
                    break;
                case 27:
                    Event = true;
                    break;
            }
        }

        void SummonSilvanas(uint64 player)
        {
            if (Creature* sylvanas = me->SummonCreature(NPC_LADY_SYLVANAS_QTHFS, -1563.58f, 1554.83f, 31.2228f))
            {
                sylvanas->SetSeerGUID(player);
                sylvanas->SetVisible(false);
                uiSylvanasGUID = sylvanas->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, sylvanas->AI()))
                    escort->Start();
            }

            if (Creature* warhowl = me->SummonCreature(NPC_GENERAL_WARHOWL_QTHFS, -1560.37f, 1555.82f, 31.2305f))
            {
                warhowl->SetSeerGUID(player);
                warhowl->SetVisible(false);
                uiWarhowlGUID = warhowl->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, warhowl->AI()))
                    escort->Start();
            }

            if (Creature* crenshaw = me->SummonCreature(NPC_HIGH_EXECUTOR_CRENSHAW_QTHFS, -1561.6f, 1554.87f, 31.225f))
            {
                crenshaw->SetSeerGUID(player);
                crenshaw->SetVisible(false);
                uiCrenshawGUID = crenshaw->GetGUID();

                if (npc_escortAI* escort = CAST_AI(npc_escortAI, crenshaw->AI()))
                    escort->Start();
            }
        }

        void DespawnEvent()
        {
            if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                sylvanas->DespawnOrUnsummon(5000);

            if (Creature* warhowl = Unit::GetCreature(*me, uiWarhowlGUID))
                warhowl->DespawnOrUnsummon(5000);

            if (Creature* crenshaw = Unit::GetCreature(*me, uiCrenshawGUID))
                crenshaw->DespawnOrUnsummon(5000);

            me->DespawnOrUnsummon(5000);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (uiFailTimer <= diff)
            {
                if (Player* player = GetPlayerForEscort())
                    if (player->GetQuestStatus(QUEST_THE_HUNT_FOR_SYLVANAS) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(QUEST_THE_HUNT_FOR_SYLVANAS);

                DespawnEvent();
            } else
                uiFailTimer -= diff;

            if (CheckSwim)
                if (Player* player = GetPlayerForEscort())
                    if (!player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
                    {
                        CheckSwim = false;

                        if (player->GetQuestStatus(QUEST_THE_HUNT_FOR_SYLVANAS) == QUEST_STATUS_INCOMPLETE)
                            player->FailQuest(QUEST_THE_HUNT_FOR_SYLVANAS);

                        if (Ide)
                        {
                            DoScriptText(PLAYER_SAY_ME_NOOB, player);

                            if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                            {
                                sylvanas->DoPersonalScriptText(SYLVANAS_YELL_FISH, player);
                                sylvanas->CastSpell(player, SPELL_SHOOT_QSKA, false);
                            }
                        }

                        DespawnEvent();
                    }

                    if (Event)
                        if (uiEventTimer <= diff)
                        {
                            ++uiPhase;

                            if (Player* player = GetPlayerForEscort())
                                switch (uiPhase)
                            {
                                case 1:
                                    Event = false;
                                    uiEventTimer = 500;
                                    me->Whisper(TOBIAS_RAID_BOSS_WHISPER, player->GetGUID(), true);
                                    break;
                                case 2:
                                    Event = false;
                                    uiEventTimer = 500;
                                    me->DoPersonalScriptText(TOBIAS_SAY_NOT_SEEN, player);
                                    SetEscortPaused(false);
                                    break;
                                case 3:
                                    uiEventTimer = 8000;
                                    if (Creature* general = me->FindNearestCreature(NPC_FORSAKEN_GENERAL_QTHFS, 50.0f))
                                        general->DoPersonalScriptText(FORSAKEN_GENERAL_YELL, player, true);
                                    break;
                                case 4:
                                    Event = false;
                                    me->DoPersonalScriptText(TOBIAS_SAY_DID_YOU_HEAR, player);
                                    break;
                                case 5:
                                    if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
                                    {
                                        Event = false;
                                        CheckSwim = true;
                                        SummonSilvanas(player->GetGUID());
                                    } else
                                        --uiPhase;
                                    break;
                                case 6:
                                    uiEventTimer = 8000;
                                    if (Creature* warhowl = Unit::GetCreature(*me, uiWarhowlGUID))
                                    {
                                        if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                            sylvanas->SetFacingToObject(warhowl);
                                        warhowl->DoPersonalScriptText(WARHOWL_SAY_LOSING, player);
                                    }
                                    break;
                                case 7:
                                    uiEventTimer = 6000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        sylvanas->DoPersonalScriptText(SYLVANAS_SAY_ASSURE, player);
                                    break;
                                case 8:
                                    uiEventTimer = 10000;
                                    if (Creature* warhowl = Unit::GetCreature(*me, uiWarhowlGUID))
                                        warhowl->DoPersonalScriptText(WARHOWL_SAY_PLAGUE, player);
                                    break;
                                case 9:
                                    uiEventTimer = 12000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        sylvanas->DoPersonalScriptText(SYLVANAS_SAY_TONE, player);
                                    break;
                                case 10:
                                    uiEventTimer = 7000;
                                    if (Creature* warhowl = Unit::GetCreature(*me, uiWarhowlGUID))
                                        warhowl->DoPersonalScriptText(WARHOWL_SAY_GOOD_BY, player);
                                    break;
                                case 11:
                                    uiEventTimer = 12000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        sylvanas->DoPersonalScriptText(SYLVANAS_SAY_GO_WITH_HONOR, player);
                                    if (Creature* warhowl = Unit::GetCreature(*me, uiWarhowlGUID))
                                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, warhowl->AI()))
                                            escort->SetEscortPaused(false);
                                    if (Creature* crenshaw = Unit::GetCreature(*me, uiCrenshawGUID))
                                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, crenshaw->AI()))
                                            escort->SetEscortPaused(false);
                                    break;
                                case 12:
                                    uiEventTimer = 9000;
                                    if (Creature* crenshaw = Unit::GetCreature(*me, uiCrenshawGUID))
                                        crenshaw->DoPersonalScriptText(CRENSHAW_SAY_ORDER, player);
                                    break;
                                case 13:
                                    uiEventTimer = 12000;
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        sylvanas->DoPersonalScriptText(SYLVANAS_SAY_WHAT_QUESTION, player);
                                    break;
                                case 14:
                                    uiEventTimer = 7000;
                                    CheckSwim = false;
                                    player->CompleteQuest(QUEST_THE_HUNT_FOR_SYLVANAS);
                                    if (Creature* crenshaw = Unit::GetCreature(*me, uiCrenshawGUID))
                                        crenshaw->DoPersonalScriptText(CRENSHAW_AS_YOU_WISH, player);
                                    if (Creature* sylvanas = Unit::GetCreature(*me, uiSylvanasGUID))
                                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, sylvanas->AI()))
                                            escort->SetEscortPaused(false);
                                    break;
                                case 15:
                                    Event = false;
                                    if (Creature* crenshaw = Unit::GetCreature(*me, uiCrenshawGUID))
                                        if (npc_escortAI* escort = CAST_AI(npc_escortAI, crenshaw->AI()))
                                            escort->SetEscortPaused(false);
                                    me->DespawnOrUnsummon();
                                    break;
                            }
                        } else
                            uiEventTimer -= diff;
        }
    };
};

class npc_lady_sylvanas_qthfs : public CreatureScript
{
public:
    npc_lady_sylvanas_qthfs() : CreatureScript("npc_lady_sylvanas_qthfs") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_sylvanas_qthfsAI(creature);
    }

    struct npc_lady_sylvanas_qthfsAI : public npc_escortAI
    {
        npc_lady_sylvanas_qthfsAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            SetDespawnAtFar(false);
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 2:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* tobias = summoner->ToCreature())
                            CAST_AI(npc_tobias_mistmantle_qthfs::npc_tobias_mistmantle_qthfsAI, tobias->AI())->Ide = true;
                break;
            case 3:
                SetEscortPaused(true);
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* tobias = summoner->ToCreature())
                            CAST_AI(npc_tobias_mistmantle_qthfs::npc_tobias_mistmantle_qthfsAI, tobias->AI())->Event = true;
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_general_warhowl_qthfs : public CreatureScript
{
public:
    npc_general_warhowl_qthfs() : CreatureScript("npc_general_warhowl_qthfs") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_general_warhowl_qthfsAI(creature);
    }

    struct npc_general_warhowl_qthfsAI : public npc_escortAI
    {
        npc_general_warhowl_qthfsAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            SetDespawnAtFar(false);
        }

        void WaypointReached(uint32 point)
        {
            if (point == 4)
                SetEscortPaused(true);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_high_executor_crenshaw_qthfs : public CreatureScript
{
public:
    npc_high_executor_crenshaw_qthfs() : CreatureScript("npc_high_executor_crenshaw_qthfs") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_executor_crenshaw_qthfsAI(creature);
    }

    struct npc_high_executor_crenshaw_qthfsAI : public npc_escortAI
    {
        npc_high_executor_crenshaw_qthfsAI(Creature* creature) : npc_escortAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            SetDespawnAtFar(false);
        }

        void WaypointReached(uint32 point)
        {
            if (point == 4 || point == 5)
                SetEscortPaused(true);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

///////////
// Quest Slowing the Inevitable 24920
///////////

class npc_captured_riding_bat : public CreatureScript
{
public:
    npc_captured_riding_bat() : CreatureScript("npc_captured_riding_bat") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captured_riding_batAI(creature);
    }

    struct npc_captured_riding_batAI : public npc_vehicle_escortAI
    {
        npc_captured_riding_batAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->SetCanFly(true);
        }

        void FinishEscort()
        {
            me->DespawnOrUnsummon();
        }

        void WaypointReached(uint32 point)
        { }

        void UpdateAI(const uint32 diff)
        {
            npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class spell_captured_riding_bat : public SpellScriptLoader
{
public:
    spell_captured_riding_bat() : SpellScriptLoader("spell_captured_riding_bat") { }

    class spell_captured_riding_bat_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_captured_riding_bat_SpellScript)

            void OnSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Unit* caster = GetCaster();

            if (!caster)
                return;

            float x, y, z = caster->GetPositionZ();

            if (Creature* bat = caster->FindNearestCreature(NPC_CAPTUREND_RIDING_BAT, 15.0f))
                bat->GetNearPoint2D(x, y, 3.0f, bat->GetOrientation() + M_PI / 2);
            else
                caster->GetPosition(x, y);

            if (Creature* bat = caster->SummonCreature(NPC_CAPTURED_RIDING_BAT, x, y, z, 0.0f))
                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, bat->AI()))
                {
                    caster->EnterVehicle(bat, 0);
                    bat->EnableAI();
                    bat->SetSpeed(MOVE_FLIGHT, 4.0f, true);

                    for (int i = 0; i < 25; ++i)
                        escort->AddWaypoint(i, BatWP[i][0], BatWP[i][1], BatWP[i][2]);

                    escort->Start(true);
                }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_captured_riding_bat_SpellScript::OnSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_captured_riding_bat_SpellScript();
    }
};

///////////
// Quest Patriarch's Blessing 24679
///////////

class npc_gilneas_funeral_camera : public CreatureScript
{
public:
    npc_gilneas_funeral_camera() : CreatureScript("npc_gilneas_funeral_camera") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_funeral_cameraAI(creature);
    }

    struct npc_gilneas_funeral_cameraAI : public ScriptedAI
    {
        npc_gilneas_funeral_cameraAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            uiGreymaneGUID = 0;
            uiLornaGUID = 0;
            uiCrowleyGUID = 0;
            uiCloneGUID = 0;
            uiEventTimer = 700;
            uiPhase = 0;

            if (me->isSummon())
                Event = true;
            else
                Event = false;

            Despawn = false;
        }

        uint64 uiGreymaneGUID;
        uint64 uiLornaGUID;
        uint64 uiCrowleyGUID;
        uint64 uiCloneGUID;
        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;
        bool Despawn;

        void PassengerBoarded(Unit* /*who*/, int8 /*seatId*/, bool apply)
        {
            if (!apply)
            {
                DespawnEvent();
                Event = false;
            }
        }

        void OnCharmed(bool /*charm*/)
        {
            me->NeedChangeAI = false;
            me->IsAIEnabled = true;
        }

        void SummonEventCreatures()
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Player* player = summoner->ToPlayer())
                    {
                        uint64 uiPlayerGUID = player->GetGUID();

                        me->SummonGameObject(GO_OFFERINGS, -1645.87f, 1903.36f, 29.9745f, 0, 0, 0, 0, 0, 0);

                        if (Creature* greymane = me->SummonCreature(NPC_KING_GENN_GREYMANE_QPB, -1644.238f, 1904.066f, 30.04164f, 3.56047f))
                        {
                            greymane->SetSeerGUID(uiPlayerGUID);
                            greymane->SetVisible(false);
                            uiGreymaneGUID = greymane->GetGUID();
                        }

                        if (Creature* crowley = me->SummonCreature(NPC_LORD_DARIUS_CROWLEY_QPB, -1642.351f, 1907.467f, 30.03497f, 3.577925f))
                        {
                            crowley->SetSeerGUID(uiPlayerGUID);
                            crowley->SetVisible(false);
                            uiCrowleyGUID = crowley->GetGUID();
                        }

                        if (Creature* lorna = me->SummonCreature(NPC_LORNA_CROWLEY_QPB, -1640.884f, 1903.595f, 30.03497f, 3.351032f))
                        {
                            lorna->SetSeerGUID(uiPlayerGUID);
                            lorna->SetVisible(false);
                            uiLornaGUID = lorna->GetGUID();
                        }

                        if (Creature* clone = me->SummonCreature(NPC_CLONE_QPB, -1640.681f, 1905.854f, 30.03497f, 3.682645f))
                        {
                            clone->SetSeerGUID(uiPlayerGUID);
                            clone->SetVisible(false);
                            player->AddAura(SPELL_MIRROR_IMAGE, clone);

                            if (Item const* item = (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND)))
                                clone->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, item->GetEntry());

                            if (Item const* item = (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND)))
                                clone->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, item->GetEntry());

                            if (Item const* item = (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED)))
                                clone->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, item->GetEntry());

                            uiCloneGUID = clone->GetGUID();
                        }
                    }
        }

        void DespawnEvent()
        {
            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Player* player = summoner->ToPlayer())
                    {
                        Despawn = true;
                        player->CastSpell(player, SPELL_FADE_TO_BLACK, false);
                    }

                    if (GameObject* go = me->FindNearestGameObject(GO_OFFERINGS, 30.0f))
                        go->RemoveFromWorld();

                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                        greymane->DespawnOrUnsummon();

                    if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                        crowley->DespawnOrUnsummon();

                    if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                        lorna->DespawnOrUnsummon();

                    if (Creature* clone = Unit::GetCreature(*me, uiCloneGUID))
                        clone->DespawnOrUnsummon();

                    me->DespawnOrUnsummon(500);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Despawn)
            {
                Despawn = false;

                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                            player->TeleportTo(Gilneas2, -1725.0f, 1871.91f, 17.7934f, 3.17831f);
            }

            if (Event)
                if (uiEventTimer <= diff)
                {
                    ++uiPhase;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                                switch (uiPhase)
                            {
                                case 1:
                                    uiEventTimer = 7000;
                                    SummonEventCreatures();
                                    player->EnterVehicle(me);
                                    me->EnableAI();
                                    break;
                                case 2:
                                    uiEventTimer = 9000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                    {
                                        greymane->DoPersonalScriptText(GREYMANE_SAY_QPB_1, player);
                                        greymane->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                                    }
                                    break;
                                case 3:
                                    uiEventTimer = 7000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                    {
                                        greymane->DoPersonalScriptText(GREYMANE_SAY_QPB_2, player);
                                        greymane->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                                    }
                                    break;
                                case 4:
                                    uiEventTimer = 9000;
                                    if (Creature* lorna = Unit::GetCreature(*me, uiLornaGUID))
                                        lorna->DoPersonalScriptText(LORNA_SAY_QPB, player);
                                    break;
                                case 5:
                                    uiEventTimer = 5000;
                                    if (Creature* crowley = Unit::GetCreature(*me, uiCrowleyGUID))
                                    {
                                        crowley->DoPersonalScriptText(CROWLEY_SAY_QPB, player);
                                        crowley->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                                    }
                                    if (Creature* clone = Unit::GetCreature(*me, uiCloneGUID))
                                        clone->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                                    break;
                                case 6:
                                    uiEventTimer = 7000;
                                    if (Creature* greymane = Unit::GetCreature(*me, uiGreymaneGUID))
                                    {
                                        greymane->DoPersonalScriptText(GREYMANE_SAY_QPB_3, player);
                                        greymane->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                                    }
                                    break;
                                case 7:
                                    player->ExitVehicle();
                                    break;
                            }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class spell_place_blessed_offerings : public SpellScriptLoader
{
public:
    spell_place_blessed_offerings() : SpellScriptLoader("spell_place_blessed_offerings") { }

    class spell_place_blessed_offerings_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_place_blessed_offerings_SpellScript)

            void SummonOfferings(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Unit* caster = GetCaster();

            if (!(caster && caster->GetTypeId() == TYPEID_PLAYER))
                return;

            if (Creature* camera = caster->SummonCreature(NPC_GILNEAS_FUNERAL_CAMERA, -1647.91f, 1897.56f, 32.4459f, 1.0f))
            {
                caster->CastSpell(caster, SPELL_FADE_TO_BLACK, true);
                camera->SetSeerGUID(caster->GetGUID());
                camera->SetVisible(false);
            }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_place_blessed_offerings_SpellScript::SummonOfferings, EFFECT_0, SPELL_EFFECT_TRANS_DOOR);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_place_blessed_offerings_SpellScript();
    }
};

class npc_jack_derrington : public CreatureScript
{
public:
    npc_jack_derrington() : CreatureScript("npc_jack_derrington") { }

    void SendTrainerList(Player* player, Creature* creature, uint32 uiReqSkill)
    {
        std::string  strTitle = player->GetSession()->GetTrinityString(LANG_NPC_TAINER_HELLO);;

        TrainerSpellData const* trainer_spells = creature->GetTrainerSpells();

        if (!trainer_spells)
            return;

        WorldPacket data(SMSG_TRAINER_LIST, 8 + 4 + 4 + 4 + trainer_spells->spellList.size() * 38 + strTitle.size() + 1);
        data << creature->GetGUID();
        data << uint32(trainer_spells->trainerType);
        data << uint32(1);

        size_t count_pos = data.wpos();
        data << uint32(trainer_spells->spellList.size());

        uint32 count = 0;

        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
        {
            TrainerSpell const* tSpell = &itr->second;

            TrainerSpellState state = player->GetTrainerSpellState(tSpell);

            if (tSpell->reqSkill != uiReqSkill)
                continue;

            data << uint32(tSpell->spell);
            data << uint8(state);
            data << uint32(tSpell->spellCost);

            data << uint8(tSpell->reqLevel);
            data << uint32(tSpell->reqSkill);
            data << uint32(tSpell->reqSkillValue);

            data << uint32(0);
            data << uint32(0);
            data << uint32(0);

            data << uint32(0);

            ++count;
        }

        data << strTitle;

        data.put<uint32>(count_pos, count);
        player->GetSession()->SendPacket(&data);

        return;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(MENU_MAIN, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[2], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[3], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[4], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_GATHERING_AND_MINING_PROFESSIONS, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[5], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[6], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[7], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[8], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[9], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[10], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[11], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[12], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(MENU_CRAFTING_PROFESSIONS, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[14], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_HERBALISM);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[5], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[9], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_HERBALISM, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[16], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_MINING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[6], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[8], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[10], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_MINING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[18], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_SKINNING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[11], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_SKINNING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[13], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_ALCHEMY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[2], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[9], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_ALCHEMY, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[17], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_BLACKSMITHING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[3], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_BLACKSMITHING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 8:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[23], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_ENCHANTING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[12], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_ENCHANTING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 9:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[19], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_ENGINEERING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[3], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_ENGINERING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[15], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_INSCRIPTION);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[2], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_INSCRIPTION, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[20], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_JEWELCRAFTING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[3], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_JEWELCRAFTING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[21], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_LEATHERWORKING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[4], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_LEATHERWORKING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GossipMenu[22], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + SKILL_TAILORING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[7], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipMenu[1], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(MENU_TAILORING, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_HERBALISM:
            SendTrainerList(player, creature, SKILL_HERBALISM);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_ALCHEMY:
            SendTrainerList(player, creature, SKILL_ALCHEMY);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_INSCRIPTION:
            SendTrainerList(player, creature, SKILL_INSCRIPTION);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_MINING:
            SendTrainerList(player, creature, SKILL_MINING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_BLACKSMITHING:
            SendTrainerList(player, creature, SKILL_BLACKSMITHING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_SKINNING:
            SendTrainerList(player, creature, SKILL_SKINNING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_ENGINEERING:
            SendTrainerList(player, creature, SKILL_ENGINEERING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_JEWELCRAFTING:
            SendTrainerList(player, creature, SKILL_JEWELCRAFTING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_LEATHERWORKING:
            SendTrainerList(player, creature, SKILL_LEATHERWORKING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_TAILORING:
            SendTrainerList(player, creature, SKILL_TAILORING);
            break;
        case GOSSIP_ACTION_INFO_DEF + SKILL_ENCHANTING:
            SendTrainerList(player, creature, SKILL_ENCHANTING);
            break;
        }

        return true;
    }
};

class npc_prince_liam_greymane_dh : public CreatureScript
{
public:
    npc_prince_liam_greymane_dh() : CreatureScript("npc_prince_liam_greymane_dh") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prince_liam_greymane_dhAI(creature);
    }

    struct npc_prince_liam_greymane_dhAI : public Scripted_NoMovementAI
    {
        npc_prince_liam_greymane_dhAI(Creature* c) : Scripted_NoMovementAI(c)
        {
            SetCombatMovement(false);
        }

        uint32 uiShootTimer;
        bool miss;

        void Reset()
        {
            uiShootTimer = 1000;
            miss = false;

            if (Unit* target = me->FindNearestCreature(NPC_FORSAKEN_INVADER, 40.0f))
                AttackStart(target);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;
                Unit* target = NULL;

                if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                    if (target = me->FindNearestCreature(NPC_FORSAKEN_INVADER, 40.0f))
                        if (target != me->getVictim())
                        {
                            me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                            me->CombatStart(target);
                            me->AddThreat(target, 1000);
                        }

                        if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                        {
                            if (me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                            {
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                                me->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                                me->SendMeleeAttackStop(me->getVictim());
                            }

                            me->CastSpell(me->getVictim(), SPELL_SHOOT, false);
                        } else
                            if (!me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                            {
                                me->AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                                me->SendMeleeAttackStart(me->getVictim());
                            }
            } else
                uiShootTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_duskhaven_watchman : public CreatureScript
{
public:
    npc_duskhaven_watchman() : CreatureScript("npc_duskhaven_watchman") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_duskhaven_watchmanAI(creature);
    }

    struct npc_duskhaven_watchmanAI : public Scripted_NoMovementAI
    {
        npc_duskhaven_watchmanAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
        }

        uint32 uiShootTimer;
        uint32 uiFindEnemyTimer;
        bool miss;

        void Reset()
        {
            uiShootTimer = 1000;
            uiFindEnemyTimer = 1500;
            miss = false;

            if (Unit* target = me->FindNearestCreature(NPC_FORSAKEN_INVADER, 40.0f))
            {
                AttackStart(target);
                return;
            }

            if (Unit* target = me->FindNearestCreature(NPC_FORSAKEN_FOOTSOLDIER, 40.0f))
                AttackStart(target);
        }

        void FindEnemy()
        {
            Creature* target = NULL;

            if (target = me->FindNearestCreature(NPC_FORSAKEN_FOOTSOLDIER, 40.0f))
            {
                if (target != me->getVictim())
                {
                    if (me->getVictim())
                    {
                        if (target == me->getVictim())
                            return;

                        me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                    }

                    me->AddThreat(target, 1000);
                    target->SetInCombatWith(me);
                    me->SetInCombatWith(target);
                }

                return;
            }

            std::list<Creature*> EnemyList;
            me->GetCreatureListWithEntryInGrid(EnemyList, NPC_FORSAKEN_INVADER, 50.0f);

            if (EnemyList.empty())
                return;

            float curr_dist = 100500;

            for (std::list<Creature*>::const_iterator itr = EnemyList.begin(); itr != EnemyList.end(); ++itr)
            {
                if ((*itr)->isInCombat())
                    continue;

                if (!(*itr)->isAlive())
                    continue;

                if (!(*itr)->IsWithinLOSInMap(me))
                    continue;

                if (!(*itr)->isInAccessiblePlaceFor(me))
                    continue;

                float dist = me->GetDistance2d((*itr));

                if (dist < curr_dist)
                {
                    curr_dist = dist;
                    target = (*itr);
                }
            }

            if (target)
            {
                if (me->getVictim())
                {
                    if (target == me->getVictim())
                        return;

                    me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                }

                target->SetInCombatWith(me);
                me->SetInCombatWith(target);
                target->AddThreat(me, 100500.0f);
                me->AddThreat(target, 100500.0f);
                float x, y, z;
                float t_x, t_y;
                target->GetPosition(t_x, t_y);
                me->GetNearPoint(me, x, y, z, me->GetObjectSize(), 0.0f, me->GetAngle(t_x, t_y));
                float currentGroundLevel = me->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);
                float curr_angle = me->GetAngle(x, y);

                if (z > currentGroundLevel)
                    z = currentGroundLevel;

                target->GetMotionMaster()->MoveCharge(x, y, z);
                return;
            }

            return;
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiFindEnemyTimer <= diff)
            {
                uiFindEnemyTimer = 10000;

                if (me->getVictim())
                    if (me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                        return;

                FindEnemy();
            } else
                uiFindEnemyTimer -= diff;

            if (!UpdateVictim())
                return;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;

                if (!me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                {
                    if (me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                    {
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                        me->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                        me->SendMeleeAttackStop(me->getVictim());
                    }

                    me->CastSpell(me->getVictim(), SPELL_SHOOT, false);
                } else
                    if (!me->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                    {
                        me->AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                        me->SendMeleeAttackStart(me->getVictim());
                    }
            } else
                uiShootTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_forsaken_invader : public CreatureScript
{
public:
    npc_forsaken_invader() : CreatureScript("npc_forsaken_invader") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_invaderAI(creature);
    }

    struct npc_forsaken_invaderAI : public ScriptedAI
    {
        npc_forsaken_invaderAI(Creature* creature) : ScriptedAI(creature)
        {
            miss = false;
            me->_ReactDistance = 20.0f;
        }

        bool miss;

        void DamageTaken(Unit* attacker, uint32 &/*damage*/)
        {
            if (attacker->GetTypeId() != TYPEID_PLAYER)
                return;

            Unit* victim = NULL;

            if (victim = me->getVictim())
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    return;

            if (victim)
                me->getThreatManager().modifyThreatPercent(victim, -100);

            AttackStart(attacker);
            me->AddThreat(attacker, 10005000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Leader of the Pack 14386
///////////

class npc_dark_ranger_thyala : public CreatureScript
{
public:
    npc_dark_ranger_thyala() : CreatureScript("npc_dark_ranger_thyala") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_ranger_thyalaAI(creature);
    }

    struct npc_dark_ranger_thyalaAI : public Scripted_NoMovementAI
    {
        npc_dark_ranger_thyalaAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
        }

        std::list<Player*> lPlayers;
        uint32 uiShootTimer;
        uint32 uiKnockbackTimer;

        void Reset()
        {
            uiShootTimer = 500;
            uiKnockbackTimer = urand(2000, 5000);
        }

        void DamageTaken(Unit* attacker, uint32 &damage)
        {
            if (me->GetHealth() > damage)
                return;

            std::list<HostileReference*> tList = me->getThreatManager().getThreatList();

            if (!tList.empty())

                for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                    if (Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                        if (Player* player = target->ToPlayer())
                            player->KilledMonsterCredit(NPC_DARK_RANGER_THYALA, 0);
        }

        void JustDied(Unit* /*who*/)
        {
            std::list<Creature*> lMastiffs;
            me->GetCreatureListWithEntryInGrid(lMastiffs, NPC_ATTACK_MASTIFF, 50.0f);

            if (!lMastiffs.empty())
                for (std::list<Creature*>::const_iterator itr = lMastiffs.begin(); itr != lMastiffs.end(); ++itr)
                    if ((*itr)->isAlive())
                    {
                        (*itr)->SetReactState(REACT_PASSIVE);
                        (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        (*itr)->CombatStop();
                        (*itr)->GetMotionMaster()->MoveTargetedHome();
                        (*itr)->DespawnOrUnsummon(3000);
                    }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1000;
                DoCast(me->getVictim(), SPELL_SHOOT_LOTP);
            } else
                uiShootTimer -= diff;

            if (uiKnockbackTimer <= diff)
            {
                uiKnockbackTimer = urand(1000, 5000);
                DoCast(SPELL_KNOCKBACK_LOTP);
            } else
                uiKnockbackTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_attack_mastiff : public CreatureScript
{
public:
    npc_attack_mastiff() : CreatureScript("npc_attack_mastiff") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_attack_mastiffAI(creature);
    }

    struct npc_attack_mastiffAI : public ScriptedAI
    {
        npc_attack_mastiffAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiDemoralizingRoarTimer;
        uint32 uiLeapTimer;
        uint32 uiTaunt;

        void Reset()
        {
            uiDemoralizingRoarTimer = 2000;
            uiLeapTimer = 1000;
            uiTaunt = 1000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiDemoralizingRoarTimer <= diff)
            {
                uiDemoralizingRoarTimer = urand(5000, 15000);
                DoCast(SPELL_DEMORALIZING_ROAR);
            } else
                uiDemoralizingRoarTimer -= diff;

            if (uiLeapTimer <= diff)
            {
                uiLeapTimer = 1000;
                DoCast(me->getVictim(), SPELL_LEAP);
            } else
                uiLeapTimer -= diff;

            if (uiTaunt <= diff)
            {
                uiTaunt = 1000;
                DoCast(me->getVictim(), SPELL_TAUNT_LOTP);
            } else
                uiTaunt -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class spell_call_attack_mastiffs : public SpellScriptLoader
{
public:
    spell_call_attack_mastiffs() : SpellScriptLoader("spell_call_attack_mastiffs") { }

    class spell_call_attack_mastiffs_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_call_attack_mastiffs_SpellScript)

            void SummonMastiffs(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Unit* caster = GetCaster();

            if (!caster)
                return;

            if (Creature* thyala = caster->FindNearestCreature(NPC_DARK_RANGER_THYALA, 30.0f))
            {
                float angle = thyala->GetHomePosition().GetOrientation();

                for (int i = 0; i < 12; ++i)
                {
                    float x, y, z;
                    uint32 DistRoll = urand(30, 50);
                    int RadRoll = irand(-45, 45);
                    thyala->GetNearPoint(thyala, x, y, z, 0, 0.0f + DistRoll, angle + RadRoll * M_PI / 180);
                    float currentGroundLevel = thyala->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);
                    float curr_angle = thyala->GetAngle(x, y);

                    if (z > currentGroundLevel)
                        z = currentGroundLevel;

                    if (Creature* mastiff = caster->SummonCreature(NPC_ATTACK_MASTIFF, x, y, z, curr_angle))
                    {
                        mastiff->CombatStart(thyala, true);
                        thyala->CombatStart(mastiff, true);
                        mastiff->AddThreat(thyala, 100500);
                        float x, y, z;
                        thyala->GetPosition(x, y, z);
                        mastiff->GetMotionMaster()->MoveCharge(x, y, z);
                    }
                }
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_call_attack_mastiffs_SpellScript::SummonMastiffs, EFFECT_2, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_call_attack_mastiffs_SpellScript();
    }
};

/////////
//  Hayward Brothers
////////

class npc_hayward_brother : public CreatureScript
{
public:
    npc_hayward_brother() : CreatureScript("npc_hayward_brother") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hayward_brotherAI(creature);
    }

    struct npc_hayward_brotherAI : public Scripted_NoMovementAI
    {
        npc_hayward_brotherAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
        }

        uint64 uiPrevTargetGUID;
        uint32 uiFindEnemyTimer;
        bool miss;

        void Reset()
        {
            uiPrevTargetGUID = 0;
            uiFindEnemyTimer = 1500;
            miss = false;

            if (Unit* target = me->FindNearestCreature(NPC_FORSAKEN_CASTAWAY, 40.0f))
                AttackStart(target);
        }

        void FindEnemy()
        {
            std::list<Creature*> EnemyList;
            me->GetCreatureListWithEntryInGrid(EnemyList, NPC_FORSAKEN_CASTAWAY, 50.0f);

            if (EnemyList.empty())
                return;

            Creature* target = NULL;
            float curr_dist = 100500;

            for (std::list<Creature*>::const_iterator itr = EnemyList.begin(); itr != EnemyList.end(); ++itr)
            {
                if ((*itr)->isInCombat())
                    continue;

                if (!(*itr)->isAlive())
                    continue;

                if (!(*itr)->IsWithinLOSInMap(me))
                    continue;

                if (!(*itr)->isInAccessiblePlaceFor(me))
                    continue;

                float dist = me->GetDistance2d((*itr));

                if (dist < curr_dist)
                {
                    curr_dist = dist;
                    target = (*itr);
                }
            }

            if (target)
            {
                if (me->getVictim())
                {
                    if (target == me->getVictim())
                        return;

                    me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
                }

                target->SetInCombatWith(me);
                me->SetInCombatWith(target);
                target->AddThreat(me, 100500.0f);
                me->AddThreat(target, 100500.0f);
                float x, y, z;
                float t_x, t_y;
                target->GetPosition(t_x, t_y);
                me->GetNearPoint(me, x, y, z, me->GetObjectSize(), 0.0f, me->GetAngle(t_x, t_y));
                float currentGroundLevel = me->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);
                float curr_angle = me->GetAngle(x, y);

                if (z > currentGroundLevel)
                    z = currentGroundLevel;

                uint32 uiTargetGUID = target->GetGUID();

                if (uiPrevTargetGUID == uiTargetGUID)
                {
                    target->GetMotionMaster()->MoveJump(x, y, z, 10, 10);
                    target->Relocate(x, y, z);
                    uiPrevTargetGUID = 0;
                    return;
                }

                uiPrevTargetGUID = uiTargetGUID;
                target->GetMotionMaster()->MoveCharge(x, y, z);
                return;
            }

            return;
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiFindEnemyTimer <= diff)
            {
                uiFindEnemyTimer = 10000;

                if (me->getVictim())
                    if (me->IsWithinMeleeRange(me->getVictim(), 0.0f))
                        return;

                FindEnemy();
            } else
                uiFindEnemyTimer -= diff;

            if (!UpdateVictim())
                return;

            if (me->getVictim())
                if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                    if (me->getVictim()->GetHealthPct() < 90)
                        miss = true;
                    else
                        miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

/////////
//  Forsaken Catapult
////////

class npc_forsaken_catapult_sti : public CreatureScript
{
public:
    npc_forsaken_catapult_sti() : CreatureScript("npc_forsaken_catapult_sti") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_catapult_stiAI(creature);
    }

    struct npc_forsaken_catapult_stiAI : public ScriptedAI
    {
        npc_forsaken_catapult_stiAI(Creature* creature) : ScriptedAI(creature)
        {
            CanCast = false;

            for (int i = 0; i < 5; ++i)
                if (me->GetDistance(CataCastPos[i][0], CataCastPos[i][1], CataCastPos[i][2]) < 15.0f)
                {
                    CanCast = true;
                    me->setActive(true);
                }
        }

        uint32 uiCastTimer;
        bool CanCast;

        void Reset()
        {
            uiCastTimer = 500;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CanCast)
                if (uiCastTimer <= diff)
                {
                    uiCastTimer = urand(2000, 7000);
                    float x, y, z;
                    me->GetNearPoint2D(x, y, urand(100, 150), me->GetOrientation());
                    z = me->GetBaseMap()->GetHeight(x, y, MAX_HEIGHT);
                    me->CastSpell(x, y, z, SPELL_PLAGUE_BARREL_LAUNCH, false);
                } else
                    uiCastTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Knee-Deep 24678
///////////

class npc_knee_deep_attacker : public CreatureScript
{
public:
    npc_knee_deep_attacker() : CreatureScript("npc_knee_deep_attacker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_knee_deep_attackerAI(creature);
    }

    struct npc_knee_deep_attackerAI : public ScriptedAI
    {
        npc_knee_deep_attackerAI(Creature* creature) : ScriptedAI(creature)
        {
            CanAttackTimer = 0;
        }

        uint32 CanAttackTimer;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SWING_TORCH)
            {
                float x, y, z;
                me->GetNearPoint2D(x, y, urand(5, 10), me->GetAngle(caster->GetPositionX(), caster->GetPositionY()) + M_PI);
                z = me->GetPositionZ();
                me->GetMotionMaster()->MoveCharge(x, y, z);
                CanAttackTimer = 5000;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CanAttackTimer <= diff)
                DoMeleeAttackIfReady();
            else
                CanAttackTimer -= diff;
        }
    };
};

class npc_gilneas_tremors_credit : public CreatureScript
{
public:
    npc_gilneas_tremors_credit() : CreatureScript("npc_gilneas_tremors_credit") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_tremors_creditAI(creature);
    }

    struct npc_gilneas_tremors_creditAI : public ScriptedAI
    {
        npc_gilneas_tremors_creditAI(Creature* creature) : ScriptedAI(creature)
        {
            me->setActive(true);
            uiTremorTimerFirst = urand(30000, 150000);
            uiTremorTimerSecond = urand(20000, 100000);
            uiTremorTimerThird = urand(10000, 50000);
        }

        uint32 uiTremorTimerFirst;
        uint32 uiTremorTimerSecond;
        uint32 uiTremorTimerThird;

        void Tremor(uint32 uiPhase, uint32 uiSpellId)
        {
            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                if (Player* player = i->getSource())
                    if (uiPhase & player->GetPhaseMask())
                        player->CastSpell(player, uiSpellId, false);
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiTremorTimerFirst <= diff)
            {
                uiTremorTimerFirst = urand(30000, 150000);
                Tremor(4096, SPELL_CATACLYSM_TYPE_1);
            } else
                uiTremorTimerFirst -= diff;

            if (uiTremorTimerSecond <= diff)
            {
                uiTremorTimerSecond = urand(20000, 100000);
                Tremor(8192, SPELL_CATACLYSM_TYPE_2);
            } else
                uiTremorTimerSecond -= diff;

            if (uiTremorTimerThird <= diff)
            {
                uiTremorTimerThird = urand(10000, 50000);
                Tremor(16384, SPELL_CATACLYSM_TYPE_3);
            } else
                uiTremorTimerThird -= diff;
        }
    };
};

////////
// Class Trainers
///////

class npc_huntsman_blake : public CreatureScript
{
public:
    npc_huntsman_blake() : CreatureScript("npc_huntsman_blake") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_HUNTER)
        {
            player->SEND_GOSSIP_MENU(MENU_HUNTSMAN_BLAKE, creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_STEADY_SHOT)
            creature->DoPersonalScriptText(HUNTSMAN_BLAKE_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_STEADY_SHOT)
            creature->DoPersonalScriptText(HUNTSMAN_BLAKE_SAY_2, player);

        return true;
    }
};

class npc_vitus_darkwalker : public CreatureScript
{
public:
    npc_vitus_darkwalker() : CreatureScript("npc_vitus_darkwalker") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_IMMOLATE)
            creature->DoPersonalScriptText(VITUS_DARKWALKER_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_IMMOLATE)
            creature->DoPersonalScriptText(VITUS_DARKWALKER_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_WARLOCK)
        {
            player->SEND_GOSSIP_MENU(MENU_VITUS_DARKWALKER, creature->GetGUID());
            return true;
        }

        return false;
    }
};

class npc_loren_the_fence : public CreatureScript
{
public:
    npc_loren_the_fence() : CreatureScript("npc_loren_the_fence") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_EVISCERATE)
            creature->DoPersonalScriptText(LOREN_THE_FENCE_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_EVISCERATE)
            creature->DoPersonalScriptText(LOREN_THE_FENCE_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_ROGUE)
        {
            player->SEND_GOSSIP_MENU(MENU_LOREN_THE_FENCE, creature->GetGUID());
            return true;
        }

        return false;
    }
};

class npc_sister_almyra : public CreatureScript
{
public:
    npc_sister_almyra() : CreatureScript("npc_sister_almyra") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_FLASH_HEAL)
            creature->DoPersonalScriptText(SISTER_ALMYRA_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_FLASH_HEAL)
            creature->DoPersonalScriptText(SISTER_ALMYRA_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_PRIEST)
        {
            player->SEND_GOSSIP_MENU(MENU_SISTER_ALMYRA, creature->GetGUID());
            return true;
        }

        return false;
    }
};

class npc_celestine_of_the_harves : public CreatureScript
{
public:
    npc_celestine_of_the_harves() : CreatureScript("npc_celestine_of_the_harves") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_A_REJUVENATING_TOUCH)
            creature->DoPersonalScriptText(CELESTINE_OF_THE_HARVES_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_A_REJUVENATING_TOUCH)
            creature->DoPersonalScriptText(CELESTINE_OF_THE_HARVES_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_DRUID)
        {
            player->SEND_GOSSIP_MENU(MENU_CELESTINE_OF_THE_HARVES, creature->GetGUID());
            return true;
        }

        return false;
    }
};

class npc_myriam_spellwaker : public CreatureScript
{
public:
    npc_myriam_spellwaker() : CreatureScript("npc_myriam_spellwaker") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ARCANE_MISSILES)
            creature->DoPersonalScriptText(MYRIAM_SPELLWAKER_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ARCANE_MISSILES)
            creature->DoPersonalScriptText(MYRIAM_SPELLWAKER_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_MAGE)
        {
            player->SEND_GOSSIP_MENU(MENU_MYRIAM_SPELLWAKER, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_myriam_spellwakerAI(creature);
    }

    struct npc_myriam_spellwakerAI : public Scripted_NoMovementAI
    {
        npc_myriam_spellwakerAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            uiBoltTimer = 2000;
            SetCombatMovement(false);
        }

        uint32 uiBoltTimer;

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiBoltTimer <= diff)
            {
                uiBoltTimer = 2000;
                DoCast(me->getVictim(), SPELL_FROSTBOLT_VISUAL_ONLY);
            } else
                uiBoltTimer -= diff;
        }
    };
};

class npc_sergeant_cleese : public CreatureScript
{
public:
    npc_sergeant_cleese() : CreatureScript("npc_sergeant_cleese") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CHARGE)
            creature->DoPersonalScriptText(SERGEANT_CLEESE_SAY_1, player);

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CHARGE)
            creature->DoPersonalScriptText(SERGEANT_CLEESE_SAY_2, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_WARRIOR)
        {
            player->SEND_GOSSIP_MENU(MENU_SERGEANT_CLEESE, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sergeant_cleeseAI(creature);
    }

    struct npc_sergeant_cleeseAI : public ScriptedAI
    {
        npc_sergeant_cleeseAI(Creature* creature) : ScriptedAI(creature)
        {
            miss = false;
            me->_ReactDistance = 20.0f;
        }

        bool miss;

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->getVictim()->GetTypeId() == TYPEID_UNIT)
                if (me->getVictim()->GetHealthPct() < 90)
                    miss = true;
                else
                    miss = false;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_admiral_nightwind : public CreatureScript
{
public:
    npc_admiral_nightwind() : CreatureScript("npc_admiral_nightwind") { }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_RUTTHERAN_VILLANGE)
        {
            player->RemoveAllAuras();
            WorldLocation loc;
            loc.m_mapId       = 1;
            loc.m_positionX   = 8181.06f;
            loc.m_positionY   = 1001.92f;
            loc.m_positionZ   = 7.0589f;
            player->TeleportTo(loc);
            player->SetHomebind(loc, 702);
            player->SaveToDB();
        }
        return true;
    }
};

void AddSC_gilneas()
{
    new npc_gilneas_city_guard_gate();
    new npc_panicked_citizen_gate();
    new npc_gilnean_crow();
    new npc_prince_liam_greymane_intro();
    new npc_lieutenant_walden();
    new npc_prince_liam_greymane_phase_1();
    new npc_rampaging_worgen();
    new npc_gilneas_city_guard();
    new npc_king_genn_greymane_qod();
    new npc_qems_citizen();
    new npc_qems_worgen();
    new npc_worgen_attacker();
    new npc_lord_darius_crowley();
    new npc_josiah_avery_worgen();
    new npc_josiah_avery_human();
    new npc_lorna_crowley_qfs();
    new npc_gilneas_mastiff();
    new npc_bloodfang_lurker();
    new npc_king_genn_greymane();
    new npc_cannon_camera();
    new npc_lord_godfrey_qska();
    new npc_saved_aranas();
    new npc_vehicle_genn_horse();
    new npc_king_genn_greymane_qtr();
    new npc_crowley_horse();
    new npc_bloodfang_stalker();
    new npc_crowley_horse_second_round();
    new npc_lord_darius_crowley_qls();
    new npc_king_genn_greymane_qls();
    new npc_krennan_aranas_qls();
    new npc_slain_watchman();
    new npc_horrid_abomination();
    new npc_forsaken_catapult_qtbs();
    new npc_james();
    new npc_ashley();
    new npc_cynthia();
    new npc_drowning_watchman();
    new npc_prince_liam_greymane();
    new npc_chance_the_cat();
    new npc_lucius_the_cruel();
    new npc_wahl();
    new npc_mountain_horse_vehicle();
    new npc_mountain_horse_follower();
    new npc_swift_mountain_horse();
    new npc_gwen_armstead();
    new npc_king_genn_greymane_c3();
    new npc_stagecoach_harness_summoner();
    new npc_stagecoach_harness();
    new npc_crash_survivor();
    new npc_swamp_crocolisk();
    new npc_koroth_the_hillbreaker_qiao_friend();
    new npc_captain_asther_qiao();
    new npc_prince_liam_greymane_qiao();
    new npc_koroth_the_hillbreaker_qiao();
    new npc_dark_scout_summoner();
    new npc_dark_scout();
    new npc_lord_darius_crowley_qaod();
    new npc_tobias_mistmantle_qaod();
    new npc_taldoren_tracker();
    new npc_tobias_mistmantle_qtbwo();
    new npc_lord_godfrey_qnhnb();
    new npc_lorna_crowley_qnhnb();
    new npc_king_genn_greymane_qnhnb();
    new npc_krennan_aranas_qbatr();
    new npc_lord_godfrey_qbatr();
    new npc_king_genn_greymane_qbatr();
    new npc_prince_liam_greymane_tbfgc();
    new npc_gilnean_militia_tbfgc();
    new npc_myriam_spellwaker_tbfgc();
    new npc_sister_almyra_tbfgc();
    new npc_dark_ranger_elite_tbfgc();
    new npc_emberstone_cannon();
    new npc_lorna_crowley_tbfgc();
    new npc_lord_darius_crowley_tbfgc();
    new worgen_warrior_tbfgc();
    new npc_gorerot();
    new npc_king_genn_greymane_tbfgc();
    new npc_lady_sylvanas_windrunner_tbfgc();
    new npc_damaged_catapult_tbfgc();
    new npc_lorna_crowley();
    new npc_krennan_aranas_tbfgc();
    new npc_lorna_crowley_qthfs();
    new npc_tobias_mistmantle_qthfs();
    new npc_lady_sylvanas_qthfs();
    new npc_general_warhowl_qthfs();
    new npc_high_executor_crenshaw_qthfs();
    new npc_captured_riding_bat();
    new npc_gilneas_funeral_camera();
    new npc_jack_derrington();
    new npc_prince_liam_greymane_dh();
    new npc_duskhaven_watchman();
    new npc_forsaken_invader();
    new npc_dark_ranger_thyala();
    new npc_attack_mastiff();
    new npc_hayward_brother();
    new npc_forsaken_catapult_sti();
    new npc_knee_deep_attacker();
    new npc_gilneas_tremors_credit();
    new npc_huntsman_blake();
    new npc_vitus_darkwalker();
    new npc_loren_the_fence();
    new npc_sister_almyra();
    new npc_celestine_of_the_harves();
    new npc_myriam_spellwaker();
    new npc_sergeant_cleese();
    new npc_gwen_armstead_qi();
    new npc_admiral_nightwind();

    new go_merchant_square_door();
    new go_crate_of_mandrake_essence();
    new go_koroth_banner();
    new go_qnhnb_well();
    new go_ball_and_chain_qld();

    new spell_summon_crowley_horse();
    new spell_summon_crowley_horse_second_round();
    new spell_curse_of_the_worgen_summon();
    new spell_curse_of_the_worgen_invis();
    new spell_launch_qtbs();
    new spell_round_up_horse();
    new spell_belysras_talisman();
    new spell_horn_of_taldoren();
    new spell_worgen_combat_transform_fx();
    new spell_place_blessed_offerings();
    new spell_call_attack_mastiffs();
    new spell_captured_riding_bat();
    new spell_summon_carriage();
}
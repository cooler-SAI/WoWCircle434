/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "kezan.h"

///////////
// Class quests and trainers
///////////

class npc_sister_goldskimmer : public CreatureScript
{
public:
    npc_sister_goldskimmer() : CreatureScript("npc_sister_goldskimmer") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_FLASH_HEAL)
            creature->DoPersonalScriptText(SISTER_GOLDSKIMMER_QUEST_ACCEPT, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_PRIEST)
        {
            player->SEND_GOSSIP_MENU(14601, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sister_goldskimmerAI(creature);
    }

    struct npc_sister_goldskimmerAI : public ScriptedAI
    {
        npc_sister_goldskimmerAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 4)
                CanCast = false;
            else
                CanCast = true;
        }

        bool CanCast;

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->IsWithinDistInMap(who, 30.0f))
                return;

            if (!CanCast)
                return;

            if (Player* player = who->ToPlayer())
                if (!player->HasAura(SPELL_POWER_WORD_FORTITUDE))
                {
                    DoCast(player, SPELL_POWER_WORD_FORTITUDE);
                    me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                    uint8 roll = urand(0, 6);
                    me->DoPersonalScriptText(SISTER_GOLDSKIMMER_RANDOM_SPEACH - roll, player);
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

class npc_maxx_avalanche : public CreatureScript
{
public:
    npc_maxx_avalanche() : CreatureScript("npc_maxx_avalanche") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PRIMAL_STRIKE)
            creature->DoPersonalScriptText(MAXX_AVALANCHE_QUEST_ACCEPT, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_SHAMAN)
        {
            player->SEND_GOSSIP_MENU(50009, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_maxx_avalancheAI(creature);
    }

    struct npc_maxx_avalancheAI : public ScriptedAI
    {
        npc_maxx_avalancheAI(Creature* creature) : ScriptedAI(creature)
        {
            uiEventTimer = 3000;
            uiPhase = 0;

            switch (me->GetPhaseMask())
            {
            case 4:
                CanCast = false;
                break;
            case 16384:
                CanCast = false;
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                break;
            default:
                CanCast = true;
                break;
            }
        }

        uint32 uiEventTimer;
        uint8 uiPhase;
        bool CanCast;

        void UpdateAI(const uint32 diff)
        {
            if (!CanCast)
                return;

            if (uiEventTimer <= diff)
            {
                ++uiPhase;
                switch (uiPhase)
                {
                case 1:
                    uiEventTimer = 2000;
                    me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                    if (Creature* target = me->FindNearestCreature(NPC_FIZZ_LIGHTER, 30.0f))
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        Position pos;
                        target->GetRandomNearPosition(pos, 3.0f);
                        me->CastSpell(pos.m_positionX, pos.m_positionY, pos.m_positionZ, SPELL_LIGHTNING_BOLT_FOR_FIZZ, false);
                    }
                    break;
                case 2:
                    uiEventTimer = 2000;
                    me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                    if (Creature* target = me->FindNearestCreature(NPC_EVOL_FINGERS, 30.0f))
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        Position pos;
                        target->GetRandomNearPosition(pos, 3.0f);
                        me->CastSpell(pos.m_positionX, pos.m_positionY, pos.m_positionZ, SPELL_LIGHTNING_BOLT_FOR_EVOL, false);
                    }
                    break;
                case 3:
                    uiEventTimer = urand(20000, 80000);
                    uiPhase = 0;
                    uint8 roll = urand(0, 6);
                    DoScriptText(MAXX_AVALANCHE_RANDOM_SPEACH - roll, me);
                    break;
                }
            } else
                uiEventTimer -= diff;
        }
    };
};

class npc_fizz_lighter : public CreatureScript
{
public:
    npc_fizz_lighter() : CreatureScript("npc_fizz_lighter") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ARCANE_MISSILES)
            creature->DoPersonalScriptText(FIZZ_LIGHTER_QUEST_ACCEPT, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_MAGE)
        {
            player->SEND_GOSSIP_MENU(50011, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fizz_lighterAI(creature);
    }

    struct npc_fizz_lighterAI : public Scripted_NoMovementAI
    {
        npc_fizz_lighterAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
            uiCastTimer = 3000;

            switch (me->GetPhaseMask())
            {
            case 4:
                CanCast = false;
                break;
            case 16384:
                CanCast = false;
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                break;
            default:
                CanCast = true;
                break;
            }
        }

        uint32 uiCastTimer;
        bool CanCast;

        void UpdateAI(const uint32 diff)
        {
            if (CanCast)
                if (uiCastTimer <= diff)
                {
                    uiCastTimer = 3000;
                    DoCast(SPELL_FIREBALL);
                } else
                    uiCastTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_evol_fingers : public CreatureScript
{
public:
    npc_evol_fingers() : CreatureScript("npc_evol_fingers") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_IMMOLATE)
            creature->DoPersonalScriptText(EVOL_FINGERS_QUEST_ACCEPT, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_WARLOCK)
        {
            player->SEND_GOSSIP_MENU(50013, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_evol_fingersAI(creature);
    }

    struct npc_evol_fingersAI : public Scripted_NoMovementAI
    {
        npc_evol_fingersAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            SetCombatMovement(false);
            uiCastTimer = 3000;

            switch (me->GetPhaseMask())
            {
            case 4:
                CanCast = false;
                break;
            case 16384:
                CanCast = false;
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                break;
            default:
                CanCast = true;
                break;
            }
        }

        uint32 uiCastTimer;
        bool CanCast;

        void UpdateAI(const uint32 diff)
        {
            if (CanCast)
                if (uiCastTimer <= diff)
                {
                    if (Creature* target = me->FindNearestCreature(NPC_FIZZ_LIGHTER, 40.0f))
                        DoCast(target, SPELL_SHADOW_BOLT);

                    uiCastTimer = 3000;
                } else
                    uiCastTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_bamm_megabomb : public CreatureScript
{
public:
    npc_bamm_megabomb() : CreatureScript("npc_bamm_megabomb") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_STEADY_SHOT)
            creature->DoPersonalScriptText(BAMM_MEGABOMB_QUEST_ACCEPT, player);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bamm_megabombAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_HUNTER)
        {
            player->SEND_GOSSIP_MENU(14798, creature->GetGUID());
            return true;
        }

        return false;
    }

    struct npc_bamm_megabombAI : public ScriptedAI
    {
        npc_bamm_megabombAI(Creature* creature) : ScriptedAI(creature)
        {
            uiCastTimer = urand(3500, 5000);

            switch (me->GetPhaseMask())
            {
            case 4:
                CanCast = false;
                break;
            case 16384:
                CanCast = false;
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                break;
            default:
                CanCast = true;
                break;
            }
        }

        uint32 uiCastTimer;
        bool CanCast;

        void UpdateAI(const uint32 diff)
        {
            if (CanCast)
                if (uiCastTimer <= diff)
                {
                    uiCastTimer = urand(3500, 5000);
                    DoCast(SPELL_SHOOT_GUN);
                } else
                    uiCastTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_slinky_sharpshiv : public CreatureScript
{
public:
    npc_slinky_sharpshiv() : CreatureScript("npc_slinky_sharpshiv") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_EVISCERATE:
            creature->DoPersonalScriptText(SLINKY_SHARPSHIV_QUEST_ACCEPT, player);
            break;
        }

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_WALTZ_RIGHT_IN)
            if (player->GetQuestStatus(QUEST_THE_GREAT_BANK_HEIST) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_LIBERATE_THE_KAJAMITE) == QUEST_STATUS_REWARDED)
            {
                //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_5);
                //player->AddAura(PHASE_QUEST_ZONE_SPECIFIC_6, player);
                player->SaveToDB();
            }

            return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_ROGUE)
        {
            player->PrepareQuestMenu(creature->GetGUID());
            player->SEND_GOSSIP_MENU(14811, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slinky_sharpshivAI(creature);
    }

    struct npc_slinky_sharpshivAI : public ScriptedAI
    {
        npc_slinky_sharpshivAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        }

        void UpdateAI(const uint32 diff)
        { }
    };
};

class npc_warrior_matic_nx_01 : public CreatureScript
{
public:
    npc_warrior_matic_nx_01() : CreatureScript("npc_warrior_matic_nx_01") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CHARGE)
            creature->DoPersonalScriptText(WARRIOR_MATIC_NX_01_QUEST_ACCEPT, player);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() != CLASS_WARRIOR)
        {
            player->SEND_GOSSIP_MENU(50015, creature->GetGUID());
            return true;
        }

        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warrior_matic_nx_01AI(creature);
    }

    struct npc_warrior_matic_nx_01AI : public ScriptedAI
    {
        npc_warrior_matic_nx_01AI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
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
// Quests:
//     Taking Care of Business 14138,
//     Kaja'Cola 25473,
//     Megs in Marketing 28349,
///////////

class npc_sassy_sardwrench : public CreatureScript
{
public:
    npc_sassy_sardwrench() : CreatureScript("npc_sassy_sardwrench") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 uiGossipId = 17571;

        if ((player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_INCOMPLETE) &&
            player->GetQuestStatus(QUEST_PIRATE_PARTY_CRASHERS) == QUEST_STATUS_NONE)
            uiGossipId = 17572;

        if ((player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_INCOMPLETE) &&
            player->GetQuestStatus(QUEST_PIRATE_PARTY_CRASHERS) == QUEST_STATUS_NONE)
            uiGossipId = 17572;

        if ((player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_COMPLETE) &&
            player->GetQuestStatus(QUEST_PIRATE_PARTY_CRASHERS) & (QUEST_STATUS_NONE | QUEST_STATUS_INCOMPLETE))
            uiGossipId = 17573;

        if ((player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_COMPLETE) &&
            player->GetQuestStatus(QUEST_PIRATE_PARTY_CRASHERS) & (QUEST_STATUS_NONE | QUEST_STATUS_INCOMPLETE))
            uiGossipId = 17573;

        if ((player->GetQuestStatus(QUEST_PIRATE_PARTY_CRASHERS) == QUEST_STATUS_COMPLETE) &&
            player->GetQuestStatus(QUEST_A_BAZILLION_MACAROONS) == QUEST_STATUS_NONE)
            uiGossipId = 17574;

        if (player->GetQuestStatus(QUEST_A_BAZILLION_MACAROONS) & (QUEST_STATUS_COMPLETE | QUEST_STATUS_INCOMPLETE))
            uiGossipId = 17575;

        if (player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_COMPLETE &&
            player->GetQuestStatus(QUEST_THE_GREAT_BANK_HEIST) == QUEST_STATUS_COMPLETE &&
            player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_COMPLETE &&
            player->GetQuestStatus(QUEST_LIBERATE_THE_KAJAMITE) == QUEST_STATUS_COMPLETE)
            uiGossipId = 17576;

        if (player->GetQuestStatus(QUEST_LIFE_SAVINGS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GO_TO_THE_BOAT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->PrepareQuestMenu(creature->GetGUID());
        player->SEND_GOSSIP_MENU(uiGossipId, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            float x, y, z = creature->GetPositionZ();
            creature->GetNearPoint2D(x, y, 3.0f, creature->GetOrientation() + M_PI / 4);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_8, false);
            player->DestroyItemCount(ITEM_HOTROAD_KEY, 1, true);

            if (Creature* hotrod = player->SummonCreature(NPC_HOT_ROD_LS, x, y, z))
                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, hotrod->AI()))
                {
                    if (Creature* sassy = player->SummonCreature(NPC_SASSY_HARDWRENCH, x, y, z))
                        sassy->EnterVehicle(hotrod, 0);

                    player->EnterVehicle(hotrod, 1);
                    hotrod->EnableAI();
                    escort->AddWaypoint(1, -8425.38f, 1328.87f, 101.956f);
                    escort->AddWaypoint(2, -8421.79f, 1333.12f, 102.364f);
                    escort->AddWaypoint(3, -8412.47f, 1335.64f, 102.326f);
                    escort->AddWaypoint(4, -8400.14f, 1337.37f, 101.982f);
                    escort->AddWaypoint(5, -8376.50f, 1348.69f, 101.975f);
                    escort->AddWaypoint(6, -8364.13f, 1347.29f, 100.312f);
                    escort->AddWaypoint(7, -8354.41f, 1343.26f, 95.8217f);
                    escort->AddWaypoint(8, -8344.45f, 1338.25f, 89.7618f);
                    escort->AddWaypoint(9, -8339.29f, 1335.17f, 87.6991f);
                    escort->AddWaypoint(10, -8333.11f, 1331.05f, 87.0341f);
                    escort->AddWaypoint(11, -8321.08f, 1321.99f, 87.9551f);
                    escort->AddWaypoint(12, -8315.27f, 1317.73f, 87.2524f);
                    escort->AddWaypoint(13, -8309.10f, 1314.06f, 84.8207f);
                    escort->AddWaypoint(14, -8303.91f, 1311.26f, 82.2196f);
                    escort->AddWaypoint(15, -8297.86f, 1308.01f, 78.5731f);
                    escort->AddWaypoint(16, -8294.21f, 1306.56f, 75.9687f);
                    escort->AddWaypoint(17, -8292.03f, 1305.61f, 73.5784f);
                    escort->AddWaypoint(18, -8287.09f, 1303.27f, 68.7732f);
                    escort->AddWaypoint(19, -8282.96f, 1301.28f, 65.0758f);
                    escort->AddWaypoint(20, -8276.19f, 1299.01f, 59.8176f);
                    escort->AddWaypoint(21, -8271.50f, 1297.45f, 56.4913f);
                    escort->AddWaypoint(22, -8264.58f, 1295.90f, 51.9402f);
                    escort->AddWaypoint(23, -8259.09f, 1294.71f, 48.8402f);
                    escort->AddWaypoint(24, -8251.53f, 1293.56f, 45.0274f);
                    escort->AddWaypoint(25, -8246.01f, 1292.83f, 42.5180f);
                    escort->AddWaypoint(26, -8238.32f, 1292.11f, 39.2295f);
                    escort->AddWaypoint(27, -8230.16f, 1291.58f, 36.3618f);
                    escort->AddWaypoint(28, -8224.49f, 1291.36f, 34.6277f);
                    escort->AddWaypoint(29, -8217.40f, 1291.24f, 32.7357f);
                    escort->AddWaypoint(30, -8210.30f, 1291.24f, 31.1584f);
                    escort->AddWaypoint(31, -8204.61f, 1291.38f, 30.1318f);
                    escort->AddWaypoint(32, -8197.20f, 1291.88f, 29.1144f);
                    escort->AddWaypoint(33, -8190.99f, 1292.30f, 28.6688f);
                    escort->AddWaypoint(34, -8183.25f, 1293.64f, 27.2814f);
                    escort->AddWaypoint(35, -8174.77f, 1294.94f, 26.0061f);
                    escort->AddWaypoint(36, -8167.61f, 1295.76f, 25.0049f);
                    escort->AddWaypoint(37, -8159.68f, 1296.62f, 24.0758f);
                    escort->AddWaypoint(38, -8153.06f, 1297.29f, 23.2302f);
                    escort->AddWaypoint(39, -8146.75f, 1297.82f, 22.2031f);
                    escort->AddWaypoint(40, -8139.64f, 1297.23f, 20.8587f);
                    escort->AddWaypoint(41, -8135.00f, 1297.57f, 20.5206f);
                    escort->AddWaypoint(42, -8130.32f, 1300.50f, 19.9457f);
                    escort->AddWaypoint(43, -8127.43f, 1306.52f, 19.3136f);
                    escort->AddWaypoint(44, -8126.20f, 1314.47f, 18.7197f);
                    escort->AddWaypoint(45, -8125.68f, 1329.52f, 17.8607f);
                    escort->AddWaypoint(46, -8124.89f, 1341.55f, 17.1133f);
                    escort->AddWaypoint(47, -8124.27f, 1348.29f, 17.3852f);
                    escort->AddWaypoint(48, -8123.51f, 1359.30f, 19.3157f);
                    escort->AddWaypoint(49, -8122.60f, 1371.38f, 22.2654f);
                    escort->AddWaypoint(50, -8122.03f, 1377.20f, 23.1136f);
                    escort->AddWaypoint(51, -8121.24f, 1383.81f, 23.6746f);
                    escort->AddWaypoint(52, -8120.48f, 1389.00f, 23.7922f);
                    escort->AddWaypoint(53, -8119.60f, 1395.60f, 23.3315f);
                    escort->AddWaypoint(54, -8118.66f, 1400.76f, 22.4183f);
                    escort->AddWaypoint(55, -8117.30f, 1407.06f, 20.4558f);
                    escort->AddWaypoint(56, -8116.07f, 1412.83f, 18.1914f);
                    escort->AddWaypoint(57, -8114.92f, 1418.72f, 16.1875f);
                    escort->AddWaypoint(58, -8113.23f, 1429.29f, 13.0687f);
                    escort->AddWaypoint(59, -8112.00f, 1436.06f, 11.5173f);
                    escort->AddWaypoint(60, -8110.77f, 1443.05f, 10.2880f);
                    escort->AddWaypoint(61, -8109.64f, 1449.17f, 9.81804f, 500);
                    escort->AddWaypoint(62, -8108.60f, 1456.10f, 9.98186f);
                    escort->AddWaypoint(63, -8108.21f, 1461.50f, 10.0528f);
                    escort->AddWaypoint(64, -8112.41f, 1467.96f, 10.0984f);
                    escort->AddWaypoint(65, -8119.51f, 1478.24f, 10.4105f);
                    escort->AddWaypoint(66, -8121.80f, 1485.42f, 10.6638f);
                    escort->AddWaypoint(67, -8122.04f, 1492.62f, 11.1742f);
                    escort->AddWaypoint(68, -8121.35f, 1500.88f, 10.5241f);
                    escort->AddWaypoint(69, -8118.24f, 1509.03f, 10.3345f);
                    escort->AddWaypoint(70, -8114.80f, 1516.22f, 10.0675f);
                    escort->AddWaypoint(71, -8107.86f, 1526.28f, 9.52589f);
                    escort->AddWaypoint(72, -8101.88f, 1535.21f, 9.34943f);
                    escort->AddWaypoint(73, -8099.31f, 1546.64f, 9.12364f);
                    escort->AddWaypoint(74, -8098.55f, 1556.10f, 8.94064f);
                    escort->AddWaypoint(75, -8096.11f, 1625.51f, 8.78228f);
                    escort->AddWaypoint(76, -8096.47f, 1649.65f, 10.0208f);
                    escort->AddWaypoint(77, -8096.74f, 1663.73f, 11.5271f);
                    escort->AddWaypoint(78, -8103.78f, 1691.23f, 15.3531f);
                    escort->AddWaypoint(79, -8109.90f, 1696.20f, 16.6564f);
                    escort->AddWaypoint(80, -8113.23f, 1698.49f, 18.9307f);
                    escort->AddWaypoint(81, -8115.83f, 1700.98f, 20.8800f);
                    escort->AddWaypoint(82, -8119.49f, 1704.42f, 23.4167f);
                    escort->AddWaypoint(83, -8124.64f, 1709.90f, 26.8272f);
                    escort->AddWaypoint(84, -8128.63f, 1715.32f, 29.3983f);
                    escort->AddWaypoint(85, -8132.33f, 1721.63f, 31.6928f);
                    escort->AddWaypoint(86, -8134.51f, 1726.81f, 33.0131f);
                    escort->AddWaypoint(87, -8136.43f, 1732.03f, 34.0754f);
                    escort->AddWaypoint(88, -8137.48f, 1738.16f, 34.8704f);
                    escort->AddWaypoint(89, -8137.91f, 1743.78f, 35.1258f);
                    escort->AddWaypoint(90, -8137.37f, 1748.61f, 34.9439f);
                    escort->AddWaypoint(91, -8136.48f, 1754.65f, 34.5787f);
                    escort->AddWaypoint(92, -8134.38f, 1759.58f, 33.9260f);
                    escort->AddWaypoint(93, -8131.15f, 1765.65f, 32.8294f);
                    escort->AddWaypoint(94, -8127.14f, 1770.74f, 31.4358f);
                    escort->AddWaypoint(95, -8121.74f, 1776.90f, 29.6606f);
                    escort->AddWaypoint(96, -8116.82f, 1781.36f, 28.4168f);
                    escort->AddWaypoint(97, -8111.85f, 1784.92f, 28.7040f);
                    escort->AddWaypoint(98, -8108.42f, 1787.96f, 29.3762f);
                    escort->AddWaypoint(99, -8104.37f, 1792.24f, 30.3825f);
                    escort->AddWaypoint(100, -8101.48f, 1796.07f, 31.3296f);
                    escort->AddWaypoint(101, -8098.53f, 1800.13f, 32.4053f);
                    escort->AddWaypoint(102, -8095.65f, 1804.58f, 33.6369f);
                    escort->AddWaypoint(103, -8093.22f, 1808.60f, 34.8034f);
                    escort->AddWaypoint(104, -8090.76f, 1812.98f, 36.3530f);
                    escort->AddWaypoint(105, -8088.47f, 1816.71f, 37.5583f);
                    escort->AddWaypoint(106, -8084.40f, 1823.03f, 39.7250f);
                    escort->AddWaypoint(107, -8077.03f, 1824.59f, 41.3678f);
                    escort->AddWaypoint(108, -8070.16f, 1822.37f, 42.4776f);
                    escort->AddWaypoint(109, -8062.44f, 1821.97f, 42.8076f);
                    escort->AddWaypoint(110, -8056.33f, 1822.18f, 42.8170f);
                    escort->AddWaypoint(111, -8049.89f, 1822.38f, 42.5627f);
                    escort->AddWaypoint(112, -8043.23f, 1822.53f, 42.2247f);
                    escort->AddWaypoint(113, -8034.60f, 1822.42f, 39.8810f);
                    escort->AddWaypoint(114, -8026.67f, 1821.64f, 36.1786f);
                    escort->AddWaypoint(115, -8014.71f, 1820.55f, 29.8471f);
                    escort->AddWaypoint(116, -8000.99f, 1819.48f, 24.6067f);
                    escort->AddWaypoint(117, -7993.08f, 1822.92f, 21.2857f);
                    escort->AddWaypoint(118, -7983.36f, 1827.66f, 16.8506f);
                    escort->AddWaypoint(119, -7976.36f, 1830.73f, 13.8622f);
                    escort->AddWaypoint(120, -7968.22f, 1832.91f, 11.6773f);
                    escort->AddWaypoint(121, -7960.49f, 1833.61f, 10.2075f);
                    escort->AddWaypoint(122, -7953.07f, 1836.06f, 8.50395f);
                    escort->AddWaypoint(123, -7949.61f, 1835.68f, 6.69894f);
                    escort->AddWaypoint(124, -7918.63f, 1835.01f, 5.54989f);
                    escort->AddWaypoint(125, -7884.50f, 1836.57f, 4.28574f);
                    escort->AddWaypoint(126, -7849.60f, 1845.08f, 7.19778f, 0, true);
                    escort->SetSpeedZ(21.0f);
                    escort->Start(true);
                }
        }
        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_KAJA_COLA:
            creature->DoPersonalScriptText(SASSY_SAY_LATER, player);
            break;
        case QUEST_LIFE_OF_THE_PARTY_MALE:
        case QUEST_LIFE_OF_THE_PARTY_FEMALE:
            player->RemoveAura(SPELL_AWESOME_PARTY_ENSEMBLE);
            player->RemoveAura(SPELL_LOTP_OUTFIT_SECONDARY);
            player->RemoveAura(SPELL_LOTP_OUTFIT_FEMALE);
            player->RemoveAura(SPELL_LOTP_OUTFIT_MALE);
            player->RemoveAura(SPELL_LOTP_PAPARAZZI);
            //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_1);
            //player->AddAura(PHASE_QUEST_ZONE_SPECIFIC_2, player);
            break;
        case QUEST_PIRATE_PARTY_CRASHERS:
            //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_2);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_3, false); // wtf? why dont work?
            break;
        case QUEST_A_BAZILLION_MACAROONS:
            //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_4);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_5, false);
            break;
        case QUEST_THE_GREAT_BANK_HEIST:
            if (player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_LIBERATE_THE_KAJAMITE) == QUEST_STATUS_REWARDED)
            {
                //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_5);
                //player->AddAura(PHASE_QUEST_ZONE_SPECIFIC_6, player);
                player->SaveToDB();
            }
            break;
        }

        return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_TAKING_CARE_OF_BUSINESS:
            creature->DoPersonalScriptText(SASSY_SAY_LOL, player);
            break;
        case QUEST_DO_IT_YOURSELF:
            creature->DoPersonalScriptText(SASSY_SAY_HELP_HIM, player);
            break;
        case QUEST_447:
            if (player->GetPhaseMask() == 8192)
                player->CompleteQuest(QUEST_447);
            else
                creature->DoPersonalScriptText(SASSY_SAY_CANT_LOOK, player);
            break;
        case QUEST_LIFE_SAVINGS:
            if (player->GetPhaseMask() == 8192)
                creature->DoPersonalScriptText(SASSY_SAY_GO_TO_THE_GALLYWIX_BOAT, player);
            break;
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sassy_sardwrenchAI(creature);
    }

    struct npc_sassy_sardwrenchAI : public ScriptedAI
    {
        npc_sassy_sardwrenchAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_foreman_dampwick : public CreatureScript
{
public:
    npc_foreman_dampwick() : CreatureScript("npc_foreman_dampwick") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 uiGossipId = 14684;

        if (player->GetQuestStatus(QUEST_GOOD_HELP_IS_HARD_TO_FIND) == QUEST_STATUS_COMPLETE &&
            player->GetQuestStatus(QUEST_TROUBLE_IN_THE_MINES) == QUEST_STATUS_COMPLETE)
            uiGossipId = 14685;

        if (creature->GetPhaseMask() == 4)
            uiGossipId = 14761;

        if (player->GetQuestStatus(QUEST_THE_UNINVITED_GUEST) & (QUEST_STATUS_COMPLETE | QUEST_STATUS_INCOMPLETE))
            uiGossipId = 14762;

        player->PrepareQuestMenu(creature->GetGUID());
        player->SEND_GOSSIP_MENU(uiGossipId, creature->GetGUID());
        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_TAKING_CARE_OF_BUSINESS)
        {
            Psc new_psc;
            new_psc.uiPlayerGUID = player->GetGUID();
            new_psc.uiEventTimer = 0;
            new_psc.uiPhase = 0;
            CAST_AI(npc_foreman_dampwickAI, creature->AI())->lPlayers.push_back(new_psc);
        }

        if (quest->GetQuestId() == QUEST_LIBERATE_THE_KAJAMITE)
            if (player->GetQuestStatus(QUEST_THE_GREAT_BANK_HEIST) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_REWARDED)
            {
                //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_5);
                //player->AddAura(PHASE_QUEST_ZONE_SPECIFIC_6, player);
                player->SaveToDB();
            }

            return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_KAJA_COLA)
            creature->DoPersonalScriptText(DAMPWICK_SAY_BOSS, player);
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_foreman_dampwickAI(creature);
    }

    struct npc_foreman_dampwickAI : public ScriptedAI
    {
        npc_foreman_dampwickAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayers.clear();
            uiRandomYellTimer = 1000;

            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

            if (me->GetPhaseMask() == 1)
            {
                CanYell = true;
                me->setActive(true);
            } else
                CanYell = false;
        }

        std::list<Psc> lPlayers;
        uint32 uiRandomYellTimer;
        bool CanYell;

        void UpdateAI(const uint32 diff)
        {
            if (CanYell)
                if (uiRandomYellTimer <= diff)
                {
                    uiRandomYellTimer = urand(20000, 60000);
                    Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                    if (lPlayers.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        if (Player* player = itr->getSource())
                            if (player->GetPhaseMask() == 1 && player->GetAreaId() == AREA_KAJAMITE_MINE)
                            {
                                uint8 roll = urand(0, 7);
                                me->DoPersonalScriptText(FOREMAN_DAMPWICK_RANDOM_YELL - roll, player, true);
                            }
                } else
                    uiRandomYellTimer -= diff;

                if (!lPlayers.empty())
                    for (std::list<Psc>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                        if ((*itr).uiEventTimer <= diff)
                        {
                            ++(*itr).uiPhase;

                            if (Player* player = Unit::GetPlayer(*me, (*itr).uiPlayerGUID))
                            {
                                switch ((*itr).uiPhase)
                                {
                                case 1:
                                    (*itr).uiEventTimer = 1000;
                                    me->DoPersonalScriptText(DAMPWICK_SAY_NICE, player);
                                    break;
                                case 2:
                                    (*itr).uiEventTimer = 3000;
                                    DoCast(SPELL_SASSYS_INCENTIVE);
                                    me->DoPersonalScriptText(DAMPWICK_YELL_OUCH, player);
                                    break;
                                case 3:
                                    me->DoPersonalScriptText(DAMPWICK_SAY_OK, player);
                                    itr = lPlayers.erase(itr);
                                    break;
                                }
                            } else
                                itr = lPlayers.erase(itr);
                        } else
                        {
                            (*itr).uiEventTimer -= diff;
                            ++itr;
                        }

                        if (!UpdateVictim())
                            return;

                        DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest Good Help is Hard to Find 14069
///////////

class npc_defiant_troll : public CreatureScript
{
public:
    npc_defiant_troll() : CreatureScript("npc_defiant_troll") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defiant_trollAI(creature);
    }

    struct npc_defiant_trollAI : public ScriptedAI
    {
        npc_defiant_trollAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            uint8 roll = urand(0, 3);

            if (roll < 3)
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, TrollRandomEmoteState[roll]);
            else
                me->AddAura(SPELL_SLEEP, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class spell_goblin_all_in_1_der_belt_shocker : public SpellScriptLoader
{
public:
    spell_goblin_all_in_1_der_belt_shocker() : SpellScriptLoader("spell_goblin_all_in_1_der_belt_shocker") { }

    class spell_goblin_all_in_1_der_belt_shocker_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_goblin_all_in_1_der_belt_shocker_SpellScript)

            void DummyEffect(SpellEffIndex effIndex)
        {
            Creature* troll = NULL;

            Unit* caster = GetCaster();

            if (Unit* target = GetExplTargetUnit())
                troll = target->ToCreature();

            if (!troll)
                return;

            if (!(troll->GetEntry() == NPC_DEFIANT_TROLL))
                return;

            caster->ToPlayer()->KilledMonsterCredit(NPC_DEFIANT_TROLL, 0);  // stupid hack
            troll->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            troll->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            troll->RemoveAllAuras();
            troll->DespawnOrUnsummon(5000);
            uint8 roll = urand(0, 7);
            DoScriptText(DEFIANT_TROLL_RANDOM_SAY - roll, troll);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_goblin_all_in_1_der_belt_shocker_SpellScript::DummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_goblin_all_in_1_der_belt_shocker_SpellScript();
    }
};

///////////
// Quest Rolling with my Homies 14071
///////////

const uint32 BoomVisualEffect[12] ={17582, 17583, 7244, 10014, 9991, 8792, 8095, 14577, 18406, 13935, 13933, 13934};

class npc_megs_dreadshredder : public CreatureScript
{
public:
    npc_megs_dreadshredder() : CreatureScript("npc_megs_dreadshredder") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->GetPhaseMask() == 4)
        {
            player->SEND_GOSSIP_MENU(14658, creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ROLLING_WITH_MY_HOMIES)
            if (Creature* hotrod = player->GetVehicleCreatureBase())
            {
                if (hotrod->GetEntry() == NPC_HOT_ROD)
                    if (Vehicle* vehicle = hotrod->GetVehicleKit())
                    {
                        player->KilledMonsterCredit(NPC_HOTROD_KILL_CREDIT, 0);

                        for (int i = 0; i < 3; ++i)
                            if (Unit* passenger = vehicle->GetPassenger(i + 1))
                                player->KilledMonsterCredit(InvisHomies[i], 0);
                            else
                                player->AddAura(DetectSpells[i], player);

                        return true;
                    }
            } else
            {
                for (int i = 0; i < 3; ++i)
                    player->CastSpell(player, DetectSpells[i], false);

                creature->Whisper(DREADSHREDDER_SAY_USE_KEYS, player->GetGUID(), true);
                creature->DoPersonalScriptText(DREADSHREDDER_SAY_YOU_LIKE_IT, player);
            }

            return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ROLLING_WITH_MY_HOMIES)
            creature->DoPersonalScriptText(DREADSHREDDER_SAY_FINISH, player);

        if (quest->GetQuestId() == QUEST_ROBBING_HOODS)
            if (player->GetQuestStatus(QUEST_THE_GREAT_BANK_HEIST) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_LIBERATE_THE_KAJAMITE) == QUEST_STATUS_REWARDED)
            {
                //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_5);
                //player->AddAura(PHASE_QUEST_ZONE_SPECIFIC_6, player);
                player->SaveToDB();
            }

            return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_megs_dreadshredderAI(creature);
    }

    struct npc_megs_dreadshredderAI : public ScriptedAI
    {
        npc_megs_dreadshredderAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_homies_follower : public CreatureScript
{
public:
    npc_homies_follower() : CreatureScript("npc_homies_follower") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ROLLING_WITH_MY_HOMIES)
            if (Creature* hotrod = player->GetVehicleCreatureBase())
                if (hotrod->GetEntry() == NPC_HOT_ROD)
                    if (Vehicle* vehicle = hotrod->GetVehicleKit())
                    {
                        player->KilledMonsterCredit(NPC_HOTROD_KILL_CREDIT, 0);

                        for (int i = 0; i < 3; ++i)
                            if (Unit* passenger = vehicle->GetPassenger(i + 1))
                                player->KilledMonsterCredit(InvisHomies[i], 0);
                            else
                                player->AddAura(DetectSpells[i], player);

                        return true;
                    }

                    return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_homies_followerAI(creature);
    }

    struct npc_homies_followerAI : public FollowerAI
    {
        npc_homies_followerAI(Creature* creature) : FollowerAI(creature)
        {
            Follow = false;
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        bool Follow;

        void EnterEvadeMode()
        {
            FollowerAI::EnterEvadeMode();
        }

        void UpdateAI(const uint32 diff)
        {
            if (Follow)
            {
                Follow = false;

                if (me->isSummon())
                    if (Player* player = me->ToTempSummon()->GetSummoner()->ToPlayer())
                    {
                        float angle = 0.0f;

                        switch (me->GetEntry())
                        {
                        case NPC_ACE:
                            angle = M_PI + M_PI / 4;
                            break;
                        case NPC_GOBBER:
                            angle = M_PI;
                            break;
                        case NPC_IZZY:
                            angle = M_PI - M_PI / 4;
                            break;
                        }

                        StartFollow(player, 0, NULL, angle, 0.5f);
                    }
            }

            FollowerAI::UpdateAI(diff);
        }
    };
};

class npc_hot_rod : public CreatureScript
{
public:
    npc_hot_rod() : CreatureScript("npc_hot_rod") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hot_rodAI(creature);
    }

    struct npc_hot_rodAI : public ScriptedAI
    {
        npc_hot_rodAI(Creature* creature) : ScriptedAI(creature)
        {
            uiSpeedTimer = 100;
            uiSayTimer = 5000;
            uiInWaterTimer = 1000;
            Speed = 0.5f;
            Boost = 0.0f;
            IsBoost = false;
            Moving = false;
            Event = false;
            Despawn = false;
            Boom = false;
            Say = false;
            InWater = false;
            me->AddAura(SPELL_DOWN_IN_THE_DEEPS_SPEEDBARAGE, me);
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        uint32 uiSpeedTimer;
        uint32 uiSayTimer;
        uint32 uiInWaterTimer;
        float Speed;
        float Boost;
        bool IsBoost;
        bool Moving;
        bool Event;
        bool Despawn;
        bool Boom;
        bool Say;
        bool InWater;

        void Reset()
        {
            if (Boom)
                me->AddAura(SPELL_BOOM_INVIS_MODEL, me);
        }

        void DamageTaken(Unit* /*done_by*/, uint32 &damage)
        {
            damage = 0;
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CREATE_STOLEN_LOOT)
                if (Vehicle* vehicle = me->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Player* player = passenger->ToPlayer())
                        {
                            if (player->HasItemCount(ITEM_STOLEN_LOOT, 12))
                                return;

                            ItemPosCountVec dest;
                            uint32 no_space = 0;
                            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_STOLEN_LOOT, 1, &no_space);

                            if (msg != EQUIP_ERR_OK)
                                return;

                            Item* item = player->StoreNewItem(dest, ITEM_STOLEN_LOOT, true, Item::GenerateItemRandomPropertyId(ITEM_STOLEN_LOOT));

                            if (!item)
                            {
                                player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
                                return;
                            }

                            if (item)
                                player->SendNewItem(item, 1, true, true);
                        }

                        if (!Boom && spell->Id == SPELL_ROCKET)
                            if (Vehicle* vehicle = me->GetVehicleKit())
                                if (Unit* passenger = vehicle->GetPassenger(0))
                                {
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    Boom = true;
                                    passenger->ExitVehicle();
                                    DoCast(SPELL_BOOM_INVIS_MODEL);

                                    for (int i = 0; i < 12; ++i)
                                        me->SendPlaySpellVisualKit(BoomVisualEffect[i], 0);
                                }
        }

        void PressureCitizensEvent(uint8 s_roll, uint32 text_id)
        {
            Say = true;
            uint8 h_roll = urand(1, 3);

            if (Vehicle* vehicle = me->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(h_roll))
                    if (Creature* homies = passenger->ToCreature())
                        if (Unit* passenger_player = vehicle->GetPassenger(0))
                            if (Player* player = passenger_player->ToPlayer())
                            {
                                uint8 roll = urand(0, s_roll);
                                homies->DoPersonalScriptText(text_id - roll, player);
                            }
        }

        void SetBoost(bool apply)
        {
            if (apply)
            {
                IsBoost = true;
                Boost = Speed / 2;
                me->SetSpeed(MOVE_RUN, Speed + Boost, true);
            } else
            {
                IsBoost = false;
                Boost = 0.0f;
                me->SetSpeed(MOVE_RUN, Speed, true);
            }
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!(seatId == 0 && !apply))
                return;

            Player* player = who->ToPlayer();

            if (!player)
                return;

            Despawn = true;

            if (Vehicle* vehicle = me->GetVehicleKit())
                for (int i = 1; i < 4; ++i)
                    if (Unit* passenger = vehicle->GetPassenger(i))
                        if (Creature* homies = passenger->ToCreature())
                            homies->DespawnOrUnsummon();
        }

        void OnCharmed(bool /*charm*/)
        {
            me->IsAIEnabled = true;
            me->NeedChangeAI = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Say)
                if (uiSayTimer <= diff)
                {
                    Say = false;
                    uiSayTimer = 5000;
                } else
                    uiSayTimer -= diff;

                if (Despawn)
                {
                    Despawn = false;
                    uint16 uiDespawnTimer = 0;

                    if (Boom)
                        uiDespawnTimer = 5000;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                            {
                                player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
                                player->AddAura(SPELL_KEYS_TO_THE_HOT_ROD, player);
                            }

                            me->DespawnOrUnsummon(uiDespawnTimer);
                }

                if (!Moving && Event)
                {
                    Event = false;
                    Vehicle* vehicle = me->GetVehicleKit();

                    if (!vehicle)
                        return;

                    Player* player = NULL;

                    if (Unit* passenger = vehicle->GetPassenger(0))
                        player = passenger->ToPlayer();

                    if (!player)
                        return;

                    QuestStatusMap::iterator itr = player->getQuestStatusMap().find(QUEST_ROLLING_WITH_MY_HOMIES);

                    if (itr == player->getQuestStatusMap().end())
                        return;

                    QuestStatusData& q_status = itr->second;
                    Quest const* qInfo = sObjectMgr->GetQuestTemplate(QUEST_ROLLING_WITH_MY_HOMIES);

                    if (!qInfo)
                        return;

                    if (q_status.Status == QUEST_STATUS_INCOMPLETE)
                        for (uint32 i = 0; i < 3; ++i)
                            if (q_status.CreatureOrGOCount[i + 1] == 0)
                                if (Creature* invis_homies = player->FindNearestCreature(InvisHomies[i], 10.0f))
                                {
                                    player->KilledMonsterCredit(InvisHomies[i], 0);
                                    player->RemoveAura(DetectSpells[i]);
                                    float x, y, z, o;
                                    invis_homies->GetPosition(x, y, z, o);

                                    if (Creature* homies = player->SummonCreature(Homies[i], x, y, z, o))
                                    {
                                        homies->SetSeerGUID(player->GetGUID());
                                        homies->SetVisible(false);
                                        homies->EnterVehicle(me, i + 1);
                                    }
                                }
                }

                if (uiSpeedTimer <= diff)
                {
                    uiSpeedTimer = 100;

                    if (me->IsInWater())
                        InWater = true;

                    if (me->isMoving())
                    {
                        DoCast(SPELL_HOT_ROD_KNOCKBACK);
                        Moving = true;
                        Event = true;

                        if (Speed < 3.0f)
                        {
                            if (IsBoost)
                            {
                                Boost = Speed / 2;
                                Speed = Speed + 0.2;
                            } else
                                Speed = Speed + 0.1;

                            me->SetSpeed(MOVE_RUN, Speed + Boost, true);
                        }
                    } else
                    {
                        Moving = false;

                        if (Speed > 0.5f)
                        {
                            Speed = 0.5f;
                            Boost = 0.0f;
                            me->SetSpeed(MOVE_RUN, Speed, true);
                        }
                    }
                } else
                    uiSpeedTimer -= diff;

                if (InWater)
                    if (uiInWaterTimer <= diff)
                    {
                        InWater = false;
                        uiInWaterTimer = 1000;

                        if (me->IsInWater() && !Boom)
                            if (Vehicle* vehicle = me->GetVehicleKit())
                                if (Unit* passenger = vehicle->GetPassenger(0))
                                {
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    Boom = true;
                                    passenger->ExitVehicle();
                                    DoCast(SPELL_BOOM_INVIS_MODEL);
                                    DoCast(SPELL_COSMETIC_WATER_STEAM_IMPACT);
                                    DoCast(SPELL_WATER_EXPLOSION);
                                    DoCast(SPELL_DUSTY_EXPLOSION);
                                }
                    } else
                        uiInWaterTimer -= diff;
        }
    };
};

class npc_kezan_citizen : public CreatureScript
{
public:
    npc_kezan_citizen() : CreatureScript("npc_kezan_citizen") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kezan_citizenAI(creature);
    }

    struct npc_kezan_citizenAI : public ScriptedAI
    {
        npc_kezan_citizenAI(Creature* creature) : ScriptedAI(creature)
        {
            uiHotRodGUID = 0;
            uiCastTimer = 1500;
            uiEventTimer = 5000;
            uiKnockbackTimer = 5000;
            uiCastPhase = 0;
            uiKnockbackData = DATA_KNOCKBACK_NONE;
            Event = false;
            Cast = false;
            uiRandomEmoteTimer = urand(2000, 5000);
            uiColaCanTimer = urand(4000, 8000);
            CheckPosition();
        }

        uint64 uiHotRodGUID;
        uint32 uiEventTimer;
        uint32 uiCastTimer;
        uint32 uiKnockbackTimer;
        uint32 uiRandomEmoteTimer;
        uint32 uiColaCanTimer;
        uint8 uiCastPhase;
        uint8 uiKnockbackData;
        bool Event;
        bool Cast;
        bool AtTribune;

        void CheckPosition()
        {
            if (me->HasAura(SPELL_PANICKED_CITIZEN_INVIS))
                return;

            Position pos;
            me->GetPosition(&pos);

            for (int i = 0; i < 8; ++i)
            {
                float dist = pos.GetExactDist(CitizenAtTribune[i][0], CitizenAtTribune[i][1], CitizenAtTribune[i][2]);

                if (dist < 7.0f)
                {
                    AtTribune = true;
                    uiKnockbackData = DATA_KNOCKBACK_IMMUNE;
                    return;
                }
            }

            for (int i = 0; i < 12; ++i)
            {
                float dist = pos.GetExactDist(CitizenInBank[i][0], CitizenInBank[i][1], CitizenInBank[i][2]);

                if (dist < 7.0f)
                {
                    uiKnockbackData = DATA_KNOCKBACK_IMMUNE;
                    return;
                }
            }

            AtTribune = false;
        }

        uint32 GetData(uint32 data) const
        {
            if (data == DATA_KNOCKBACK)
                return uiKnockbackData;

            return 0;
        }

        void MoveScare(Unit* caster)
        {
            Event = true;
            me->HandleEmoteCommand(EMOTE_ONESHOT_COWER);
            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
            float h_angle = caster->GetOrientation();
            float m_angle = caster->GetAngle(me);
            float angle = me->GetAngle(caster);
            float x, y, z = me->GetPositionZ();

            if (h_angle - m_angle < 0)
                me->GetNearPoint2D(x, y, 7.0f, angle + M_PI + M_PI / 4);
            else
                me->GetNearPoint2D(x, y, 7.0f, angle + M_PI - M_PI / 4);

            me->GetMotionMaster()->MovePoint(0, x, y, z);
        }

        void MoveKnockback(Creature* hotrod)
        {
            if (hotrod->GetEntry() == NPC_HOT_ROD)
            {
                uint8 a_roll = urand(0, 150);

                if (a_roll < 10)
                {
                    me->setFaction(14);
                    Cast = true;
                    uiHotRodGUID = hotrod->GetGUID();
                    uiCastPhase = 0;
                    uint8 y_roll = urand(0, 4);
                    uiKnockbackData = DATA_KNOCKBACK_ALARM;
                    CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->PressureCitizensEvent(5, HOMIES_RANDOM_ALARM);

                    if (Vehicle* vehicle = hotrod->GetVehicleKit())
                        if (Unit* passenger = vehicle->GetPassenger(0))
                            if (Player* player = passenger->ToPlayer())
                                me->DoPersonalScriptText(CITIZEN_RANDOM_ALARM - y_roll, player);
                } else
                {
                    uiKnockbackData = DATA_KNOCKBACK_KNOCKBACK;
                    uint8 s_roll = urand(0, 13);
                    uint8 e_roll = urand(0, 6);
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, RollingEmoteState[e_roll]);

                    if (!CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->Say)
                    {
                        CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->PressureCitizensEvent(10, HOMIES_RANDOM_SAY);

                        if (Vehicle* vehicle = hotrod->GetVehicleKit())
                            if (Unit* passenger = vehicle->GetPassenger(0))
                                if (Player* player = passenger->ToPlayer())
                                    me->DoPersonalScriptText(CITIZEN_RANDOM_SAY - s_roll, player);
                    }
                }
            } else
                if (hotrod->GetEntry() == NPC_HOT_ROD_LS)
                {
                    uiKnockbackData = DATA_KNOCKBACK_KNOCKBACK;
                    uint8 s_roll = urand(0, 13);
                    uint8 e_roll = urand(0, 6);
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, RollingEmoteState[e_roll]);
                }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_HOT_ROD_HONK_HORN)
                if (!(Event && uiKnockbackData != DATA_KNOCKBACK_NONE || uiKnockbackData == DATA_KNOCKBACK_IMMUNE))
                    MoveScare(caster);
        }

        void UpdateAI(const uint32 diff)
        {
            if (AtTribune)
            {
                if (uiRandomEmoteTimer <= diff)
                {
                    uint8 roll = urand(0, 14);
                    me->HandleEmoteCommand(TribuneEmotes[roll]);
                    uiRandomEmoteTimer = urand(3000, 7000);
                } else
                    uiRandomEmoteTimer -= diff;

                if (uiColaCanTimer <= diff)
                {
                    DoCast(SPELL_COLA_CAN);
                    uiColaCanTimer = urand(4000, 10000);
                } else
                    uiColaCanTimer -= diff;

                return;
            }

            if (Cast)
                if (uiCastTimer <= diff)
                {
                    ++uiCastPhase;

                    if (Unit* hotrod = Unit::GetCreature(*me, uiHotRodGUID))
                        switch (uiCastPhase)
                    {
                        case 1:
                            {
                                uiCastTimer = 500;
                                uint8 e_roll = urand(0, 4);
                                me->HandleEmoteCommand(RollingEmote[e_roll]);
                                me->SetFacingToObject(hotrod);
                            }
                            break;
                        case 2:
                            {
                                uiCastTimer = 500;
                                me->SendPlaySpellVisualKit(SPELL_VISUAL_START_FIRE_ROCKET_1, 0);
                                me->SendPlaySpellVisualKit(SPELL_VISUAL_START_FIRE_ROCKET_2, 0);
                                float x, y, z;
                                hotrod->GetPosition(x, y, z);
                                me->GetMotionMaster()->MoveCharge(x, y, z);
                            }
                            break;
                        case 3:
                            {
                                uiCastTimer = 1500;
                                float x, y, z;
                                hotrod->GetPosition(x, y, z);
                                me->GetMotionMaster()->MoveCharge(x, y, z);
                                me->CastSpell(hotrod, SPELL_ROCKET, false);
                                me->setFaction(2159);
                            }
                            break;
                        case 4:
                            Cast = false;
                            uiCastTimer = 1500;
                            uiKnockbackData = DATA_KNOCKBACK_NONE;
                            me->DespawnOrUnsummon();
                            break;
                    }
                } else
                    uiCastTimer -= diff;

                if (uiKnockbackData == DATA_KNOCKBACK_KNOCKBACK)
                    if (uiKnockbackTimer <= diff)
                    {
                        uiKnockbackTimer = 5000;
                        uiKnockbackData = DATA_KNOCKBACK_NONE;
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                        me->GetMotionMaster()->MoveTargetedHome();
                    } else
                        uiKnockbackTimer -= diff;

                    if (Event)
                        if (uiEventTimer <= diff)
                        {
                            Event = false;
                            uiEventTimer = 5000;
                            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MoveTargetedHome();
                        } else
                            uiEventTimer -= diff;

                        if (!UpdateVictim())
                            return;

                        DoMeleeAttackIfReady();
        }
    };
};

class spell_keys_to_the_hot_rod : public SpellScriptLoader
{
public:
    spell_keys_to_the_hot_rod() : SpellScriptLoader("spell_keys_to_the_hot_rod") { }

    class spell_keys_to_the_hot_rod_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_keys_to_the_hot_rod_AuraScript);

        uint64 uiSummonedHomies[3];

        bool Load()
        {
            memset(&uiSummonedHomies, 0, sizeof(uiSummonedHomies));
            return true;
        }

        void SummonHomies(Player* player, int i)
        {
            if (player->GetVehicle())
                return;

            float x, y, z, o;
            player->GetPosition(x, y, z, o);

            if (Creature* homies = player->SummonCreature(Homies[i], x, y, z, o))
            {
                homies->SetSeerGUID(player->GetGUID());
                homies->SetVisible(false);

                if (Creature* summoned_homies = Unit::GetCreature(*player, uiSummonedHomies[i]))
                    summoned_homies->DespawnOrUnsummon();

                uiSummonedHomies[i] = homies->GetGUID();
                CAST_AI(npc_homies_follower::npc_homies_followerAI, homies->AI())->Follow = true;
            }
        }

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            switch (player->getGender())
            {
            case GENDER_MALE:
                {
                    if (player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_COMPLETE)
                        return;

                    if (player->GetQuestRewardStatus(QUEST_LIFE_OF_THE_PARTY_MALE))
                        return;
                }
                break;
            case GENDER_FEMALE:
                {
                    if (player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_COMPLETE)
                        return;

                    if (player->GetQuestRewardStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE))
                        return;
                }
                break;
            }

            if (player->GetQuestRewardStatus(QUEST_ROLLING_WITH_MY_HOMIES))
            {
                for (int i = 0; i < 3; ++i)
                    SummonHomies(player, i);

                return;
            }

            QuestStatusMap::iterator itr = player->getQuestStatusMap().find(QUEST_ROLLING_WITH_MY_HOMIES);

            if (itr == player->getQuestStatusMap().end())
                return;

            QuestStatusData& q_status = itr->second;

            if (q_status.Status == QUEST_STATUS_INCOMPLETE)
            {
                for (int i = 0; i < 3; ++i)
                    if (q_status.CreatureOrGOCount[i + 1] == 1)
                        SummonHomies(player, i);
            } else
                for (int i = 0; i < 3; ++i)
                    SummonHomies(player, i);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            for (int i = 0; i < 3; ++i)
                if (Creature* summoned_homies = Unit::GetCreature(*player, uiSummonedHomies[i]))
                {
                    summoned_homies->DespawnOrUnsummon();
                    uiSummonedHomies[i] = 0;
                }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_keys_to_the_hot_rod_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_keys_to_the_hot_rod_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_keys_to_the_hot_rod_AuraScript();
    }
};

class spell_summon_hot_rod : public SpellScriptLoader
{
public:
    spell_summon_hot_rod() : SpellScriptLoader("spell_summon_hot_rod") { }

    class spell_summon_hot_rod_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_hot_rod_SpellScript)

            void SummonHomies(Player* summoner, Creature* hotrod, int i)
        {
            float x, y, z, o;
            summoner->GetPosition(x, y, z, o);

            if (Creature* homies = summoner->SummonCreature(Homies[i - 1], x, y, z, o))
            {
                homies->SetSeerGUID(summoner->GetGUID());
                homies->SetVisible(false);
                homies->EnterVehicle(hotrod, i);
            }
        }

        void SummonHotRod(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            float x, y, z, o;
            player->GetPosition(x, y, z, o);

            if (Creature* hotrod = player->SummonCreature(NPC_HOT_ROD, x, y, z, o))
            {
                if (Pet* pet = player->GetPet())
                    player->CastSpell(player, 36562, false);

                player->EnterVehicle(hotrod, 0);
                hotrod->EnableAI();
                hotrod->CombatStart(player);
                player->CombatStart(hotrod);
                player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
                player->AddAura(SPELL_KEYS_TO_THE_HOT_ROD, player);
                player->ClearUnitState(UNIT_STATE_ONVEHICLE);

                switch (player->getGender())
                {
                case GENDER_MALE:
                    {
                        if (player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_INCOMPLETE ||
                            player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_COMPLETE)
                            return;

                        if (player->GetQuestRewardStatus(QUEST_LIFE_OF_THE_PARTY_MALE))
                            return;
                    }
                    break;
                case GENDER_FEMALE:
                    {
                        if (player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_INCOMPLETE ||
                            player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_COMPLETE)
                            return;

                        if (player->GetQuestRewardStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE))
                            return;
                    }
                    break;
                }

                if (player->GetQuestRewardStatus(QUEST_ROLLING_WITH_MY_HOMIES))
                {
                    for (int i = 1; i < 4; ++i)
                        SummonHomies(player, hotrod, i);

                    return;
                }

                QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_ROLLING_WITH_MY_HOMIES);

                if (incomplete_quest == player->getQuestStatusMap().end())
                    return;

                QuestStatusData& q_status = incomplete_quest->second;

                for (int i = 1; i < 4; ++i)
                    if (q_status.CreatureOrGOCount[i] == 1)
                        SummonHomies(player, hotrod, i);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_summon_hot_rod_SpellScript::SummonHotRod, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_summon_hot_rod_SpellScript();
    }
};

class spell_punch_it_boost : public SpellScriptLoader
{
public:
    spell_punch_it_boost() : SpellScriptLoader("spell_punch_it_boost") { }

    class spell_punch_it_boost_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_punch_it_boost_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            PreventDefaultAction();

            Creature* hotrod = NULL;

            if (Unit* target = GetTarget())
                hotrod = target->ToCreature();

            if (!hotrod)
                return;

            if (hotrod->GetEntry() != NPC_HOT_ROD)
                return;

            hotrod->SendPlaySpellVisualKit(SPELL_VISUAL_BOOS_1, 0);
            hotrod->SendPlaySpellVisualKit(SPELL_VISUAL_BOOS_2, 0);

            CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->SetBoost(true);
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            PreventDefaultAction();

            Creature* hotrod = NULL;

            if (Unit* target = GetTarget())
                hotrod = target->ToCreature();

            if (!hotrod)
                return;

            if (hotrod->GetEntry() != NPC_HOT_ROD)
                return;

            CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->SetBoost(false);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_punch_it_boost_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_punch_it_boost_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_punch_it_boost_AuraScript();
    }
};

class spell_hot_rod_honk_horn : public SpellScriptLoader
{
public:
    spell_hot_rod_honk_horn() : SpellScriptLoader("spell_hot_rod_honk_horn") { }

    class spell_hot_rod_honk_horn_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hot_rod_honk_horn_SpellScript)

            void Beep(SpellEffIndex effIndex)
        {
            Player* player = NULL;

            if (Unit* caster = GetCaster())
                if (Vehicle* vehicle = caster->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        player = passenger->ToPlayer();

            if (!player)
                return;

            uint8 roll = urand(0, 19);
            LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
            char const* text = sObjectMgr->GetTrinityString(PLAYER_RANDOM_YELL - roll, loc_idx);
            player->Yell(text, LANG_UNIVERSAL);
            roll = urand(0, 3);
            player->HandleEmoteCommand(RollingEmote[roll]);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_hot_rod_honk_horn_SpellScript::Beep, EFFECT_0, SPELL_EFFECT_PLAY_SOUND);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_hot_rod_honk_horn_SpellScript();
    }
};

class spell_hot_rod_knockback : public SpellScriptLoader
{
public:
    spell_hot_rod_knockback() : SpellScriptLoader("spell_hot_rod_knockback") { }

    class spell_hot_rod_knockback_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hot_rod_knockback_SpellScript)

            void Knockback(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Creature* citizen = NULL;
            Creature* hotrod = NULL;

            if (Unit* target = GetHitUnit())
                citizen = target->ToCreature();

            if (Unit* caster = GetCaster())
                hotrod = caster->ToCreature();

            if (!(citizen && hotrod))
                return;

            if (citizen->AI()->GetData(DATA_KNOCKBACK) != DATA_KNOCKBACK_NONE)
                return;

            float speed = CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->Speed;
            float boost = CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->Boost;

            if (speed + boost > 2.5f)
            {
                if (citizen->IsNonMeleeSpellCasted(true))
                    citizen->InterruptNonMeleeSpells(true);

                float x, y;
                hotrod->GetPosition(x, y);
                citizen->KnockbackFrom(x, y, 15.0f, 10.0f);
                CAST_AI(npc_kezan_citizen::npc_kezan_citizenAI, citizen->AI())->MoveKnockback(hotrod);
            } else if (!CAST_AI(npc_hot_rod::npc_hot_rodAI, hotrod->AI())->Event)
                CAST_AI(npc_kezan_citizen::npc_kezan_citizenAI, citizen->AI())->MoveScare(hotrod);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_hot_rod_knockback_SpellScript::Knockback, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_hot_rod_knockback_SpellScript();
    }
};

///////////
// Quests:
//     The Replacements    24488,
//     Necessary Roughness 24502,
//     Fourth and Goal     28414,
///////////

class npc_bilgewater_buccaneer_enter : public CreatureScript
{
public:
    npc_bilgewater_buccaneer_enter() : CreatureScript("npc_bilgewater_buccaneer_enter") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, "Enter on Bilgewater buccaneer, go go go", 631, QUEST_NECESSARY_ROUGHNESS);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        } else if (player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, "Enter on Bilgewater buccaneer, go go go", 631, QUEST_FOURTH_AND_GOAL);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == QUEST_NECESSARY_ROUGHNESS)
        {
            player->CastSpell(player, SPELL_NR_SUMMON_BILGEWATER_BUCCANEER, false);
            return true;
        } else if (action = QUEST_FOURTH_AND_GOAL)
        {
            player->CastSpell(player, SPELL_FaG_SUMMON_BILGEWATER_BUCCANEER, false);
            return true;
        }
        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bilgewater_buccaneer_enterAI(creature);
    }

    struct npc_bilgewater_buccaneer_enterAI : public ScriptedAI
    {
        npc_bilgewater_buccaneer_enterAI(Creature* creature) : ScriptedAI(creature)
        { }
    };
};


class npc_coach_crosscheck : public CreatureScript
{
public:
    npc_coach_crosscheck() : CreatureScript("npc_coach_crosscheck") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) & QUEST_STATUS_COMPLETE)
        {
            player->PrepareQuestMenu(creature->GetGUID());
            player->SEND_GOSSIP_MENU(15122, creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_THE_REPLACEMENTS:
            creature->DoPersonalScriptText(COACH_SAY_GOTTA_HURRY, player);
            break;
        case QUEST_NECESSARY_ROUGHNESS:
            creature->DoPersonalScriptText(COACH_SAY_SHREDDER, player);
            player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_4, player);
            break;
        case QUEST_FOURTH_AND_GOAL:
            player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_4, player);
            break;
        }

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_FOURTH_AND_GOAL)
        {
            player->RemoveAura(SPELL_INCREASED_MOD_DETECTED_RANGE);
            creature->Whisper(PLAYER_SAY_WHAT_IS_THIS, player->GetGUID(), true);
            CAST_AI(npc_coach_crosscheckAI, creature->AI())->StartNewEvent(player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coach_crosscheckAI(creature);
    }

    struct npc_coach_crosscheckAI : public ScriptedAI
    {
        npc_coach_crosscheckAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayersGUID.clear();
            lPlayers.clear();
            lEventPlayers.clear();
        }

        std::list<uint64> lPlayersGUID;
        std::list<QFAG> lPlayers;
        std::list<Psc> lEventPlayers;

        void StartNewEvent(uint64 uiPlayerGUID)
        {
            if (!lEventPlayers.empty())
                for (std::list<Psc>::const_iterator itr = lEventPlayers.begin(); itr != lEventPlayers.end(); ++itr)
                    if (itr->uiPlayerGUID == uiPlayerGUID)
                        return;

            Psc new_psc;
            new_psc.uiPlayerGUID = uiPlayerGUID;
            new_psc.uiPhase = 0;
            new_psc.uiEventTimer = 1000;
            lEventPlayers.push_back(new_psc);
        }

        void FinishDeathwingEvent(uint64 uiPlayerGUID)
        {
            if (lPlayers.empty())
                return;

            for (std::list<QFAG>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                if (itr->uiPlayerGUID == uiPlayerGUID)
                {
                    lPlayers.erase(itr);
                    return;
                } else
                    ++itr;
        }

        void StartDeathwingEvent(Player* player)
        {
            if (!player)
                return;

            uint64 uiPlayerGUID = player->GetGUID();

            if (!lPlayers.empty())
                for (std::list<QFAG>::iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (itr->uiPlayerGUID == uiPlayerGUID)
                        if (Unit::GetCreature(*me, itr->uiDeathwingGUID))
                            return;
                        else
                            itr = lPlayers.erase(itr);
                    else
                        ++itr;

            if (Creature* deathwing = player->SummonCreature(NPC_DEATHWING_QFAG, -8178.59f, 1482.14f, 83.99894f, 3.106686f))
            {
                QFAG new_QFAG;
                new_QFAG.uiPlayerGUID = uiPlayerGUID;
                new_QFAG.uiDeathwingGUID = deathwing->GetGUID();
                lPlayers.push_back(new_QFAG);

                if (npc_vehicle_escortAI* escort = CAST_AI(npc_vehicle_escortAI, deathwing->AI()))
                {
                    escort->AddWaypoint(1, -8200.11f, 1482.16f, 80.1797f);
                    escort->AddWaypoint(2, -8414.07f, 1487.01f, 186.857f);
                    escort->AddWaypoint(3, -8583.05f, 1515.07f, 236.993f);
                    escort->AddWaypoint(4, -8626.69f, 1554.29f, 241.131f);
                    escort->AddWaypoint(5, -8619.00f, 1590.41f, 243.703f);
                    escort->AddWaypoint(6, -8598.17f, 1611.67f, 245.374f);
                    escort->AddWaypoint(7, -8579.87f, 1616.72f, 246.272f);
                    escort->AddWaypoint(8, -8550.54f, 1609.86f, 242.346f);
                    escort->AddWaypoint(9, -8301.08f, 1485.95f, 88.0648f);
                    escort->AddWaypoint(10, -8287.33f, 1484.92f, 85.2588f);
                    escort->AddWaypoint(11, -7800.26f, 1207.90f, 172.478f);
                    escort->Start();
                    deathwing->SetCanFly(true);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!lEventPlayers.empty())
                for (std::list<Psc>::iterator itr = lEventPlayers.begin(); itr != lEventPlayers.end();)
                    if ((*itr).uiEventTimer <= diff)
                    {
                        if (Player* player = Unit::GetPlayer(*me, itr->uiPlayerGUID))
                        {
                            ++(*itr).uiPhase;

                            switch ((*itr).uiPhase)
                            {
                            case 1:
                                {
                                    (*itr).uiEventTimer = 1000;
                                    std::list<Creature*> lHomies;
                                    player->GetCreatureListWithEntryInGrid(lHomies, NPC_ACE, 15.0f);

                                    if (!lHomies.empty())
                                        for (std::list<Creature*>::const_iterator homie = lHomies.begin(); homie != lHomies.end(); ++homie)
                                            if ((*homie)->isSummon())
                                                if (uint64 guid = (*homie)->ToTempSummon()->GetSummonerGUID())
                                                    if (guid == (*itr).uiPlayerGUID)
                                                    {
                                                        (*homie)->DoPersonalScriptText(ACE_SAY_WHAT_IS_THIS, player);
                                                        return;
                                                    }
                                }
                                break;
                            case 2:
                                {
                                    (*itr).uiEventTimer = 1000;
                                    std::list<Creature*> lHomies;
                                    player->GetCreatureListWithEntryInGrid(lHomies, NPC_IZZY, 15.0f);

                                    if (!lHomies.empty())
                                        for (std::list<Creature*>::const_iterator homie = lHomies.begin(); homie != lHomies.end(); ++homie)
                                            if ((*homie)->isSummon())
                                                if (uint64 guid = (*homie)->ToTempSummon()->GetSummonerGUID())
                                                    if (guid == (*itr).uiPlayerGUID)
                                                    {
                                                        (*homie)->DoPersonalScriptText(IZZY_SAY_WHAT_IS_THIS, player);
                                                        return;
                                                    }
                                }
                                break;
                            case 3:
                                me->DoPersonalScriptText(COACH_SAY_GOOD_WORK, player);
                                itr = lEventPlayers.erase(itr);
                                break;
                            }
                        } else
                            itr = lEventPlayers.erase(itr);
                    } else
                        (*itr).uiEventTimer -= diff;


                    if (!lPlayersGUID.empty())
                    {
                        for (std::list<uint64>::iterator itr = lPlayersGUID.begin(); itr != lPlayersGUID.end(); ++itr)
                            if (Player* player = Unit::GetPlayer(*me, (*itr)))
                            {
                                player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
                                player->AddAura(SPELL_KEYS_TO_THE_HOT_ROD, player);
                                player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_4, player);
                                player->RemoveAura(SPELL_INCREASED_MOD_DETECTED_RANGE);
                            }

                            lPlayersGUID.clear();
                    }


                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_steamwheedle_shark : public CreatureScript
{
public:
    npc_steamwheedle_shark() : CreatureScript("npc_steamwheedle_shark") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steamwheedle_sharkAI(creature);
    }

    struct npc_steamwheedle_sharkAI : public npc_escortAI
    {
        npc_steamwheedle_sharkAI(Creature* creature) : npc_escortAI(creature)
        {
            SetDespawnAtFar(false);
            Hit = false;
        }

        bool Hit;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_THROW_FOOTBOMB)
            {
                if (Hit)
                    return;

                if (Vehicle* vehicle = caster->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Player* player = passenger->ToPlayer())
                            if (uint64 guid = me->GetSeerGUID())
                                if (player->GetGUID() == guid)
                                {
                                    Hit = true;
                                    player->PlayDirectSound(SOUND_WOW_1, player);
                                    player->KilledMonsterCredit(NPC_STEAMWHEEDLE_SHARK, me->GetGUID());
                                }

                                me->CastSpell(me, SPELL_PERMANENT_FEIGN_DEATH, false);
                                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_THROW_FOOTBOMB, true);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetSpeed(MOVE_RUN, 0.0f, true);
                                me->SetSpeed(MOVE_WALK, 0.0f, true);
            }
        }

        void WaypointReached(uint32 point)
        {
            if (point == 1)
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Creature* buccaneer = summoner->ToCreature())
                            buccaneer->AI()->DoAction(ACTION_FAIL_NECESSARY_ROUGHNESS);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };
};

class npc_bilgewater_buccaneer : public CreatureScript
{
public:
    npc_bilgewater_buccaneer() : CreatureScript("npc_bilgewater_buccaneer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bilgewater_buccaneerAI(creature);
    }

    struct npc_bilgewater_buccaneerAI : public ScriptedAI
    {
        npc_bilgewater_buccaneerAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
        {
            creature->AddUnitState(UNIT_STATE_ROOT);
            creature->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    me->SetSeerGUID(summoner->GetGUID());
                    me->SetVisible(false);
                }

                uiEventTimer = 2000;
                Event = false;
        }

        SummonList lSummons;
        uint32 uiEventTimer;
        bool Event;

        void OnCharmed(bool /*charm*/)
        {
            me->EnableAI();
        }

        void DoAction(const int32 action)
        {
            if (Vehicle* vehicle = me->GetVehicleKit())
                vehicle->RemoveAllPassengers();
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (seatId != 0)
                return;

            Player* player = who->ToPlayer();

            if (!player)
                return;

            if (apply)
            {
                Event = true;
                player->PlayDirectSound(SOUND_WOW_1, player);
                player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_4);
                player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
                player->AddAura(SPELL_KEYS_TO_THE_HOT_ROD, player);
            } else
            {
                player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_4, player);
                lSummons.DespawnAll();

                if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
                    player->FailQuest(QUEST_NECESSARY_ROUGHNESS);

                if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 30.0f))
                    CAST_AI(npc_coach_crosscheck::npc_coach_crosscheckAI, coach->AI())->lPlayersGUID.push_back(player->GetGUID());
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    uiEventTimer = 2000;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                                if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
                                {
                                    uint64 uiPlayerGUID = player->GetGUID();
                                    me->Whisper(BUCCANEER_SAY_FOOTBOMBS, uiPlayerGUID, true);

                                    for (int i = 0; i < 8; ++i)
                                        if (Creature* shark = me->SummonCreature(NPC_STEAMWHEEDLE_SHARK, SharkSummonPos[i][0], SharkSummonPos[i][1], SharkSummonPos[i][2], SharkSummonPos[i][3], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000))
                                        {
                                            shark->SetSeerGUID(uiPlayerGUID);
                                            shark->SetVisible(false);
                                            lSummons.Summon(shark);

                                            if (npc_escortAI* escort = CAST_AI(npc_escortAI, shark->AI()))
                                                escort->Start(false);
                                        }
                                }
                } else
                    uiEventTimer -= diff;
        }
    };
};

class spell_permanent_feign_death : public SpellScriptLoader
{
public:
    spell_permanent_feign_death() : SpellScriptLoader("spell_permanent_feign_death") { }

    class spell_permanent_feign_deathAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_permanent_feign_deathAuraScript)

            void EffectDeath(AuraEffect const * /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();

            if (!target)
                return;

            target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

            if (target->GetTypeId() == TYPEID_UNIT)
                if (target->GetEntry() == NPC_STEAMWHEEDLE_SHARK)
                    return;

            target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_permanent_feign_deathAuraScript::EffectDeath, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript *GetAuraScript() const
    {
        return new spell_permanent_feign_deathAuraScript();
    }
};

class npc_bilgewater_buccaneer_goal : public CreatureScript
{
public:
    npc_bilgewater_buccaneer_goal() : CreatureScript("npc_bilgewater_buccaneer_goal") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bilgewater_buccaneer_goalAI(creature);
    }

    struct npc_bilgewater_buccaneer_goalAI : public ScriptedAI
    {
        npc_bilgewater_buccaneer_goalAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->AddUnitState(UNIT_STATE_ROOT);
            creature->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            uiSeatData = DATA_NONE;

            if (creature->isSummon())
                if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
                {
                    creature->SetSeerGUID(summoner->GetGUID());
                    creature->SetVisible(false);
                }
        }

        uint8 uiSeatData;

        void SetData(uint32 /*type*/, uint32 data)
        {
            uiSeatData = data;
        }

        void DoAction(const int32 action)
        {
            if (Vehicle* vehicle = me->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(0))
                {
                    uiSeatData = DATA_SEAT_IN_CHANGE;
                    passenger->ChangeSeat(1);
                }
        }

        void Reset()
        {
            me->AddAura(SPELL_INCREASED_MOD_DETECTED_RANGE, me);
            me->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_5, me);
        }

        void OnCharmed(bool /*charm*/)
        {
            me->EnableAI();
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            Player* player = who->ToPlayer();

            if (!player)
                return;

            if (apply)
            {
                if (seatId == 0)
                {
                    player->PlayDirectSound(SOUND_WOW_2, player);
                    player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_4);
                    player->RemoveAura(SPELL_KEYS_TO_THE_HOT_ROD);
                    player->AddAura(SPELL_KEYS_TO_THE_HOT_ROD, player);
                    player->CastSpell(player, SPELL_INCREASED_MOD_DETECTED_RANGE, false);

                    if (player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) == QUEST_STATUS_COMPLETE)
                    {
                        uiSeatData = DATA_SEAT_IN_CHANGE;
                        player->ChangeSeat(1);
                        return;
                    }

                    player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_5, player);
                    me->Whisper(BUCCANEER_SAY_FOOTBOMB, player->GetGUID(), true);
                } else
                    if (uiSeatData == DATA_SEAT_IN_CHANGE)
                        uiSeatData = DATA_NONE;
            } else
            {
                if (uiSeatData != DATA_SEAT_IN_CHANGE)
                    if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 30.0f))
                        CAST_AI(npc_coach_crosscheck::npc_coach_crosscheckAI, coach->AI())->lPlayersGUID.push_back(player->GetGUID());
            }
        }

        void UpdateAI(const uint32 diff)
        { }
    };
};

class npc_fourth_and_goal_target : public CreatureScript
{
public:
    npc_fourth_and_goal_target() : CreatureScript("npc_fourth_and_goal_target") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fourth_and_goal_targetAI(creature);
    }

    struct npc_fourth_and_goal_targetAI : public ScriptedAI
    {
        npc_fourth_and_goal_targetAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->setActive(true);
            creature->SetReactState(REACT_PASSIVE);
            creature->AddAura(SPELL_MARK, creature);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void UpdateAI(const uint32 diff)
        { }
    };
};

class npc_deathwing_qfag : public CreatureScript
{
public:
    npc_deathwing_qfag() : CreatureScript("npc_deathwing_qfag") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathwing_qfagAI(creature);
    }

    struct npc_deathwing_qfagAI : public npc_vehicle_escortAI
    {
        npc_deathwing_qfagAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            me->setActive(true);
            uiEventTimer = 0;
            uiPhase = 0;
            Event = false;

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Player* player = summoner->ToPlayer())
                        player->PlayDirectSound(DEATHWING_SOUND_1, player);
        }

        uint32 uiEventTimer;
        uint8 uiPhase;
        bool Event;

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == 87226)
            {
                Event = true;
                DoCast(SPELL_XPLOSION);

                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            player->PlayDirectSound(DEATHWING_SOUND_3, player);

                            if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 30.0f))
                                CAST_AI(npc_coach_crosscheck::npc_coach_crosscheckAI, coach->AI())->FinishDeathwingEvent(player->GetGUID());
                        }
            }
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 1:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            me->DoPersonalScriptText(DEATHWING_YELL, player);

                            if (Creature* buccaneer = player->GetVehicleCreatureBase())
                            {
                                player->AddAura(SPELL_QUEST_INVISIBILITY_DETECTION_6, player);
                                buccaneer->AI()->DoAction(0);
                            }
                        }
                        break;
            case 3:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                        {
                            DoCast(SPELL_DEATHWINGS_ATTACK);
                            player->CastSpell(player, SPELL_CHARACTER_EARTHQUAKE, false);
                            player->PlayDirectSound(DEATHWING_SOUND_2, player);
                        }
                        break;
            case 10:
                Event = true;
                SetEscortPaused(true);
                break;
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
                        uiEventTimer = 500;

                        if (me->isSummon())
                            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                if (Player* player = summoner->ToPlayer())
                                    if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 90.0f))
                                    {
                                        coach->SetFacingToObject(me);
                                        coach->DoPersonalScriptText(COACH_NOT_AT_MY_FIELD, player);
                                    }
                                    break;
                    case 2:
                        uiEventTimer = 1000;

                        if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 90.0f))
                        {
                            coach->SendPlaySpellVisualKit(SPELL_VISUAL_START_FIRE_ROCKET_1, 0);
                            coach->SendPlaySpellVisualKit(SPELL_VISUAL_START_FIRE_ROCKET_2, 0);
                        }
                        break;
                    case 3:
                        Event = false;
                        uiEventTimer = 500;

                        if (Creature* coach = me->FindNearestCreature(NPC_COACH_CROSSCHECK, 90.0f))
                        {
                            if (me->isSummon())
                                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                    if (Player* player = summoner->ToPlayer())
                                        CAST_AI(npc_coach_crosscheck::npc_coach_crosscheckAI, coach->AI())->FinishDeathwingEvent(player->GetGUID());

                            coach->CastSpell(me, 87226, false);
                        }
                        break;
                    case 4:
                        Event = false;
                        uiEventTimer = 500;
                        SetEscortPaused(false);
                        break;
                    }
                } else
                    uiEventTimer -= diff;

                npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class spell_kick_footbomb : public SpellScriptLoader
{
public:
    spell_kick_footbomb() : SpellScriptLoader("spell_kick_footbomb") { }

    class spell_kick_footbomb_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_kick_footbomb_SpellScript)

            void OnKick(SpellEffIndex effIndex)
        {
            float x, y, z;
            GetExplTargetDest()->GetPosition(x, y, z);

            if (sqrt(pow(x + 8507.12, 2) + pow(y - 1485.39, 2) + pow(z - 199.23, 2)) > 50.0f)
                return;

            Unit* caster = GetCaster();

            if (!caster)
                return;

            if (Vehicle* vehicle = caster->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(0))
                    if (Player* player = passenger->ToPlayer())
                        if (player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) == QUEST_STATUS_INCOMPLETE)
                        {
                            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_5);
                            player->PlayDirectSound(SOUND_WOW_2, player);
                            player->KilledMonsterCredit(NPC_FOURTH_AND_GOAL_TARGET, 0);

                            if (Creature* coach = player->FindNearestCreature(NPC_COACH_CROSSCHECK, 30.0f))
                                CAST_AI(npc_coach_crosscheck::npc_coach_crosscheckAI, coach->AI())->StartDeathwingEvent(player);
                        }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_kick_footbomb_SpellScript::OnKick, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_kick_footbomb_SpellScript();
    }
};

///////////
// Quest Do it Yourself 14070
///////////

class npc_bruno_flameretardant : public CreatureScript
{
public:
    npc_bruno_flameretardant() : CreatureScript("npc_bruno_flameretardant") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bruno_flameretardantAI(creature);
    }

    struct npc_bruno_flameretardantAI : public ScriptedAI
    {
        npc_bruno_flameretardantAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiVisualCastTimer;

        void MoveInLineOfSight(Unit* who)
        {
            if (me->isInCombat())
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
                if (Player* player = who->ToPlayer())
                {
                    QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                    if (incomplete_quest == player->getQuestStatusMap().end())
                        return;

                    QuestStatusData& q_status = incomplete_quest->second;
                    uint64 uiPlayerGUID = player->GetGUID();

                    if (q_status.CreatureOrGOCount[0] == 0)
                    {
                        me->DoPersonalScriptText(NPC_BRUNO_SAY_1, player);
                        me->setFaction(AGGRESSIVE_FACTION);
                        AttackStart(player);

                        for (int i = 0; i < 3; ++i)
                        {
                            std::list<Creature*> lHomies;
                            me->GetCreatureListWithEntryInGrid(lHomies, Homies[i], 25.0f);

                            for (std::list<Creature*>::const_iterator itr = lHomies.begin(); itr != lHomies.end(); ++itr)
                                if ((*itr)->isSummon())
                                    if ((*itr)->ToTempSummon()->GetSummonerGUID() == uiPlayerGUID)
                                    {
                                        uint8 roll = urand(0, 6);
                                        (*itr)->DoPersonalScriptText(HOMIES_RANDOM_ATTACK_YELL - roll, player);
                                        (*itr)->AI()->AttackStart(me);
                                    }
                        }
                    }
                }
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (me->GetHealth() <= damage)
            {
                std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();

                damage = 0;
                me->CombatStop(true);
                me->setFaction(ORIGINAL_FACTION);

                if (tList.empty())
                    return;

                for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                    if (Unit* attacker = Unit::GetUnit((*me), (*itr)->getUnitGuid()))
                    {
                        attacker->CombatStop();

                        if (attacker->GetTypeId() == TYPEID_UNIT)
                            attacker->ToCreature()->AI()->EnterEvadeMode();

                        if (Player* player = attacker->ToPlayer())
                        {
                            me->DoPersonalScriptText(NPC_BRUNO_SAY_2, player);
                            player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                        }
                    }

                    EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.CreatureOrGOCount[0] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_frankie_gearslipper : public CreatureScript
{
public:
    npc_frankie_gearslipper() : CreatureScript("npc_frankie_gearslipper") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frankie_gearslipperAI(creature);
    }

    struct npc_frankie_gearslipperAI : public ScriptedAI
    {
        npc_frankie_gearslipperAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiThrowTimer;
        uint32 uiVisualCastTimer;

        void Reset()
        {
            uiThrowTimer = urand(2000, 7000);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->isInCombat())
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
                if (Player* player = who->ToPlayer())
                {
                    QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                    if (incomplete_quest == player->getQuestStatusMap().end())
                        return;

                    QuestStatusData& q_status = incomplete_quest->second;
                    uint64 uiPlayerGUID = player->GetGUID();

                    if (q_status.CreatureOrGOCount[1] == 0)
                    {
                        me->DoPersonalScriptText(NPC_FRANKIE_SAY_1, player);
                        me->setFaction(AGGRESSIVE_FACTION);
                        AttackStart(player);

                        for (int i = 0; i < 3; ++i)
                        {
                            std::list<Creature*> lHomies;
                            me->GetCreatureListWithEntryInGrid(lHomies, Homies[i], 25.0f);

                            for (std::list<Creature*>::const_iterator itr = lHomies.begin(); itr != lHomies.end(); ++itr)
                                if ((*itr)->isSummon())
                                    if ((*itr)->ToTempSummon()->GetSummonerGUID() == uiPlayerGUID)
                                    {
                                        uint8 roll = urand(0, 6);
                                        (*itr)->DoPersonalScriptText(HOMIES_RANDOM_ATTACK_YELL - roll, player);
                                        (*itr)->AI()->AttackStart(me);
                                    }
                        }
                    }
                }
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (me->GetHealth() <= damage)
            {
                std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();

                damage = 0;
                me->CombatStop(true);
                me->setFaction(ORIGINAL_FACTION);

                if (tList.empty())
                    return;

                for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                    if (Unit* attacker = Unit::GetUnit((*me), (*itr)->getUnitGuid()))
                    {
                        attacker->CombatStop();

                        if (attacker->GetTypeId() == TYPEID_UNIT)
                            attacker->ToCreature()->AI()->EnterEvadeMode();

                        if (Player* player = attacker->ToPlayer())
                        {
                            me->DoPersonalScriptText(NPC_FRANKIE_SAY_2, player);
                            player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                        }
                    }

                    EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.CreatureOrGOCount[1] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            if (uiThrowTimer <= diff)
            {
                uiThrowTimer = urand(2000, 7000);
                DoCast(me->getVictim(), SPELL_THROW_GEARS);
            } else
                uiThrowTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_jack_the_hammer : public CreatureScript
{
public:
    npc_jack_the_hammer() : CreatureScript("npc_jack_the_hammer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jack_the_hammerAI(creature);
    }

    struct npc_jack_the_hammerAI : public ScriptedAI
    {
        npc_jack_the_hammerAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiKnockbackTimer;
        uint32 uiVisualCastTimer;

        void Reset()
        {
            uiKnockbackTimer = urand(3000, 8000);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->isInCombat())
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
                if (Player* player = who->ToPlayer())
                {
                    QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                    if (incomplete_quest == player->getQuestStatusMap().end())
                        return;

                    QuestStatusData& q_status = incomplete_quest->second;
                    uint64 uiPlayerGUID = player->GetGUID();

                    if (q_status.CreatureOrGOCount[2] == 0)
                    {
                        me->DoPersonalScriptText(JACK_THE_HAMMER_SAY_HAMMER, player);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        me->setFaction(AGGRESSIVE_FACTION);
                        AttackStart(player);

                        for (int i = 0; i < 3; ++i)
                        {
                            std::list<Creature*> lHomies;
                            me->GetCreatureListWithEntryInGrid(lHomies, Homies[i], 25.0f);

                            for (std::list<Creature*>::const_iterator itr = lHomies.begin(); itr != lHomies.end(); ++itr)
                                if ((*itr)->isSummon())
                                    if ((*itr)->ToTempSummon()->GetSummonerGUID() == uiPlayerGUID)
                                    {
                                        uint8 roll = urand(0, 6);
                                        (*itr)->DoPersonalScriptText(HOMIES_RANDOM_ATTACK_YELL - roll, player);
                                        (*itr)->AI()->AttackStart(me);
                                    }
                        }
                    }
                }
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (me->GetHealth() <= damage)
            {
                std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();

                damage = 0;
                me->CombatStop(true);
                me->setFaction(ORIGINAL_FACTION);

                if (tList.empty())
                    return;

                for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                    if (Unit* attacker = Unit::GetUnit((*me), (*itr)->getUnitGuid()))
                    {
                        attacker->CombatStop();

                        if (attacker->GetTypeId() == TYPEID_UNIT)
                            attacker->ToCreature()->AI()->EnterEvadeMode();

                        if (Player* player = attacker->ToPlayer())
                        {
                            me->DoPersonalScriptText(NPC_JACK_SAY, player);
                            player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                        }
                    }

                    EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.CreatureOrGOCount[2] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            if (uiKnockbackTimer <= diff)
            {
                uiKnockbackTimer = urand(3000, 8000);
                DoCast(me->getVictim(), SPELL_KNOCKBACK);
            } else
                uiKnockbackTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_sudsy_magee : public CreatureScript
{
public:
    npc_sudsy_magee() : CreatureScript("npc_sudsy_magee") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sudsy_mageeAI(creature);
    }

    struct npc_sudsy_mageeAI : public ScriptedAI
    {
        npc_sudsy_mageeAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiFrostboltTimer;
        uint32 uiBubbleBlastTimer;
        uint32 uiVisualCastTimer;

        void Reset()
        {
            uiFrostboltTimer = 1000;
            uiBubbleBlastTimer = 500;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->isInCombat())
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
                if (Player* player = who->ToPlayer())
                {
                    QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                    if (incomplete_quest == player->getQuestStatusMap().end())
                        return;

                    QuestStatusData& q_status = incomplete_quest->second;
                    uint64 uiPlayerGUID = player->GetGUID();

                    if (q_status.CreatureOrGOCount[3] == 0)
                    {
                        me->DoPersonalScriptText(NPC_SUDSY_SAY_1, player);
                        me->setFaction(AGGRESSIVE_FACTION);
                        AttackStart(player);

                        for (int i = 0; i < 3; ++i)
                        {
                            std::list<Creature*> lHomies;
                            me->GetCreatureListWithEntryInGrid(lHomies, Homies[i], 25.0f);

                            for (std::list<Creature*>::const_iterator itr = lHomies.begin(); itr != lHomies.end(); ++itr)
                                if ((*itr)->isSummon())
                                    if ((*itr)->ToTempSummon()->GetSummonerGUID() == uiPlayerGUID)
                                    {
                                        uint8 roll = urand(0, 6);
                                        (*itr)->DoPersonalScriptText(HOMIES_RANDOM_ATTACK_YELL - roll, player);
                                        (*itr)->AI()->AttackStart(me);
                                    }
                        }
                    }
                }
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (me->GetHealth() <= damage)
            {
                std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();

                damage = 0;
                me->CombatStop(true);
                me->setFaction(ORIGINAL_FACTION);

                if (tList.empty())
                    return;

                for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                    if (Unit* attacker = Unit::GetUnit((*me), (*itr)->getUnitGuid()))
                    {
                        attacker->CombatStop();

                        if (attacker->GetTypeId() == TYPEID_UNIT)
                            attacker->ToCreature()->AI()->EnterEvadeMode();

                        if (Player* player = attacker->ToPlayer())
                        {
                            me->DoPersonalScriptText(NPC_SUDSY_SAY_2, player);
                            player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                        }
                    }

                    EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest = player->getQuestStatusMap().find(QUEST_DO_IT_YOURSELF);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.CreatureOrGOCount[3] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            if (uiBubbleBlastTimer <= diff)
            {
                uiBubbleBlastTimer = urand(2000, 8000);
                me->CastStop();
                DoCast(me->getVictim(), SPELL_BUBBLE_BLAST);
            } else
                uiBubbleBlastTimer -= diff;

            if (uiFrostboltTimer <= diff)
            {
                uiFrostboltTimer = 3500;
                DoCast(me->getVictim(), SPELL_FROSTBOLT);
            } else
                uiFrostboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quests:
//    Off to the Bank 26711, 26712
//    The New You 14110, 14109
///////////

class npc_chip_endale : public CreatureScript
{
public:
    npc_chip_endale() : CreatureScript("npc_chip_endale") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getGender() == GENDER_MALE)
        {
            player->SEND_GOSSIP_MENU(50017, creature->GetGUID());
            return true;
        }

        if (creature->GetPhaseMask() & 1024)
        {
            player->SEND_GOSSIP_MENU(14682, creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_OFF_TO_THE_BANK_FEMALE:
            creature->DoPersonalScriptText(CHIP_ENDALE_SAY_START, player);
            break;
        case QUEST_LIFE_OF_THE_PARTY_FEMALE:
            player->CastSpell(player, SPELL_AWESOME_PARTY_ENSEMBLE, false);
            player->CastSpell(player, SPELL_LOTP_OUTFIT_SECONDARY, false);
            player->CastSpell(player, SPELL_LOTP_OUTFIT_FEMALE, false);
            player->CastSpell(player, SPELL_LOTP_PAPARAZZI, false);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_1, false);
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_5);
            creature->DoPersonalScriptText(CHIP_ENDALE_SAY_GO_TO_THE_PARTY, player);
            creature->Whisper(CHIP_ENDALE_EMOTE_GO, player->GetGUID(), true);
            break;
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chip_endaleAI(creature);
    }

    struct npc_chip_endaleAI : public ScriptedAI
    {
        npc_chip_endaleAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayers.clear();

            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        }

        std::list<Psc_ls> lPlayers;

        void UpdateAI(const uint32 diff)
        {
            if (!lPlayers.empty())
                for (std::list<Psc_ls>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                    if ((*itr).uiPersonalTimer <= diff)
                    {
                        if (Player* player = Unit::GetPlayer(*me, (*itr).uiPlayerGUID))
                            me->DoPersonalScriptText(CHIP_ENDALE_SAY_GAME_OVER, player);

                        itr = lPlayers.erase(itr);
                    } else
                    {
                        (*itr).uiPersonalTimer -= diff;
                        ++itr;
                    }


                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_candy_cane : public CreatureScript
{
public:
    npc_candy_cane() : CreatureScript("npc_candy_cane") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getGender() == GENDER_FEMALE)
        {
            player->SEND_GOSSIP_MENU(14680, creature->GetGUID());
            return true;
        }

        if (creature->GetPhaseMask() & 1024)
        {
            player->SEND_GOSSIP_MENU(14682, creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_OFF_TO_THE_BANK_MALE:
            creature->DoPersonalScriptText(CANDY_CANE_SAY_START, player);
            break;
        case QUEST_LIFE_OF_THE_PARTY_MALE:
            player->CastSpell(player, SPELL_AWESOME_PARTY_ENSEMBLE, false);
            player->CastSpell(player, SPELL_LOTP_OUTFIT_SECONDARY, false);
            player->CastSpell(player, SPELL_LOTP_OUTFIT_MALE, false);
            player->CastSpell(player, SPELL_LOTP_PAPARAZZI, false);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_1, false);
            player->RemoveAura(SPELL_QUEST_INVISIBILITY_DETECTION_5);
            creature->DoPersonalScriptText(CANDY_CANE_SAY_GO_TO_THE_PARTY, player);
            creature->Whisper(CANDY_CANE_EMOTE_GO, player->GetGUID(), true);
            break;
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_candy_caneAI(creature);
    }

    struct npc_candy_caneAI : public ScriptedAI
    {
        npc_candy_caneAI(Creature* creature) : ScriptedAI(creature)
        {
            lPlayers.clear();

            if (me->GetPhaseMask() == 16384)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        }

        std::list<Psc_ls> lPlayers;

        void UpdateAI(const uint32 diff)
        {
            if (!lPlayers.empty())
                for (std::list<Psc_ls>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                    if ((*itr).uiPersonalTimer <= diff)
                    {
                        if (Player* player = Unit::GetPlayer(*me, (*itr).uiPlayerGUID))
                            me->DoPersonalScriptText(CANDY_CANE_SAY_GAME_OVER, player);

                        itr = lPlayers.erase(itr);
                    } else
                    {
                        (*itr).uiPersonalTimer -= diff;
                        ++itr;
                    }

                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_fbok_bank_teller : public CreatureScript
{
public:
    npc_fbok_bank_teller() : CreatureScript("npc_fbok_bank_teller") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_NEW_YOU_FEMALE || quest->GetQuestId() == QUEST_THE_NEW_YOU_MALE)
        {
            creature->DoPersonalScriptText(FBOK_BANK_TELLER_SAY_COOL, player);

            if (Creature* teller = creature->FindNearestCreature(NPC_FBOK_BANK_TELLER, 15.0f))
                teller->DoPersonalScriptText(FBOK_BANK_TELLER_SAY_COOL, player);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fbok_bank_tellerAI(creature);
    }

    struct npc_fbok_bank_tellerAI : public ScriptedAI
    {
        npc_fbok_bank_tellerAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() > 3)
                CanSay = false;
            else
                CanSay = true;
        }

        uint32 uiNextTimer;
        bool CanSay;

        void Reset()
        {
            uiNextTimer = urand(25000, 35000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (CanSay)
                if (uiNextTimer <= diff)
                {
                    DoScriptText(FBOK_BANK_TELLER_SAY_NEXT, me);
                    uiNextTimer = urand(25000, 35000);
                } else
                    uiNextTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_szabo : public CreatureScript
{
public:
    npc_szabo() : CreatureScript("npc_szabo") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 uiTextID = 14700;

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_INCOMPLETE) &&
            !player->HasItemCount(ITEM_NEW_OUTFIT, 1))
        {
            uiTextID = 14701;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, NEW_OUTFIT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_COMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_COMPLETE) ||
            player->HasItemCount(ITEM_NEW_OUTFIT, 1))
            uiTextID = 14703;

        player->SEND_GOSSIP_MENU(uiTextID, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->SEND_GOSSIP_MENU(14702, creature->GetGUID());
            player->CastSpell(player, SPELL_CREATE_HIP_NEW_OUTFIT, false);

            if (Creature* citizen = creature->FindNearestCreature(NPC_KEZAN_CITIZEN, 30.0f))
                citizen->DoPersonalScriptText(CITIZEN_SAY_SZABO_SO_COOL, player);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_szaboAI(creature);
    }

    struct npc_szaboAI : public ScriptedAI
    {
        npc_szaboAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiVisualCastTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest;

                        if (player->getGender() == GENDER_FEMALE)
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_FEMALE);
                        else
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_MALE);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.ItemCount[1] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_missa_spekkies : public CreatureScript
{
public:
    npc_missa_spekkies() : CreatureScript("npc_missa_spekkies") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 uiTextID = 14704;

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_INCOMPLETE) &&
            !player->HasItemCount(ITEM_COOL_SHADES, 1))
        {
            uiTextID = 14705;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, COOL_SHADES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_COMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_COMPLETE) ||
            player->HasItemCount(ITEM_COOL_SHADES, 1))
            uiTextID = 14707;

        player->SEND_GOSSIP_MENU(uiTextID, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->SEND_GOSSIP_MENU(14706, creature->GetGUID());
            player->CastSpell(player, SPELL_CREATE_COOL_SHADES, false);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_missa_spekkiesAI(creature);
    }

    struct npc_missa_spekkiesAI : public ScriptedAI
    {
        npc_missa_spekkiesAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiVisualCastTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest;

                        if (player->getGender() == GENDER_FEMALE)
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_FEMALE);
                        else
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_MALE);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.ItemCount[2] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_gappy_silvertooth : public CreatureScript
{
public:
    npc_gappy_silvertooth() : CreatureScript("npc_gappy_silvertooth") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 uiTextID = 14696;

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_INCOMPLETE) &&
            !player->HasItemCount(ITEM_SHINY_BLING, 1))
        {
            uiTextID = 14697;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, SHINY_BLING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }

        if ((player->GetQuestStatus(QUEST_THE_NEW_YOU_FEMALE) == QUEST_STATUS_COMPLETE ||
            player->GetQuestStatus(QUEST_THE_NEW_YOU_MALE) == QUEST_STATUS_COMPLETE) ||
            player->HasItemCount(ITEM_SHINY_BLING, 1))
            uiTextID = 14699;

        player->SEND_GOSSIP_MENU(uiTextID, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->SEND_GOSSIP_MENU(14698, creature->GetGUID());
            player->CastSpell(player, SPELL_CREATE_SHINY_BLING, false);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gappy_silvertoothAI(creature);
    }

    struct npc_gappy_silvertoothAI : public ScriptedAI
    {
        npc_gappy_silvertoothAI(Creature* creature) : ScriptedAI(creature)
        {
            uiVisualCastTimer = 2000;
        }

        uint32 uiVisualCastTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiVisualCastTimer <= diff)
            {
                uiVisualCastTimer = 2000;
                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                if (lPlayers.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (Player* player = itr->getSource())
                    {
                        QuestStatusMap::iterator incomplete_quest;

                        if (player->getGender() == GENDER_FEMALE)
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_FEMALE);
                        else
                            incomplete_quest = player->getQuestStatusMap().find(QUEST_THE_NEW_YOU_MALE);

                        if (incomplete_quest == player->getQuestStatusMap().end())
                            return;

                        QuestStatusData& q_status = incomplete_quest->second;

                        if (q_status.ItemCount[0] == 0)
                        {
                            me->SendPlaySpellVisual(SPELL_VISUAL_CATS_MARK);
                        }
                    }
            } else
                uiVisualCastTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quests Life of the Party 14113, 14153
///////////

class npc_kezan_partygoer : public CreatureScript
{
public:
    npc_kezan_partygoer() : CreatureScript("npc_kezan_partygoer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kezan_partygoerAI(creature);
    }

    struct npc_kezan_partygoerAI : public ScriptedAI
    {
        npc_kezan_partygoerAI(Creature* creature) : ScriptedAI(creature)
        {
            reset = false;
            uiResetTimer = 25000;
            uiSpeachTimer = urand(10000, 40000);
            ChooseAura();
        }

        uint32 uiResetTimer;
        uint32 uiSpeachTimer;
        bool reset;

        void ChooseAura()
        {
            me->RemoveAllAuras();
            uint8 roll = urand(TYPE_BUBBLY, TYPE_HORS_DOEUVRES);

            switch (roll)
            {
            case TYPE_BUBBLY:
                me->AddAura(AURA_PARTYGOER_BUBBLY, me);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VIRTUAL_ITEM_BUBBLY);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_EAT_NO_SHEATHE);
                break;
            case TYPE_BUCKET:
                me->AddAura(AURA_PARTYGOER_BUCKET, me);
                me->AddAura(AURA_DRUNKEN_STUN, me);
                me->AddAura(AURA_DRUNKEN_INTOXICATION, me);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                break;
            case TYPE_DANCE:
                me->AddAura(AURA_PARTYGOER_DANCE, me);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DANCE);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                break;
            case TYPE_FIREWORKS:
                me->AddAura(AURA_PARTYGOER_FIREWORKS, me);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VIRTUAL_ITEM_FIREWORKS);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                break;
            case TYPE_HORS_DOEUVRES:
                me->AddAura(AURA_PARTYGOER_HORS_DOEUVRES, me);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VIRTUAL_ITEM_HORS_DOEUVRES);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_EAT_NO_SHEATHE);
                break;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (me->HasAura(AURA_HAPPY_PARTYGOER))
                return;

            if (Player* player = caster->ToPlayer())
                if (player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_MALE) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_FEMALE) == QUEST_STATUS_INCOMPLETE)
                {
                    switch (spell->Id)
                    {
                    case SPELL_BUBBLY:
                        if (me->HasAura(AURA_PARTYGOER_BUBBLY))
                        {
                            reset = true;
                            me->RemoveAura(AURA_PARTYGOER_BUBBLY);
                            me->AddAura(AURA_HAPPY_PARTYGOER, me);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER_MALE, 0);
                            DoCast(SPELL_PARTYGOER_BUBBLY);
                            uint8 roll = urand(0, 2);
                            me->DoPersonalScriptText(PARTYGOER_RANDOM_SAY_OK_BUBBLY - roll, player);
                        }
                        break;
                    case SPELL_BUCKET:
                        if (me->HasAura(AURA_PARTYGOER_BUCKET))
                        {
                            reset = true;
                            me->RemoveAura(AURA_PARTYGOER_BUCKET);
                            me->RemoveAura(AURA_DRUNKEN_STUN);
                            me->RemoveAura(AURA_DRUNKEN_INTOXICATION);
                            me->AddAura(AURA_HAPPY_PARTYGOER, me);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            DoCast(SPELL_PARTYGOER_SUMMON_BUCKET);
                            DoCast(SPELL_PARTYGOER_VOMIT);
                            player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER_MALE, 0);
                            uint8 roll = urand(0, 3);
                            me->DoPersonalScriptText(PARTYGOER_RANDOM_SAY_OK_BUCKET - roll, player);
                        }
                        break;
                    case SPELL_DANCE:
                        if (me->HasAura(AURA_PARTYGOER_DANCE))
                        {
                            reset = true;
                            me->RemoveAura(AURA_PARTYGOER_DANCE);
                            me->AddAura(AURA_HAPPY_PARTYGOER, me);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            DoCast(SPELL_PARTYGOER_SUMMON_DISCO_BALL);
                            DoCast(SPELL_PARTYGOER_DANCE);
                            player->CastSpell(player, SPELL_PARTYGOER_DANCE, false);
                            player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER_MALE, 0);
                            uint8 roll = urand(0, 2);
                            me->DoPersonalScriptText(PARTYGOER_RANDOM_SAY_OK_DANCE - roll, player);
                        }
                        break;
                    case SPELL_FIREWORKS:
                        if (me->HasAura(AURA_PARTYGOER_FIREWORKS))
                        {
                            reset = true;
                            me->RemoveAura(AURA_PARTYGOER_FIREWORKS);
                            me->AddAura(AURA_HAPPY_PARTYGOER, me);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            player->CastSpell(player, SPELL_PARTYGOER_FIREWORKS_BLUE, false);
                            player->CastSpell(player, SPELL_PARTYGOER_FIREWORKS_GREEN, false);
                            player->CastSpell(player, SPELL_PARTYGOER_FIREWORKS_RED, false);
                            player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER_MALE, 0);
                            uint8 roll = urand(0, 2);
                            me->DoPersonalScriptText(PARTYGOER_RANDOM_SAY_OK_FIREWORKS - roll, player);
                        }
                        break;
                    case SPELL_HORS_DOEUVRES:
                        if (me->HasAura(AURA_PARTYGOER_HORS_DOEUVRES))
                        {
                            reset = true;
                            me->RemoveAura(AURA_PARTYGOER_HORS_DOEUVRES);
                            me->AddAura(AURA_HAPPY_PARTYGOER, me);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            DoCast(SPELL_PARTYGOER_HORS_DOEUVRES);
                            player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER_MALE, 0);
                            uint8 roll = urand(0, 2);
                            me->DoPersonalScriptText(PARTYGOER_RANDOM_SAY_OK_HORS_DOEUVRES - roll, player);
                        }
                        break;
                    }
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (reset)
                if (uiResetTimer <= diff)
                {
                    reset = false;
                    uiResetTimer = 25000;
                    ChooseAura();
                } else
                    uiResetTimer -= diff;

                if (me->HasAura(AURA_HAPPY_PARTYGOER))
                    return;

                if (uiSpeachTimer <= diff)
                {
                    uiSpeachTimer = urand(10000, 40000);
                    uint8 roll = urand(0, 1);

                    if (me->HasAura(AURA_PARTYGOER_BUBBLY))
                        DoScriptText(PARTYGOER_RANDOM_SAY_NEED_BUBBLY - roll, me);

                    if (me->HasAura(AURA_PARTYGOER_BUCKET))
                        DoScriptText(PARTYGOER_RANDOM_SAY_NEED_BUCKET - roll, me);

                    if (me->HasAura(AURA_PARTYGOER_DANCE))
                        DoScriptText(PARTYGOER_RANDOM_SAY_NEED_DANCE - roll, me);

                    if (me->HasAura(AURA_PARTYGOER_FIREWORKS))
                        DoScriptText(PARTYGOER_RANDOM_SAY_NEED_FIREWORKS - roll, me);

                    if (me->HasAura(AURA_PARTYGOER_HORS_DOEUVRES))
                        DoScriptText(PARTYGOER_RANDOM_SAY_NEED_HORS_DOEUVRES - roll, me);
                } else
                    uiSpeachTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quests The Great Bank Heist 14122
///////////

class go_first_bank_of_kezan_vault : public GameObjectScript
{
public:
    go_first_bank_of_kezan_vault() : GameObjectScript("go_first_bank_of_kezan_vault") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_THE_GREAT_BANK_HEIST) == QUEST_STATUS_INCOMPLETE)
            if (!player->GetVehicle())
                player->CastSpell(player, SPELL_TGBH_SUMMON_BUNNY_VEHICLE, false);
        return true;
    }
};

class npc_first_bank_of_kezan_vault : public CreatureScript
{
public:
    npc_first_bank_of_kezan_vault() : CreatureScript("npc_first_bank_of_kezan_vault") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_first_bank_of_kezan_vaultAI(creature);
    }

    struct npc_first_bank_of_kezan_vaultAI : public ScriptedAI
    {
        npc_first_bank_of_kezan_vaultAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->AddUnitState(UNIT_STATE_ROOT);
            creature->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
            creature->setPowerType(POWER_MANA);
            creature->SetMaxPower(POWER_MANA, 100);
            creature->SetVisible(false);
            creature->SetReactState(REACT_PASSIVE);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            uiEventTimer = 3000;
            uiDecreaseTimer = 2000;
            uiReRollTimer = 5000;
            uiPhase = 0;
            Event = false;
            Decrease = false;
            ReRoll = false;
        }

        uint32 uiEventTimer;
        uint32 uiDecreaseTimer;
        uint32 uiReRollTimer;
        uint8 uiPhase;
        bool Event;
        bool Decrease;
        bool ReRoll;

        void ChangeAura()
        {
            Player* player;

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    player = summoner->ToPlayer();

            if (!player)
                return;

            player->AddAura(SPELL_TIMER, player);
            uint8 roll = urand(ROLL_INFINIFOLD_LOCKPICK, ROLL_EAR_O_SCOPE);

            switch (roll)
            {
            case ROLL_INFINIFOLD_LOCKPICK:
                me->Whisper(VAULT_SAY_USE_INFINIFOLD_LOCKPICK, player->GetGUID(), true);
                me->AddAura(SPELL_TGBH_BUNNY_LOCKPICK_AURA, me);
                break;
            case ROLL_BLASTCRACKERS:
                me->Whisper(VAULT_SAY_USE_BLASTCRACKERS, player->GetGUID(), true);
                me->AddAura(SPELL_TGBH_BUNNY_EXPLOSIVES_AURA, me);
                break;
            case ROLL_KAJAMITE_DRILL:
                me->Whisper(VAULT_SAY_USE_KAJAMITE_DRILL, player->GetGUID(), true);
                me->AddAura(SPELL_TGBH_BUNNY_DRILL_AURA, me);
                break;
            case ROLL_AMAZING_G_RAY:
                me->Whisper(VAULT_SAY_USE_AMAZING_G_RAY, player->GetGUID(), true);
                me->AddAura(SPELL_TGBH_BUNNY_G_RAY_AURA, me);
                break;
            case ROLL_EAR_O_SCOPE:
                me->Whisper(VAULT_SAY_USE_EAR_O_SCOPE, player->GetGUID(), true);
                me->AddAura(SPELL_TGBH_BUNNY_LISTEN_AURA, me);
                break;
            }

            me->Whisper(VAULT_SAY_EMPTY_TEXT, player->GetGUID(), true);
        }

        void RemoveEventAuras()
        {
            me->RemoveAura(SPELL_TIMER);
            me->RemoveAura(SPELL_TGBH_BUNNY_LOCKPICK_AURA);
            me->RemoveAura(SPELL_TGBH_BUNNY_EXPLOSIVES_AURA);
            me->RemoveAura(SPELL_TGBH_BUNNY_DRILL_AURA);
            me->RemoveAura(SPELL_TGBH_BUNNY_G_RAY_AURA);
            me->RemoveAura(SPELL_TGBH_BUNNY_LISTEN_AURA);
        }

        void Hit(Player* player, bool correct)
        {
            if (correct)
            {
                DoCast(SPELL_TGBH_POWER_CORRECT);
                me->Whisper(VAULT_SAY_CORRECT, player->GetGUID(), true);

                if (me->GetPower(POWER_MANA) == 100)
                {
                    me->Whisper(VAULT_SAY_SUCCESS, player->GetGUID(), true);
                    player->ExitVehicle();

                    if (player->HasItemCount(ITEM_PERSONAL_RICHES, 1))
                        return;

                    ItemPosCountVec dest;
                    uint32 no_space = 0;
                    uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_PERSONAL_RICHES, 1, &no_space);

                    if (msg != EQUIP_ERR_OK)
                        return;

                    Item* item = player->StoreNewItem(dest, ITEM_PERSONAL_RICHES, true, Item::GenerateItemRandomPropertyId(ITEM_PERSONAL_RICHES));

                    if (!item)
                    {
                        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
                        return;
                    }

                    if (item)
                        player->SendNewItem(item, 1, true, true);
                }
            } else
            {
                me->ModifyPower(POWER_MANA, -10);
                me->Whisper(VAULT_SAY_INCORRECT, player->GetGUID(), true);
            }

            Event = true;
            Decrease = false;
            ReRoll = false;
            RemoveEventAuras();
            player->RemoveAura(SPELL_TIMER);
            me->Whisper(VAULT_SAY_EMPTY_TEXT, player->GetGUID(), true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            Player* player;

            if (Vehicle* vehicle = me->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(0))
                    player = passenger->ToPlayer();

            if (!player)
                return;

            switch (spell->Id)
            {
            case SPELL_INFINIFOLD_LOCKPICK:
                if (me->HasAura(SPELL_TGBH_BUNNY_LOCKPICK_AURA))
                    Hit(player, true);
                else
                    Hit(player, false);
                break;
            case SPELL_BLASTCRACKERS:
                if (me->HasAura(SPELL_TGBH_BUNNY_EXPLOSIVES_AURA))
                    Hit(player, true);
                else
                    Hit(player, false);
                break;
            case SPELL_KAJAMITE_DRILL:
                if (me->HasAura(SPELL_TGBH_BUNNY_DRILL_AURA))
                    Hit(player, true);
                else
                    Hit(player, false);
                break;
            case SPELL_AMAZING_G_RAY:
                if (me->HasAura(SPELL_TGBH_BUNNY_G_RAY_AURA))
                    Hit(player, true);
                else
                    Hit(player, false);
                break;
            case SPELL_EAR_O_SCOPE:
                if (me->HasAura(SPELL_TGBH_BUNNY_LISTEN_AURA))
                    Hit(player, true);
                else
                    Hit(player, false);
                break;
            }
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!apply)
                return;

            if (Player* player = who->ToPlayer())
            {
                player->KilledMonsterCredit(me->GetEntry(), 0);
                uiEventTimer = 3000;
                uiPhase = 0;
                Event = true;
            }
        }

        void OnCharmed(bool charm)
        {
            if (charm)
                me->EnableAI();
        }

        void UpdateAI(const uint32 diff)
        {
            if (Event)
                if (uiEventTimer <= diff)
                {
                    ++uiPhase;

                    if (me->isSummon())
                        if (uint64 uiPlayerGUID = me->ToTempSummon()->GetSummonerGUID())
                            switch (uiPhase)
                        {
                            case 1:
                                uiEventTimer = 3500;
                                me->Whisper(VAULT_SAY_BREAKING, uiPlayerGUID, true);
                                me->Whisper(VAULT_SAY_EMPTY_TEXT, uiPlayerGUID, true);
                                break;
                            case 2:
                                uiEventTimer = 3500;
                                me->Whisper(VAULT_SAY_USE_GOBLIN_ALL_IN_1_DER_BELT, uiPlayerGUID, true);
                                me->Whisper(VAULT_SAY_EMPTY_TEXT, uiPlayerGUID, true);
                                break;
                            case 3:
                                uiEventTimer = 5000;
                                me->Whisper(VAULT_SAY_VAULT_WILL_BE_CRACKED, uiPlayerGUID, true);
                                me->Whisper(VAULT_SAY_EMPTY_TEXT, uiPlayerGUID, true);
                                break;
                            case 4:
                                uiEventTimer = 2000;
                                me->Whisper(VAULT_SAY_GOOD_LUCK, uiPlayerGUID, true);
                                me->Whisper(VAULT_SAY_EMPTY_TEXT, uiPlayerGUID, true);
                                break;
                            case 5:
                                ChangeAura();
                                Event = false;
                                ReRoll = true;
                                Decrease = true;
                                uiEventTimer = 1000;
                                uiDecreaseTimer = 2000;
                                uiReRollTimer = 5000;
                                uiPhase = 4;
                                break;
                        }
                } else
                    uiEventTimer -= diff;

                if (ReRoll)
                    if (uiReRollTimer <= diff)
                    {
                        Event = true;
                        ReRoll = false;
                        me->ModifyPower(POWER_MANA, -10);
                        RemoveEventAuras();

                        if (me->isSummon())
                            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                if (Player* player = summoner->ToPlayer())
                                {
                                    me->Whisper(VAULT_SAY_IS_NO_TIME, player->GetGUID(), true);
                                    me->Whisper(VAULT_SAY_EMPTY_TEXT, player->GetGUID(), true);
                                }
                    } else
                        uiReRollTimer -= diff;

                    if (Decrease)
                        if (uiDecreaseTimer <= diff)
                        {
                            uiDecreaseTimer = 2000;
                            me->ModifyPower(POWER_MANA, -5);
                        } else
                            uiDecreaseTimer -= diff;
        }
    };
};

///////////
// Quests Robbing Hoods 14121
///////////

class npc_hired_looter : public CreatureScript
{
public:
    npc_hired_looter() : CreatureScript("npc_hired_looter") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hired_looterAI(creature);
    }

    struct npc_hired_looterAI : public ScriptedAI
    {
        npc_hired_looterAI(Creature* creature) : ScriptedAI(creature)
        {
            uiDieTimer = 500;
            uiThrowTimer = urand(5000, 15000);
            uiPhase = 0;
            Died = false;
        }

        uint32 uiDieTimer;
        uint32 uiThrowTimer;
        uint8 uiPhase;
        bool Died;

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (Died)
                return;

            if (spell->Id == SPELL_HOT_ROD_KNOCKBACK)
            {
                Died = true;

                if (Vehicle* vehicle = caster->GetVehicleKit())
                    if (Unit* passenger = vehicle->GetPassenger(0))
                        if (Player* player = passenger->ToPlayer())
                            if (player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_INCOMPLETE)
                                me->CastSpell(player, SPELL_CREATE_STOLEN_LOOT, false);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Died)
                if (uiDieTimer <= diff)
                {
                    Died = false;
                    uiDieTimer = 500;
                    me->Kill(me);
                } else
                    uiDieTimer -= diff;

                if (!UpdateVictim())
                {
                    if (!Died)
                        if (uiThrowTimer <= diff)
                        {
                            ++uiPhase;

                            switch (uiPhase)
                            {
                            case 1:
                                uiThrowTimer = 1000;
                                DoCast(SPELL_THROW_SUPER_SPLODY_COCKTAIL);
                                break;
                            case 2:
                                uiPhase = 0;
                                uiThrowTimer = urand(3000, 10000);
                                uint8 roll = urand(0, 5);
                                me->HandleEmoteCommand(LooterRandomEmote[roll]);
                                break;
                            }
                        } else
                            uiThrowTimer -= diff;

                        return;
                }

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quests Waltz Right In 14123
///////////

class npc_villa_mook : public CreatureScript
{
public:
    npc_villa_mook() : CreatureScript("npc_villa_mook") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_villa_mookAI(creature);
    }

    struct npc_villa_mookAI : public ScriptedAI
    {
        npc_villa_mookAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterEvadeMode()
        {
            me->setFaction(ORIGINAL_FACTION);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->isInCombat())
            {
                if (Player* player = who->ToPlayer())
                    if (me->IsWithinDistInMap(player, 15.0f) && !me->isInCombat())
                        if (player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_COMPLETE)
                        {
                            me->setFaction(AGGRESSIVE_FACTION);
                            AttackStart(player);
                        }

                        return;
            }

            if (Player* player = who->ToPlayer())

                if (player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_COMPLETE)
                    if (!player->HasAura(SPELL_HONARTS_INGENIOUS_CAPOFMOOK_FOOLERY))
                    {
                        player->CastSpell(player, SPELL_HONARTS_INGENIOUS_CAPOFMOOK_FOOLERY, false);

                        if (player->IsVehicle())
                        {
                            float x, y, z, o;
                            player->GetPosition(x, y, z, o);
                            if (Creature* mook = player->SummonCreature(NPC_MOOK_DISGUISE, x, y, z, o))
                                mook->EnterVehicle(player, 0);
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
// Quests Liberate the Kaja'mite 14124
///////////

class go_kajamite_deposit : public GameObjectScript
{
public:
    go_kajamite_deposit() : GameObjectScript("go_kajamite_deposit") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        Position pos;

        for (int i = 0; i < 3; ++i)
        {
            go->GetRandomNearPosition(pos, 3.0f);
            go->SummonGameObject(GO_KAJAMITE_CHUNK, pos.m_positionX, pos.m_positionY, pos.m_positionZ, pos.m_orientation, 0, 0, 0, 0, 60000);
        }

        return true;
    }
};

class npc_pirate_party_crasher : public CreatureScript
{
public:
    npc_pirate_party_crasher() : CreatureScript("npc_pirate_party_crasher") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pirate_party_crasherAI(creature);
    }

    struct npc_pirate_party_crasherAI : public ScriptedAI
    {
        npc_pirate_party_crasherAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiSpeachTimer;
        uint32 uiSwashbucklingSliceTimer;

        void Reset()
        {
            uiSpeachTimer = urand(10000, 120000);
            uiSwashbucklingSliceTimer = urand(2000, 5000);
            me->SetReactState(REACT_PASSIVE);
        }

        void DamageTaken(Unit* /*done_by*/, uint32 &damage)
        {
            if (me->GetReactState() == REACT_PASSIVE)
                me->SetReactState(REACT_AGGRESSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiSpeachTimer <= diff)
            {
                uiSpeachTimer = urand(10000, 40000);
                uint8 roll = urand(0, 7);
                DoScriptText(PIRATE_PARTY_CRASHER_RANDOM_SAY - roll, me);
            } else
                uiSpeachTimer -= diff;

            if (!UpdateVictim())
                return;

            if (uiSwashbucklingSliceTimer <= diff)
            {
                uiSwashbucklingSliceTimer = urand(2000, 5000);
                DoCast(me->getVictim(), SPELL_SWASHBUCKLING_SLICE);
            } else
                uiSwashbucklingSliceTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_ktc_waiter : public CreatureScript
{
public:
    npc_ktc_waiter() : CreatureScript("npc_ktc_waiter") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ktc_waiterAI(creature);
    }

    struct npc_ktc_waiterAI : public ScriptedAI
    {
        npc_ktc_waiterAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiFindTargetTimer;

        void Reset()
        {
            uiFindTargetTimer = 1000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (uiFindTargetTimer <= diff)
                {
                    uiFindTargetTimer = 1000;

                    if (Creature* pirate = me->FindNearestCreature(NPC_PIRATE_PARTY_CRASHER, 20.0f))
                    {
                        pirate->AI()->AttackStart(me);
                        me->CombatStart(pirate);
                        pirate->CombatStart(me);
                    }
                } else
                    uiFindTargetTimer -= diff;

                return;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class npc_kezan_socialite : public CreatureScript
{
public:
    npc_kezan_socialite() : CreatureScript("npc_kezan_socialite") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_HONARTS_INGENIOUS_CAPOFMOOK_FOOLERY))
        {
            player->SEND_GOSSIP_MENU(17550, creature->GetGUID());
            return true;
        }

        return false;
    }
};

///////////
// Hobart Grapplehammer, Assistant Greely
///////////

class npc_hobart_grapplehammer : public CreatureScript
{
public:
    npc_hobart_grapplehammer() : CreatureScript("npc_hobart_grapplehammer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hobart_grapplehammerAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->GetPhaseMask() == 1)
        {
            player->SEND_GOSSIP_MENU(50003, creature->GetGUID());
            return true;
        } else
        {
            player->SEND_GOSSIP_MENU(50004, creature->GetGUID());
            return true;
        }
    }

    struct npc_hobart_grapplehammerAI : public ScriptedAI
    {
        npc_hobart_grapplehammerAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->setActive(true);
            uiMechachickenGUID = 0;
            uiGreelyGUID = 0;
            uiChickenEventTimer = 7000;
            uiKajaroEventTimer = 7000;
            uiBunnyTimer = 1000;
            uiGreelyResponseId = 0;
            uiChickenPhase = 0;
            uiKajaroPhase = 0;
            uiKajaroEventType = 0;
            memset(&m_auiLeftBunny, 0, sizeof(m_auiLeftBunny));
            memset(&m_auiRightBunny, 0, sizeof(m_auiRightBunny));
            bunny_activated = false;

            switch (me->GetPhaseMask())
            {
            case 3:
                Chicken = true;
                Kajaro = false;
                me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                break;
            case 4:
                Chicken = false;
                Kajaro = false;
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                break;
            default:
                Chicken = false;
                Kajaro = true;
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                break;
            }

            if (Creature* greely = me->SummonCreature(NPC_ASSISTANT_GREELY, -8405.3f, 1360.25f, 104.021f, 3.61756f))
                uiGreelyGUID = greely->GetGUID();

            for (int i = 0; i < 7; ++i)
            {
                if (Creature* left_bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY,
                    LeftBunnySummPos[i][0], LeftBunnySummPos[i][1], LeftBunnySummPos[i][2], LeftBunnySummPos[i][3]))
                    m_auiLeftBunny[i] = left_bunny->GetGUID();

                if (Creature* right_bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY,
                    RightBunnySummPos[i][0], RightBunnySummPos[i][1], RightBunnySummPos[i][2], RightBunnySummPos[i][3]))
                    m_auiRightBunny[i] = right_bunny->GetGUID();
            }
        }

        uint64 m_auiLeftBunny[7];
        uint64 m_auiRightBunny[7];
        uint64 uiMechachickenGUID;
        uint64 uiGreelyGUID;
        uint32 uiChickenEventTimer;
        uint32 uiKajaroEventTimer;
        uint32 uiBunnyTimer;
        uint32 uiGreelyResponseId;
        uint8 uiChickenPhase;
        uint8 uiKajaroEventType;
        uint8 uiKajaroPhase;
        bool bunny_activated;
        bool Chicken;
        bool Kajaro;

        void UpdateAI(const uint32 diff)
        {
            if (Kajaro)
                if (uiKajaroEventTimer <= diff)
                {
                    ++uiKajaroPhase;

                    switch (uiKajaroPhase)
                    {
                    case 1:
                        uiKajaroEventTimer = KajaroType[uiKajaroEventType].uiHobartTimer;
                        DoScriptText(KajaroType[uiKajaroEventType].uiHobartTextId, me);
                        break;
                    case 2:
                        uiKajaroEventTimer = urand(21000, 35000);

                        if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                            DoScriptText(KajaroType[uiKajaroEventType].uiGreelyTextId, greely);

                        uiKajaroEventType = urand(0, 7);
                        uiKajaroPhase = 0;
                        break;
                    }
                } else
                    uiKajaroEventTimer -= diff;

                if (Chicken)
                {
                    if (bunny_activated)
                        if (uiBunnyTimer <= diff)
                        {
                            uiBunnyTimer = 1000;
                            uint8 l_roll;
                            uint8 r_roll;

                            if (uiChickenPhase < 37)
                            {
                                l_roll = urand(3, 4);
                                r_roll = urand(3, 4);
                            } else
                            {
                                l_roll = urand(1, 2);
                                r_roll = urand(1, 2);
                            }

                            for (int i = 0; i < l_roll; ++i)
                                if (Creature* bunny = Unit::GetCreature(*me, m_auiLeftBunny[urand(0, 6)]))
                                    bunny->CastSpell(bunny, SPELL_LIGHTNING_IMPACT, false);

                            for (int i = 0; i < r_roll; ++i)
                                if (Creature* bunny = Unit::GetCreature(*me, m_auiRightBunny[urand(0, 6)]))
                                    bunny->CastSpell(bunny, SPELL_LIGHTNING_IMPACT, false);
                        } else
                            uiBunnyTimer -= diff;

                        if (uiChickenEventTimer <= diff)
                        {
                            ++uiChickenPhase;
                            switch (uiChickenPhase)
                            {
                            case 1:
                                uiChickenEventTimer = 2500;
                                DoScriptText(HOBART_YELL_PREPARE, me);
                                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    greely->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                break;
                            case 2:
                                uiChickenEventTimer = 3000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                {
                                    greely->GetMotionMaster()->MovePoint(POINT_TESTING_PLATFORM, -8405.96f, 1357.84f, 104.709f);
                                    DoScriptText(GREELY_SAY_SHEESH, greely);
                                }
                                break;
                            case 3:
                                uiChickenEventTimer = 4000;

                                if (Creature* mechachicken = me->SummonCreature(NPC_MICRO_MECHACHICKEN, -8406.926f, 1356.2f, 104.8625f, 1.53589f))
                                {
                                    uiMechachickenGUID = mechachicken->GetGUID();
                                    mechachicken->CastSpell(mechachicken, SPELL_COSMETIC_POLYMORPH_IMPACT_VISUAL, false);
                                }

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    greely->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                                break;
                            case 4:
                                uiChickenEventTimer = 6000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_ONE_MECHACHICKEN, greely);
                                break;
                            case 5:
                                uiChickenEventTimer = 8000;
                                DoScriptText(HOBART_SAY_DEAR_GREELY, me);
                                break;
                            case 6:
                                uiChickenEventTimer = 2000;
                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SIGH, greely);
                                break;
                            case 7:
                                uiChickenEventTimer = 2000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_YES_DOCTOR, greely);
                                break;
                            case 8:
                                uiChickenEventTimer = 5000;
                                DoScriptText(HOBART_SAY_EXCELLENT, me);
                                break;
                            case 9:
                                uiChickenEventTimer = 2000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    greely->GetMotionMaster()->MovePoint(POINT_BACK_OF_THE_DOCTOR_1, -8405.3f, 1360.25f, 104.021f);
                                break;
                            case 10:
                                uiChickenEventTimer = 7000;
                                DoScriptText(HOBART_SAY_THANK_YOU, me);
                                break;
                            case 11:
                                uiChickenEventTimer = 5000;
                                DoScriptText(HOBART_SAY_HORRIBLE_PROBLEM, me);
                                break;
                            case 12:
                                uiChickenEventTimer = 8000;
                                DoScriptText(HOBART_SAY_GREATEST_GOBLIN, me);
                                break;
                            case 13:
                                uiChickenEventTimer = 1000;
                                DoScriptText(HOBART_SAY_FLIP_THE_SWITCH, me);
                                break;
                            case 14:
                                uiChickenEventTimer = 1000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_COUGHS, greely);
                                break;
                            case 15:
                                uiChickenEventTimer = 2500;
                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_AT_THE_CONTROLS, greely);
                                break;
                            case 16:
                                uiChickenEventTimer = 4000;
                                DoScriptText(HOBART_SAY_QUITE_RIGHT, me);
                                break;
                            case 17:
                                uiChickenEventTimer = 3500;
                                DoScriptText(HOBART_SAY_FLIPPING, me);
                                break;
                            case 18:
                                uiChickenEventTimer = 5000;
                                bunny_activated = true;
                                break;
                            case 19:
                                uiChickenEventTimer = 6000;
                                DoScriptText(HOBART_SAY_CAN_YOU_FEEL, me);
                                break;
                            case 20:
                                uiChickenEventTimer = 6000;
                                DoScriptText(HOBART_SAY_GROCERY_STORE, me);
                                break;
                            case 21:
                                uiChickenEventTimer = 7000;
                                DoScriptText(HOBART_SAY_FLIPPING_SECOND, me);
                                break;
                            case 22:
                                uiChickenEventTimer = 500;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    if (Creature* left_bunny = Unit::GetCreature(*me, m_auiLeftBunny[0]))
                                    {
                                        left_bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, chicken->GetGUID());
                                        left_bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_LIGHTNING_BEAM);
                                    }

                                    if (Creature* right_bunny = Unit::GetCreature(*me, m_auiRightBunny[0]))
                                    {
                                        right_bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, chicken->GetGUID());
                                        right_bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_LIGHTNING_BEAM);
                                    }
                                }
                                break;
                            case 23:
                                uiChickenEventTimer = 3000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_02);
                                    chicken->CastSpell(chicken, SPELL_MECHACHICKEN_OVERLOAD, false);
                                }
                                break;
                            case 24:
                                uiChickenEventTimer = 2000;
                                DoScriptText(HOBART_SAY_WAIT, me);

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->CastSpell(chicken, SPELL_MICRO_MECHACHICKEN_GROW, false);
                                }

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    greely->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);

                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                                break;
                            case 25:
                                uiChickenEventTimer = 1500;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                {
                                    greely->CastSpell(greely, SPELL_STEALTH, false);
                                    greely->GetMotionMaster()->MovePoint(POINT_RUN_AWAY_FROM_CHICKEN, -8404.18f, 1364.28f, 104.021f);
                                }
                                break;
                            case 26:
                                uiChickenEventTimer = 2000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_YA_THINK, greely);
                                break;
                            case 27:
                                uiChickenEventTimer = 4000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->CastSpell(chicken, SPELL_MICRO_MECHACHICKEN_GROW, false);
                                }
                                break;
                            case 28:
                                uiChickenEventTimer = 2000;
                                DoScriptText(HOBART_YELL_I_KNOW, me);
                                break;
                            case 29:
                                uiChickenEventTimer = 2000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->CastSpell(chicken, SPELL_MICRO_MECHACHICKEN_GROW, false);
                                }
                                break;
                            case 30:
                                uiChickenEventTimer = 2000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_YELL_FAR_RIGHT, greely);
                                break;
                            case 31:
                                uiChickenEventTimer = 2000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->CastSpell(chicken, SPELL_MICRO_MECHACHICKEN_GROW, false);
                                }
                                break;
                            case 32:
                                uiChickenEventTimer = 2000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->CastSpell(chicken, SPELL_MICRO_MECHACHICKEN_GROW, false);
                                }
                                break;
                            case 33:
                                uiChickenEventTimer = 2000;
                                DoScriptText(HOBART_YELL_I_ALWAYS_LOVED, me);
                                break;
                            case 34:
                                uiChickenEventTimer = 2000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_YELL_WHAT, greely);
                                break;
                            case 35:
                                uiChickenEventTimer = 5000;
                                DoScriptText(HOBART_YELL_FLIPPING_THE_OVERRIDE, me);
                                break;
                            case 36:
                                uiChickenEventTimer = 3000;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->RemoveAllAuras();
                                }

                                if (Creature* left_bunny = Unit::GetCreature(*me, m_auiLeftBunny[0]))
                                {
                                    left_bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
                                    left_bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
                                }

                                if (Creature* right_bunny = Unit::GetCreature(*me, m_auiRightBunny[0]))
                                {
                                    right_bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
                                    right_bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
                                }
                                break;
                            case 37:
                                uiChickenEventTimer = 4000;
                                DoScriptText(HOBART_SAY_ARE_WE_STILL_ALIVE, me);
                                break;
                            case 38:
                                uiChickenEventTimer = 3000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                {
                                    greely->RemoveAllAuras();
                                    DoScriptText(GREELY_SAY_WERE_STILL_ALIVE, greely);
                                    greely->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                                    greely->GetMotionMaster()->MovePoint(POINT_BACK_OF_THE_DICTOR_2, -8405.3f, 1360.25f, 104.021f);
                                }
                                break;
                            case 39:
                                uiChickenEventTimer = 4000;
                                DoScriptText(HOBART_SAY_WELL_OF_COURSE, me);
                                break;
                            case 40:
                                uiChickenEventTimer = 6000;
                                bunny_activated = false;

                                if (Creature* chicken = Unit::GetCreature(*me, uiMechachickenGUID))
                                {
                                    chicken->CastSpell(chicken, SPELL_MECHACHICKEN_EXPLOSION, false);
                                    chicken->PlayDirectSound(SOUND_MECHANICAL_CHICKEN_SOUNDS);
                                    chicken->DespawnOrUnsummon(3000);
                                }
                                break;
                            case 41:
                                uiChickenEventTimer = 500;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_IS_THAT_EGG, greely);
                                break;
                            case 42:
                                uiChickenEventTimer = 5000;
                                DoScriptText(HOBART_OPEN_MOUTH, me);
                                break;
                            case 43:
                                uiChickenEventTimer = 5000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_SAY_ALWAYS_LOVED, greely);
                                break;
                            case 44:
                                uiChickenEventTimer = 3000;
                                DoScriptText(HOBART_SAY_MISS_GREELY, me);
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                                break;
                            case 45:
                                uiChickenEventTimer = 3000;

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    DoScriptText(GREELY_BOTH_AGREE, greely);
                                break;
                            case 46:
                                uiChickenEventTimer = 180000;
                                uiChickenPhase = 0;
                                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                                if (Creature* greely = Unit::GetCreature(*me, uiGreelyGUID))
                                    greely->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                break;
                            }
                        } else
                            uiChickenEventTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_assistant_greely : public CreatureScript
{
public:
    npc_assistant_greely() : CreatureScript("npc_assistant_greely") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_assistant_greelyAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->GetPhaseMask() == 1)
        {
            player->SEND_GOSSIP_MENU(50005, creature->GetGUID());
            return true;
        } else
        {
            player->SEND_GOSSIP_MENU(50006, creature->GetGUID());
            return true;
        }
    }

    struct npc_assistant_greelyAI : public ScriptedAI
    {
        npc_assistant_greelyAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
            me->setActive(true);
            uiFollowTimer = 150;

            switch (me->GetPhaseMask())
            {
            case 3:
                Follow = false;
                break;
            case 4:
                Follow = false;
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                break;
            default:
                Follow = true;
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
                break;
            }
        }

        uint32 uiFollowTimer;
        bool Follow;

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (id)
            {
            case POINT_TESTING_PLATFORM:
                me->SetFacingTo(4.14084f);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                break;
            case POINT_BACK_OF_THE_DOCTOR_1:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        me->SetFacingToObject(summoner);
                        DoScriptText(GREELY_SHAKES_HEAD, me);
                    }
                    break;
            case POINT_RUN_AWAY_FROM_CHICKEN:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        me->SetFacingToObject(summoner);
                break;
            case POINT_BACK_OF_THE_DICTOR_2:
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        me->SetFacingToObject(summoner);
                        summoner->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                    }
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Follow)
                if (uiFollowTimer <= diff)
                {
                    uiFollowTimer = 150;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            me->SetFacingToObject(summoner);
                } else
                    uiFollowTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_kaja_cola_balloon : public CreatureScript
{
public:
    npc_kaja_cola_balloon() : CreatureScript("npc_kaja_cola_balloon") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kaja_cola_balloonAI(creature);
    }

    struct npc_kaja_cola_balloonAI : public ScriptedAI
    {
        npc_kaja_cola_balloonAI(Creature* creature) : ScriptedAI(creature)
        {
            uiYellTimer = 1000;
            if (me->GetPhaseMask() == 3087)
            {
                me->setActive(true);
                Yell = true;
            } else
            {
                me->setActive(false);
                Yell = false;
            }
        }

        uint32 uiYellTimer;
        bool Yell;

        void UpdateAI(const uint32 diff)
        {
            if (Yell)
                if (uiYellTimer <= diff)
                {
                    uiYellTimer = urand(30000, 60000);
                    uint8 roll = urand(0, 7);
                    DoScriptText(KAJA_COLA_BALLOON_RANDOM_YELL - roll, me);
                } else
                    uiYellTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class npc_vinny_slapchop : public CreatureScript
{
public:
    npc_vinny_slapchop() : CreatureScript("npc_vinny_slapchop") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vinny_slapchopAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, ru ? VINNY_VENDOR : VINNY_VENDOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        player->SEND_GOSSIP_MENU(50007, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }

    struct npc_vinny_slapchopAI : public ScriptedAI
    {
        npc_vinny_slapchopAI(Creature* creature) : ScriptedAI(creature)
        {
            uiSayTimer = urand(10000, 40000);
        }

        uint32 uiSayTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiSayTimer <= diff)
            {
                uiSayTimer = urand(10000, 40000);
                uint8 roll = urand(0, 7);
                DoScriptText(VINNY_SLAPCHOP_RANDOM_SAY - roll, me);
            } else
                uiSayTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_general_purpose_bunny : public CreatureScript
{
public:
    npc_general_purpose_bunny() : CreatureScript("npc_general_purpose_bunny") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_general_purpose_bunnyAI(creature);
    }

    struct npc_general_purpose_bunnyAI : public ScriptedAI
    {
        npc_general_purpose_bunnyAI(Creature* creature) : ScriptedAI(creature)
        {
            me->setActive(true);
            uiCastTimer = 1000;
        }

        uint32 uiCastTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiCastTimer <= diff)
            {
                if (me->GetPhaseMask() == 3072)
                    uiCastTimer = urand(15000, 30000);
                else
                    uiCastTimer = urand(5000, 15000);

                uint8 roll = urand(0, 401);
                me->CastSpell(MeteorTarget[roll][0], MeteorTarget[roll][1], MeteorTarget[roll][2], SPELL_FIERY_BOULDER, false);
            } else
                uiCastTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_trade_prince_gallywix : public CreatureScript
{
public:
    npc_trade_prince_gallywix() : CreatureScript("npc_trade_prince_gallywix") { }


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trade_prince_gallywixAI(creature);
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_A_BAZILLION_MACAROONS)
        {
            player->AddAura(SPELL_DEATHWING_ATACK_LOOMING, player);

            if (Creature* caster = player->SummonCreature(NPC_A_BAZILLION_MACAROONS_FIERY_BOULDER_CASTER, -8431.653f, 1350.738f, 129.1189f))
            {
                caster->SetSeerGUID(player->GetGUID());
                caster->SetVisible(false);
                caster->CastSpell(caster, SPELL_A_BAZILLION_MACAROONS_FIERY_BOULDER, false);
                caster->DespawnOrUnsummon(5000);
            }
        }

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
        case QUEST_THE_UNINVITED_GUEST:
            //player->RemoveAura(PHASE_QUEST_ZONE_SPECIFIC_3);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_4, false);
            break;
        case QUEST_LIFE_SAVINGS:
            player->CastSpell(player, SPELL_LIFE_SAVINGS_QUEST_COMPLETE, false);
            player->SendMovieStart(22);
            break;
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        switch (creature->GetPhaseMask())
        {
        case 2048:
            player->SEND_GOSSIP_MENU(15808, creature->GetGUID());
            return true;
        case 28672:
            player->PrepareQuestMenu(creature->GetGUID());
            player->SEND_GOSSIP_MENU(17818, creature->GetGUID());
            return true;
        default:
            return false;
        }
    }

    struct npc_trade_prince_gallywixAI : public ScriptedAI
    {
        npc_trade_prince_gallywixAI(Creature* creature) : ScriptedAI(creature)
        {
            if (me->GetPhaseMask() == 1032)
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            me->setActive(true);
            uiYellTimer = 1000;
        }

        uint32 uiYellTimer;

        void UpdateAI(const uint32 diff)
        {
            if (uiYellTimer <= diff)
            {
                uiYellTimer = urand(20000, 40000);

                switch (me->GetPhaseMask())
                {
                case 1:
                    {
                        uint32 roll = urand(0, 7);

                        switch (roll)
                        {
                        case 0:
                        case 1:
                        case 2:
                            {
                                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                                if (lPlayers.isEmpty())
                                    return;

                                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                    if (Player* player = itr->getSource())
                                        if (player->GetPhaseMask() == 1)
                                            me->DoPersonalScriptText(GALLYWIX_RANDOM_YELL_FIRST_PHASE - roll, player);
                            }
                            break;
                        case 3:
                            {
                                Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();

                                if (lPlayers.isEmpty())
                                    return;

                                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                    if (Player* player = itr->getSource())
                                        if (player->GetPhaseMask() == 1 && player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) != QUEST_STATUS_COMPLETE)
                                            me->DoPersonalScriptText(GALLYWIX_RANDOM_YELL_FIRST_PHASE - roll, player);
                            }
                            break;
                        default:
                            DoScriptText(GALLYWIX_RANDOM_YELL_FIRST_PHASE - roll, me);
                            break;
                        }
                    }
                    break;
                case 2048:
                    {
                        uint32 roll = urand(0, 5);
                        DoScriptText(GALLYWIX_RANDOM_YELL_SECOND_PHASE - roll, me);
                    }
                    break;
                case 12288:
                    {
                        uint32 roll = urand(2, 5);
                        DoScriptText(GALLYWIX_RANDOM_YELL_SECOND_PHASE - roll, me);
                    }
                    break;
                }
            } else
                uiYellTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_goblin_supermodel : public CreatureScript
{
public:
    npc_goblin_supermodel() : CreatureScript("npc_goblin_supermodel") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goblin_supermodelAI(creature);
    }

    struct npc_goblin_supermodelAI : public ScriptedAI
    {
        npc_goblin_supermodelAI(Creature* creature) : ScriptedAI(creature)
        {
            uiStreamBunnyGUID = 0;
            uiHoseBunnyGUID = 0;
            me->SetReactState(REACT_PASSIVE);
            uiCheckTimer = 2000;
        }

        uint64 uiStreamBunnyGUID;
        uint64 uiHoseBunnyGUID;
        uint32 uiCheckTimer;

        void Reset()
        {
            if (me->GetPhaseMask() > 15)
            {
                me->AddAura(SPELL_PERMAMENT_FEIGN_DEATH, me);
                return;
            }

            uint32 uiTargetEntry = 0;

            switch (me->GetEntry())
            {
            case NPC_GOBLIN_SUPERMODEL_2:
                uiTargetEntry = NPC_GOBLIN_SUPERMODEL;
                if (Creature* bunny = Unit::GetCreature(*me, uiHoseBunnyGUID))
                {
                    bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                    bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                } else
                    if (Creature* bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY, -8067.288f, 1503.156f, 10.37352f, 0.0f))
                    {
                        uiHoseBunnyGUID = bunny->GetGUID();
                        bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                        bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                    }
                    break;
            case NPC_GOBLIN_SUPERMODEL_3:
                uiTargetEntry = NPC_GOBLIN_SUPERMODEl_4;
                if (Creature* bunny = Unit::GetCreature(*me, uiHoseBunnyGUID))
                {
                    bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                    bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                } else
                    if (Creature* bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY, -8067.618f, 1494.582f, 10.49888f, 0.0f))
                    {
                        uiHoseBunnyGUID = bunny->GetGUID();
                        bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                        bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                    }
                    break;
            case NPC_GOBLIN_SUPERMODEl_4:
                uiTargetEntry = NPC_GOBLIN_SUPERMODEL_3;
                if (Creature* bunny = Unit::GetCreature(*me, uiHoseBunnyGUID))
                {
                    bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                    bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                } else
                    if (Creature* bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY, -8067.885f, 1485.495f, 10.51868f, 0.0f))
                    {
                        uiHoseBunnyGUID = bunny->GetGUID();
                        bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                        bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                    }
                    break;
            case NPC_GOBLIN_SUPERMODEL:
                uiTargetEntry = NPC_GOBLIN_SUPERMODEL_2;
                if (Creature* bunny = Unit::GetCreature(*me, uiHoseBunnyGUID))
                {
                    bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                    bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                } else
                    if (Creature* bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY, -8067.641f, 1494.582f, 10.52287f, 0.0f))
                    {
                        uiHoseBunnyGUID = bunny->GetGUID();
                        bunny->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                        bunny->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_HOSE);
                    }
                    break;
            }

            std::list<Creature*> lTargets;
            me->GetCreatureListWithEntryInGrid(lTargets, uiTargetEntry, 30.0f);

            if (lTargets.empty())
                return;

            for (std::list<Creature*>::const_iterator itr = lTargets.begin(); itr != lTargets.end(); ++itr)
                if ((*itr)->GetPhaseMask() == 15)
                    if (Creature* target = (*itr))
                    {
                        float x, y, z, o;
                        target->GetPosition(x, y, z, o);

                        if (Creature* bunny = Unit::GetCreature(*me, uiStreamBunnyGUID))
                        {
                            bunny->GetMotionMaster()->MoveFollow(target, 0.0f, 0);
                            me->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, bunny->GetGUID());
                            me->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_STREAM);
                        } else
                            if (Creature* bunny = me->SummonCreature(NPC_ELM_GENERAL_PURPOSE_BUNNY_NOT_FLOATING, x, y, z, o))
                            {
                                uiStreamBunnyGUID = bunny->GetGUID();
                                bunny->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                                me->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, bunny->GetGUID());
                                me->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_GAS_FIGHTER_GAS_STREAM);
                            }
                    }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiCheckTimer <= diff)
            {
                uiCheckTimer = 30000;
                Reset();
            } else
                uiCheckTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

class spell_kaja_kola : public SpellScriptLoader
{
public:
    spell_kaja_kola() : SpellScriptLoader("spell_kaja_kola") { }

    class spell_kaja_kola_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_kaja_kola_SpellScript)

            void RandomIdea(SpellEffIndex effIndex)
        {
            Player* player = NULL;

            if (Unit* target = GetCaster())
                player = target->ToPlayer();

            if (!player)
                return;

            uint8 roll = urand(0, 57);
            player->DoPersonalScriptText(KAJA_COLA_RANDOM_IDEA - roll, player);
            player->AddAura(SPELL_KAJA_COLA, player);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_kaja_kola_SpellScript::RandomIdea, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_kaja_kola_SpellScript();
    }
};

class npc_brute_enforcer : public CreatureScript
{
public:
    npc_brute_enforcer() : CreatureScript("npc_brute_enforcer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brute_enforcerAI(creature);
    }

    struct npc_brute_enforcerAI : public ScriptedAI
    {
        npc_brute_enforcerAI(Creature* creature) : ScriptedAI(creature)
        {
            me->setActive(true);
            lPlayers.clear();
            uiBruteGUID = 0;

            if (me->GetPhaseMask() != 15)
                return;

            if (!me->isSummon())
                if (Creature* brute = me->SummonCreature(NPC_BRUTE_ENFORCER, -8451.56f, 1249.52f, 57.3227f, 4.24208f))
                    uiBruteGUID = brute->GetGUID();
        }

        std::list<Psc_t> lPlayers;
        uint64 uiBruteGUID;

        bool CanSay(uint64 uiPlayerGUID)
        {
            for (std::list<Psc_t>::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                if (itr->uiPlayerGUID == uiPlayerGUID)
                    return false;

            return true;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetPhaseMask() != 15)
                return;

            if (!me->IsWithinDistInMap(who, 10.0f) || !me->HasInArc(2 * M_PI / 3, who))
                return;

            float z = me->GetPositionZ();

            if (z - who->GetPositionZ() < 0)
                return;

            Player* player = who->ToPlayer();

            if (!player)
                return;

            uint64 uiPlayerGUID = player->GetGUID();

            if (me->isSummon())
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (Creature* brute = summoner->ToCreature())
                        if (!CAST_AI(npc_brute_enforcerAI, brute->AI())->CanSay(uiPlayerGUID))
                            return;

                for (std::list<Psc_t>::iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    if (itr->uiPlayerGUID == uiPlayerGUID)
                    {
                        if (itr->uiEventMask != CAN_SAY)
                            return;

                        (*itr).uiEventMask = SAID;
                        uint8 roll = urand(0, 3);
                        me->DoPersonalScriptText(BRUTE_ENFORCER_SECOND_RANDOM_SAY - roll, player);
                    }
            } else
            {
                if (!CanSay(uiPlayerGUID))
                    return;

                if (Creature* brute = Unit::GetCreature(*me, uiBruteGUID))
                    if (!CAST_AI(npc_brute_enforcerAI, brute->AI())->CanSay(uiPlayerGUID))
                        return;

                uint8 roll = urand(0, 3);
                me->DoPersonalScriptText(BRUTE_ENFORCER_FIRST_RANDOM_SAY - roll, player);
                Psc_t new_psc;
                new_psc.uiEventMask = SAID;
                new_psc.uiPlayerGUID = uiPlayerGUID;
                new_psc.uiEventTimer = urand(20000, 40000);
                lPlayers.push_back(new_psc);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!lPlayers.empty())
                for (std::list<Psc_t>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
                {
                    if ((*itr).uiEventMask == CAN_SAY)
                    {
                        ++itr;
                        continue;
                    }

                    if ((*itr).uiEventTimer <= diff)
                    {
                        if (Creature* brute = Unit::GetCreature(*me, uiBruteGUID))
                        {
                            Psc_t new_psc;
                            new_psc.uiEventMask = CAN_SAY;
                            new_psc.uiPlayerGUID = (*itr).uiPlayerGUID;
                            new_psc.uiEventTimer = urand(20000, 40000);
                            CAST_AI(npc_brute_enforcerAI, brute->AI())->lPlayers.push_back(new_psc);
                        }

                        itr = lPlayers.erase(itr);
                    } else
                    {
                        (*itr).uiEventTimer -= diff;
                        ++itr;
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

///////////
// Quest 447 14125
///////////

class go_overloaded_generator : public GameObjectScript
{
public:
    go_overloaded_generator() : GameObjectScript("go_overloaded_generator") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        std::list<Creature*> lControls;
        player->GetAllMinionsByEntry(lControls, NPC_OVERLOADED_GENERATOR);

        //if (!player->HasAura(PHASE_QUEST_ZONE_SPECIFIC_6))
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_6, false);

        if (!lControls.empty())
            for (std::list<Creature*>::iterator itr = lControls.begin(); itr != lControls.end();)
                if (Creature* generator = (*itr))
                {
                    player->m_Controlled.erase(generator);
                    itr = lControls.erase(itr);
                    generator->DespawnOrUnsummon();
                } else
                    ++itr;

                if (Creature* generator = player->SummonCreature(NPC_OVERLOADED_GENERATOR, -8420.861f, 1372.611f, 105.7554f))
                {
                    player->m_Controlled.insert(generator);
                    generator->SetSeerGUID(player->GetGUID());
                    generator->SetVisible(false);
                    generator->CastSpell(generator, SPELL_OVERLOADED_GENERATOR_VISUAL, false);
                }

                return false;
    }
};

class go_stove_leak : public GameObjectScript
{
public:
    go_stove_leak() : GameObjectScript("go_stove_leak") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        std::list<Creature*> lControls;
        player->GetAllMinionsByEntry(lControls, NPC_STOVE_LEAK);

        //if (!player->HasAura(PHASE_QUEST_ZONE_SPECIFIC_6))
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_6, false);

        if (!lControls.empty())
            for (std::list<Creature*>::iterator itr = lControls.begin(); itr != lControls.end();)
                if (Creature* leak = (*itr))
                {
                    player->m_Controlled.erase(leak);
                    itr = lControls.erase(itr);
                    leak->DespawnOrUnsummon();
                } else
                    ++itr;

                if (Creature* leak = player->SummonCreature(NPC_STOVE_LEAK, -8402.417f, 1371.373f, 105.6856f))
                {
                    player->m_Controlled.insert(leak);
                    leak->SetSeerGUID(player->GetGUID());
                    leak->SetVisible(false);
                    leak->CastSpell(leak, SPELL_STOVE_LEAK_VISUAL, false);
                }

                return false;
    }
};

class go_smoldering_bed : public GameObjectScript
{
public:
    go_smoldering_bed() : GameObjectScript("go_smoldering_bed") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        //if (!player->HasAura(PHASE_QUEST_ZONE_SPECIFIC_6))
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_6, false);

        player->CastSpell(player, SPELL_SMOLDERING_BED_PRECAST_MASTER, false);
        return false;
    }
};

class spell_smoldering_bed_precast_master : public SpellScriptLoader
{
public:
    spell_smoldering_bed_precast_master() : SpellScriptLoader("spell_smoldering_bed_precast_master") { }

    class spell_smoldering_bed_precast_master_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_smoldering_bed_precast_master_SpellScript)

            void Effect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Player* player = NULL;

            if (Unit* caster = GetCaster())
                player = caster->ToPlayer();

            if (!player)
                return;

            std::list<Creature*> lControls;
            player->GetAllMinionsByEntry(lControls, NPC_SMOLDERING_BED);

            if (!lControls.empty())
                for (std::list<Creature*>::iterator itr = lControls.begin(); itr != lControls.end();)
                    if (Creature* bed = (*itr))
                    {
                        player->m_Controlled.erase(bed);
                        itr = lControls.erase(itr);
                        bed->DespawnOrUnsummon();
                    } else
                        ++itr;

                    if (Creature* bed = player->SummonCreature(NPC_SMOLDERING_BED, -8402.31f, 1363.601f, 118.27f))
                    {
                        player->m_Controlled.insert(bed);
                        bed->SetSeerGUID(player->GetGUID());
                        bed->SetVisible(false);
                        bed->CastSpell(bed, SPELL_SMOLDERING_BED_VISUAL, false);
                    }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_smoldering_bed_precast_master_SpellScript::Effect, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_smoldering_bed_precast_master_SpellScript();
    }
};

class go_gasbot_control_panel : public GameObjectScript
{
public:
    go_gasbot_control_panel() : GameObjectScript("go_gasbot_control_panel") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        QuestStatusMap::iterator itr = player->getQuestStatusMap().find(QUEST_447);

        if (itr == player->getQuestStatusMap().end())
            return false;

        QuestStatusData& q_status = itr->second;

        if (q_status.Status == QUEST_STATUS_INCOMPLETE)
            for (int i = 0; i < 3; ++i)
                if (q_status.CreatureOrGOCount[i] != 1)
                    return false;

        std::list<Creature*> lControls;
        player->GetAllMinionsByEntry(lControls, NPC_GASBOT);

        if (!lControls.empty())
            for (std::list<Creature*>::iterator itr = lControls.begin(); itr != lControls.end();)
                if (Creature* gasbot = (*itr))
                {
                    player->m_Controlled.erase(gasbot);
                    itr = lControls.erase(itr);
                    gasbot->DespawnOrUnsummon();
                } else
                    ++itr;

                if (Creature* gasbot = player->SummonCreature(NPC_GASBOT, -8424.346f, 1328.036f, 102.0427f, 1.570796f))
                {
                    player->m_Controlled.insert(gasbot);
                    gasbot->SetSeerGUID(player->GetGUID());
                    gasbot->SetVisible(false);
                    float x, y, z;
                    gasbot->GetPosition(x, y, z);

                    if (Vehicle* vehicle = gasbot->GetVehicleKit())
                        for (int i = 0; i < 4; ++i)
                            if (Creature* target = gasbot->SummonCreature(NPC_GASBOT_GAS_TARGET, x, y, z))
                            {
                                target->SetSeerGUID(player->GetGUID());
                                target->SetVisible(false);
                                target->EnterVehicle(gasbot, i);
                            }

                            gasbot->CastSpell(gasbot, SPELL_GASBOT_GAS_STREAM, false);
                }

                return true;
    }
};

class npc_gasbot : public CreatureScript
{
public:
    npc_gasbot() : CreatureScript("npc_gasbot") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gasbotAI(creature);
    }

    struct npc_gasbotAI : public npc_escortAI
    {
        npc_gasbotAI(Creature* creature) : npc_escortAI(creature)
        {
            Go = true;
            Boom = false;
            Say = false;
            uiGoTimer = 1500;
            uiSayTimer = 500;
            me->SetPhaseMask(12288, true);
            SetDespawnAtEnd(false);
            SetDespawnAtFar(false);
        }

        void Reset()
        {
            if (Boom)
                me->AddAura(SPELL_BOOM_INVIS_MODEL, me);
        }

        uint32 uiGoTimer;
        uint32 uiSayTimer;
        bool Say;
        bool Go;
        bool Boom;

        void DespawnPlayerControled(Player* player, uint32 uiControledEntry)
        {
            std::list<Creature*> lControls;
            player->GetAllMinionsByEntry(lControls, uiControledEntry);

            if (!lControls.empty())
                for (std::list<Creature*>::iterator itr = lControls.begin(); itr != lControls.end();)
                    if (Creature* controled = (*itr))
                    {
                        player->m_Controlled.erase(controled);
                        itr = lControls.erase(itr);
                        controled->DespawnOrUnsummon();
                    } else
                        ++itr;
        }

        void WaypointReached(uint32 point)
        {
            if (point == 3)
            {
                Player* player = NULL;

                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        player = summoner->ToPlayer();

                if (!player)
                    return;

                DoCast(SPELL_GASBOT_EXPLOSION);
                //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_7, false);

                if (Vehicle* vehicle = me->GetVehicleKit())
                    for (int i = 0; i < 4; ++i)
                        if (Unit* passenger = vehicle->GetPassenger(i))
                            if (Creature* target = passenger->ToCreature())
                                target->DespawnOrUnsummon();

                DespawnPlayerControled(player, NPC_OVERLOADED_GENERATOR);
                DespawnPlayerControled(player, NPC_STOVE_LEAK);
                DespawnPlayerControled(player, NPC_SMOLDERING_BED);
                player->m_Controlled.erase(me);
                player->KilledMonsterCredit(NPC_GASBOT, 0);

                me->DespawnOrUnsummon(2000);
                me->AddAura(SPELL_BOOM_INVIS_MODEL, me);
                Boom = true;
                Say = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (Say)
                if (uiSayTimer <= diff)
                {
                    Say = false;
                    uiSayTimer = 500;

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                                if (Creature* claims_adjuster = player->FindNearestCreature(NPC_CLAIMS_ADJUSTER, 75.0f))
                                    claims_adjuster->DoPersonalScriptText(CLAIMS_ADJUSTER_YELL_VICTIM, player);
                } else
                    uiSayTimer -= diff;

                if (Go)
                    if (uiGoTimer <= diff)
                    {
                        Go = false;
                        uiGoTimer = 1500;
                        Start(false);
                    } else
                        uiGoTimer -= diff;

                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
        }
    };
};

class npc_hotrod_escort : public CreatureScript
{
public:
    npc_hotrod_escort() : CreatureScript("npc_hotrod_escort") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hotrod_escortAI(creature);
    }

    struct npc_hotrod_escortAI : public npc_vehicle_escortAI
    {
        npc_hotrod_escortAI(Creature* creature) : npc_vehicle_escortAI(creature)
        {
            Event = false;
            uiEventPhase = 1;
            uiEventTimer = 1000;
            uiCastTimer = 100;
        }

        uint32 uiEventTimer;
        uint32 uiCastTimer;
        uint8 uiEventPhase;
        bool Event;

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
            case 61:
                Event = true;
                if (me->isSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        if (Player* player = summoner->ToPlayer())
                            if (Vehicle* vehicle = me->GetVehicleKit())
                                if (Unit* passenger = vehicle->GetPassenger(0))
                                    if (Creature* sassy = passenger->ToCreature())
                                        sassy->DoPersonalScriptText(SASSY_SAY_GO_AROUND, player);
                break;
            case 125:
                me->CastSpell(me, SPELL_HOT_ROD_SKID, false);
                break;
            case 126:
                Event = true;
                uiEventPhase = 2;
                uiEventTimer = 3500;
                if (Vehicle* vehicle = me->GetVehicleKit())
                {
                    if (Unit* passenger = vehicle->GetPassenger(1))
                        passenger->ExitVehicle();

                    if (me->isSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (Player* player = summoner->ToPlayer())
                            {
                                if (Unit* passenger = vehicle->GetPassenger(0))
                                    passenger->DoPersonalScriptText(SASSY_SAY_OH_NO, player);

                                player->CompleteQuest(QUEST_LIFE_SAVINGS);
                                player->SaveToDB();

                                switch (player->getGender())
                                {
                                case GENDER_MALE:
                                    if (Creature* candy_cane = me->FindNearestCreature(NPC_CANDY_CANE, 30.0f))
                                    {
                                        Psc_ls new_psc;
                                        new_psc.uiPersonalTimer = 1000;
                                        new_psc.uiPlayerGUID = player->GetGUID();
                                        CAST_AI(npc_candy_cane::npc_candy_caneAI, candy_cane->AI())->lPlayers.push_back(new_psc);
                                    }
                                    break;
                                case GENDER_FEMALE:
                                    if (Creature* chip_endale = me->FindNearestCreature(NPC_CHIP_ENDALE, 30.0f))
                                    {
                                        Psc_ls new_psc;
                                        new_psc.uiPersonalTimer = 1000;
                                        new_psc.uiPlayerGUID = player->GetGUID();
                                        CAST_AI(npc_chip_endale::npc_chip_endaleAI, chip_endale->AI())->lPlayers.push_back(new_psc);
                                    }
                                    break;
                                }
                            }
                }
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiCastTimer <= diff)
            {
                uiCastTimer = 100;
                DoCast(SPELL_LS_HOT_ROD_KNOCKBACK);
            } else
                uiCastTimer -= diff;

            if (Event)
                if (uiEventTimer <= diff)
                {
                    Event = false;
                    uiEventTimer = 1000;

                    if (uiEventPhase == 1)
                    {
                        if (me->isSummon())
                            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                if (Player* player = summoner->ToPlayer())
                                    if (Vehicle* vehicle = me->GetVehicleKit())
                                        if (Unit* passenger = vehicle->GetPassenger(0))
                                            if (Creature* sassy = passenger->ToCreature())
                                                sassy->DoPersonalScriptText(SASSY_YELL_OUT_OF_THE_WAY, player);
                    } else
                    {
                        if (Vehicle* vehicle = me->GetVehicleKit())
                            if (Unit* passenger = vehicle->GetPassenger(0))
                                if (Creature* sassy = passenger->ToCreature())
                                    sassy->DespawnOrUnsummon();

                        me->DespawnOrUnsummon();
                    }
                } else
                    uiEventTimer -= diff;

                npc_vehicle_escortAI::UpdateAI(diff);
        }
    };
};

class npc_villa_mook_ls : public CreatureScript
{
public:
    npc_villa_mook_ls() : CreatureScript("npc_villa_mook_ls") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_villa_mook_lsAI(creature);
    }

    struct npc_villa_mook_lsAI : public ScriptedAI
    {
        npc_villa_mook_lsAI(Creature* creature) : ScriptedAI(creature)
        {
            uiKnockbackTimer = 5000;
            uiKnockbackData = DATA_KNOCKBACK_NONE;
        }

        uint32 uiKnockbackTimer;
        uint32 uiKnockbackData;

        uint32 GetData(uint32 data) const
        {
            if (data == DATA_KNOCKBACK)
                return uiKnockbackData;

            return 0;
        }

        void MoveKnockback(Creature* hotrod)
        {
            if (hotrod->GetEntry() == NPC_HOT_ROD_LS)
            {
                uiKnockbackData = DATA_KNOCKBACK_KNOCKBACK;
                uint8 s_roll = urand(0, 13);
                uint8 e_roll = urand(0, 6);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, RollingEmoteState[e_roll]);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiKnockbackData == DATA_KNOCKBACK_KNOCKBACK)
                if (uiKnockbackTimer <= diff)
                {
                    uiKnockbackTimer = 5000;
                    uiKnockbackData = DATA_KNOCKBACK_NONE;
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                    me->GetMotionMaster()->MoveTargetedHome();
                } else
                    uiKnockbackTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
        }
    };
};

class spell_ls_hot_rod_knockback : public SpellScriptLoader
{
public:
    spell_ls_hot_rod_knockback() : SpellScriptLoader("spell_ls_hot_rod_knockback") { }

    class spell_ls_hot_rod_knockback_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ls_hot_rod_knockback_SpellScript)

            void Knockback(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            Creature* creature = NULL;
            Creature* hotrod = NULL;

            if (Unit* target = GetHitUnit())
                creature = target->ToCreature();

            if (Unit* caster = GetCaster())
                hotrod = caster->ToCreature();

            if (!(creature && hotrod))
                return;

            if (creature->AI()->GetData(DATA_KNOCKBACK) != DATA_KNOCKBACK_NONE)
                return;

            if (creature->IsNonMeleeSpellCasted(true))
                creature->InterruptNonMeleeSpells(true);

            float x, y;
            hotrod->GetPosition(x, y);

            switch (creature->GetEntry())
            {
            case NPC_VILLA_MOOK:
                creature->KnockbackFrom(x, y, 7.0f, 10.0f);
                CAST_AI(npc_villa_mook_ls::npc_villa_mook_lsAI, creature->AI())->MoveKnockback(hotrod);
                break;
            default:
                creature->KnockbackFrom(x, y, 15.0f, 10.0f);
                CAST_AI(npc_kezan_citizen::npc_kezan_citizenAI, creature->AI())->MoveKnockback(hotrod);
                break;
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_ls_hot_rod_knockback_SpellScript::Knockback, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ls_hot_rod_knockback_SpellScript();
    }
};

class spell_radio : public SpellScriptLoader
{
public:
    spell_radio() : SpellScriptLoader("spell_radio") { }

    class  spell_radioSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_radioSpellScript);

        bool Validate(SpellInfo const* spellEntry)
        {
            st = false;
            return true;
        }

        bool Load()
        {
            return true;
        }

        void SendPacketToPlayers(WorldPacket const* data, Player* player) const
        {
            player->GetSession()->SendPacket(data);
        }

        void HandleOnHit()
        {
            if (st)
                return;

            if (Unit* caster = GetCaster()->GetOwner())
            {
                //if (caster->GetTypeId() == TYPEID_PLAYER)
                //    GetCaster()->PlayDistanceSound(RADIO, caster->ToPlayer());
                //st = true;
            }
        }

    private:
        bool st;

        void Register()
        {
            OnHit += SpellHitFn(spell_radioSpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_radioSpellScript();
    }
};

class go_ls_rope_ladder : public GameObjectScript
{
public:
    go_ls_rope_ladder() : GameObjectScript("go_ls_rope_ladder") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        player->TeleportTo(LOST_ISLES, -7864.02f, 1831.4f, -0.956502f, 6.12391f);
        player->GetMotionMaster()->MoveJump(-7861.84f, 1831.27f, 8.53755f, 5.0f, 15.0f);
        return true;
    }
};

class go_yacht_boarding_mortar : public GameObjectScript
{
public:
    go_yacht_boarding_mortar() : GameObjectScript("go_yacht_boarding_mortar") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        player->TeleportTo(LOST_ISLES, -7879.2f, 1828.67f, 5.55337f, 0.251894f);
        player->CastSpell(player, SPELL_LAST_CHANCE_YACHT_BOARDING_MORTAR, false);
        return true;
    }
};

class item_keys_to_the_hot_rod : public ItemScript
{
public:
    item_keys_to_the_hot_rod() : ItemScript("item_keys_to_the_hot_rod") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        player->CastSpell(player, SPELL_SUMMON_HOT_ROD, false);

        return true;
    }
};

void AddSC_kezan()
{
    new npc_sister_goldskimmer();
    new npc_maxx_avalanche();
    new npc_fizz_lighter();
    new npc_evol_fingers();
    new npc_bamm_megabomb();
    new npc_slinky_sharpshiv();
    new npc_warrior_matic_nx_01();
    new npc_sassy_sardwrench();
    new npc_foreman_dampwick();
    new npc_defiant_troll();
    new npc_megs_dreadshredder();
    new npc_hot_rod();
    new npc_homies_follower();
    new npc_kezan_citizen();
    new npc_coach_crosscheck();
    new npc_steamwheedle_shark();
    new npc_bilgewater_buccaneer_enter();
    new npc_bilgewater_buccaneer();
    new npc_bilgewater_buccaneer_goal();
    new npc_fourth_and_goal_target();
    new npc_deathwing_qfag();
    new npc_bruno_flameretardant();
    new npc_frankie_gearslipper();
    new npc_jack_the_hammer();
    new npc_sudsy_magee();
    new npc_chip_endale();
    new npc_candy_cane();
    new npc_fbok_bank_teller();
    new npc_szabo();
    new npc_missa_spekkies();
    new npc_gappy_silvertooth();
    new npc_kezan_partygoer();
    new npc_first_bank_of_kezan_vault();
    new npc_hired_looter();
    new npc_villa_mook();
    new npc_pirate_party_crasher();
    new npc_ktc_waiter();
    new npc_kezan_socialite();
    new npc_hobart_grapplehammer();
    new npc_assistant_greely();
    new npc_kaja_cola_balloon();
    new npc_vinny_slapchop();
    new npc_general_purpose_bunny();
    new npc_trade_prince_gallywix();
    new npc_goblin_supermodel();
    new npc_brute_enforcer();
    new npc_gasbot();
    new npc_hotrod_escort();
    new npc_villa_mook_ls();

    new go_first_bank_of_kezan_vault();
    new go_kajamite_deposit();
    new go_overloaded_generator();
    new go_stove_leak();
    new go_smoldering_bed();
    new go_gasbot_control_panel();
    new go_ls_rope_ladder();
    new go_yacht_boarding_mortar();

    new spell_goblin_all_in_1_der_belt_shocker();
    new spell_keys_to_the_hot_rod();
    new spell_summon_hot_rod();
    new spell_punch_it_boost();
    new spell_hot_rod_honk_horn();
    new spell_hot_rod_knockback();
    new spell_permanent_feign_death();
    new spell_kaja_kola();
    new spell_smoldering_bed_precast_master();
    new spell_ls_hot_rod_knockback();
    new spell_kick_footbomb();
    new spell_radio();

    new item_keys_to_the_hot_rod();
}
/*
*
* Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
* Copyright (C) 2011-2012 ArkCORE <http://www.arkania.net/>
*
*/

#include "lost_isles.h"

class npc_Zapnozzle : public CreatureScript
{
public:
    npc_Zapnozzle() : CreatureScript("npc_Zapnozzle") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ZapnozzleAI(creature);
    }

    bool OnQuestReward(Player* player, Creature* creature, const Quest* _Quest, uint32 )
    {
        if (_Quest->GetQuestId() == QUEST_DONT_GO_TO_THE_LIGHT)
            creature->AI()->DoAction(1);
        return true;
    }

    struct npc_ZapnozzleAI : public ScriptedAI
    {
        npc_ZapnozzleAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset ()
        {
            eventTalk = 0;
            eventTalk2 = 0;
            mui_talk = 2000;
            mui_talk2 = 2000;
            isEventInProgress = false;
            start = false;
        }

        void DoAction(const int32 param)
        {
            if (param == 1)
                isEventInProgress = true;
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (id == 1)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!isEventInProgress)
            {
                if (mui_talk <= diff)
                {
                    mui_talk = 6000;
                    if (!start)
                        player = me->FindNearestPlayer(10);
                    if (!player)
                        return;
                    if (!start)
                    {
                        if (!player->HasAura(SPELL_DEAD_STILL))
                            return;
                        else
                            start = true;
                    }
                    switch (eventTalk)
                    {
                    case 0:
                        me->MonsterSay(SAY_D_A, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 1:
                        me->MonsterSay(SAY_D_B, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 2:
                        if (Creature* gizmo = me->FindNearestCreature(NPC_GIZMO, 10))
                            gizmo->MonsterSay(SAY_D_C, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 3:
                        me->MonsterSay(SAY_D_D, LANG_UNIVERSAL, player->GetGUID());
                        me->CastSpell(player, SPELL_DEFIBRILLATE, true);
                        break;
                    default:
                        break;
                    }
                    eventTalk++;
                }
                else
                    mui_talk -= diff;
            }
            if (!isEventInProgress)
                return;
            if (mui_talk2 <= diff)
            {
                mui_talk2 = 6000;
                if (!start)
                    player = me->FindNearestPlayer(10);
                if (!player)
                    return;
                switch(eventTalk2)
                {
                case 0:
                    me->MonsterSay(SAY_D_E, LANG_UNIVERSAL, player->GetGUID());
                    me->CastSpell(player, SPELL_DEFIBRILLATE, true);
                    player->RemoveAurasDueToSpell(SPELL_DEAD_STILL);
                    player->SetPhaseMask(1, true);
                    break;
                case 1:
                    me->MonsterSay(SAY_D_F, LANG_UNIVERSAL, player->GetGUID());
                    break;
                case 2:
                    me->MonsterSay(SAY_D_G, LANG_UNIVERSAL, player->GetGUID());
                    break;
                case 3:
                    me->CastSpell(player, SPELL_DEFIBRILLATE, true);
                    me->MonsterSay(SAY_D_H, LANG_UNIVERSAL, player->GetGUID());
                    mui_talk2 = 2000;
                    break;
                case 4:
                    me->SetSpeed(MOVE_SWIM, 2, true);
                    me->GetMotionMaster()->MovePoint(1, cPostitions[0]);
                    break;
                default:
                    eventTalk2 = 0;
                    isEventInProgress = false;
                    eventTalk = 0;
                    start = false;
                    break;
                }
                eventTalk2++;
            }
            else
                mui_talk2 -= diff;
        }

    private:
        bool isEventInProgress, start;
        uint32 mui_talk, mui_talk2;
        unsigned int eventTalk, eventTalk2;
        Player *player;
    };
};

class npc_Mechumide : public CreatureScript
{
public:
    npc_Mechumide() : CreatureScript("npc_Mechumide") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_MINER_TROUBLES)
        {
            if (Creature* mineur = player->SummonCreature(NPC_FRIGHTENED_MINER, player->GetPositionX() + 2, player->GetPositionY(),  player->GetPositionZ() + 2,  player->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                if (Creature* chariot = player->SummonCreature(NPC_MINE_TROUBLES_ORE_CART, player->GetPositionX() - 2, player->GetPositionY(),  player->GetPositionZ() + 2,  player->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    mineur->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    chariot->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    mineur->CastSpell(chariot, SPELL_MINE_TROUBLES_ORE_CART_CHAIN, true);
                    chariot->GetMotionMaster()->MoveFollow(mineur, 1.0f, 1.0f);
                    mineur->MonsterSay(SAY_0, LANG_UNIVERSAL, 0);
                    CAST_AI(npc_escortAI, (mineur->AI()))->Start(true, true, player->GetGUID(), _Quest);
                }
        }
        return true;
    }
};

class npc_mineur_gob : public CreatureScript
{
public:
    npc_mineur_gob() : CreatureScript("npc_mineur_gob") { }

    struct npc_mineur_gobAI : public npc_escortAI
    {
        npc_mineur_gobAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 i)
        {
            SetRun(false);
            switch(i)
            {
            case 6:
                me->MonsterSay(SAY_1, LANG_UNIVERSAL, 0);
                //me->LoadEquipment(2901, true);
                me->HandleEmoteCommand(467);
                break;
            case 9:
                me->MonsterSay(SAY_2, LANG_UNIVERSAL, 0);
                //me->LoadEquipment(2901, true);
                me->HandleEmoteCommand(467);
                break;
            case 13:
                me->MonsterSay(SAY_3, LANG_UNIVERSAL, 0);
                break;
            case 12:
                //me->LoadEquipment(2901, true);
                me->HandleEmoteCommand(467);
                break;
            case 17:
                //me->LoadEquipment(2901, true);
                me->HandleEmoteCommand(467);
                break;
            case 18:
                if (Player* player = me->FindNearestPlayer(20))
                {
                    me->MonsterSay(SAY_4, LANG_UNIVERSAL, player->GetGUID());
                    player->KilledMonsterCredit(NPC_MINE_TROUBLES_KILL_CREDIT, 0);
                }
                else
                    me->MonsterSay(SAY_4, LANG_UNIVERSAL, 0);
                if (Creature* c = me->FindNearestCreature(NPC_MINE_TROUBLES_ORE_CART, 10))
                    c->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }

        void SetHoldState(bool bOnHold)
        {
            SetEscortPaused(bOnHold);
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (m_ui_attack <= diff)
            {
                if (Creature* cb = me->FindNearestCreature(NPC_SMART_MINING_MONKEY, 10, true))
                    SetHoldState(true);
                else
                    SetHoldState(false);
                m_ui_attack = 1000;
            }
            else m_ui_attack -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 m_ui_attack;
        uint32 krennansay;
        bool AfterJump;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mineur_gobAI(creature);
    }
};

class spell_68281 : public SpellScriptLoader
{
public:
    spell_68281() : SpellScriptLoader("spell_68281") { }

    class  spell_68281SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_68281SpellScript);

        bool Validate(SpellInfo const* spellEntry)
        {
            st = false;
            st2 = false;
            return true;
        }

        bool Load()
        {
            return true;
        }

        void HandleBeforeHit()
        {
            if (st2)
                return;

            if (GetCastItem())
                if (Unit* caster = GetCastItem()->GetOwner())
                    caster->CastSpell(caster, SPELL_FLASH_EFFECT, true);
            st2 = true;
        }

        void HandleAfterHit()
        {
            if (st)
                return;
            st = true;
        }

        void Unload()
        {
            if (GetCastItem())
                if (Unit* caster = GetCastItem()->GetOwner())
                    caster->RemoveAura(SPELL_FLASH_EFFECT);
        }

    private:
        bool st, st2;

        void Register()
        {
            BeforeHit += SpellHitFn(spell_68281SpellScript::HandleBeforeHit);
            AfterHit += SpellHitFn(spell_68281SpellScript::HandleAfterHit);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_68281SpellScript();
    }
};

class npc_singe_bombe : public CreatureScript
{
public:
    npc_singe_bombe() : CreatureScript("npc_singe_bombe") { }

    struct npc_singe_bombeAI : public ScriptedAI
    {
        npc_singe_bombeAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            mui_rocket = 45000;
            canCastRocket = true;
            isActive = true;
            isRandomMoving = false;
        }

        void DoAction(const int32 param)
        {
            isActive = false;
            canCastRocket = false;
            me->CastSpell(me, SPELL_EXPLODING_BANANAS, true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_NITRO_POTASSIUM_BANANAS)
            {
                isActive = false;
                canCastRocket = false;
                if (me->IsNonMeleeSpellCasted(true))
                    me->InterruptNonMeleeSpells(true);
                if (Creature* t = me->SummonCreature(me->GetEntry(), me->GetPositionX(), me->GetPositionY(),  me->GetPositionZ(),
                    me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    t->AI()->DoAction(0);
                    t->DespawnOrUnsummon(3000);
                }
                me->DespawnOrUnsummon();
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who->ToPlayer())
                return;
            if (!me->IsWithinDistInMap(who, 20.0f))
                return;
            if (canCastRocket)
                me->CastSpell(me, SPELL_BOMB, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!isActive)
                return;
            if (mui_rocket <= diff)
            {
                canCastRocket = true;
                mui_rocket = urand(30000, 45000);
                if (!isRandomMoving)
                {
                    me->GetMotionMaster()->MoveRandom(10.0f);
                    isRandomMoving = true;
                }
            }
            else
                mui_rocket -= diff;
        }

    private:
        uint32 mui_rocket;
        bool canCastRocket;
        bool isActive;
        bool isRandomMoving;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_singe_bombeAI (creature);
    }
};

class spell_weed_whacker : public SpellScriptLoader
{
public:
    spell_weed_whacker() : SpellScriptLoader("spell_weed_whacker") { }

    class spell_weed_whacker_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_weed_whacker_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetCastItem())
                if (Unit* caster = GetCastItem()->GetOwner())
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* player = caster->ToPlayer();
                        if (player->HasAura(SPELL_WEED_WHACKER))
                            player->RemoveAura(SPELL_WEED_WHACKER);
                        else if (player->GetQuestStatus(QUEST_WEED_WHACKER) == QUEST_STATUS_INCOMPLETE)
                            player->CastSpell(player, SPELL_WEED_WHACKER, true);
                    }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_weed_whacker_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_weed_whacker_SpellScript();
    }
};

class npc_lianne_gobelin : public CreatureScript
{
public:
    npc_lianne_gobelin() : CreatureScript("npc_lianne_gobelin") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lianne_gobelinAI(creature);
    }

    struct npc_lianne_gobelinAI : public ScriptedAI
    {
        npc_lianne_gobelinAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            if (me->GetVehicleKit())
                if (Creature* c = me->FindNearestCreature(NPC_ORC_SCOUT, 10))
                    c->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, me, false);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* c = me->FindNearestCreature(NPC_ORC_SCOUT, 100))
                c->ToCreature()->AI()->Talk(irand(0, 7));
        }
    };
};

class npc_killag_sangrecroc : public CreatureScript
{
public:
    npc_killag_sangrecroc() : CreatureScript("npc_killag_sangrecroc") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_TO_THE_CLIFFS)
        {
            if (Creature* t = player->SummonCreature(NPC_BASTIA, player->GetPositionX(), player->GetPositionY(),  player->GetPositionZ(),
                player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
            {
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, false);
                CAST_AI(npc_escortAI, (t->AI()))->Start(false, true, player->GetGUID(), _Quest);
            }
        }
        if (_Quest->GetQuestId() == QUEST_IFRARED_IFDRADEAD)
        {
            player->RemoveAura(SPELL_ORC_SCOUT);
            player->RemoveAura(SPELL_IFRARED_HEAT_FOALS);
            for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                if ((*itr)->GetTypeId() == TYPEID_UNIT && (*itr)->GetEntry() == NPC_ORC_SCOUT_2)
                    (*itr)->ToCreature()->DespawnOrUnsummon();
        }
        return true;
    }

    bool OnQuestReward(Player* player, Creature* creature, const Quest *_Quest, uint32 )
    {
        if (_Quest->GetQuestId() == QUEST_IFRARED_IFDRADEAD)
        {
            player->RemoveAura(SPELL_ORC_SCOUT);
            player->RemoveAura(SPELL_IFRARED_HEAT_FOALS);
            for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                if ((*itr)->GetTypeId() == TYPEID_UNIT && (*itr)->GetEntry() == NPC_ORC_SCOUT_2)
                    (*itr)->ToCreature()->DespawnOrUnsummon();
        }
        return true;
    }
};

class npc_pant_gob : public CreatureScript
{
public:
    npc_pant_gob() : CreatureScript("npc_pant_gob") { }

    struct npc_pant_gobAI : public npc_escortAI
    {
        npc_pant_gobAI(Creature* creature) : npc_escortAI(creature) { }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Start(false, true, who->GetGUID());
        }

        void WaypointReached(uint32 i)
        {
            SetRun(true);
            switch(i)
            {
            case 17:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pant_gobAI (creature);
    }
};

class npc_gyrocoptere_quest_giver : public CreatureScript
{
public:
    npc_gyrocoptere_quest_giver() : CreatureScript("npc_gyrocoptere_quest_giver") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_PRECIOUS_CARGO)
        {
            if (Creature* t = player->SummonCreature(NPC_PRIDE_OF_KEZAN, creature->GetPositionX(), creature->GetPositionY(),  creature->GetPositionZ(),
                creature->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
            {
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, false);
                CAST_AI(npc_escortAI, (t->AI()))->Start(false, true, player->GetGUID(), _Quest);
                t->AI()->Talk(SAY_GYRO, player->GetGUID());
            }
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_PRECIOUS_CARGO) != QUEST_STATUS_NONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, SAY_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

};

class npc_tornade_gob : public CreatureScript
{
public:
    npc_tornade_gob() : CreatureScript("npc_tornade_gob") { }

    struct npc_tornade_gobAI : public npc_escortAI
    {
        npc_tornade_gobAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset()
        {
            _checkQuest = 1000;
            _checkDespawn = 1000;
            isBoarded = false;
            isBoarded2 = false;
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (apply)
            {
                me->SetCanFly(true);
                me->SetSpeed(MOVE_FLIGHT, 3.0f);
                isBoarded = true;
                me->AddAura(SPELL_CYCLONE_OF_THE_ELEMENTS, me);
                Start(false, true, who->GetGUID(), NULL, false, true);
            }
            else
                me->RemoveAura(SPELL_CYCLONE_OF_THE_ELEMENTS);
        }

        void WaypointReached(uint32 i)
        {
            me->SetCanFly(true);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (isBoarded)
            {
                if (isBoarded2)
                {
                    if (_checkDespawn <= diff)
                    {
                        me->GetVehicleKit()->RemoveAllPassengers();
                        me->DespawnOrUnsummon();
                        _checkDespawn = 1000;
                    }
                    else
                        _checkDespawn -= diff;
                }
                else
                {
                    if (_checkQuest <= diff)
                    {
                        if (me->GetVehicleKit())
                            if (Unit* u = me->GetVehicleKit()->GetPassenger(0))
                                if (u->GetTypeId() == TYPEID_PLAYER)
                                {
                                    Player* player = u->ToPlayer();
                                    if (player->GetQuestStatus(QUEST_WARCHIEVS_REVENGE) == QUEST_STATUS_COMPLETE)
                                    {
                                        isBoarded2 = true;
                                        _checkDespawn = 70000;
                                        SetEscortPaused(true);
                                        me->GetMotionMaster()->MovePoint(1, cPostitions[1]);
                                    }
                                }
                                _checkQuest = 1000;
                    }
                    else
                        _checkQuest -= diff;
                }
            }
        }

    private:
        uint32 _checkQuest;
        uint32 _checkDespawn;
        bool isBoarded;
        bool isBoarded2;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tornade_gobAI (creature);
    }
};

class gob_fronde_gobelin : public GameObjectScript
{
public:
    gob_fronde_gobelin() : GameObjectScript("gob_fronde_gobelin") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_UP_UP_AWAY) != QUEST_STATUS_NONE)
        {
            if (Creature* t = player->SummonCreature(NPC_SLING_ROCKET, go->GetPositionX(), go->GetPositionY(),  go->GetPositionZ() + 7,  go->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, false);
            return true;
        }
        return true;
    }
};

class npc_fusee_gob : public CreatureScript
{
public:
    npc_fusee_gob() : CreatureScript("npc_fusee_gob") { }

    struct npc_fusee_gobAI : public npc_escortAI
    {
        npc_fusee_gobAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset()
        {
            me->SetCanFly(true);
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (apply)
                Start(false, true, who->GetGUID());
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 5.0f);
        }

        void WaypointReached(uint32 i)
        {
            if (i == 3)
            {
                me->CastSpell(me, SPELL_UP_UP_AWAY_KILL_CREDIT_EXPLOSION, true);
                me->GetVehicleKit()->RemoveAllPassengers();
                me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fusee_gobAI (creature);
    }
};

class gob_dyn_gobelin : public GameObjectScript
{
public:
    gob_dyn_gobelin() : GameObjectScript("gob_dyn_gobelin") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_ITS_A_TOWN_IN_A_BOX) != QUEST_STATUS_NONE)
        {
            player->CastSpell(player, SPELL_ITS_A_TOWN_IN_A_BOX_PLUNGER_EFF, true);
            if (Creature* t = player->SummonCreature(9100000, go->GetPositionX(), go->GetPositionY(),  go->GetPositionZ() + 2,  go->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30 * IN_MILLISECONDS))
            {
                t->SetPhaseMask(4, true);
                t->CastSpell(t, SPELL_ITS_A_TOWN_IN_A_BOX_EFF1, true);
                t->CastSpell(t, SPELL_ITS_A_TOWN_IN_A_BOX_EFF2, true);
                t->CastSpell(t, SPELL_ITS_A_TOWN_IN_A_BOX_EFF3, true);
                t->CastSpell(t, SPELL_ITS_A_TOWN_IN_A_BOX_EFF4, true);
            }
            return true;
        }
        return true;
    }
};

class npc_poule : public CreatureScript
{
public:
    npc_poule() : CreatureScript("npc_poule") { }

    struct npc_pouleAI : public ScriptedAI
    {
        npc_pouleAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            me->GetMotionMaster()->MoveRandom(10.0f);
        }

        void DoAction(const int32 param)
        {
            me->CastSpell(me, SPELL_EXPLODING_BANANAS, true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_NITRO_POTASSIUM_BANANAS)
            {
                if (me->IsNonMeleeSpellCasted(true))
                    me->InterruptNonMeleeSpells(true);
                if (Creature* t = me->SummonCreature(me->GetEntry(), me->GetPositionX(), me->GetPositionY(),  me->GetPositionZ(),
                    me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    t->AI()->DoAction(0);
                    t->DespawnOrUnsummon(3000);
                }
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pouleAI (creature);
    }
};

class spell_egg_gob : public SpellScriptLoader
{
public:
    spell_egg_gob() : SpellScriptLoader("spell_egg_gob") { }

    class spell_egg_gobSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_egg_gobSpellScript);

        bool Validate(SpellInfo const* spellEntry)
        {
            return true;
        }

        bool Load()
        {
            if (Unit* caster = GetCastItem()->GetOwner())
            {
                if (GameObject* go = caster->FindNearestGameObject(GO_PIEGE, 5))
                {
                    if (Creature* c = go->FindNearestCreature(NPC_SPINY_RAPTOR, 10))
                        c->AI()->DoAction(1);
                }
            }
            return true;
        }

        void HandleActivateGameobject(SpellEffIndex effIndex)
        { }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_egg_gobSpellScript::HandleActivateGameobject, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_egg_gobSpellScript();
    }
};

class npc_raptore_gob : public CreatureScript
{
public:
    npc_raptore_gob() : CreatureScript("npc_raptore_gob") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raptore_gobAI(creature);
    }

    struct npc_raptore_gobAI : public ScriptedAI
    {
        npc_raptore_gobAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset ()
        {
            start = true;
            me->GetMotionMaster()->MoveRandom(20);
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (id == 42 && !start)
            {
                if (Player* player = me->FindNearestPlayer(10))
                    player->SummonGameObject(GO_UNDEFINED_1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, 0, 0, 0, 0, 10 * IN_MILLISECONDS);
                me->Kill(me);
            }
        }

        void DoAction(const int32 param)
        {
            if (param == 1)
            {
                if (go = me->FindNearestGameObject(GO_PIEGE, 25))
                {
                    start = false;
                    me->CastSpell(me, SPELL_SUMMON_SPINY_RAPTOR_EGG, true);
                    me->GetMotionMaster()->MovePoint(42, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            DoMeleeAttackIfReady();
        }

    private:
        bool start;
        GameObject* go;
    };
};

class SparkSearcher
{
public:
    SparkSearcher(Creature const* source, float range) : _source(source), _range(range) { }

    bool operator()(Unit* unit)
    {
        if (!unit->isAlive())
            return false;

        switch (unit->GetEntry())
        {
        case NPC_MECHASHARK_X_STEAM:
            break;
        default:
            return false;
        }

        if (!unit->IsWithinDist(_source, _range, false))
            return false;
        return true;
    }

private:
    Creature const* _source;
    float _range;
};

class gob_spark_gobelin : public GameObjectScript
{
public:
    gob_spark_gobelin() : GameObjectScript("gob_spark_gobelin") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_A_GOBLIN_IN_SHARKS_CLOTHING) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature *t = player->SummonCreature(NPC_MECHASHARK_X_STEAM, go->GetPositionX(), go->GetPositionY(),  go->GetPositionZ() + 2,  go->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30 * IN_MILLISECONDS))
            {
                std::list<Creature*> temp;
                SparkSearcher check(t, 100.0f);
                Trinity::CreatureListSearcher<SparkSearcher> searcher(t, temp, check);
                t->VisitNearbyGridObject(100.0f, searcher);
                for (std::list<Creature*>::iterator itr = temp.begin(); itr != temp.end(); ++itr)
                    if ((*itr)->ToTempSummon())
                        if ((*itr)->ToTempSummon()->GetSummoner())
                            if ((*itr)->ToTempSummon()->GetSummoner()->GetTypeId() == TYPEID_PLAYER)
                                if ((*itr)->ToTempSummon()->GetSummoner()->GetGUID() == player->GetGUID() && (*itr)->GetGUID() != t->GetGUID())
                                    (*itr)->DespawnOrUnsummon();
                player->EnterVehicle(t);
            }
            return true;
        }
        return true;
    }
};

class npc_young_naga_gob : public CreatureScript
{
public:
    npc_young_naga_gob() : CreatureScript("npc_young_naga_gob") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_young_naga_gobAI(creature);
    }

    struct npc_young_naga_gobAI : public ScriptedAI
    {
        npc_young_naga_gobAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset ()
        {
            ui_findPlayer = 1000;
            me->GetMotionMaster()->MoveRandom(5);
        }

        void UpdateAI(const uint32 diff)
        {
            if (ui_findPlayer <= diff)
            {
                if (Player* player = me->FindNearestPlayer(10))
                {
                    if (player->HasAura(SPELL_IRRESISTIBLE_POOL_PONY) && player->GetQuestStatus(QUEST_IRRESISTIBLE_POOL_PONY) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* naga = player->SummonCreature(NPC_NAGA_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 40 * IN_MILLISECONDS))
                        {
                            naga->CastSpell(naga, SPELL_IRRESISTIBLE_POOL_PONY_EFF, true);
                            naga->GetMotionMaster()->MoveFollow(player, urand (urand(urand(1, 2), urand(3, 4)), urand(5, 6)),  urand(urand(urand(1, 2), urand(3, 4)), urand(5, 6)));
                            player->KilledMonsterCredit(NPC_NAGA_HATCHLING_KILL_CREDIT, 1);
                        }
                        me->DespawnOrUnsummon();
                    }
                }
                ui_findPlayer = 1000;
            }
            else
                ui_findPlayer -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 ui_findPlayer;
    };
};

class YoungNagaSearcher
{
public:
    YoungNagaSearcher(Creature const* source, float range) : _source(source), _range(range) { }

    bool operator()(Unit* unit)
    {
        if (!unit->isAlive())
            return false;

        switch (unit->GetEntry())
        {
        case NPC_NAGA_HATCHLING_1:
            break;
        case NPC_NAGA_HATCHLING_2:
            break;
        case NPC_NAGA_HATCHLING_3:
            break;
        case NPC_NAGA_HATCHLING_4:
            break;
        default:
            return false;
        }

        if (!unit->IsWithinDist(_source, _range, false))
            return false;
        return true;
    }

private:
    Creature const* _source;
    float _range;
};

class npc_megs_isle_gob : public CreatureScript
{
public:
    npc_megs_isle_gob() : CreatureScript("npc_megs_isle_gob") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_SURREND_OR_ELSE)
            player->SummonCreature(NPC_CRACK, player->GetPositionX(), player->GetPositionY(),  player->GetPositionZ(),  player->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
        else if (_Quest->GetQuestId() == QUEST_GET_BACK_TO_TOWN)
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_6, true);
        return true;
    }

    bool OnQuestReward(Player* player, Creature* creature, const Quest *_Quest, uint32 )
    {
        if (_Quest->GetQuestId() == QUEST_IRRESISTIBLE_POOL_PONY)
        {
            std::list<Creature*> temp;
            YoungNagaSearcher check(creature, 900.0f);
            Trinity::CreatureListSearcher<YoungNagaSearcher> searcher(creature, temp, check);
            creature->VisitNearbyGridObject(900.0f, searcher);
            for (std::list<Creature*>::iterator itr = temp.begin(); itr != temp.end(); ++itr)
                if ((*itr)->ToTempSummon())
                    if ((*itr)->ToTempSummon()->GetSummoner())
                        if ((*itr)->ToTempSummon()->GetSummoner()->GetTypeId() == TYPEID_PLAYER)
                            if ((*itr)->ToTempSummon()->GetSummoner()->GetGUID() == player->GetGUID())
                                (*itr)->DespawnOrUnsummon();
        }
        return true;
    }
};

class npc_crack_isle_gob : public CreatureScript
{
public:
    npc_crack_isle_gob() : CreatureScript("npc_crack_isle_gob") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crack_isle_gobAI(creature);
    }

    struct npc_crack_isle_gobAI : public ScriptedAI
    {
        npc_crack_isle_gobAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset ()
        {
            playerGUID = 0;
            if (Unit* p = me->ToTempSummon()->GetSummoner())
                if (p->GetTypeId() == TYPEID_PLAYER)
                    playerGUID = p->GetGUID();
            if (playerGUID == 0)
            {
                me->DespawnOrUnsummon();
                return;
            }
            if (Player* player =  Unit::GetPlayer(*me, playerGUID))
            {
                me->GetMotionMaster()->MoveFollow(player, 1.0f, 1.0f);
                me->MonsterSay(QUEST_RESET_CRACK, LANG_UNIVERSAL, player->GetGUID());
            }
            else
                me->DespawnOrUnsummon();
            ui_findPlayer = 1000;
            start = false;
            event = false;
            combats = false;
            mui_event = 2000;
            event_p = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!start)
            {
                if (ui_findPlayer <= diff)
                {
                    if (Creature* zone = me->FindNearestCreature(NPC_FACELESS_OF_THE_DEEP_VOID_ZONE, 3))
                    {
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                        {
                            me->MonsterSay(CRACK_PROVOC, LANG_UNIVERSAL, player->GetGUID());
                            if (naga = player->SummonCreature(NPC_FACELESS_OF_THE_DEEP, zone->GetPositionX(), zone->GetPositionY(), zone->GetPositionZ() + 2, zone->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60 * IN_MILLISECONDS))
                                naga->setFaction(35);
                        }
                        zone->DespawnOrUnsummon();
                        start = true;
                        event = true;
                    }
                    ui_findPlayer = 1000;
                }
                else
                    ui_findPlayer -= diff;
            }
            else if (event)
            {
                if (mui_event <= diff)
                {
                    mui_event  = 4000;
                    switch (event_p)
                    {
                    case 0:
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                            naga->MonsterYell(NAGA_SAY_A, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 1:
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                            naga->MonsterYell(NAGA_SAY_B, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 2:
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                            naga->MonsterYell(NAGA_SAY_C, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    case 3:
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                            naga->MonsterYell(NAGA_SAY_D, LANG_UNIVERSAL, player->GetGUID());
                        break;
                    default:
                        if (Player* player =  Unit::GetPlayer(*me, playerGUID))
                            me->MonsterYell(CRACK_EVADE, LANG_UNIVERSAL, player->GetGUID());
                        mui_event = 2000;
                        event = false;
                        combats = true;
                        break;
                    }
                    event_p++;
                }
                else mui_event -= diff;
            }
            else if (combats)
            {
                if (mui_event <= diff)
                {
                    combats = false;
                    naga->setFaction(14);
                    mui_event  = 4000;
                    me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                }
                else
                    mui_event -= diff;
            }
            else
            {
                if (mui_event <= diff)
                    me->DespawnOrUnsummon();
                else
                    mui_event -= diff;
            }
        }

    private:
        uint32 ui_findPlayer;
        Creature *naga;
        uint64 playerGUID;
        bool start, event, combats;
        uint32 mui_event;
        int event_p;
    };
};

class npc_oom_isle_gob : public CreatureScript
{
public:
    npc_oom_isle_gob() : CreatureScript("npc_oom_isle_gob") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oom_isle_gobAI(creature);
    }

    struct npc_oom_isle_gobAI : public ScriptedAI
    {
        npc_oom_isle_gobAI(Creature* creature) : ScriptedAI(creature) { }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CANNON_BLAST)
                caster->Kill(me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };
};

class npc_ceint : public CreatureScript
{
public:
    npc_ceint() : CreatureScript("npc_ceint") { }

    bool OnQuestReward(Player* player, Creature* creature, const Quest* _Quest, uint32 )
    {
        if (_Quest->GetQuestId() == QUEST_ZOMBIES_VS_SUUPER_BOOSTER_R_B)
        {
            if (Creature* c = player->FindNearestCreature(NPC_SUPER_BOOSTER_ROCKET_BOOTS, 10))
            {
                player->RemoveAura(SPELL_ZVSBRB_SUMMON_SPELL);
                player->RemoveAura(SPELL_ZVSBRB_DAMAGE_TRIGGER);
                c->Kill(c);
            }
        }
        return true;
    }
};

class spell_boot_gob : public SpellScriptLoader
{
public:
    spell_boot_gob() : SpellScriptLoader("spell_boot_gob") { }

    class spell_boot_gobSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_boot_gobSpellScript);

        bool Validate(SpellInfo const * spellEntry)
        {
            st = false;
            return true;
        }

        bool Load()
        {
            return true;
        }

        void HandleOnHit()
        {
            if (Unit* caster = GetCastItem()->GetOwner())
                if (caster->GetTypeId() == TYPEID_PLAYER)
                    caster->ToPlayer()->GetMotionMaster()->ForceMoveJump(1480.31f, 1269.97f, 110.0f, 50.0f, 50.0f, 300.0f);
        }

    private:
        bool st;

        void Register()
        {
            OnHit += SpellHitFn(spell_boot_gobSpellScript::HandleOnHit);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_boot_gobSpellScript();
    }
};

class npc_izzy_airplane : public CreatureScript
{
public:
    npc_izzy_airplane() : CreatureScript("npc_izzy_airplane") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_OLD_FRIENDS)
        {
            if (Creature* airplane = player->SummonCreature(NPC_FLYING_BOMBER, creature->GetPositionX(), creature->GetPositionY(),  creature->GetPositionZ(),  creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 2, airplane, false);
                if (Creature* t = player->SummonCreature(NPC_SASSY_HARDWRENCH, creature->GetPositionX(),  creature->GetPositionY(),  creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 750000))
                    t->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, airplane, false);
            }
        }
        return true;
    }
};

class npc_avion_gob : public CreatureScript
{
public:
    npc_avion_gob() : CreatureScript("npc_avion_gob") { }

    struct npc_avion_gobAI : public npc_escortAI
    {
        npc_avion_gobAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset()
        {
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 6.0f);
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                if (apply)
                    Start(false, true, who->GetGUID());
        }

        void WaypointReached(uint32 i)
        {
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 6.0f);
            Creature* chipie = me->FindNearestCreature(NPC_SASSY_HARDWRENCH, 5, true);
            if (!chipie)
                return;
            switch(i)
            {
            case 1:
                if (Player* player = me->FindNearestPlayer(10))
                    chipie->MonsterSay(SAY_6, LANG_UNIVERSAL, player->GetGUID());
                break;
            case 4:
                chipie->MonsterSay(SAY_7, LANG_UNIVERSAL, 0);
                break;
            case 8:
                chipie->MonsterSay(SAY_8, LANG_UNIVERSAL, 0);
                break;
            case 17:
                Creature* chipie;
                Player* player;

                if (Unit* unit = me->GetVehicleKit()->GetPassenger(0))
                    chipie = unit->ToCreature();
                if (Unit* unit = me->GetVehicleKit()->GetPassenger(1))
                    player = unit->ToPlayer();
                if (chipie && player)
                {
                    me->GetVehicleKit()->RemoveAllPassengers();
                    player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 3, me, false);
                    chipie->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, me, false);
                    me->RemoveAura(SPELL_FLYING_BOMBERS_ROPE_LADDER);
                }
                chipie->MonsterSay(SAY_9, LANG_UNIVERSAL, 0);
                break;
            case 26:
                me->GetVehicleKit()->RemoveAllPassengers();
                if (Creature *chip = me->FindNearestCreature(NPC_SASSY_HARDWRENCH, 5, true))
                    chip->DespawnOrUnsummon();
                me->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_avion_gobAI (creature);
    }
};

class npc_tremblement_volcano : public CreatureScript
{
public:
    npc_tremblement_volcano() : CreatureScript("npc_tremblement_volcano") { }

    struct npc_tremblement_volcanoAI : public ScriptedAI
    {
        npc_tremblement_volcanoAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            mui_soufle = 2000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (mui_soufle <= diff)
            {
                me->CastSpell(me, SPELL_CAMERA_SHAKE, true);
                mui_soufle = 3000;
            }
            else
                mui_soufle -= diff;
        }

    private:
        uint32 mui_soufle;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tremblement_volcanoAI (creature);
    }
};

class npc_Chariot : public CreatureScript
{
public:
    npc_Chariot() : CreatureScript("npc_Chariot") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_WILD_MINE_CART_RIDE)
        {
            if (Creature* chariot = player->SummonCreature(NPC_MINE_CART, creature->GetPositionX(), creature->GetPositionY(),  creature->GetPositionZ(),  creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                int cnt = 1;
                chariot->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, chariot, false);
                for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                    if ((*itr)->GetTypeId() == TYPEID_UNIT && ((*itr)->GetEntry() == NPC_ACE || (*itr)->GetEntry() == NPC_ASISTANT_GREELY || (*itr)->GetEntry() == NPC_IZZY || (*itr)->GetEntry() == NPC_GOBBER))
                    {
                        cnt++;
                        (*itr)->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, cnt, chariot, false);
                    }
                    CAST_AI(npc_escortAI, (chariot->AI()))->Start(false, true, player->GetGUID(), _Quest);
            }
        }
        return true;
    }
};

class npc_Chariot2 : public CreatureScript
{
public:
    npc_Chariot2() : CreatureScript("npc_Chariot2") { }

    struct npc_Chariot2AI : public npc_escortAI
    {
        npc_Chariot2AI(Creature* creature) : npc_escortAI(creature) { }

        uint32 krennansay;
        bool AfterJump;

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_UNIT)
                if (!apply)
                    who->ToCreature()->DespawnOrUnsummon();
        }

        void WaypointReached(uint32 i)
        {
            switch(i)
            {
            case 12:
                me->GetVehicleKit()->RemoveAllPassengers();
                if (Creature* t = me->SummonCreature(NPC_CAULDRON_TRIGGER, me->GetPositionX(), me->GetPositionY(),  me->GetPositionZ(),  me->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10 * IN_MILLISECONDS))
                {
                    t->CastSpell(t, SPELL_MECHANISM_EXPLOSION, true);
                    t->CastSpell(t, SPELL_DALARAN_FORGEARMS_SMOKEFX, true);
                }
                me->DespawnOrUnsummon();
                if (Unit *player = me->ToTempSummon()->GetSummoner())
                    player->GetMotionMaster()->MoveJump(2354.36f, 1943.21f, 24.0f, 20.0f, 20.0f);
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Chariot2AI (creature);
    }
};

class gob_red_but : public GameObjectScript
{
public:
    gob_red_but() : GameObjectScript("gob_red_but") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_GOOD_BYE_SWEET_OIL) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->GetPhaseMask() == 32768)
                return false;

            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_9, true);
            player->AddAura(SPELL_VOLCANIC_SKYBOX, player);
            player->KilledMonsterCredit(NPC_SWEET_OIL_KILL_CREDIT, 0);
            if (Creature* t = player->SummonCreature(NPC_CAULDRON_TRIGGER, 2477.0f, 2082.0f,  14.0f,  go->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10 * IN_MILLISECONDS))
            {
                t->SetPhaseMask(32768, true);
                t->CastSpell(t, SPELL_MECHANISM_EXPLOSION, true);
                t->CastSpell(t, SPELL_DALARAN_FORGEARMS_SMOKEFX, true);
            }
            if (Creature* t = player->SummonCreature(NPC_CAULDRON_TRIGGER, 2499.28f, 2091.48f,  17.0f,  go->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10 * IN_MILLISECONDS))
            {
                t->SetPhaseMask(32768, true);
                t->CastSpell(t, SPELL_MECHANISM_EXPLOSION, true);
                t->CastSpell(t, SPELL_DALARAN_FORGEARMS_SMOKEFX, true);
            }
            if (Creature* t = player->SummonCreature(NPC_CAULDRON_TRIGGER, 2450.424f, 2068.89f,  28.0f,  go->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10 * IN_MILLISECONDS))
            {
                t->SetPhaseMask(32768, true);
                t->SetCanFly(true);
                t->CastSpell(t, SPELL_MECHANISM_EXPLOSION, true);
                t->CastSpell(t, SPELL_DALARAN_FORGEARMS_SMOKEFX, true);
            }
            return true;
        }
        return true;
    }
};

class npc_grilly_2 : public CreatureScript
{
public:
    npc_grilly_2() : CreatureScript("npc_grilly_2") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_THE_SLAVE_PITS)
        {
            if (Creature* pant = player->SummonCreature(NPC_FOOT_BOMB_UNIFORM, player->GetPositionX(), player->GetPositionY(),
                player->GetPositionZ(),  player->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, pant, false);
        }
        return true;
    }
};

class npc_Prince : public CreatureScript
{
public:
    npc_Prince() : CreatureScript("npc_Prince") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_PrinceAI(creature);
    }

    struct npc_PrinceAI : public ScriptedAI
    {
        npc_PrinceAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset ()
        {
            eventTalk = 0;
            eventTalk2 = 0;
            mui_talk = 2000;
            mui_talk2 = 2000;
            isEventInProgress = false;
            start = false;
            end = false;
            mui1 = 10000;
            mui2 = 30000;
            mui3 = 50000;
            mui4 = 40000;
            mui5 = 60000;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (isEventInProgress)
                return;
            if (who->ToPlayer())
                return;
            if (!me->IsWithinDistInMap(who, 5.0f))
                return;
            if (end)
                return;
            if (who->GetEntry() == NPC_ULTIMATE_FOOT_BOMB_UNIFORM)
            {
                isEventInProgress = true;
                me->setFaction(14);
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (end)
            {
                damage = 0;
                return;
            }
            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->setFaction(35);
                //DoScriptText(-1039588, me);
                end = true;
                if (Creature* c = me->FindNearestCreature(NPC_ULTIMATE_FOOT_BOMB_UNIFORM, 30))
                    c->DespawnOrUnsummon();
                if (Player* pl = me->FindNearestPlayer(10))
                    pl->KilledMonsterCredit(NPC_TRADE_PRINCE_GALLYWIX, 0);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!isEventInProgress)
                return;

            if (end)
            {
                if (mui_talk <= diff)
                {
                    mui_talk = 10000;
                    switch (eventTalk)
                    {
                    case 0:
                        //		  DoScriptText(-1039590, me);
                        break;
                    case 1:
                        if (Creature* c = me->FindNearestCreature(NPC_THRALL, 30))
                        {
                            //		      DoScriptText(-1039591, c);
                        }
                        break;
                    case 2:
                        //		  DoScriptText(-1039589, me);
                        break;
                    case 3:
                        if (Creature* c = me->FindNearestCreature(NPC_THRALL, 30))
                        {
                            //		      DoScriptText(-1039592, c);
                        }
                        break;
                    case 4:
                        if (Creature* c = me->FindNearestCreature(NPC_THRALL, 30))
                        {
                            //		      DoScriptText(-1039593, c);
                        }
                        break;
                    case 5:
                        me->DespawnOrUnsummon();
                        if (Creature* c = me->FindNearestCreature(NPC_THRALL, 30))
                        {
                            c->DespawnOrUnsummon();
                        }
                        break;
                    default:
                        break;
                    }
                    eventTalk++;
                }
                else
                    mui_talk -= diff;
            }
            else
            {
                if (!me->getVictim())
                    return;
                if (mui1 <= diff)
                {
                    switch (urand(0, 4))
                    {
                    case 0:
                        me->CastSpell(me->getVictim(), SPELL_REVENUE_STREAM, true);
                        //		  DoScriptText(-1039583, me);
                        break;
                    case 1:
                        me->CastSpell(me->getVictim(), SPELL_UNLOAD_TOXIC_ASSETS, true);
                        //		  DoScriptText(-1039584, me);
                        break;
                    case 2:
                        me->CastSpell(me->getVictim(), SPELL_YOU_ARE_FIRED, true);
                        //		  DoScriptText(-1039586, me);
                        break;
                    case 3:
                        me->CastSpell(me->getVictim(), SPELL_CALL_A_MEETING, true);
                        //		  DoScriptText(-1039582, me);
                        break;
                    case 4:
                        me->CastSpell(me->getVictim(), SPELL_DOWNSIZING, true);
                        break;
                    default:
                        me->CastSpell(me->getVictim(), SPELL_DOWNSIZING, true);
                        break;
                    }
                    mui1 = 5000;
                }
                else
                    mui1 -= diff;
            }
            DoMeleeAttackIfReady();
        }

    private:
        bool isEventInProgress, start, end;
        uint32 mui_talk, mui_talk2, mui1, mui2, mui3, mui4, mui5;
        unsigned int eventTalk, eventTalk2;
        Player* player;
    };
};

class npc_boot : public CreatureScript
{
public:
    npc_boot() : CreatureScript("npc_boot") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* _Quest)
    {
        if (_Quest->GetQuestId() == QUEST_VICTORY)
        {
            player->AddAura(SPELL_ROCKIN_POWER_INFUSED_ROCKET_BOOTS, player);
            //player->CastSpell(player, PHASE_QUEST_ZONE_SPECIFIC_4, true);
            player->AddAura(SPELL_VOLCANIC_SKYBOX, player);
            player->GetMotionMaster()->ForceMoveJump(2352.31f, 2483.97f, 13.0f, 15.0f, 20.0f, 150.0f);
        }

        if (_Quest->GetQuestId() == QUEST_THE_PRIDE_OF_KEZAN)
        {
            if (Creature* t = player->SummonCreature(NPC_PRIDE_OF_KEZAN, player->GetPositionX(), player->GetPositionY(),  player->GetPositionZ(),
                player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3 * IN_MILLISECONDS))
            {
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, false);
                t->SetCanFly(true);
                t->SetSpeed(MOVE_FLIGHT, 6.0f);
            }
        }

        if (_Quest->GetQuestId() == QUEST_FINAL_CONFRONTATION)
        {
            if (Creature* t = player->SummonCreature(NPC_ULTIMATE_FOOT_BOMB_UNIFORM, player->GetPositionX(), player->GetPositionY(),  player->GetPositionZ(),
                player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3 * IN_MILLISECONDS))
                player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, false);
        }

        return true;
    }
};

class BootSearcher
{
public:
    bool operator()(WorldObject* object)
    {
        if (!object)
            return true;
        Unit* unit = object->ToUnit();
        if (!unit || !unit->isAlive() || unit->GetTypeId() == TYPEID_PLAYER)
            return true;

        if (unit->ToCreature())
        {
            switch (unit->ToCreature()->GetEntry())
            {
            case NPC_GOBLIN_ZOMBIE:
                return false;
            default:
                break;
            }
        }
        return true;
    }
};

class spell_boot_damage : public SpellScriptLoader
{
public:
    spell_boot_damage() : SpellScriptLoader("spell_boot_damage") { }

    class spell_boot_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_boot_damage_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(BootSearcher());
            if (GetCaster())
                if (!GetCaster()->HasAura(SPELL_SUPER_BOOSTER_ROCKET_BOOTS))
                {
                    GetCaster()->RemoveAura(SPELL_ZVSBRB_DAMAGE_TRIGGER);
                    targets.clear();
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boot_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_boot_damage_SpellScript();
    }
};

void AddSC_lost_isle()
{
    new npc_Zapnozzle();
    new npc_singe_bombe();
    new npc_mineur_gob();
    new npc_Mechumide();
    new npc_lianne_gobelin();
    new npc_killag_sangrecroc();
    new npc_pant_gob();
    new npc_gyrocoptere_quest_giver();
    new npc_tornade_gob();
    new npc_fusee_gob();
    new npc_poule();
    new npc_raptore_gob();
    new npc_young_naga_gob();
    new npc_megs_isle_gob();
    new npc_crack_isle_gob();
    new npc_oom_isle_gob();
    new npc_ceint();
    new npc_izzy_airplane();
    new npc_avion_gob();
    new npc_Chariot();
    new npc_Chariot2();
    new npc_grilly_2();
    new npc_Prince();
    new npc_boot();

    new spell_68281();
    new spell_weed_whacker();
    new spell_boot_damage();
    new spell_boot_gob();

    new gob_fronde_gobelin();
    new gob_dyn_gobelin();
    new gob_spark_gobelin();
    new gob_red_but();
}

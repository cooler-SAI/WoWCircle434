#include "ScriptPCH.h"
#include "LFGMgr.h"
#include "Group.h"
#include "deadmines.h"

enum Spells
{
    SPELL_WHO_IS_THAT       = 89339,
    SPELL_SETIATED          = 89267,
    SPELL_SETIATED_H        = 92834,
    SPELL_NAUSEATED         = 89732,
    SPELL_NAUSEATED_H       = 92066,
    SPELL_ROTTEN_AURA       = 89734,
    SPELL_ROTTEN_AURA_DMG   = 89734,
    SPELL_CAULDRON          = 89250,
    SPELL_CAULDRON_VISUAL   = 89251,
    SPELL_CAULDRON_FIRE     = 89252,

    SPELL_THROW_FOOD_01     = 90557,
    SPELL_THROW_FOOD_02     = 90560,
    SPELL_THROW_FOOD_03     = 90603,
    SPELL_THROW_FOOD_04     = 89739,
    SPELL_THROW_FOOD_05     = 90605,
    SPELL_THROW_FOOD_06     = 90556,
    SPELL_THROW_FOOD_07     = 90680,
    SPELL_THROW_FOOD_08     = 90559,
    SPELL_THROW_FOOD_09     = 90602,
    SPELL_THROW_FOOD_10     = 89263,
    SPELL_THROW_FOOD_11     = 90604,
    SPELL_THROW_FOOD_12     = 90555,
    SPELL_THROW_FOOD_13     = 90606,
};

enum Adds
{
    NPC_BABY_MURLOC         = 48672,

    NPC_CAULDRON            = 47754,

    NPC_BUN                 = 48301,
    NPC_MISTERY_MEAT        = 48297,
    NPC_BREAD_LOAF          = 48300,
    NPC_STEAK               = 48296,
    NPC_CORN                = 48006,
    NPC_MELON               = 48294,

    NPC_ROTTEN_SNEAK        = 48295,
    NPC_ROTTEN_CORN         = 48276,
    NPC_ROTTEN_LOAF         = 48299,
    NPC_ROTTEN_MELON        = 48293,
    NPC_ROTTEN_MISTERY_MEAT = 48298,
    NPC_ROTTEN_BUN          = 48302,
};

enum Events
{
    EVENT_THROW_FOOD        = 1,
};

const Position notePos = {-74.3611f, -820.014f, 40.3714f, 0.0f};

const uint32 GOOD_FOOD[] = {
    SPELL_THROW_FOOD_01,
    SPELL_THROW_FOOD_02,
    SPELL_THROW_FOOD_06,
    SPELL_THROW_FOOD_07,
    SPELL_THROW_FOOD_08,
    SPELL_THROW_FOOD_10,
    SPELL_THROW_FOOD_12,
};
const uint32 BAD_FOOD[] = {
    SPELL_THROW_FOOD_03,
    SPELL_THROW_FOOD_04,
    SPELL_THROW_FOOD_05,
    SPELL_THROW_FOOD_07,
    SPELL_THROW_FOOD_09,
    SPELL_THROW_FOOD_11,
    SPELL_THROW_FOOD_13,
};

class boss_captain_cookie : public CreatureScript
{
    public:
        boss_captain_cookie() : CreatureScript("boss_captain_cookie") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_captain_cookieAI (pCreature);
        }

        struct boss_captain_cookieAI : public BossAI
        {
            boss_captain_cookieAI(Creature* pCreature) : BossAI(pCreature, DATA_CAPTAIN)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (instance->GetBossState(DATA_ADMIRAL) == DONE)
                    me->SetReactState(REACT_AGGRESSIVE);
                else
                    me->SetReactState(REACT_PASSIVE);
                DoCast(SPELL_WHO_IS_THAT);
                me->setActive(true);
            }

            void Reset()
            {
                _Reset();
                nextFoodWillBeGood = false;
            }

            void EnterCombat(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                {
                    me->RemoveAurasDueToSpell(SPELL_WHO_IS_THAT);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    DoCastAOE(SPELL_CAULDRON, true);
                }
                events.ScheduleEvent(EVENT_THROW_FOOD, 5000);
                instance->SetBossState(DATA_CAPTAIN, IN_PROGRESS);
            }

            void JustDied(Unit* killer)
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                {
                    Player* pPlayer = players.begin()->getSource();
                    if (pPlayer && pPlayer->GetGroup())
                        sLFGMgr->FinishDungeon(pPlayer->GetGroup()->GetGUID(), 326);
                }
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* pPlayer = itr->getSource())
                        pPlayer->CompletedAchievement(sAchievementMgr->GetAchievement(IsHeroic() ? 5083 : 628));

                _JustDied();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THROW_FOOD:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                uint32 nextFoodIndex = urand(0, 6);
                                if (nextFoodWillBeGood)
                                    DoCast(target, GOOD_FOOD[nextFoodIndex]);
                                else
                                    DoCast(target, BAD_FOOD[nextFoodIndex]);
                                nextFoodWillBeGood = !nextFoodWillBeGood;
                            }
                            events.ScheduleEvent(EVENT_THROW_FOOD, 2100);
                            break;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                switch (id)
                {
                case POINT_CAPTAIN_ENTER_DECK:
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveAurasDueToSpell(SPELL_WHO_IS_THAT);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    DoCastAOE(SPELL_CAULDRON, true);
                    break;
                }
            }
        protected:
            bool nextFoodWillBeGood;
        };
};

class npc_captain_cookie_cauldron : public CreatureScript
{
    public:
        npc_captain_cookie_cauldron() : CreatureScript("npc_captain_cookie_cauldron") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_captain_cookie_cauldronAI (pCreature);
        }

        struct npc_captain_cookie_cauldronAI : public ScriptedAI
        {
            npc_captain_cookie_cauldronAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            bool bReady;
            uint32 uiSeatTimer;

            void Reset() 
            {
                DoCast(me, SPELL_CAULDRON_VISUAL, true);
                DoCast(me, SPELL_CAULDRON_FIRE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                bReady = false;
                uiSeatTimer = 1000;
            }

            void EnterCombat(Unit* /*who*/) { }

            void UpdateAI(const uint32 diff) 
            {
                if (!bReady)
                {
                    if (uiSeatTimer <= diff)
                    {
                        bReady = true;
                        if (me->GetOwner())
                            me->GetOwner()->EnterVehicle(me);
                    }
                    else
                        uiSeatTimer -= diff;
                }
            }
        };
};

class npc_captain_cookie_good_food : public CreatureScript
{
    public:
        npc_captain_cookie_good_food() : CreatureScript("npc_captain_cookie_good_food") { }
     
        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_captain_cookie_good_foodAI (pCreature);
        }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            InstanceScript* pInstance = pCreature->GetInstanceScript();
            if (!pInstance)
                return false;
            if (pInstance->GetBossState(DATA_CAPTAIN) != IN_PROGRESS)
                return false;
            switch (pInstance->instance->GetDifficulty())
            {
            case DUNGEON_DIFFICULTY_NORMAL:
                pPlayer->CastSpell(pPlayer, SPELL_SETIATED, true);
                break;
            case DUNGEON_DIFFICULTY_HEROIC:
                pPlayer->CastSpell(pPlayer, SPELL_SETIATED_H, true);
                break;
            }
            pCreature->DespawnOrUnsummon();
            return false;
        }

        struct npc_captain_cookie_good_foodAI : public ScriptedAI
        {
            npc_captain_cookie_good_foodAI(Creature *c) : ScriptedAI(c) 
            {
                pInstance = c->GetInstanceScript();
            }
     
            InstanceScript* pInstance;
            EventMap events;

            void Reset()
            {
                if (!pInstance)
                    return;
            }

            void JustDied(Unit* killer)
            {
                if (!pInstance)
                    return;

                me->DespawnOrUnsummon();
            }
            void UpdateAI(const uint32 diff)
            {
                if (!pInstance)
                    return;

                if (pInstance->GetBossState(DATA_CAPTAIN) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }
     
        };
};

class npc_captain_cookie_bad_food : public CreatureScript
{
    public:
        npc_captain_cookie_bad_food() : CreatureScript("npc_captain_cookie_bad_food") { }
     
        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_captain_cookie_bad_foodAI (pCreature);
        }
     
        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            InstanceScript* pInstance = pCreature->GetInstanceScript();
            if (!pInstance)
                return false;
            if (pInstance->GetBossState(DATA_CAPTAIN) != IN_PROGRESS)
                return false;
            switch (pInstance->instance->GetDifficulty())
            {
                case DUNGEON_DIFFICULTY_NORMAL:
                    pPlayer->CastSpell(pPlayer, SPELL_NAUSEATED, true);
                    break;
                case DUNGEON_DIFFICULTY_HEROIC:
                    pPlayer->CastSpell(pPlayer, SPELL_NAUSEATED_H, true);
                    break;
            }
            pCreature->DespawnOrUnsummon();
            return false;
        }

        struct npc_captain_cookie_bad_foodAI : public ScriptedAI
        {
            npc_captain_cookie_bad_foodAI(Creature *c) : ScriptedAI(c) 
            {
                pInstance = c->GetInstanceScript();
            }
     
            InstanceScript* pInstance;
            EventMap events;

            void Reset()
            {
                if (!pInstance)
                    return;

                DoCast(SPELL_ROTTEN_AURA);
            }

            void JustDied(Unit* killer)
            {
                if (!pInstance)
                    return;

                me->DespawnOrUnsummon();
            }
            void UpdateAI(const uint32 diff)
            {
                if (!pInstance)
                    return;

                if (pInstance->GetBossState(DATA_CAPTAIN) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }
     
        };
};

class spell_captain_cookie_setiated : public SpellScriptLoader
{
    public:
        spell_captain_cookie_setiated() : SpellScriptLoader("spell_captain_cookie_setiated") { }


        class spell_captain_cookie_setiated_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_captain_cookie_setiated_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;
                GetHitUnit()->RemoveAuraFromStack(SPELL_NAUSEATED);
                GetHitUnit()->RemoveAuraFromStack(SPELL_NAUSEATED_H);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_captain_cookie_setiated_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_captain_cookie_setiated_SpellScript();
        }
};

class spell_captain_cookie_nauseated : public SpellScriptLoader
{
    public:
        spell_captain_cookie_nauseated() : SpellScriptLoader("spell_captain_cookie_nauseated") { }


        class spell_captain_cookie_nauseated_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_captain_cookie_nauseated_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;
                GetHitUnit()->RemoveAuraFromStack(SPELL_SETIATED);
                GetHitUnit()->RemoveAuraFromStack(SPELL_SETIATED_H);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_captain_cookie_nauseated_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_captain_cookie_nauseated_SpellScript();
        }
};

void AddSC_boss_captain_cookie()
{
    new boss_captain_cookie();
    new npc_captain_cookie_cauldron();
    new npc_captain_cookie_good_food();
    new npc_captain_cookie_bad_food();
    new spell_captain_cookie_setiated();
    new spell_captain_cookie_nauseated();
}
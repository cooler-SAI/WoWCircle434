#include"ScriptPCH.h"
#include"Spell.h"
#include"blackwing_descent.h"

//todo: реализовать reroure power, low health, корректный double attack
enum ScriptTexts
{
    SAY_AGGRO        = 0,
    SAY_MASSACRE    = 1,
    SAY_OFFLINE        = 2,
    SAY_ONLINE        = 3,
};

enum Spells
{
    //chimaeron
    SPELL_BERSERK            = 26662,
    SPELL_FAST_ASLEEP        = 82706,
    SPELL_DOUBLE_ATTACK        = 88826,
    SPELL_DOUBLE_ATTACK_2    = 82882,
    SPELL_FEUD                = 88872,
    SPELL_BREAK                = 82881,
    SPELL_MASSACRE            = 82848,
    SPELL_CAUSTIC_SLIME_M    = 82913,
    SPELL_CAUSTIC_SLIME_T    = 82871,
    SPELL_CAUSTIC_SLIME        = 82935,
    SPELL_MORTALITY            = 82890,
    SPELL_MORTALITY_SELF    = 82934,

    //bile o tron 800
    SPELL_FINKLES_MIXTURE            = 82705,
    SPELL_REROUTE_POWER                = 88861,
    SPELL_SYSTEMS_FAILURE            = 88853,
    SPELL_FINKLES_MIXTURE_VISUAL    = 91106,

    SPELL_LOW_HEALTH        = 89084,

    SPELL_SHADOW_WHIP        = 91304,
    SPELL_MOCKING_SHADOWS    = 91307,
};

enum Events
{
    EVENT_CAUSTIC_SLIME        = 1,
    EVENT_BREAK                = 2,
    EVENT_MASSACRE            = 3,
    EVENT_FEUD                = 4,
    EVENT_DOUBLE_ATTACK        = 5,
    EVENT_ACTIVATE_BUFF        = 6,
    EVENT_CAUSTIC_SLIME_1    = 7,
    EVENT_CAUSTIC_SLIME_2    = 8,
    EVENT_FEUD_OFF            = 9,
    EVENT_BERSERK            = 10,
    EVENT_NEXT_WAYPOINT        = 11,
};

enum Actions
{
    ACTION_BILE_O_TRON_START    = 1,
    ACTION_BILE_O_TRON_ONLINE    = 2,
    ACTION_BILE_O_TRON_OFFLINE    = 3,
    ACTION_BILE_O_TRON_RESET    = 4,
};

Position const bilePos[6] =
{
    {-135.79f, 15.56f, 73.16f, 4.64f},
    {-129.17f, -10.48f, 73.07f, 5.63f},
    {-106.18f, -18.53f, 72.79f, 1.55f},
    {-77.95f, 0.702f, 73.09f, 1.50f},
    {-77.46f, 31.038f, 73.17f, 4.48f},
    {-120.42f, 34.49f, 72.05f, 4.11f},
};

const Position chimaeronnefariusspawnPos = {-105.85f, -18.08f, 90.0f, 1.70f}; 

class boss_chimaeron : public CreatureScript
{
public:
    boss_chimaeron() : CreatureScript("boss_chimaeron") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_chimaeronAI(pCreature);
    }

    struct boss_chimaeronAI : public BossAI
    {
        boss_chimaeronAI(Creature* pCreature) : BossAI(pCreature, DATA_CHIMAERON), summons(me)
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
            DoCast(me, SPELL_FAST_ASLEEP);
        }

        EventMap events;
        SummonList summons;
        uint8 massacrecount; //каждые 3 массакра будут распри
        uint32 uiFeudSlimeTimer;
        uint8 stage;
        uint8 uiVictims;
        bool bFeud;

        void InitializeAI()
        {
            if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(BDScriptName))
                me->IsAIEnabled = false;
            else if (!me->isDead())
                Reset();
        }

        void Reset()
        {
            _Reset();

            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MORTALITY);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOCKING_SHADOWS);
            instance->SetData(DATA_BILE_O_TRON_800, 0);
            if (Creature* bileotron800 = Unit::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                bileotron800->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
            massacrecount = 0;
            stage = 0;
            uiFeudSlimeTimer = 5000;
            uiVictims = 0;
            bFeud = false;

            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);

            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false); 
            me->SetReactState(REACT_DEFENSIVE);
            events.Reset();
            summons.DespawnAll();
        }

        void JustReachedHome()
        {
            _JustReachedHome();
            if (Creature* pBileotron = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                pBileotron->AI()->DoAction(ACTION_BILE_O_TRON_RESET);
            DoCast(SPELL_FAST_ASLEEP);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            if (me->isInCombat())
                summon->SetInCombatWithZone();
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                uiVictims++;
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            summons.Despawn(summon);
        }

        void EnterCombat(Unit* who)
        {
            if (IsHeroic())
                events.ScheduleEvent(EVENT_BERSERK, 7.5 * MINUTE * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BREAK, 15000);
            events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 10000);
            events.ScheduleEvent(EVENT_MASSACRE, 31000);
            instance->SetData(DATA_BILE_O_TRON_800, 1);
            if (Creature* bileotron800 = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                bileotron800->AI()->DoCast(SPELL_FINKLES_MIXTURE);
            Talk(SAY_AGGRO);
            instance->SetBossState(DATA_CHIMAERON, IN_PROGRESS);
            if (IsHeroic())
                me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, chimaeronnefariusspawnPos);
            if (IsHeroic())
                if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                    pNefarius->AI()->DoAction(ACTION_CHIMAERON_INTRO);
            DoZoneInCombat();
        }
        
        void DamageDealt(Unit* victim, uint32 &damage, DamageEffectType damageType)
        {
            if (victim->HasAura(SPELL_FINKLES_MIXTURE))
                if (victim->GetHealth() >= 10000 && damage >= victim->GetHealth())
                    damage = victim->GetHealth() - 1;
        }

        void JustDied(Unit* /*who*/)
        {
            _JustDied();
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MORTALITY);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOCKING_SHADOWS);
            instance->SetData(DATA_BILE_O_TRON_800, 0);
            summons.DespawnAll();
            if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
            {
                pBileotron800->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
                pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_RESET);
            }
            if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 
                me->GetPositionX(),
                me->GetPositionY(),
                me->GetPositionZ(),
                0.0f))
                pNefarius->AI()->DoAction(ACTION_CHIMAERON_DEATH);
        }

        void SpellHit(Unit* who, const SpellEntry* spell)
        {
            if (spell->Id == SPELL_SHADOW_WHIP)
            {
                me->CastStop();
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                me->RemoveAurasDueToSpell(SPELL_FEUD);    
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetDistance(me->GetHomePosition()) >= 60.0f)
            {
                EnterEvadeMode();
                return;
            }

            if (me->HealthBelowPct(20) && stage == 0 && !bFeud)
            {
                stage = 1;
                me->InterruptNonMeleeSpells(false);
                events.CancelEvent(EVENT_CAUSTIC_SLIME);
                events.CancelEvent(EVENT_MASSACRE);
                events.CancelEvent(EVENT_FEUD);
                if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                        pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_OFFLINE);
                if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                        pNefarius->AI()->DoAction(ACTION_CHIMAERON_LOW);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                DoCast(me, SPELL_MORTALITY);
                DoCast(me, SPELL_MORTALITY_SELF);
                return;
            }

            events.Update(diff);

            if (bFeud)
            {
                if (uiFeudSlimeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                        me->CastSpell(target, SPELL_CAUSTIC_SLIME_M, true);
                    uiFeudSlimeTimer = 4900;
                }
                else
                    uiFeudSlimeTimer -= diff;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BREAK:
                    DoCast(me->getVictim(), SPELL_BREAK);
                    events.ScheduleEvent(EVENT_DOUBLE_ATTACK, 1000);
                    events.ScheduleEvent(EVENT_BREAK, 15000);
                    break;
                case EVENT_DOUBLE_ATTACK:
                    DoCast(me, SPELL_DOUBLE_ATTACK);
                    break;
                case EVENT_MASSACRE:
                    events.CancelEvent(EVENT_DOUBLE_ATTACK);
                    events.CancelEvent(EVENT_CAUSTIC_SLIME);
                    Talk(SAY_MASSACRE);
                    DoCast(SPELL_MASSACRE);
                    massacrecount++;
                    if (massacrecount == 3)
                    {
                        massacrecount = 0;
                        events.ScheduleEvent(EVENT_FEUD, 5000);
                    }
                    else
                    {
                        events.ScheduleEvent(EVENT_BREAK, 10000);
                        events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 17000);
                        events.ScheduleEvent(EVENT_MASSACRE, 30000);
                    }
                    break;
                case EVENT_FEUD:
                    bFeud = true;
                    instance->SetData(DATA_BILE_O_TRON_800, 0);
                    Talk(SAY_OFFLINE);
                    if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                        pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_OFFLINE);
                    uiFeudSlimeTimer = 15000;
                    DoCast(me, SPELL_FEUD);
                    if (IsHeroic())
                        if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                            pNefarius->AI()->DoAction(ACTION_CHIMAERON_FEUD);
                    events.ScheduleEvent(EVENT_FEUD_OFF, 30000);
                    events.ScheduleEvent(EVENT_ACTIVATE_BUFF, 30000);
                    events.ScheduleEvent(EVENT_MASSACRE, 30500);
                    break;
                case EVENT_FEUD_OFF:
                    bFeud = false;
                    break;
                case EVENT_CAUSTIC_SLIME:
                {
                    std::list<Unit*> pTargets;
                    pTargets.clear();
                    uint32 num = RAID_MODE(1, 2, 1, 2);
                    for (uint32 i = 0; i < num; i++)
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 0, true))
                            pTargets.push_back(pTarget);
                    if (!pTargets.empty())
                        for (std::list<Unit*>::const_iterator itr = pTargets.begin() ; itr != pTargets.end() ; ++itr)
                            DoCast((*itr), SPELL_CAUSTIC_SLIME_M, true);
                    events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 5000);
                    break;
                }
                case EVENT_ACTIVATE_BUFF:
                    Talk(SAY_ONLINE);
                    if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BILE_O_TRON_800)))
                        pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_ONLINE);
                    break;
                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK);
                    break;
                }
            }
            if (me->HasAura(SPELL_DOUBLE_ATTACK) && !me->m_extraAttacks)
                me->RemoveAurasDueToSpell(SPELL_DOUBLE_ATTACK);
            if (!me->HasAura(SPELL_FEUD))
                DoMeleeAttackIfReady();
        }
    };
};

#define GOSSIP_MENU_1 666660
#define GOSSIP_OPTION_1 "I suppose you'll be needing a key for this cage? Wait, don't tell me. The horrific gibbering monster behind me ate it, right?"
#define GOSSIP_MENU_2 666661
#define GOSSIP_OPTION_2 "You were trapped, as I recall. This situation seems oddly similar."
#define GOSSIP_MENU_3 666662
#define GOSSIP_OPTION_3 "Gnomes in Lava Suits, for example."
#define GOSSIP_MENU_4 666663
#define GOSSIP_OPTION_4 "No, I, uh, haven't seen it. You were saying?"
#define GOSSIP_MENU_5 666664
#define GOSSIP_OPTION_5 "Restrictions? What restrictions?"
#define GOSSIP_MENU_6 666665

class npc_finkle_einhorn : public CreatureScript
{
public:
    npc_finkle_einhorn() : CreatureScript("npc_finkle_einhorn") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript* pInstance = pCreature->GetInstanceScript();
        if (!pInstance)
            return false;
        if (pInstance->GetData(DATA_CHIMAERON) == IN_PROGRESS || pInstance->GetData(DATA_BILE_O_TRON_800) == 1)
            return false;
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_OPTION_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_1, pCreature->GetGUID());
        return true;
    }

     bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        InstanceScript* pInstance = pCreature->GetInstanceScript();
        if (!pInstance)
            return false;
        pPlayer->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_OPTION_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_OPTION_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_3, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_OPTION_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_4, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_OPTION_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_5, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_6, pCreature->GetGUID());
            pInstance->SetData(DATA_BILE_O_TRON_800, 1);
            if (Creature* pBileotron = ObjectAccessor::GetCreature(*pCreature, pInstance->GetData64(DATA_BILE_O_TRON_800)))
                pBileotron->AI()->DoAction(ACTION_BILE_O_TRON_START);
            break;
        }
        return true;
    }
};

#define SAY_BILE_O_TRON_START "The Bile-O-Tron springs to life and begins to emit a foul smelling substance."

class npc_bile_o_tron_800 : public CreatureScript
{
public:
    npc_bile_o_tron_800() : CreatureScript("npc_bile_o_tron_800") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bile_o_tron_800AI (creature);
    }

    struct npc_bile_o_tron_800AI : public ScriptedAI
    {
        npc_bile_o_tron_800AI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            pCreature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        EventMap events;
        uint8 uiWaypoint;

        void Reset()
        {
            events.Reset();
            uint8 uiWaypoint = 0;
        }


        void DoAction(const int32 action)
        {
            if (!pInstance)
                return;

            switch(action)
            {
            case ACTION_BILE_O_TRON_START:
                me->MonsterTextEmote(SAY_BILE_O_TRON_START, 0, false);
                DoCast(me, SPELL_FINKLES_MIXTURE_VISUAL, true);
                me->GetMotionMaster()->MovePoint(1, bilePos[0]);
                break;
            case ACTION_BILE_O_TRON_ONLINE:
                pInstance->SetData(DATA_BILE_O_TRON_800, 1);
                DoCast(me, SPELL_FINKLES_MIXTURE, true);
                break;
            case ACTION_BILE_O_TRON_OFFLINE:
                pInstance->SetData(DATA_BILE_O_TRON_800, 0);
                me->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
                break;
            case ACTION_BILE_O_TRON_RESET:
                events.Reset();
                me->RemoveAllAuras();
                me->GetMotionMaster()->MoveTargetedHome();
                break;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if(uiWaypoint >= 5)
                uiWaypoint = 0;
            else
                uiWaypoint++;
            
            events.ScheduleEvent(EVENT_NEXT_WAYPOINT, 1000);            
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_NEXT_WAYPOINT:
                    DoCast(me, SPELL_FINKLES_MIXTURE_VISUAL, true);
                    me->GetMotionMaster()->MovePoint(1, bilePos[uiWaypoint]);
                    break;
                }
            }
        }
    };
};

class spell_chimaeron_double_attack : public SpellScriptLoader
{
    public:
        spell_chimaeron_double_attack() : SpellScriptLoader("spell_chimaeron_double_attack") { }

        class spell_chimaeron_double_attack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_chimaeron_double_attack_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (aurEff->GetBase()->GetOwner()->GetTypeId() == TYPEID_UNIT)
                    aurEff->GetBase()->GetOwner()->ToUnit()->m_extraAttacks = 1;
            }
            
            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_chimaeron_double_attack_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_chimaeron_double_attack_AuraScript();
        }
};

void AddSC_boss_chimaeron()
{
    new boss_chimaeron();
    new npc_finkle_einhorn();
    new npc_bile_o_tron_800();
    new spell_chimaeron_double_attack();
}
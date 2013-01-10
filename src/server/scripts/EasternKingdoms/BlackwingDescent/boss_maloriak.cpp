#include"ScriptPCH.h"
#include"Spell.h"
#include "blackwing_descent.h"

//todo: реализовать визуальные эффекты смены фаз, когда появится го котла.

enum ScriptTexts
{
    SAY_AGGRO        = 0,
    SAY_DEATH        = 1,
    SAY_KILL        = 2,
    SAY_RED            = 3,
    SAY_GREEN        = 4,
    SAY_BLUE        = 5,
    SAY_ABERRATIONS    = 7,
    SAY_PRIME        = 8,
};
enum Spells
{
    //shared
    SPELL_THROW_RED_BOTTLE_A    = 77925,
    SPELL_THROW_RED_BOTTLE_B    = 77928,
    SPELL_DRINK_RED_BOTTLE        = 88699,
    SPELL_FIRE_IMBUED            = 78896,
    SPELL_THROW_BLUE_BOTTLE_A    = 77932,
    SPELL_THROW_BLUE_BOTTLE_B    = 77934,
    SPELL_DRINK_BLUE_BOTTLE        = 88700,
    SPELL_FROST_IMBUED            = 78895,
    SPELL_THROW_GREEN_BOTTLE_A    = 77937,
    SPELL_THROW_GREEN_BOTTLE_B    = 77938,
    SPELL_THROW_BLACK_BOTTLE_A    = 92831,
    SPELL_THROW_BLACK_BOTTLE_B    = 92837,
    SPELL_DRINK_BLACK_BOTTLE    = 92828,
    SPELL_DEBILITATING_SLIME    = 77615,
    SPELL_BERSERK                = 64238,

    //1&2 phases
    SPELL_RELEASE_ABERRATIONS    = 77569,
    SPELL_ARCANE_STORM            = 77896,
    SPELL_ARCANE_STORM_DMG        = 77908,
    SPELL_ARCANE_STORM_DMG_25    = 92961,
    SPELL_ARCANE_STORM_DMG_10H    = 92962,
    SPELL_ARCANE_STORM_DMG_25H    = 92963,
    SPELL_REMEDY                = 77912,

    //1 - fire phase
    SPELL_CONSUMING_FLAMES        = 77786,
    SPELL_SCORCHING_BLAST        = 77679,

    //2 - frost phase
    SPELL_FLASH_FREEZE            = 77699,
    SPELL_FLASH_FREEZE_25        = 92978,
    SPELL_FLASH_FREEZE_10H        = 92979,
    SPELL_FLASH_FREEZE_25H        = 92980,
    SPELL_BITING_CHILL            = 77760,
    SPELL_BITING_CHILL_DMG        = 77763,
    SPELL_BITING_CHILL_DMG_25    = 92975,
    SPELL_BITING_CHILL_DMG_10H    = 92976,
    SPELL_BITING_CHILL_DMG_25H    = 92977,

    //dark phase
    SPELL_ENGULFING_DARKNESS            = 92754,
    SPELL_ENGULFING_DARKNESS_DMG        = 92787,
    SPELL_ENGULFING_DARKNESS_DMG_25        = 92981,
    SPELL_ENGULFING_DARKNESS_DMG_10H    = 92982,
    SPELL_ENGULFING_DARKNESS_DMG_25H    = 92983,
    SPELL_DARK_SLUDGE                    = 92929,
    SPELL_DARK_SLUDGE_DMG                = 92930,
    SPELL_DARK_SLUDGE_DMG_25            = 92986,
    SPELL_DARK_SLUDGE_DMG_10H            = 92987,
    SPELL_DARK_SLUDGE_DMG_25H            = 92988,

    //3 - execute phase
    SPELL_RELEASE_ALL_MINIONS    = 77991,
    SPELL_ABSOLUTE_ZERO            = 78223,
    SPELL_ABSOLUTE_ZERO_AURA    = 78201,
    SPELL_ABSOLUTE_ZERO_DMG        = 78208,
    SPELL_ABSOLUTE_ZERO_DMG_25    = 93041,
    SPELL_ABSOLUTE_ZERO_DMG_10H    = 93042,
    SPELL_ABSOLUTE_ZERO_DMG_25H    = 93043,
    SPELL_SHATTER                = 77715,
    SPELL_SHATTER_25            = 95655,
    SPELL_SHATTER_10H            = 95656,
    SPELL_SHATTER_25H            = 95657,
    SPELL_ACID_NOVA                = 78225,
    SPELL_ACID_NOVA_25            = 93011,
    SPELL_ACID_NOVA_10H            = 93012,
    SPELL_ACID_NOVA_25H            = 93013,
    SPELL_MAGMA_JETS            = 78194,
    SPELL_MAGMA_JETS_SUMMON        = 78094,
    SPELL_MAGMA_JETS_DMG        = 78095,
    SPELL_MAGMA_JETS_DMG_25        = 93014,
    SPELL_MAGMA_JETS_DMG_10H    = 93015,
    SPELL_MAGMA_JETS_DMG_25H    = 93016,

    //adds aura
    SPELL_GROWN_CATALYST        = 77987,

    //prime subject
    SPELL_REND                    = 78034,
    SPELL_FIXATE                = 78617,
};

enum Adds
{
    NPC_ABERRATION             = 41440,
    NPC_PRIME_SUBJECT         = 41841,
    NPC_FLASH_FREEZE         = 41576,
    NPC_ABSOLUTE_ZERO         = 41961,
    NPC_MAGMA_JET_1             = 50030,
    NPC_MAGMA_JET_2             = 41901,
    NPC_VILE_SWILL                = 49811,
    NPC_CAULDRON_TRIGGER    = 41505,
    // INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES (334243, 41505, 669, 15, 1, 11686, 0, -105.748, -485.384, 81.6462, 1.39626, 7200, 0, 0, 4120, 0, 0, 0, 0, 0);

};

enum Events
{
    EVENT_FIRE_PHASE            = 1,
    EVENT_FROST_PHASE            = 2,
    EVENT_GREEN_PHASE            = 3,
    EVENT_JUMP_TO                = 4,
    EVENT_ARCANE_STORM            = 5,
    EVENT_REMEDY                = 6,
    EVENT_CONSUMING_FLAMES        = 7,
    EVENT_SCORCHING_BLAST        = 8,
    EVENT_FLASH_FREEZE            = 9,
    EVENT_BITING_CHILL            = 10,
    EVENT_ABSOLUTE_ZERO            = 11,
    EVENT_ACID_NOVA                = 12,
    EVENT_MAGMA_JETS            = 13,
    EVENT_MAGMA_JETS_T            = 14,
    EVENT_RELEASE_ABERRATIONS    = 15,
    EVENT_RELEASE_ALL_MINIONS    = 16,
    EVENT_REND                    = 17,
    EVENT_FIXATE                = 18,
    EVENT_BERSERK                = 19,
    EVENT_DARK_PHASE            = 20,
    EVENT_DARK_MAGIC            = 21,
    EVENT_ENGULFING_DARKNESS    = 22,
    EVENT_DARK_SLUDGE            = 23,

};

enum Points
{
    POINT_FIRE            = 1,
    POINT_FROST            = 2,
    POINT_GREEN            = 3,
    POINT_DARK            = 4,
};

enum Others
{
    DATA_TRAPPED_PLAYER = 1,
};

const Position maloriakHomePos =  //Позиция у котла
{-105.826f, -470.104f, 73.45f, 1.67f};

const Position maloriakGreenPos = //Прыжок в центр комнаты на зеленой фазе 
{-106.61f, -437.49f, 73.40f, 4.70f};

const Position aberrationPos[3] =  //Позиция саммона аберраций
{
    {-133.99f, -438.88f, 73.38f, 5.51f},
    {-132.81f, -434.09f, 73.31f, 5.51f},
    {-137.80f, -434.15f, 73.33f, 5.51f}
};

const Position primesubjectPos[2] =  //Позиция саммона основных объектов эксперимента
{
    {-138.20f, -487.43f, 73.22f, 0.96f},
    {-70.49f, -486.12f, 73.22f, 2.21f}
};

const Position maloriaknefariusspawnPos = {-104.74f, -419.98f, 90.29f, 4.75f};
const Position vileswillPos = {-104.74f, -419.98f, 73.23f, 4.75f};

float maloriakOrientation;

class boss_maloriak : public CreatureScript
{
public:
    boss_maloriak() : CreatureScript("boss_maloriak") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_maloriakAI(pCreature);
    }

    struct boss_maloriakAI : public BossAI
    {
        boss_maloriakAI(Creature* pCreature) : BossAI(pCreature, DATA_MALORIAK), summons(me)
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
        }

        EventMap events;
        SummonList summons;
        bool bExecute;
        bool bDark;

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

            summons.DespawnAll();
            events.Reset();
            maloriakOrientation = 0;
            bExecute = false;
            bDark = false;

            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);

            instance->SetData(DATA_MALORIAK_ABERRATIONS, 18);
        }
        
        //Старт и отключение евентов для скиллов огненной фазы
        void EventFireSkills(bool on)
        {
            if (on)
            {
                events.ScheduleEvent(EVENT_RELEASE_ABERRATIONS, 8000);
                events.ScheduleEvent(EVENT_CONSUMING_FLAMES, 5000);
            }
            else
            {
                events.CancelEvent(EVENT_RELEASE_ABERRATIONS);
                events.CancelEvent(EVENT_CONSUMING_FLAMES);
                events.CancelEvent(EVENT_SCORCHING_BLAST);
            }
        }
        
        //Старт и отключение эвентов для скиллов ледяной фазы
        void EventFrostSkills(bool on)
        {
            if (on)
            {
                events.ScheduleEvent(EVENT_RELEASE_ABERRATIONS, 8000); 
                events.ScheduleEvent(EVENT_BITING_CHILL, 5000);
                events.ScheduleEvent(EVENT_FLASH_FREEZE, 15000);
            }
            else
            {
                events.CancelEvent(EVENT_RELEASE_ABERRATIONS);
                events.CancelEvent(EVENT_BITING_CHILL);
                events.CancelEvent(EVENT_FLASH_FREEZE);
            }
        }
        
        //Старт для скиллов последней фазы
        void EventExecuteSkills()
        {
            events.ScheduleEvent(EVENT_ABSOLUTE_ZERO, urand(4000, 5000));
            events.ScheduleEvent(EVENT_MAGMA_JETS, 5000);
            events.ScheduleEvent(EVENT_ACID_NOVA, urand(7000, 8000));
        }

        void EnterCombat(Unit* attacker)
        {
            if (IsHeroic())
                events.ScheduleEvent(EVENT_DARK_PHASE, 15000);
            else
                events.ScheduleEvent(EVENT_FIRE_PHASE, 15000);
            events.ScheduleEvent(EVENT_ARCANE_STORM, 5000);
            events.ScheduleEvent(EVENT_REMEDY, urand(10000, 20000));
            events.ScheduleEvent(EVENT_BERSERK, IsHeroic()? 9*MINUTE*IN_MILLISECONDS : 7*MINUTE*IN_MILLISECONDS);
            Talk(SAY_AGGRO);
            DoZoneInCombat();
            if (IsHeroic())
                me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, maloriaknefariusspawnPos);
            if (IsHeroic())
                if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                    pNefarius->AI()->DoAction(ACTION_MALORIAK_INTRO);
            instance->SetBossState(DATA_MALORIAK, IN_PROGRESS);
            
        }

        void JustReachedHome()
        {
            _JustReachedHome();
        }

        void KilledUnit(Unit* victim)
        {
            Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            Talk(SAY_DEATH);
            summons.DespawnAll();
            if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 
                me->GetPositionX(),
                me->GetPositionY(),
                me->GetPositionZ(),
                0.0f))
                pNefarius->AI()->DoAction(ACTION_MALORIAK_DEATH);
        }
        
        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            if (me->isInCombat())
                summon->SetInCombatWithZone();
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            summons.Despawn(summon);
        }

        //Обработчик для сбивания определенных кастов
        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_RELEASE_ABERRATIONS)
                    for (uint8 i = 0; i < 3; ++i)
                        if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);

            if (me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                if (me->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_spellInfo->Id == SPELL_ARCANE_STORM)
                    for (uint8 i = 0; i < 3; ++i)
                        if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                    case POINT_FIRE:
                        bDark = false;
                        Talk(SAY_RED);
                        me->AddAura(SPELL_FIRE_IMBUED, me);
                        events.RescheduleEvent(EVENT_ARCANE_STORM, 5000);
                        events.RescheduleEvent(EVENT_REMEDY, urand(10000, 20000));
                        EventFireSkills(true);
                        events.ScheduleEvent(EVENT_FROST_PHASE, 41000);
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                        break;
                    case POINT_FROST:
                        Talk(SAY_BLUE);
                        me->AddAura(SPELL_FROST_IMBUED, me);
                        EventFrostSkills(true);
                        events.ScheduleEvent(EVENT_GREEN_PHASE, 41000);
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                        break;
                    case POINT_GREEN:
                        Talk(SAY_GREEN);
                        events.ScheduleEvent(EVENT_JUMP_TO, 1000);
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        break;
                    case POINT_DARK:
                        bDark = true;
                        for (uint8 i = 0; i < 5; i++)
                            me->SummonCreature(NPC_VILE_SWILL, vileswillPos);
                        events.ScheduleEvent(EVENT_ENGULFING_DARKNESS, 5000);
                        events.ScheduleEvent(EVENT_FIRE_PHASE, 100000);
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                        break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetDistance(me->GetHomePosition()) > 60.0f)
            {
                EnterEvadeMode();
                return;
            }

            if (me->HealthBelowPct(25) && !bExecute)
            {
                bExecute = true;
                events.CancelEvent(EVENT_FIRE_PHASE);
                events.CancelEvent(EVENT_FROST_PHASE);
                events.CancelEvent(EVENT_GREEN_PHASE);
                events.CancelEvent(EVENT_DARK_PHASE);
                events.CancelEvent(EVENT_SCORCHING_BLAST);
                events.CancelEvent(EVENT_CONSUMING_FLAMES);
                events.CancelEvent(EVENT_FLASH_FREEZE);
                events.CancelEvent(EVENT_BITING_CHILL);
                events.CancelEvent(EVENT_ARCANE_STORM);
                events.CancelEvent(EVENT_ENGULFING_DARKNESS);
                me->CastStop();
                DoCast(me, SPELL_RELEASE_ALL_MINIONS);
                EventExecuteSkills();
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);    
                return;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FIRE_PHASE:
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    events.CancelEvent(EVENT_ENGULFING_DARKNESS);
                    events.DelayEvents(5000);
                    DoCast(me, SPELL_THROW_RED_BOTTLE_A);
                    me->GetMotionMaster()->MovePoint(POINT_FIRE, maloriakHomePos);
                    break;
                case EVENT_FROST_PHASE:
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    EventFireSkills(false);
                    events.DelayEvents(5000);
                    DoCast(me, SPELL_THROW_BLUE_BOTTLE_A);
                    me->GetMotionMaster()->MovePoint(POINT_FROST, maloriakHomePos);
                    break;
                case EVENT_GREEN_PHASE:
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    EventFrostSkills(false);
                    events.DelayEvents(5000);
                    DoCast(SPELL_THROW_GREEN_BOTTLE_A);
                    me->GetMotionMaster()->MovePoint(POINT_GREEN, maloriakHomePos);
                    break;
                case EVENT_JUMP_TO:
                    for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)    
                        if (Creature* summon = Unit::GetCreature(*me, *itr))
                            summon->RemoveAurasDueToSpell(SPELL_GROWN_CATALYST);
                    DoCast(me, SPELL_DEBILITATING_SLIME);
                    me->GetMotionMaster()->MoveJump(
                        maloriakGreenPos.GetPositionX(),
                        maloriakGreenPos.GetPositionY(),
                        maloriakGreenPos.GetPositionZ(),
                        15.0f, 15.0f);
                    if (IsHeroic())
                        events.ScheduleEvent(EVENT_DARK_PHASE, 41000);
                    else
                        events.ScheduleEvent(EVENT_FIRE_PHASE, 41000);
                    me->GetMotionMaster()->MoveChase(me->getVictim());
                    break;
                case EVENT_DARK_PHASE:
                    events.CancelEvent(EVENT_REMEDY);
                    events.CancelEvent(EVENT_ARCANE_STORM);
                    if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                        pNefarius->AI()->DoAction(ACTION_MALORIAK_DARK_MAGIC);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    EventFrostSkills(false);
                    events.DelayEvents(5000);
                    me->GetMotionMaster()->MovePoint(POINT_DARK, maloriakHomePos);
                    break;
                case EVENT_ARCANE_STORM:
                    DoCast(me, SPELL_ARCANE_STORM);
                    events.ScheduleEvent(EVENT_ARCANE_STORM, 27000);
                    break;
                case EVENT_REMEDY:
                    DoCast(me, SPELL_REMEDY);
                    events.ScheduleEvent(EVENT_REMEDY, urand(20000, 30000));
                    break;
                case EVENT_RELEASE_ABERRATIONS:
                    uint32 count;
                    count = instance->GetData(DATA_MALORIAK_ABERRATIONS);
                    if (count >= 3)
                        DoCast(me, SPELL_RELEASE_ABERRATIONS);
                    events.ScheduleEvent(EVENT_RELEASE_ABERRATIONS, 25000);
                    break;
                case EVENT_SCORCHING_BLAST:
                    DoCast(SPELL_SCORCHING_BLAST);
                    events.ScheduleEvent(EVENT_CONSUMING_FLAMES, 10000);
                    break;
                case EVENT_CONSUMING_FLAMES:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                        DoCast(target, SPELL_CONSUMING_FLAMES);
                    events.ScheduleEvent(EVENT_SCORCHING_BLAST, 5000);
                    break;
                case EVENT_BITING_CHILL:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                        DoCast(target, SPELL_BITING_CHILL);
                    events.ScheduleEvent(EVENT_BITING_CHILL, 13000);
                    break;
                case EVENT_FLASH_FREEZE:
                {
                    std::list<Unit*> targets;
                    uint32 minTargets = RAID_MODE<uint32>(3, 8, 3, 8);
                    SelectTargetList(targets, minTargets, SELECT_TARGET_RANDOM, -5.0f, true);
                    float minDist = 0.0f;
                    if (targets.size() >= minTargets)
                        minDist = -5.0f;
                    if (Unit* targetflashfreeze = SelectTarget(SELECT_TARGET_RANDOM, 1, minDist, true))
                        DoCast(targetflashfreeze, SPELL_FLASH_FREEZE);    
                    events.ScheduleEvent(EVENT_FLASH_FREEZE, 15000);
                    break;
                }
                case EVENT_ENGULFING_DARKNESS:
                    DoCast(me->getVictim(), SPELL_ENGULFING_DARKNESS);
                    events.ScheduleEvent(EVENT_ENGULFING_DARKNESS, 14000);
                    break;
                case EVENT_ABSOLUTE_ZERO:
                    Unit* targetabsolutezero;
                    targetabsolutezero = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
                    if (!targetabsolutezero)
                        targetabsolutezero = SelectTarget(SELECT_TARGET_RANDOM);
                    if (targetabsolutezero)
                        DoCast(targetabsolutezero, SPELL_ABSOLUTE_ZERO);
                    events.ScheduleEvent(EVENT_ABSOLUTE_ZERO, urand(20000, 30000));
                    break;
                case EVENT_MAGMA_JETS:
                    maloriakOrientation = me->GetOrientation();
                    DoCast(me, SPELL_MAGMA_JETS);
                    events.ScheduleEvent(EVENT_MAGMA_JETS, 15000);
                    break;
                case EVENT_ACID_NOVA:
                    DoCast(SPELL_ACID_NOVA);
                    events.ScheduleEvent(EVENT_ACID_NOVA, urand(15000, 20000));
                    break;
                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK);
                    break;
                }
            }
            if (!bDark)
                DoMeleeAttackIfReady();
        }
    };
};


class npc_maloriak_flash_freeze : public CreatureScript
{
    public:
        npc_maloriak_flash_freeze() : CreatureScript("npc_maloriak_flash_freeze") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_maloriak_flash_freezeAI(creature);
        }

        struct npc_maloriak_flash_freezeAI : public Scripted_NoMovementAI
        {
            npc_maloriak_flash_freezeAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
                trappedPlayer = 0;
            }

            uint64 trappedPlayer;
            uint32 existenceCheckTimer;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void SetGUID(uint64 guid, int32 type)
            {
                if (type == DATA_TRAPPED_PLAYER)
                {
                    trappedPlayer = guid;
                    existenceCheckTimer = 1000;
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, trappedPlayer))
                {
                    trappedPlayer = 0;
                    player->RemoveAurasDueToSpell(SPELL_FLASH_FREEZE);
                }
                DoCast(me, SPELL_SHATTER);
                me->DespawnOrUnsummon(800);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!trappedPlayer)
                    return;

                if (existenceCheckTimer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, trappedPlayer);
                    if (!player || player->isDead() || !player->HasAura(SPELL_FLASH_FREEZE))
                    {
                        JustDied(me);
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
                else
                    existenceCheckTimer -= diff;
            }            
        };
};

class npc_absolute_zero : public CreatureScript
{
    public:
        npc_absolute_zero() : CreatureScript("npc_absolute_zero") { }

        struct npc_absolute_zeroAI : public ScriptedAI
        {
            npc_absolute_zeroAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                creature->SetSpeed(MOVE_RUN, 0.5f);
                creature->SetSpeed(MOVE_WALK, 0.5f);
            }

            uint32 uiPauseTimer; //чтобы не срабатывало сразу при саммоне возле игрока
            uint32 uiDespawnTimer;
            bool bCanExplode; 

            void Reset()
            {
                uiPauseTimer = 3000;
                uiDespawnTimer = 15000;
                bCanExplode = false;
            }

            void IsSummonedBy(Unit* owner)
            {
                DoCast(SPELL_ABSOLUTE_ZERO_AURA);
            }

            void UpdateAI(const uint32 diff)
            {
                if ((uiPauseTimer <= diff) && !bCanExplode)
                {
                    bCanExplode = true;
                    if (Unit* target = me->SelectNearestTarget())
                        me->GetMotionMaster()->MoveFollow(target, 0.1f, 0.0f);
                }
                else
                    uiPauseTimer -= diff;

                if (uiDespawnTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    uiDespawnTimer -= diff;

                if (Unit* target = me->SelectNearestTarget())
                {
                    if ((me->GetDistance(target) <= 4.0f) && bCanExplode)
                    {
                        DoCast(SPELL_ABSOLUTE_ZERO_DMG);
                        me->DespawnOrUnsummon(800);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_absolute_zeroAI(creature);
        }
};

class npc_magma_jet : public CreatureScript
{
    public:
        npc_magma_jet() : CreatureScript("npc_magma_jet") { }

        struct npc_magma_jetAI : public Scripted_NoMovementAI
        {
            npc_magma_jetAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            EventMap events;
            Unit* creOwner;
            void IsSummonedBy(Unit* owner)
            {
                if (owner->GetTypeId() != TYPEID_UNIT)
                    return;
                creOwner = owner->ToCreature();
                Position pos;
                me->SetOrientation(maloriakOrientation);
                owner->GetNearPosition(pos, owner->GetObjectSize()/2.0f, 0.0f);
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                events.ScheduleEvent(EVENT_MAGMA_JETS_T, 200);
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_MAGMA_JETS_T)
                {
                    Position newPos;
                    me->GetNearPosition(newPos, 5.5f, 0.0f);
                    me->NearTeleportTo(newPos.GetPositionX(), newPos.GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    if (creOwner->GetDistance(me) >= 50.0f) // спавнить разломы на расстоянии до 50 от босса
                        me->DespawnOrUnsummon();
                    else
                    {
                        DoCast(SPELL_MAGMA_JETS_SUMMON);
                        events.ScheduleEvent(EVENT_MAGMA_JETS_T, 200);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_magma_jetAI(creature);
        }
};

class npc_magma_jet_summon : public CreatureScript
{
    public:
        npc_magma_jet_summon() : CreatureScript("npc_magma_jet_summon") { }

        struct npc_magma_jet_summonAI : public Scripted_NoMovementAI
        {
            npc_magma_jet_summonAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            uint32 summonTimer;

            void Reset()
            {
                DoCast(SPELL_MAGMA_JETS_DMG);
                summonTimer = 15000;
            }
            
            void UpdateAI(const uint32 diff)
            {
                if (summonTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    summonTimer -= diff;                
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_magma_jet_summonAI(creature);
        }
};

class npc_aberration : public CreatureScript
{
    public:
        npc_aberration() : CreatureScript("npc_aberration") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_aberrationAI(creature);
        }

        struct npc_aberrationAI : public ScriptedAI
        {
            npc_aberrationAI(Creature* creature) : ScriptedAI(creature)
            {
            }
            
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }


            void Reset()
            {
                DoCast(SPELL_GROWN_CATALYST);
            }
        };
};

class npc_prime_subject : public CreatureScript
{
    public:
        npc_prime_subject() : CreatureScript("npc_prime_subject") { }

        struct npc_prime_subjectAI : public ScriptedAI
        {
            npc_prime_subjectAI(Creature* creature) : ScriptedAI(creature)
            {
            }
            
            EventMap events;

            void Reset()
            {
                DoCast(SPELL_GROWN_CATALYST);
            }

            void EnterCombat(Unit* who)
            {
                events.ScheduleEvent(EVENT_FIXATE, 5000);
                events.ScheduleEvent(EVENT_REND, 12000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_FIXATE:
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                        DoCast(me->getVictim(), SPELL_FIXATE);
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                        break;
                    case EVENT_REND:
                        DoCast(me->getVictim(), SPELL_REND);
                        events.ScheduleEvent(EVENT_REND, urand(12000, 16000));
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_prime_subjectAI(creature);
        }
};

class npc_vile_swill : public CreatureScript
{
    public:
        npc_vile_swill() : CreatureScript("npc_vile_swill") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_vile_swillAI(creature);
        }

        struct npc_vile_swillAI : public ScriptedAI
        {
            npc_vile_swillAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            EventMap events;

            void Reset()
            {
            }

            void EnterCombat(Unit* who)
            {
                events.ScheduleEvent(EVENT_DARK_SLUDGE, urand(5000, 10000));
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_DARK_SLUDGE:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(pTarget, SPELL_DARK_SLUDGE);
                        events.ScheduleEvent(EVENT_DARK_SLUDGE, urand(15000, 18000));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

class spell_maloriak_flash_freeze : public SpellScriptLoader
{
    public:
        spell_maloriak_flash_freeze() : SpellScriptLoader("spell_maloriak_flash_freeze") { }

        class spell_maloriak_flash_freeze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maloriak_flash_freeze_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Position pos;
                aurEff->GetBase()->GetOwner()->GetPosition(&pos);
                if (!GetCaster())
                    return;
                if (TempSummon* summon = GetCaster()->SummonCreature(NPC_FLASH_FREEZE, pos))
                    summon->AI()->SetGUID(aurEff->GetBase()->GetOwner()->GetGUID(), DATA_TRAPPED_PLAYER);   
            }
            
            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_maloriak_flash_freeze_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_maloriak_flash_freeze_AuraScript();
        }
};


class spell_maloriak_release_aberrations : public SpellScriptLoader
{
    public:
        spell_maloriak_release_aberrations() : SpellScriptLoader("spell_maloriak_release_aberrations") { }


        class spell_maloriak_release_aberrations_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maloriak_release_aberrations_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 count;
                if (!GetCaster())
                    return;
                if (InstanceScript* pInstance = GetCaster()->GetInstanceScript())
                {
                    count = pInstance->GetData(DATA_MALORIAK_ABERRATIONS);
                    if (count >= 3)
                    {
                        pInstance->SetData(DATA_MALORIAK_ABERRATIONS, count - 3);                    
                        GetCaster()->SummonCreature(NPC_ABERRATION, aberrationPos[0], TEMPSUMMON_CORPSE_DESPAWN);
                        GetCaster()->SummonCreature(NPC_ABERRATION, aberrationPos[1], TEMPSUMMON_CORPSE_DESPAWN);
                        GetCaster()->SummonCreature(NPC_ABERRATION, aberrationPos[2], TEMPSUMMON_CORPSE_DESPAWN);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_maloriak_release_aberrations_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_maloriak_release_aberrations_SpellScript();
        }
};

class spell_maloriak_release_all_minions : public SpellScriptLoader
{
    public:
        spell_maloriak_release_all_minions() : SpellScriptLoader("spell_maloriak_release_all_minions") { }


        class spell_maloriak_release_all_minions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maloriak_release_all_minions_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 count;
                if (!GetCaster())
                    return;

                if (InstanceScript* pInstance = GetCaster()->GetInstanceScript())
                {
                    count = pInstance->GetData(DATA_MALORIAK_ABERRATIONS);
                    if (count >= 3)
                    {
                        for (uint32 i = 0; i != count; i++)
                            GetCaster()->SummonCreature(NPC_ABERRATION, aberrationPos[urand(0, 2)], TEMPSUMMON_CORPSE_DESPAWN);
                    pInstance->SetData(DATA_MALORIAK_ABERRATIONS, 0);
                    }
                    GetCaster()->SummonCreature(NPC_PRIME_SUBJECT, primesubjectPos[0], TEMPSUMMON_CORPSE_DESPAWN);
                    GetCaster()->SummonCreature(NPC_PRIME_SUBJECT, primesubjectPos[1], TEMPSUMMON_CORPSE_DESPAWN);
                }    
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_maloriak_release_all_minions_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_maloriak_release_all_minions_SpellScript();
        }
};

class spell_maloriak_remedy : public SpellScriptLoader
{
public:
    spell_maloriak_remedy() : SpellScriptLoader("spell_maloriak_remedy") { }

    class spell_maloriak_remedy_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_maloriak_remedy_AuraScript);

        void HandleTick(AuraEffect const* aurEff)
        {
            Unit* target = GetTarget();
            if (target && target->GetTypeId() == TYPEID_UNIT)
            {
                int32 baseAmount = aurEff->GetBaseAmount();
                if (baseAmount > 0)
                    const_cast<AuraEffect*>(aurEff)->SetAmount(baseAmount * aurEff->GetTickNumber());
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_maloriak_remedy_AuraScript::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript *GetAuraScript() const
    {
        return new spell_maloriak_remedy_AuraScript();
    }
};

void AddSC_boss_maloriak()
{
    new boss_maloriak();
    new npc_maloriak_flash_freeze();
    new npc_absolute_zero();
    new npc_magma_jet();
    new npc_magma_jet_summon();
    new npc_aberration();
    new npc_prime_subject();
    new npc_vile_swill();
    new spell_maloriak_flash_freeze();
    new spell_maloriak_release_aberrations();
    new spell_maloriak_release_all_minions();
    new spell_maloriak_remedy();
}
#include "ScriptPCH.h"
#include "Vehicle.h"
#include "hour_of_twilight.h"

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_DEATH   = 1,
    SAY_EVENT_1 = 2,
    SAY_EVENT_2 = 3,
    SAY_EVENT_3 = 4,
    SAY_KILL    = 5,
    SAY_SPELL   = 6,
};

enum Spells
{
    SPELL_HOLY_WALL                     = 102629,

    SPELL_SMITE                         = 104503,
    SPELL_RIGHTEOUS_SNEAR_AOE           = 103149,
    SPELL_RIGHTEOUS_SNEAR_AURA          = 103151,
    SPELL_RIGHTEOUS_SNEAR_DMG           = 103161,

    SPELL_PURIFYING_LIGHT               = 103565,
    SPELL_PURIFYING_LIGHT_TARGETING     = 103600,
    SPELL_PURIFYING_LIGHT_GROW          = 103579,
    SPELL_PURIFYING_LIGHT_SUMMON_1      = 103584,
    SPELL_PURIFYING_LIGHT_SUMMON_2      = 103585,
    SPELL_PURIFYING_LIGHT_SUMMON_3      = 103586,
    SPELL_PURIFYING_LIGHT_DUMMY         = 103578,
    SPELL_PURIFYING_BLAST               = 103648,
    SPELL_PURIFYING_BLAST_DMG           = 103651,
    SPELL_PURIFIED                      = 103654,
    SPELL_PURIFIED_DMG                  = 103653,

    SPELL_UNSTABLE_TWILIGHT_DUMMY       = 103766,

    SPELL_TWILIGHT_EPIPHANY             = 103754,
    SPELL_TWILIGHT_EPIPHANY_DMG         = 103755,
    SPELL_ENGULFING_TWILIGHT            = 103762,
    SPELL_TRANSFORM                     = 103765,

    SPELL_TWILIGHT_BLAST                = 104504,

    SPELL_TWILIGHT_SNEAR_AOE            = 103362,
    SPELL_TWILIGHT_SNEAR_AURA           = 103363,
    SPELL_TWILIGHT_SNEAR_DMG            = 103526,

    SPELL_CORRUPTING_TWILIGHT           = 103767,
    SPELL_CORRUPTING_TWILIGHT_TARGETING = 103768,
    SPELL_CORRUPTING_TWILIGHT_GROW      = 103773,
    SPELL_CORRUPTING_TWILIGHT_SUMMON_1  = 103770,
    SPELL_CORRUPTING_TWILIGHT_SUMMON_2  = 103771,
    SPELL_CORRUPTING_TWILIGHT_SUMMON_3  = 103772,
    SPELL_CORRUPTING_TWILIGHT_DUMMY     = 103769,
    SPELL_TWILIGHT_BOLT                 = 103776,
    SPELL_TWILIGHT_BOLT_DMG             = 103777,
    SPELL_TWILIGHT                      = 103774,
    SPELL_TWILIGHT_DMG                  = 103775,

    SPELL_WAVE_OF_TWILIGHT              = 103780,
    SPELL_WAVE_OF_TWILIGHT_DMG          = 103781,
};

enum Events
{
    EVENT_WAVE_OF_VIRTUE        = 1,
    EVENT_PURIFYING_LIGHT       = 2,
    EVENT_SMITE                 = 3,
    EVENT_RIGHTEOUS_SNEAR       = 4,
    EVENT_TWILIGHT_BLAST        = 5,
    EVENT_WAVE_OF_TWILIGHT      = 6,
    EVENT_CORRUPTING_TWILIGHT   = 7,
    EVENT_TWILIGHT_SNEAR        = 8,
    EVENT_CONTINUE              = 9,
    EVENT_JUMP_2                = 10,
};

enum Adds
{
    NPC_TWILIGHT_SPARK      = 55466,
    NPC_PURIFYING_LIGHT     = 55377,
    NPC_PURIFYING_BLAST     = 55427,
    NPC_WAVE_OF_VIRTUE      = 55551,
    NPC_CORRUPTING_TWILIGHT = 55467,
    NPC_TWILIGHT_BLAST      = 55468,
    NPC_WAVE_OF_TWILIGHT    = 55469,
};

enum Actions
{
    ACTION_LIGHT    = 1,
    ACTION_TWILIGHT = 2,
};

const Position sparkPos[55] = 
{
    {3606.72f, 278.965f, -120.066f, 0.f},
    {3582.16f, 347.017f, -103.266f, 0.f},
    {3559.45f, 309.341f, -112.844f, 4.78954f},
    {3563.15f, 304.613f, -67.6215f, 0.f},
    {3594.22f, 232.017f, -119.966f, 0.f},
    {3511.56f, 223.406f, -120.064f, 4.33715f},
    {3650.38f, 283.246f, -120.069f, 0.f},
    {3558.38f, 243.214f, -82.2213f, 0.f},
    {3557.03f, 266.382f, -83.5522f, 0.f},
    {3617.41f, 245.964f, -101.917f, 0.f},
    {3558.39f, 218.519f, -119.971f, 0.f},
    {3673.7f, 297.641f, -114.694f, 0.f},
    {3567.52f, 238.826f, -110.127f, 0.f},
    {3513.29f, 253.806f, -112.54f, 0.f},
    {3490.37f, 236.514f, -86.964f, 0.f},
    {3600.0f, 308.602f, -80.4853f, 0.977007f},
    {3567.42f, 264.861f, -83.1477f, 0.f},
    {3516.71f, 320.299f, -74.2921f, 0.f},
    {3526.16f, 307.196f, -112.54f, 0.f},
    {3558.09f, 209.139f, -103.909f, 0.f},
    {3483.07f, 264.76f, -59.5227f, 0.f},
    {3592.08f, 238.804f, -87.0628f, 0.f},
    {3541.43f, 205.904f, -86.0307f, 0.f},
    {3677.23f, 277.703f, -118.382f, 0.f},
    {3607.75f, 278.908f, -93.2823f, 0.f},
    {3537.41f, 235.601f, -112.54f, 0.f},
    {3705.94f, 296.691f, -98.6815f, 0.f},
    {3536.73f, 329.964f, -83.5424f, 0.f},
    {3496.33f, 345.441f, -78.1769f, 3.14159f},
    {3541.89f, 250.286f, -52.8092f, 0.f},
    {3525.11f, 338.845f, -105.34f, 0.f},
    {3457.46f, 265.877f, -84.0479f, 0.791643f},
    {3547.85f, 285.762f, -82.9028f, 0.f},
    {3475.73f, 284.866f, -99.8377f, 0.f},
    {3590.21f, 195.927f, -106.245f, 0.f},
    {3586.22f, 309.804f, -120.063f, 0.f},
    {3583.09f, 318.521f, -90.5986f, 0.f},
    {3504.15f, 269.363f, -115.227f, 0.f},
    {3548.88f, 329.049f, -120.062f, 0.f},
    {3505.04f, 217.79f, -94.908f, 0.f},
    {3522.99f, 282.682f, -44.5289f, 0.f},
    {3499.63f, 294.16f, -45.8369f, 0.f},
    {3555.95f, 322.845f, -83.6866f, 0.f},
    {3712.71f, 279.667f, -101.811f, 0.f},
    {3587.46f, 276.425f, -115.628f, 0.f},
    {3581.41f, 243.791f, -82.3891f, 5.50833f},
    {3503.89f, 315.76f, -120.056f, 0.f},
    {3599.55f, 208.035f, -89.1302f, 0.f},
    {3492.69f, 328.587f, -106.538f, 0.f},
    {3518.57f, 213.663f, -98.4859f, 0.f},
    {3488.35f, 266.621f, -120.061f, 0.f},
    {3499.28f, 235.29f, -55.3261f, 0.f},
    {3520.5f, 204.995f, -79.7515f, 0.f},
    {3521.93f, 217.024f, -59.3911f, 0.f}
};

class boss_archbishop_benedictus : public CreatureScript
{
    public:
        boss_archbishop_benedictus() : CreatureScript("boss_archbishop_benedictus") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_archbishop_benedictusAI(creature);
        }

        struct boss_archbishop_benedictusAI : public BossAI
        {
            boss_archbishop_benedictusAI(Creature* creature) : BossAI(creature, DATA_BENEDICTUS)
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
                me->setActive(true);
                me->setFaction(14);
                bPhase = false;
            }

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(HoTScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset()
            {
                _Reset();

                bPhase = false;

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_EPIPHANY_DMG);
                DespawnCreatures(NPC_PURIFYING_BLAST);
                DespawnCreatures(NPC_TWILIGHT_BLAST);

            }

            void EnterCombat(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);

                bPhase = false;

                events.ScheduleEvent(EVENT_PURIFYING_LIGHT, 10000);
                events.ScheduleEvent(EVENT_SMITE, urand(1000, 2000));
                events.ScheduleEvent(EVENT_RIGHTEOUS_SNEAR, urand(5000, 10000));

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_EPIPHANY_DMG);
                instance->SetBossState(DATA_BENEDICTUS, IN_PROGRESS);
                DoZoneInCombat();
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_LIGHT)
                {
                    EntryCheckPredicate pred(NPC_PURIFYING_LIGHT);
                    summons.DoAction(ACTION_LIGHT, pred, 3);
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();

                Talk(SAY_DEATH);

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_EPIPHANY_DMG);
                DespawnCreatures(NPC_PURIFYING_BLAST);
                DespawnCreatures(NPC_TWILIGHT_BLAST);
            }

            void JustSummoned(Creature* summon)
            {
                BossAI::JustSummoned(summon);
                if (summon->GetEntry() == NPC_TWILIGHT_SPARK)
                    summon->SetPhaseMask(2, true);
                //if (summon->GetEntry() == NPC_PURIFYING_LIGHT)
                //    summon->EnterVehicle(me, -1, true);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(60) && !bPhase)
                {
                    bPhase = true;

                    events.CancelEvent(EVENT_WAVE_OF_VIRTUE);
                    events.CancelEvent(EVENT_PURIFYING_LIGHT);
                    events.CancelEvent(EVENT_SMITE);
                    events.CancelEvent(EVENT_RIGHTEOUS_SNEAR);
                    events.ScheduleEvent(EVENT_CONTINUE, 6000);
                    DoCast(me, SPELL_TWILIGHT_EPIPHANY);
                    return;
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SMITE:
                            DoCastVictim(SPELL_SMITE);
                            events.ScheduleEvent(EVENT_SMITE, urand(5000, 7000));
                            break;
                        case EVENT_PURIFYING_LIGHT:
                            for (uint8 i = 0; i < 3; ++i)
                                me->SummonCreature(NPC_PURIFYING_LIGHT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 15.0f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000);
                            DoCast(me, SPELL_PURIFYING_LIGHT);
                            break;
                        case EVENT_RIGHTEOUS_SNEAR:
                            DoCastAOE(SPELL_RIGHTEOUS_SNEAR_AOE);
                            events.ScheduleEvent(EVENT_RIGHTEOUS_SNEAR, urand(20000, 25000));
                            break;
                        case EVENT_CONTINUE:
                            DoCast(me, SPELL_TRANSFORM, true);
                            events.ScheduleEvent(EVENT_CORRUPTING_TWILIGHT, 10000);
                            events.ScheduleEvent(EVENT_TWILIGHT_BLAST, urand(1000, 2000));
                            events.ScheduleEvent(EVENT_TWILIGHT_SNEAR, urand(5000, 10000));
                            break;
                        case EVENT_TWILIGHT_BLAST:
                            DoCastVictim(SPELL_TWILIGHT_BLAST);
                            events.ScheduleEvent(EVENT_TWILIGHT_BLAST, urand(5000, 7000));
                            break;
                        case EVENT_CORRUPTING_TWILIGHT:
                            for (uint8 i = 0; i < 3; ++i)
                                me->SummonCreature(NPC_CORRUPTING_TWILIGHT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 15.0f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000);
                            DoCast(me, SPELL_CORRUPTING_TWILIGHT);
                            break;
                        case EVENT_TWILIGHT_SNEAR:
                            DoCastAOE(SPELL_TWILIGHT_SNEAR_AOE);
                            events.ScheduleEvent(EVENT_TWILIGHT_SNEAR, urand(20000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:

            bool bPhase;

            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon();
            }
        };   
};

class npc_archbishop_benedictus_purifying_light : public CreatureScript
{
    public:
        npc_archbishop_benedictus_purifying_light() : CreatureScript("npc_archbishop_benedictus_purifying_light") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_archbishop_benedictus_purifying_lightAI(creature);
        }

        struct npc_archbishop_benedictus_purifying_lightAI : public Scripted_NoMovementAI
        {
            npc_archbishop_benedictus_purifying_lightAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
            }

            void MovementInform(uint32 type, uint32 data)
            {
                if (data == EVENT_JUMP_2)
                {
                    DoCast(me, ((me->GetEntry() == NPC_PURIFYING_LIGHT) ? SPELL_PURIFYING_BLAST_DMG : SPELL_TWILIGHT_BOLT_DMG), true);
                    me->DespawnOrUnsummon(2000);
                }
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_LIGHT)
                {
                    if (Creature* pBenedictus = me->FindNearestCreature(NPC_BENEDICTUS, 200.0f))
                        if (Unit* pTarget = pBenedictus->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            float speedZ = 10.0f;
                            float dist = me->GetExactDist2d(pTarget->GetPositionX(), pTarget->GetPositionY());
                            float speedXY = dist * 10.0f / speedZ;
                            me->GetMotionMaster()->MoveJump(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), speedXY, speedZ, EVENT_JUMP_2);
                        }                   
                }
                else if (action == ACTION_TWILIGHT)
                {
                    if (Creature* pBenedictus = me->FindNearestCreature(NPC_BENEDICTUS, 200.0f))
                        if (Unit* pTarget = pBenedictus->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            float speedZ = 10.0f;
                            float dist = me->GetExactDist2d(pTarget->GetPositionX(), pTarget->GetPositionY());
                            float speedXY = dist * 10.0f / speedZ;
                            me->GetMotionMaster()->MoveJump(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), speedXY, speedZ, EVENT_JUMP_2);
                        } 
                }
            }
        };   
};

class spell_archbishop_benedictus_purifying_light_targeting : public SpellScriptLoader
{
    public:
        spell_archbishop_benedictus_purifying_light_targeting() : SpellScriptLoader("spell_archbishop_benedictus_purifying_light_targeting") { }

        class spell_archbishop_benedictus_purifying_light_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_archbishop_benedictus_purifying_light_targeting_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster())
                    return;

                if (Creature* pBenedictus = GetCaster()->ToCreature())
                    pBenedictus->AI()->DoAction((m_scriptSpellId == SPELL_PURIFYING_LIGHT_TARGETING) ? ACTION_LIGHT : ACTION_TWILIGHT);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_archbishop_benedictus_purifying_light_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_archbishop_benedictus_purifying_light_targeting_SpellScript();
        }
};

class spell_archbishop_benedictus_righteous_snear_aoe : public SpellScriptLoader
{
    public:
        spell_archbishop_benedictus_righteous_snear_aoe() : SpellScriptLoader("spell_archbishop_benedictus_righteous_snear_aoe") { }

        class spell_archbishop_benedictus_righteous_snear_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_archbishop_benedictus_righteous_snear_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.size() <= 1)
                    return;

                if (Creature* pBenedictus = GetCaster()->ToCreature())
                    if (Unit* pTank = pBenedictus->getVictim())
                        targets.remove(pTank);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastCustomSpell(((m_scriptSpellId == SPELL_RIGHTEOUS_SNEAR_AOE) ? SPELL_RIGHTEOUS_SNEAR_AURA : SPELL_TWILIGHT_SNEAR_AURA), SPELLVALUE_AURA_STACK, 2, GetHitUnit(), true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_archbishop_benedictus_righteous_snear_aoe_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_archbishop_benedictus_righteous_snear_aoe_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_archbishop_benedictus_righteous_snear_aoe_SpellScript();
        }
};

void AddSC_boss_archbishop_benedictus()
{
    new boss_archbishop_benedictus();
    new npc_archbishop_benedictus_purifying_light();
    new spell_archbishop_benedictus_purifying_light_targeting();
    new spell_archbishop_benedictus_righteous_snear_aoe();
}
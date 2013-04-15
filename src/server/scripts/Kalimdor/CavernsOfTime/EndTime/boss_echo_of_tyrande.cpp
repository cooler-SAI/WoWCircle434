
#include "ScriptPCH.h"
#include "end_time.h"

enum Yells
{
    SAY_AGGRO       = 0,
    SAY_80          = 1,
    SAY_80_EMOTE    = 2,
    SAY_55          = 3,
    SAY_30          = 4,
    SAY_30_EMOTE    = 5,
    SAY_DEATH       = 6,
    SAY_INTRO_5     = 11,
    SAY_KILL        = 12,
    SAY_EYES        = 13,
    SAY_MOONLANCE   = 14,
};

enum Spells
{
    SPELL_MOONBOLT                      = 102193,
    SPELL_DARK_MOONLIGHT                = 102414,
    SPELL_MOONLIGHT_COSMETIC            = 108642,
    SPELL_STARDUST                      = 102173,
    SPELL_MOONLANCE_AURA                = 102150,
    SPELL_MOONLANCE_DMG                 = 102149,
    SPELL_MOONLANCE_SUMMON_1            = 102151,
    SPELL_MOONLANCE_SUMMON_2            = 102152,
    SPELL_LUNAR_GUIDANCE                = 102472,
    SPELL_TEARS_OF_ELUNE                = 102241,
    SPELL_TEARS_OF_ELUNE_SCRIPT         = 102242,
    SPELL_TEARS_OF_ELUNE_MISSILE        = 102243,
    SPELL_TEARS_OF_ELUNE_DMG            = 102244,
    SPELL_EYES_OF_GODDESS               = 102181,
    SPELL_EYES_OF_GODDESS_STUN          = 102248,

    SPELL_PIERCING_GAZE_OF_ELUNE_AURA   = 102182,
    SPELL_PIERCING_GAZE_OF_ELUNE_DMG    = 102183,

    // event
    SPELL_IN_SHADOW                     = 101841,
    SPELL_MOONLIGHT                     = 101946,

};

enum Events
{
    EVENT_MOONBOLT          = 1,
    EVENT_MOONLANCE         = 2,
    EVENT_STARDUST          = 3,
    EVENT_EYES_OF_GODDESS   = 4,
};

enum Adds
{
    NPC_MOONLANCE_1         = 54574,
    NPC_MOONLANCE_2_1       = 54580,
    NPC_MOONLANCE_2_2       = 54581,
    NPC_MOONLANCE_2_3       = 54582,
    NPC_EYE_OF_ELUNE_1      = 54939,
    NPC_EYE_OF_ELUNE_2      = 54940,
    NPC_EYE_OF_ELUNE_3      = 54941,
    NPC_EYE_OF_ELUNE_4      = 54942,

    // for aura
    NPC_STALKER             = 45979,

    NPC_POOL_OF_MOONLIGHT   = 54508,
};

enum Other
{
    POINT_MOONLANCE = 1,
};

class boss_echo_of_tyrande : public CreatureScript
{
    public:
        boss_echo_of_tyrande() : CreatureScript("boss_echo_of_tyrande") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_echo_of_tyrandeAI(pCreature);
        }

        struct boss_echo_of_tyrandeAI : public BossAI
        {
            boss_echo_of_tyrandeAI(Creature* creature) : BossAI(creature, DATA_ECHO_OF_TYRANDE)
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
                bIntro = false;
                phase = 0;
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 8);
                me->AddAura(SPELL_IN_SHADOW, me);
            }

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(ETScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset()
            {
                _Reset();

                moonlanceGUID = 0LL;
                phase = 0;
            }

            void JustDied(Unit* killer)
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() != NPC_STALKER)
                    BossAI::JustSummoned(summon);

                if (summon->GetEntry() == NPC_MOONLANCE_1)
                {
                    Player* target = ObjectAccessor::GetPlayer(*me, moonlanceGUID);
                    if (!target)
                        summon->DespawnOrUnsummon();

                    summon->SetOrientation(me->GetAngle(target));
                    Position pos;
                    summon->GetNearPosition(pos, 15.0f, 0.0f);
                    summon->GetMotionMaster()->MovePoint(POINT_MOONLANCE, pos);
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_MOONBOLT)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (bIntro)
                    return;

                if (!me->IsWithinDistInMap(who, 60.0f, false))
                    return;

                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                me->RemoveAura(SPELL_IN_SHADOW);

                Talk(SAY_INTRO_5);
                if (Creature* pStalker = me->SummonCreature(NPC_STALKER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f))
                {
                    pStalker->RemoveAllAuras();
                    pStalker->CastSpell(pStalker, SPELL_MOONLIGHT_COSMETIC, true);
                }
                bIntro = true;
            }

            void EnterCombat(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
                
                phase = 0;

                DoCast(me, SPELL_DARK_MOONLIGHT, true);

                events.ScheduleEvent(EVENT_MOONBOLT, 1000);
                events.ScheduleEvent(EVENT_MOONLANCE, urand(12000, 13000));
                events.ScheduleEvent(EVENT_STARDUST, urand(7000, 8000));
                events.ScheduleEvent(EVENT_EYES_OF_GODDESS, urand(30000, 33000));

                instance->SetBossState(DATA_ECHO_OF_TYRANDE, IN_PROGRESS);
                DoZoneInCombat();
            }

            void AttackStart(Unit* who)
            {
                if (who)
                    me->Attack(who, false);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (phase == 0 && me->HealthBelowPct(80))
                {
                    phase = 1;
                    Talk(SAY_80);
                    DoCast(me, SPELL_LUNAR_GUIDANCE);
                    return;
                }
                else if (phase == 1 && me->HealthBelowPct(55))
                {
                    phase = 2;
                    Talk(SAY_55);
                    DoCast(me, SPELL_LUNAR_GUIDANCE);
                    return;
                }
                else if (phase == 2 && me->HealthBelowPct(30))
                {
                    phase = 3;
                    Talk(SAY_30);
                    DoCast(me, SPELL_TEARS_OF_ELUNE);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOONBOLT:
                            DoCastVictim(SPELL_MOONBOLT);
                            events.ScheduleEvent(EVENT_MOONBOLT, 3000);
                            break;
                        case EVENT_MOONLANCE:
                        {
                            Unit* pTarget;
                            pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, -10.0f, true);
                            if (!pTarget)
                                pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
                            if (pTarget)
                            {
                                if (roll_chance_i(20))
                                    Talk(SAY_MOONLANCE);
                                moonlanceGUID = pTarget->GetGUID();
                                DoCast(pTarget, SPELL_MOONLANCE_SUMMON_1);
                            }
                            events.ScheduleEvent(EVENT_MOONLANCE, urand(12000, 13000));
                            break;
                        }
                        case EVENT_STARDUST:
                            DoCastAOE(SPELL_STARDUST);
                            events.ScheduleEvent(EVENT_STARDUST, urand(7000, 8000));
                            break;
                        case EVENT_EYES_OF_GODDESS:
                            Talk(SAY_EYES);
                            break;
                        default:
                            break;
                    }
                }
            }
        private:
            bool bIntro;
            uint8 phase; 
            uint64 moonlanceGUID;
        };
};

class npc_echo_of_tyrande_moonlance : public CreatureScript
{
    public:
        npc_echo_of_tyrande_moonlance() : CreatureScript("npc_echo_of_tyrande_moonlance") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_echo_of_tyrande_moonlanceAI(pCreature);
        }

        struct npc_echo_of_tyrande_moonlanceAI : public Scripted_NoMovementAI
        {
            npc_echo_of_tyrande_moonlanceAI(Creature*  pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, (me->GetEntry() == NPC_MOONLANCE_1 ? 0.6f : 0.9f), true);
            }

            void AttackStart(Unit* /*who*/) { return; }

            void MovementInform(uint32 type, uint32 data)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (data == POINT_MOONLANCE)
                {
                    if (me->GetEntry() == NPC_MOONLANCE_1)
                    {
                        Position pos1_1, pos1_2, pos2_1, pos2_2, pos3_1, pos3_2;
                        me->GetNearPosition(pos1_1, 3.0f, -(M_PI / 4.0f));
                        me->GetNearPosition(pos1_2, 30.0f, -(M_PI / 4.0f)); 
                        me->GetNearPosition(pos2_1, 3.0f, 0.0f); 
                        me->GetNearPosition(pos2_2, 30.0f, 0.0f); 
                        me->GetNearPosition(pos3_1, 3.0f, (M_PI / 4.0f)); 
                        me->GetNearPosition(pos3_2, 30.0f, (M_PI / 4.0f));
                        if (Creature* pLance1 = me->SummonCreature(NPC_MOONLANCE_2_1, pos1_1, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            pLance1->GetMotionMaster()->MovePoint(POINT_MOONLANCE, pos1_2);
                        if (Creature* pLance2 = me->SummonCreature(NPC_MOONLANCE_2_2, pos2_1, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            pLance2->GetMotionMaster()->MovePoint(POINT_MOONLANCE, pos2_2);
                        if (Creature* pLance3 = me->SummonCreature(NPC_MOONLANCE_2_3, pos3_1, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            pLance3->GetMotionMaster()->MovePoint(POINT_MOONLANCE, pos3_2);
                    }

                    me->DespawnOrUnsummon(500);
                }
            }
        };
};

class spell_echo_of_tyrande_tears_of_elune_script : public SpellScriptLoader
{
    public:
        spell_echo_of_tyrande_tears_of_elune_script() : SpellScriptLoader("spell_echo_of_tyrande_tears_of_elune_script") { }

        class spell_echo_of_tyrande_tears_of_elune_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_echo_of_tyrande_tears_of_elune_script_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
			{
				if(!GetCaster() || !GetHitUnit())
					return;

                if (roll_chance_i(50))
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_TEARS_OF_ELUNE_MISSILE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_echo_of_tyrande_tears_of_elune_script_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_echo_of_tyrande_tears_of_elune_script_SpellScript();
        }
};

void AddSC_boss_echo_of_tyrande()
{
    new boss_echo_of_tyrande;
    new npc_echo_of_tyrande_moonlance();
    new spell_echo_of_tyrande_tears_of_elune_script();
}

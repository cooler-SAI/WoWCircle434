/*
 * WowCircle 4.3.4
 * Dev: Ramusik
 */

#include "ScriptPCH.h"
#include "firelands.h"

enum Spells
{
    // Majordormo Staghelm
    SPELL_BERSERK                   = 26662, // Increases the caster's attack and movement speeds by 150% and all damage it deals by 500%. Also grants immunity to Taunt effects
    SPELL_CAT_FORM                  = 98374, // Fandral transforms into a cat when his enemies are spread out.
    SPELL_SCORPION_FORM             = 98379, // Fandral transforms into a scorpion when 7 or more of his enemies are clustered together in 10 player raids, and 18 or more in 25 player raids.
    SPELL_FURY                      = 97235, // Fandral's fury fuels his flames, increasing the damage he inflicts with Leaping Flames and Flame Scythe by 8% per application. Stacks.
    SPELL_ADRENALINE                = 97238, // Increases the caster's energy regeneration rate by 20% per application. Stacks.
    SPELL_FIERY_CYCLONE             = 98443, // Tosses all enemy targets into the air, preventing all action but making them invulnerable for 3 sec.
    SPELL_SEARING_SEEDS             = 98450, // Implants fiery seeds in the caster's enemies. Each seed grows at a different rate. When fully grown the seeds explode, inflicting 63750 Fire damage to targets within 12 yards.
    SPELL_SEARING_SEEDS_EXPLOSION   = 98620, // should be triggered when Searing Seeds removes
    SPELL_BURNING_ORBS              = 98451, // Summons Burning Orbs to attack the caster's enemies.
    SPELL_BURNING_ORBS_SUMMON       = 98565, // 10man - 2, 25man - 5
    SPELL_FLAME_SCYTHE              = 98474, // Inflicts Fire damage in front of the caster. Damage is split equally among targets hit.
    SPELL_LEAPING_FLAMES            = 98476, // Leaps at an enemy, inflicting 26036 to 29213 Fire damage in a small area and creating a Spirit of the Flame.
    SPELL_LEAPING_FLAMES_SUMMON     = 101222, // Summon Spirit of the Flame
    SPELL_LEAPING_FLAMES_PERSISTENT = 98535, // Fandral lands in a blaze of glory, igniting the ground at his destination and causing it to burn enemy enemy units for 26036 to 29213 Fire damage every 0.5 sec

    // Burning Orb
    SPELL_BURNING_ORB_PERIODIC      = 98583, // Visual. Inflicts 7650 Fire damage every 2 sec. Stacks.
};

enum Events
{
    EVENT_BERSERK           = 1,
    EVENT_CHECK_PHASE       = 2,
    EVENT_LEAPING_FLAMES    = 3,
    EVENT_FLAME_SCYTHE      = 4,
    EVENT_HUMANOID_PHASE    = 5,
    EVENT_CAT_FORM          = 6,
    EVENT_SCORPION_FORM     = 7,
};

enum Yells
{
    SAY_TRANSFORM_1         = 0, // The master's power takes on many forms ...
    SAY_TRANSFORM_2         = 1, // Behold the rage of the Firelands!
    SAY_HUMANOID_1          = 2, // Blaze of Glory!
    SAY_HUMANOID_2          = 3, // Nothing but ash!
    SAY_DEATH               = 4, // My studies... had only just begun...
    SAY_BERSERK             = 5, // So much power!
    SAY_KILL                = 6, // Burn.
                                 // Soon ALL of Azeroth will burn!
                                 // You stood in the fire!
};

enum Phases
{
    PHASE_HUMANOID      = 1,
    PHASE_CAT           = 2,
    PHASE_SCORPION      = 3
};

enum CreatureEncounterIds
{
    NPC_BURNING_ORB     = 53216,
};

class boss_majordomo_staghelm : public CreatureScript
{
    public:
        boss_majordomo_staghelm() : CreatureScript("boss_majordomo_staghelm") { }

        struct boss_majordomo_staghelmAI : public BossAI
        {
            boss_majordomo_staghelmAI(Creature* creature) : BossAI(creature, DATA_STAGHELM)
            {
            }

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(FLScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset()
            {
                _Reset();
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                events.ScheduleEvent(EVENT_BERSERK, 600000);    // 10 min
                events.ScheduleEvent(EVENT_CHECK_PHASE, 2000);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                _currentPhase = PHASE_HUMANOID;
                _changePhaseNum = 0;
            }

            void EnterCombat(Unit* attacker)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                Talk(SAY_DEATH);
                _JustDied();
            }

            void JustReachedHome()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _JustReachedHome();
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                switch (summon->GetEntry())
                {
                    case NPC_BURNING_ORB:
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);
                        summon->CastSpell(summon, SPELL_BURNING_ORB_PERIODIC, false);
                        break;
                    default:
                        break;
                }

                if (me->isInCombat())
                    DoZoneInCombat(summon);
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
                        case EVENT_CHECK_PHASE:
                        {
                            uint8 _phase = PHASE_CAT;
                            std::list<Unit*> targetList;
                            {
                                const std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
                                for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                                {
                                    if ((*itr)->getTarget()->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        std::list<Player*> PlayerList;
                                        Trinity::AnyPlayerInObjectRangeCheck checker((*itr)->getTarget(), 5.0f);
                                        Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher((*itr)->getTarget(), PlayerList, checker);
                                        (*itr)->getTarget()->VisitNearbyWorldObject(5.0f, searcher);

                                        uint8 const minTargets = Is25ManRaid() ? 18 : 7;
                                        if (PlayerList.size() >= minTargets)
                                        {
                                            _phase = PHASE_SCORPION;
                                            break;
                                        }
                                    }
                                }
                            }

                            if (_currentPhase != _phase)
                            {
                                events.CancelEvent(EVENT_LEAPING_FLAMES);
                                events.CancelEvent(EVENT_FLAME_SCYTHE);
                                me->RemoveAurasDueToSpell(SPELL_ADRENALINE);
                                if (_changePhaseNum % 3 == 2)
                                    events.ScheduleEvent(EVENT_HUMANOID_PHASE, 1000);
                                events.ScheduleEvent(_phase == PHASE_CAT ? EVENT_CAT_FORM : EVENT_SCORPION_FORM, 5000);

                                _changePhaseNum++;
                            }

                            events.ScheduleEvent(EVENT_CHECK_PHASE, 6000);
                            break;
                        }
                        case EVENT_HUMANOID_PHASE:
                            Talk(_currentPhase == PHASE_CAT ? SAY_HUMANOID_1 : SAY_HUMANOID_2);
                            me->RemoveAurasDueToSpell(SPELL_CAT_FORM);
                            me->RemoveAurasDueToSpell(SPELL_SCORPION_FORM);
                            DoCastAOE(SPELL_FIERY_CYCLONE);
                            DoCastAOE(_currentPhase == PHASE_CAT ? SPELL_SEARING_SEEDS : SPELL_BURNING_ORBS);
                            break;
                        case EVENT_CAT_FORM:
                            Talk(SAY_TRANSFORM_2);
                            me->SetPower(POWER_ENERGY, 0);
                            DoCast(me, SPELL_CAT_FORM);
                            if (_currentPhase != PHASE_HUMANOID)
                                DoCast(me, SPELL_FURY);
                            _currentPhase = PHASE_CAT;
                            events.ScheduleEvent(EVENT_LEAPING_FLAMES, 10000);
                            break;
                        case EVENT_SCORPION_FORM:
                            Talk(SAY_TRANSFORM_1);
                            me->SetPower(POWER_ENERGY, 0);
                            DoCast(me, SPELL_SCORPION_FORM);
                            if (_currentPhase != PHASE_HUMANOID)
                                DoCast(me, SPELL_FURY);
                            _currentPhase = PHASE_SCORPION;
                            events.ScheduleEvent(EVENT_FLAME_SCYTHE, 10000);
                            break;
                        case EVENT_FLAME_SCYTHE:
                            DoCastVictim(SPELL_FLAME_SCYTHE);
                            events.ScheduleEvent(EVENT_FLAME_SCYTHE, 2000);
                            break;
                        case EVENT_LEAPING_FLAMES:
                            if (me->GetPower(POWER_ENERGY) == 100)
                            {
                                DoCast(me, SPELL_LEAPING_FLAMES_SUMMON);
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                {
                                    DoCast(target, SPELL_LEAPING_FLAMES);
                                    me->CastSpell(target, SPELL_LEAPING_FLAMES_PERSISTENT, true); // doesn't work as trigger spell of 98476
                                }
                            }
                            events.ScheduleEvent(EVENT_LEAPING_FLAMES, 1000);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                uint8 _currentPhase;
                uint32 _changePhaseNum;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_majordomo_staghelmAI(creature);
        }
};

class spell_staghelm_searing_seeds_aura : public SpellScriptLoader
{
    public:
        spell_staghelm_searing_seeds_aura() : SpellScriptLoader("spell_staghelm_searing_seeds_aura") { }

        class spell_staghelm_searing_seeds_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_staghelm_searing_seeds_aura_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Aura* aura = aurEff->GetBase();
                uint32 duration = urand(3000, 45000);
                aura->SetDuration(duration);
                aura->SetMaxDuration(duration);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_SEARING_SEEDS_EXPLOSION, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_staghelm_searing_seeds_aura_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_staghelm_searing_seeds_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_staghelm_searing_seeds_aura_AuraScript();
        }
};

class spell_staghelm_burning_orbs : public SpellScriptLoader
{
    public:
        spell_staghelm_burning_orbs() : SpellScriptLoader("spell_staghelm_burning_orbs") { }

        class spell_staghelm_burning_orbs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_staghelm_burning_orbs_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint8 const orbsCount = (GetCaster()->GetMap()->GetSpawnMode() & 1) ? 5 : 2;
                for (uint8 itr = 0; itr < orbsCount; ++itr)
                    caster->CastSpell(caster, SPELL_BURNING_ORBS_SUMMON, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_staghelm_burning_orbs_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_staghelm_burning_orbs_SpellScript();
        }
};

void AddSC_boss_majordomo_staghelm()
{
    new boss_majordomo_staghelm();
    new spell_staghelm_searing_seeds_aura();
    new spell_staghelm_burning_orbs();
}

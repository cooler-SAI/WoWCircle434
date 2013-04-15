#include "ScriptPCH.h"
#include "end_time.h"

enum Yells
{
    SAY_INTRO = 1,
    SAY_AGGRO,
    SAY_DEATH,
    SAY_KILL_1,
    SAY_KILL_2,
    SAY_KILL_3,
    SAY_THROW_TOTEM,
    SAY_PULVERIZE,
};

enum Spells
{
    SPELL_BAINE_VIS   = 101624, // Visuals on boss (the totems on the back etc.)

    SPELL_THROW_TOTEM = 101615, // Triggers missile at location, with summon totem and kb.
    SPELL_PULVERIZE_J = 101626, // Jump b. target, activate platform.
    SPELL_PULVERIZE_D = 101627, // Damage spell.
    SPELL_PULV_DBM    = 101625, // DBM says this, fuck it.
    SPELL_MOLTEN_AXE  = 101836, // Extra damage on melee attack, change targets from caster to target. When he falls into lava after Pulverize.
    SPELL_MOLTEN_FIST = 101866, // Extra dmg on melee for players when they touch the lava, they get this when baine gets Molten Axe.

    SPELL_TB_TOTEM    = 101602, // Throw totem back at Baine on click.
    SPELL_TB_TOTEM_A  = 107837, // Visual aura: player has totem to throw.
};

enum Events
{
    EVENT_PULVERIZE = 1,
    EVENT_PULVERIZE_DAMAGE,
    EVENT_THROW_TOTEM,
};

enum Creatures
{
    NPC_ROCK_ISLAND = 54496,
};

class boss_echo_of_baine : public CreatureScript
{
    public:
        boss_echo_of_baine() : CreatureScript("boss_echo_of_baine") { }

        struct boss_echo_of_baineAI : public BossAI
        {
            boss_echo_of_baineAI(Creature* pCreature) : BossAI(pCreature, DATA_ECHO_OF_BAINE)
            {
                introDone = false;
            }

            bool introDone;
            Unit* pulverizeTarget;

            void Reset()
            {
                _Reset();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void EnterCombat(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_PULVERIZE, 40000);
                events.ScheduleEvent(EVENT_THROW_TOTEM, 25000);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                instance->SetBossState(DATA_ECHO_OF_BAINE, IN_PROGRESS);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (introDone)
                    return;

                if (!me->IsWithinDistInMap(who, 40.0f, false))
                    return;

                Talk(SAY_INTRO);
                introDone = true;
            }

            void JustDied(Unit* killer)
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void KilledUnit(Unit * /*victim*/)
            {
                Talk(SAY_KILL_1);
            }


            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->IsInWater() && !me->HasAura(SPELL_MOLTEN_AXE))
                    DoCast(me, SPELL_MOLTEN_AXE);

                Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();    
                if (!PlayerList.isEmpty())
                  for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (Player* playr = i->getSource())
                        if (playr->IsInWater() && !playr->HasAura(SPELL_MOLTEN_FIST))
                            playr->AddAura(SPELL_MOLTEN_FIST, playr); // Add the damage aura to players in Magma.

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PULVERIZE:
                            Talk(SAY_PULVERIZE);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true)) // SELECT_TARGET_RANDOM, 0, urand(10.0f, 100.0f), true)
                            {
                                DoCast(target, SPELL_PULVERIZE_J);
                                DoCast(target, SPELL_PULV_DBM);
                            }
                            events.ScheduleEvent(EVENT_PULVERIZE, 40000); // every 40 secs.
                            events.ScheduleEvent(EVENT_PULVERIZE_DAMAGE, 3000); // You have 3 secs to run.
                            break;

                        case EVENT_PULVERIZE_DAMAGE:
                            if(GameObject* platform = me->FindNearestGameObject(209255, 20.0f))
                                platform->SetGoState(GO_STATE_ACTIVE);                     
                            DoCast(me, SPELL_PULVERIZE_D);
                            break;

                        case EVENT_THROW_TOTEM:
                            Talk(SAY_THROW_TOTEM);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true)) // SELECT_TARGET_RANDOM, 0, urand(10.0f, 100.0f), true)
                                DoCast(target, SPELL_THROW_TOTEM);
                            events.ScheduleEvent(EVENT_THROW_TOTEM, 25000); // every 25 secs.
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_echo_of_baineAI(creature);
        }
};

void AddSC_boss_echo_of_baine()
{
    new boss_echo_of_baine();
}
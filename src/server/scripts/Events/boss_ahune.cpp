#include "ScriptPCH.h"
 
enum Creatures
{
    NPC_AHUNE                   = 25740,
    NPC_FROZEN_CORE             = 25865,
    NPC_AHUNITE_COLDWEAVE       = 25756,
    NPC_AHUNITE_FROSTWIND       = 25757,
    NPC_AHUNITE_HAILSTONE       = 25755,

    NPC_LOOT_LOC_BUNNY          = 25746
};
 
enum Spells
{
    // Ahune
    SPELL_AHUNES_SHIELD         = 45954,
    SPELL_COLD_SLAP             = 46145,
 
    SPELL_MAKE_BONFIRE          = 45930,
    SPELL_SUMMONING_VISUAL1     = 45937, // below Ahune (?)
    SPELL_SUMMONING_VISUAL2     = 45938, // below the Summoning Stone (?)
    SPELL_SUMMON_MINION_VISUAL  = 46103, // Midsummer - Ahune - Summon Minion, Lower
    SPELL_GHOST_VISUAL          = 46786,
    SPELL_RESURFACE             = 46402, // Ahune Resurfaces
 
    SPELL_LOOT_CHEST            = 45939,
    SPELL_LOOT_CHEST_HC         = 46622,
 
    SPELL_AHUNE_ACHIEVEMENT     = 62043, // Midsummer - Ahune - DIES, Achievement
 
    // Coldweave
    SPELL_BITTER_BLAST          = 46406,
 
    // Frostwind
    SPELL_LIGHTNING_SHIELD      = 12550,
    SPELL_WIND_BUFFET           = 46568,
 
    // Hailstone
    SPELL_CHILLING_AURA         = 46885, // periodic trigger
    SPELL_PULVERIZE             = 2676
};
 
enum Events
{
    // Ahune
    EVENT_SWITCH_PHASE          = 1,
    EVENT_SUMMON_HAILSTONE      = 2,
    EVENT_SUMMON_COLDWEAVE      = 3,
    EVENT_SUMMON_FROSTWIND      = 4,
    EVENT_ICE_SPEAR             = 5,
    EVENT_COLD_SLAP             = 6,
 
    // Frozen Core
    EVENT_GHOST_VISUAL          = 7,
    EVENT_RESURFACE_SOON        = 8,
 
    // Coldweave
    EVENT_BITTER_BLAST          = 9,
 
    // Frostwind
    EVENT_WIND_BUFFET           = 10,
 
    // Hailstone
    EVENT_CHILLING_AURA         = 11,
    EVENT_PULVERIZE             = 12
};
 
enum Phases
{
    PHASE_ONE = 1,
    PHASE_TWO = 2
};
 
Position const SummonPositions[5] =
{
        {-99.102100f, -233.753000f, -1.222970f}, // Ahune / Frozen-Core
        {-90.891891f, -243.488068f, -1.116222f}, // Hailstone
        {-97.389175f, -239.780701f, -1.264044f}, // Coldweave #1
        {-85.160637f, -236.127808f, -1.572418f}, // Coldweave #2
        {-96.872300f, -212.842000f, -1.149140f}  // Reward chest
};

typedef std::list<WorldObject*> ObjectList;
 
class boss_ahune : public CreatureScript
{
    public:
        boss_ahune() : CreatureScript("boss_ahune") { }
 
        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ahuneAI(creature);
        }
 
        struct boss_ahuneAI : public ScriptedAI
        {
            boss_ahuneAI(Creature* creature) : ScriptedAI(creature), summons(me) { }
 
            EventMap events;
            SummonList summons;
 
            uint64 frozenCoreGUID;
 
            void Reset()
            {
                summons.DespawnAll();

                SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
 
                events.Reset();
                events.SetPhase(PHASE_ONE);
 
                frozenCoreGUID = 0;
 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
 
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
            }
 
            void EnterCombat(Unit* who)
            {
                DoZoneInCombat();
 
                events.Reset();
                events.SetPhase(PHASE_ONE);
                events.ScheduleEvent(EVENT_SWITCH_PHASE, 90000); // phase 2 after 90 seconds
                events.ScheduleEvent(EVENT_COLD_SLAP, 500, 0, PHASE_ONE); // every 500ms in melee range in phase 1
                events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 1000, 0, PHASE_ONE); // once in every phase 1
                events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 8000, 0, PHASE_ONE); // every 7 seconds in phase 1
                events.ScheduleEvent(EVENT_ICE_SPEAR, 9500, 0, PHASE_ONE); // every 7 seconds in phase 1, first after 9.5 seconds
 
                me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), SPELL_SUMMONING_VISUAL1, true);
                me->AddAura(SPELL_AHUNES_SHIELD, me);
            }
 
            void JustDied(Unit* killer)
            {
                summons.DespawnAll();

                Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
                if (!playerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                        if (i->getSource())
                            DoCast(i->getSource(), SPELL_AHUNE_ACHIEVEMENT);
 
                ObjectList* units = GetWorldObjectsInDist(me, 100);
                for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                {
                    if (!(*itr) || (*itr)->GetTypeId() != TYPEID_UNIT)
                        continue;

                    if (Unit* nearby = (*itr)->ToUnit())
                        if (nearby->GetEntry() == NPC_LOOT_LOC_BUNNY)
                        {
                            nearby->CastSpell((Unit*)NULL, SPELL_LOOT_CHEST_HC, true);
                            break;
                        }
                }
            }
 
            void JustSummoned(Creature* summoned)
            {
                DoZoneInCombat(summoned);
 
                summons.Summon(summoned);
            }
 
            void SummonedCreatureDespawn(Creature* summoned)
            {
                summons.Despawn(summoned);
            }
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWITCH_PHASE:
                            if (events.IsInPhase(PHASE_ONE))
                            {
                                events.SetPhase(PHASE_TWO);
                                events.ScheduleEvent(EVENT_SWITCH_PHASE, 30000);
 
                                DoCast(me, SPELL_MAKE_BONFIRE);
 
                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_SUBMERGED);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
 
                                Talk(0);
 
                                // spawn core
                                if (Unit* frozenCore = me->SummonCreature(NPC_FROZEN_CORE, SummonPositions[0], TEMPSUMMON_CORPSE_DESPAWN))
                                {
                                    frozenCoreGUID = frozenCore->GetGUID();
                                    frozenCore->SetMaxHealth(me->GetMaxHealth());
                                    frozenCore->SetHealth(me->GetHealth()); // sync health on phase change
                                    frozenCore->SetOwnerGUID(me->GetGUID());
                                }
                            }
                            else
                            {
                                events.SetPhase(PHASE_ONE);
                                events.ScheduleEvent(EVENT_SWITCH_PHASE, 90000);
                                events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 1000, 0, PHASE_ONE);
                                events.ScheduleEvent(EVENT_SUMMON_FROSTWIND, 9000, 0, PHASE_ONE);
 
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
 
                                // despawn core
                                if (Creature* frozenCore = me->GetCreature(*me, frozenCoreGUID))
                                    frozenCore->DespawnOrUnsummon(0);
                            }
                            break;
                        case EVENT_COLD_SLAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 8.0f, true))
                                DoCast(target, SPELL_COLD_SLAP);
                            events.ScheduleEvent(EVENT_COLD_SLAP, 500, 0, PHASE_ONE);
                            break;
                        case EVENT_ICE_SPEAR:
                            // TODO: lots of spells involved, no idea of their order yet
                            break;
                        case EVENT_SUMMON_HAILSTONE:
                            me->SummonCreature(NPC_AHUNITE_HAILSTONE, SummonPositions[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            break;
                        case EVENT_SUMMON_COLDWEAVE: // they always come in pairs of two
                            me->CastSpell(SummonPositions[2].GetPositionX(), SummonPositions[2].GetPositionY(), SummonPositions[2].GetPositionZ(), SPELL_SUMMON_MINION_VISUAL, false);
                            me->SummonCreature(NPC_AHUNITE_COLDWEAVE, SummonPositions[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            me->CastSpell(SummonPositions[3].GetPositionX(), SummonPositions[3].GetPositionY(), SummonPositions[3].GetPositionZ(), SPELL_SUMMON_MINION_VISUAL, false);
                            me->SummonCreature(NPC_AHUNITE_COLDWEAVE, SummonPositions[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 5000, 0, PHASE_ONE);
                            break;
                        case EVENT_SUMMON_FROSTWIND: // not in first phase 1
                            me->SummonCreature(NPC_AHUNITE_FROSTWIND, SummonPositions[urand(2,3)], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            events.ScheduleEvent(EVENT_SUMMON_FROSTWIND, 7000, 0, PHASE_ONE);
                            break;
                    }
                }
 
                DoMeleeAttackIfReady();
            }

            ObjectList* GetWorldObjectsInDist(WorldObject* obj, float dist)
            {
                ObjectList* targets = new ObjectList();
                if (obj)
                {
                    Trinity::AllWorldObjectsInRange u_check(obj, dist);
                    Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(obj, *targets, u_check);
                    obj->VisitNearbyObject(dist, searcher);
                }
                return targets;
            }
        };
};
 
class npc_frozen_core : public CreatureScript
{
    public:
        npc_frozen_core() : CreatureScript("npc_frozen_core") { }
 
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frozen_coreAI(creature);
        }
 
        struct npc_frozen_coreAI : public ScriptedAI
        {
            npc_frozen_coreAI(Creature* creature) : ScriptedAI(creature) { }
 
            EventMap events;
 
            void Reset()
            {
                SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                events.Reset();
 
                events.ScheduleEvent(EVENT_RESURFACE_SOON, 15000); // after 15 seconds Emote: Ahune will soon resurface.
 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }
 
            void DamageTaken(Unit* who, uint32& damage)
            {
                if (Unit* owner = me->GetOwner())
                {
                    if (owner->GetHealth() > damage)
                        owner->SetHealth(owner->GetHealth() - damage);
                    else
                        who->Kill(owner);
                }
            }
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RESURFACE_SOON:
                            Talk(0);
                            break;
                    }
                }
            }
        };
};
 
class npc_ahunite_hailstone : public CreatureScript
{
    public:
        npc_ahunite_hailstone() : CreatureScript("npc_ahunite_hailstone") { }
 
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_hailstoneAI(creature);
        }
 
        struct npc_ahunite_hailstoneAI : public ScriptedAI
        {
            npc_ahunite_hailstoneAI(Creature* creature) : ScriptedAI(creature) { }
 
            EventMap events;
 
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_PULVERIZE, urand(6000, 8000));
 
                DoCast(me, SPELL_CHILLING_AURA);
            }
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PULVERIZE:
                            DoCastVictim(SPELL_PULVERIZE);
                            events.ScheduleEvent(EVENT_PULVERIZE, urand(6000, 8000));
                            break;
                    }
                }
 
                DoMeleeAttackIfReady();
            }
        };
};
 
class npc_ahunite_coldweave : public CreatureScript
{
    public:
        npc_ahunite_coldweave() : CreatureScript("npc_ahunite_coldweave") { }
 
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_coldweaveAI(creature);
        }
 
        struct npc_ahunite_coldweaveAI : public ScriptedAI
        {
            npc_ahunite_coldweaveAI(Creature* creature) : ScriptedAI(creature) { }
 
            EventMap events;
 
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_BITTER_BLAST, 500);
            }
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BITTER_BLAST:
                            DoCastVictim(SPELL_BITTER_BLAST);
                            events.ScheduleEvent(EVENT_BITTER_BLAST, urand(5000, 7000));
                            break;
                    }
                }
 
                DoMeleeAttackIfReady();
            }
        };
};
 
class npc_ahunite_frostwind : public CreatureScript
{
    public:
        npc_ahunite_frostwind() : CreatureScript("npc_ahunite_frostwind") { }
 
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_frostwindAI(creature);
        }
 
        struct npc_ahunite_frostwindAI : public ScriptedAI
        {
            npc_ahunite_frostwindAI(Creature* creature) : ScriptedAI(creature) { }
 
            EventMap events;
 
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_WIND_BUFFET, 2000); // TODO: get correct timing for wind buffet
 
                DoCast(me, SPELL_LIGHTNING_SHIELD);
            }
 
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);
 
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WIND_BUFFET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 120.0f))
                                DoCast(target, SPELL_WIND_BUFFET);
                            events.ScheduleEvent(EVENT_WIND_BUFFET, urand(5000, 7000));
                            break;
                    }
                }
 
                DoMeleeAttackIfReady();
            }
        };
};
 
void AddSC_boss_ahune()
{
    new boss_ahune();
    new npc_frozen_core();
    new npc_ahunite_hailstone();
    new npc_ahunite_coldweave();
    new npc_ahunite_frostwind();
}
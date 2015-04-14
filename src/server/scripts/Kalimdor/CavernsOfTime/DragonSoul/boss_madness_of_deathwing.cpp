/*
####### Phase 1 #######
# Assault Aspect                        - In progress
# Cataclysm                             - In progress
# Elementium Bolt                       - In progress
# Corrupting Parasite                   - In progress
# Hemorrhage                            - In progress
# Limb Tentacle                         - In progress
# Blistering Tentacle                   - In progress
# Mutated Corruption                    - In progress
#

####### Phase 2 #######
# Corrupted Blood                       - In progress
# Congealing Blood                      - In progress
# Elementium Fragment                   - In progress
# Elementium Terror                     - In progress

####### OTHER ########
# Script Aspects                        - In progress
# Fix loot                              - In progress
# Fix achievements & currency rewards   - In progress
# Fix 25N/10H/25H tunings               - In progress
# Fix code style                        - In progress
*/

#include "ScriptPCH.h"
#include "dragon_soul.h"
#include "Group.h"
#include "WorldSession.h"
#include "Map.h" 

// send SendPlaySpellVisualKit from limbs
// 22445 + 0 before casting visual spell
// 22446 + 0 after visual spell
enum ScriptedTexts
{
    //trall
    SAY_TRALL_START             = 0,
    SAY_TRALL_START_1           = 1,
    SAY_TRALL_BATTLE_START      = 2,
    SAY_TRALL_DEATH_DEATHWING   = 3,

    //Deathwing
    SAY_AGGRO                   = 0,
    SAY_KILL_ALL                = 1,
    SAY_SPELL_1                 = 2,
    SAY_SPELL_2                 = 3,
    SAY_SPELL_3                 = 4,
};

#define CreatCacheOfTheAspects RAID_MODE(GO_GREATER_CACHE_OF_THE_ASPECTS_10N, GO_GREATER_CACHE_OF_THE_ASPECTS_25N, GO_GREATER_CACHE_OF_THE_ASPECTS_10H, GO_GREATER_CACHE_OF_THE_ASPECTS_25H)

enum Actions
{
    ACTION_CORRUPTED_BLOOD      = 1,
    ACTION_THRALL_MENU          = 1,
    ACTION_PLATFROM_CHECK_START     = 1,
    ACTION_PLATFROM_1_COMPLETED     = 2,
    ACTION_PLATFROM_2_COMPLETED     = 3,
    ACTION_PLATFROM_3_COMPLETED     = 4,
    ACTION_PLATFROM_4_COMPLETED     = 5,
    //ACTION_PLATFROM_1_ACTIVATED     = 6,
    //ACTION_PLATFROM_2_ACTIVATED     = 7,
    //ACTION_PLATFROM_3_ACTIVATED     = 8,
    //ACTION_PLATFROM_4_ACTIVATED     = 9,
};

enum Events
{
    // Deathwing
    EVENT_ASSAULT_ASPECTS       = 1,
    EVENT_CATACLYSM,
    EVENT_CORRUPTING_PARASITE,
    EVENT_CORRUPTING_PARASITE_SUMMON,
    EVENT_ELEMENTIUM_BOLT,
    EVENT_END_ENCOUNTER,

    //tentacle
    EVENT_AGONUZUNG_PAIN,
    EVENT_CRUSH,
    EVENT_IMPALE,
    EVENT_IMPALE_ASPECT,

    //other
    EVENT_SUMMON,
    EVENT_SUMMON_MUTATED_CORRUPTION,

    //Phase
    PHASE_1,
    PHASE_2,
    HAS_20PROCENT_HEALTH_NEW_PHASE,

    //Phase 2
    EVENT_ELEMENTIUM_FRAGMENT,
    EVENT_ELEMENTIUM_TERROR,
    EVENT_CORRUPTED_BLOOD,
    EVENT_CONGEALING_BLOOD,
    EVENT_SLUMP,
    EVENT_ALTER_POWER,
    EVENT_ENCOUNTER_END,
    EVENT_BERSERK,

    //Phase 2 mob events
    EVENT_SHRAPNEL,
    EVENT_TETANUS,
    EVENT_CONGEALING_BLOOD_CAST,
    EVENT_UNSTABLE_CORRUPTION,

    //trall
    EVENT_SAY_TRALL_START,
    EVENT_SAY_TRALL_1,

    //aspects
    EVENT_NOZDORMY_EXPOSE_WEAKNESS,
    EVENT_ALEXSTRASZA_EXPOSE_WEAKNESS,
    EVENT_YSERA_EXPOSE_WEAKNESS,
    EVENT_KALECGOS_EXPOSE_WEAKNESS,
    EVENT_TIME_ZONE,
    EVENT_EXPOSE,
    EVENT_NOZDORMY_STAGE_1,
    EVENT_ALEXSTRASZA_STAGE_1,
    EVENT_YSERA_STAGE_1,
    EVENT_KALECGOS_STAGE_1,
    EVENT_CAUTERIZE,

    // MISC
    EVENT_SUMMON_REGENERATIVE_BLOOD,
    EVENT_CHECK_BOLT_POSITION,
    EVENT_BLISTERING_TENTACLE,

    // platfrom
    EVENT_PLATFROM_CHECK_1,
    EVENT_PLATFROM_STARTING,
};

enum Spells
{
    //DeathWing
    SPELL_CATACLYSM                     = 106523, //+
    SPELL_CORRUPTING_PARASITE           = 108597, // 1
    SPELL_CORRUPTING_PARASITE_TRIGER    = 108649, //+
    SPELL_CORRUPTING_PARASITE_AURA      = 108601, // 3
    SPELL_SLUMP                         = 106708,
    SPELL_CALM_MAELSTROM_SKYBOX         = 109480,
    SPELL_AGONIZING_PAIN                = 106548,
    SPELL_HEMMORHAGE                    = 105863, //+
    SPELL_HEMMORHAGE_ELEMENTIUM_TERROR  = 106767,
    SPELL_BERSERK                       = 64238,

    // elementium 
    SPELL_ELEMENTIUM_BOLT               = 105651, //+
    SPELL_ELEMENTIUM_BOLT_SUMMON        = 105658,
    SPELL_ELEMENTIUM_BLAST_AURA         = 105723,
    SPELL_ELEMENTIUM_BLAST_AURA_2       = 110628,
    SPELL_ELEMENTIUM_METEOR_TRANSFORM   = 110663, // AT DEATH

    // Mutated corruption
    SPELL_IMPALE                        = 106400, //+
    SPELL_CRUSH                         = 106385,
    SPELL_IMPALE_ASPECT                 = 107029,

    // blistering tentacle 
    SPELL_SPAWN_BLISTERING_TENTCLES     = 105551, //+
    SPELL_SPAWN_BLISTERING_TENTCLES_T   = 105549, //+
    SPELL_BLISTERING_HEAT               = 105444,
    SPELL_BLISTERING_HEAT_AURA          = 105445,

    // regenerative blood
    SPELL_DEGENERATIVE_BITE             = 105842,
    SPELL_REGENERATIVE_BLOOD_SCALE      = 105969,
    SPELL_REGENERATIVE_BLOOD_HEAL       = 110208,

    //
    SPELL_TETANUS                       = 106730, //+
    SPELL_CONGEALING_BLOOD              = 109089, //+ 
    SPELL_CONGEALING_BLOOD_2            = 109091,
    SPELL_SUMMON_ELEMENTIUM_TERROR      = 106765,
    SPELL_SUMMON_ELEMENTIUM_TERROR_T    = 106766,

    // tentacle
    SPELL_BURNING_BLODD                 = 105401,
    SPELL_BURNING_BLODD_TRIGGER         = 105408,
    SPELL_HEMORRHAGE                    = 105853,
    SPELL_UNSTABLE_CORRUPTION           = 108813, //+

    // Phase 2
    SPELL_CONGEALING_BLOOD_HEAL         = 109102,
    SPELL_CORRUPTED_BLOOD               = 106834,
    SPELL_SHRAPNEL                      = 106791, //+
    SPELL_CORRUPTED_BLOOD_BAR           = 106843,
    SPELL_SHRAPNEL_TARGET               = 106794,

    // Thrall
    SPELL_ASTRAL_RECALL                 = 108537,
    SPELL_ASTRAL_RECALL_BACK            = 110063, // TODO
    SPELL_LIMB_EMERGE_VISUAL            = 107991, 
    SPELL_IDLE                          = 106187, // tail tentacle has it
    SPELL_TRIGGER_ASPECT_BUFFS          = 106943, // casted by deathwing 56173
    SPELL_SHARE_HEALTH                  = 109547, // casted by deathwing 56173 on self ?
    SPELL_ASSAULT_ASPECTS               = 107018, // casted by deathwing 56173
    SPELL_CRASH                         = 109628, // casted by mutated tentacle and tail tentacle ?
    SPELL_ACHIEVEMENT                   = 111533,

    // Aspects
    SPELL_EXPOSE_WEAKNESS_NOZDORMU      = 106600,
    SPELL_EXPOSE_WEAKNESS_ALEXTRASZA    = 106588,
    SPELL_EXPOSE_WEAKNESS_YSERA         = 106613,
    SPELL_EXPOSE_WEAKNESS_KALECGOS      = 106624,
    SPELL_TIME_ZONE                     = 105831,
    SPELL_CONTENCTRATION_ALEXTRASZA     = 106641, 
    SPELL_CONTENCTRATION_KALECGOS       = 106644,
    SPELL_CONTENCTRATION_YSERA          = 106643,
    SPELL_CONTENCTRATION_NOZDORMU       = 106642,
    SPELL_CAUTERIZE                     = 105565,

    SPELL_NOZZDORMUS_PRESENCE           = 105823,
    SPELL_ALEXSTRASZA_PRESENCE          = 105825,


    // Jump Pad
    SPELL_CARRYING_WINDS_1              = 106663, // casted by player, from 1 to 2
    SPELL_CARRYING_WINDS_SCRIPT_1       = 106666, // casted by pad on player
    SPELL_CARRYING_WINDS_2              = 106668, // from 2 to 1
    SPELL_CARRYING_WINDS_SCRIPT_2       = 106669,
    SPELL_CARRYING_WINDS_3              = 106670, // from 2 to 3
    SPELL_CARRYING_WINDS_SCRIPT_3       = 106671,
    SPELL_CARRYING_WINDS_4              = 106672, // from 3 to 2
    SPELL_CARRYING_WINDS_SCRIPT_4       = 106673,
    SPELL_CARRYING_WINDS_5              = 106674, // from 3 to 4
    SPELL_CARRYING_WINDS_SCRIPT_5       = 106675,
    SPELL_CARRYING_WINDS_6              = 106676, // from 4 to 3
    SPELL_CARRYING_WINDS_SCRIPT_6       = 106677,

    SPELL_CARRYING_WINDS_DUMMY          = 106678, // visual ?

    SPELL_CARRYING_WINDS_SPEED_25       = 109963,
    SPELL_CARRYING_WINDS_SPEED_10H      = 109962,
    SPELL_CARRYING_WINDS_SPEED_25H      = 109961,
    SPELL_CARRYING_WINDS_SPEED          = 106664,

    SPELL_SUMMON_CHEST                  = 106999, // TODO
};

/*
110649


// 106641 106642 106643 106644 


106886 106860 106888 106889 

107029 

107973 
108840 
108850 
109087 
109589  etc
109616 
*/

enum Adds
{
    NPC_BLISTERING_TENTACLE         = 56188,
    NPC_REGENERATIVE_BLOOD          = 56263,
    NPC_TIME_ZONE                   = 56311, // summoned by 105802
    NPC_TIME_ZONE_TARGET            = 56332, // for 106761

    NPC_COSMETIC_TENTACLE           = 57693,
    NPC_TAIL_TENTACLE               = 56844,
    NPC_DEATHWIND_WING_R            = 57695,
    NPC_DEATHWIND_WING_L            = 57696,
    NPC_DEATHWIND_ARM_R             = 57686,
    NPC_DEATHWIND_ARM_L             = 57694,

    NPC_JUMP_PAD                    = 56699,

    NPC_ELEMENTIUM_BOLT             = 56262,
    NPC_CONGEALING_BLOOD            = 57798,
};

enum MiscData
{
    POINT_ELEMENTIUM_BOLT   = 1,
    POINT_CONGEALING_BLOOD  = 2,
};

const Position MutatedCorruptionPos[4] =
{
    {-12107.4f, 12201.9f, -5.32397f, 5.16617f},
    {-12028.8f, 12265.6f, -6.27147f, 4.13643f}, // arm 1
    {-11993.3f, 12286.3f, -2.58115f, 5.91667f}, // arm 2
    {-12160.9f, 12057.0f, 2.47362f,  0.73303f}
};

const Position jumpPos[] =
{
    {-12026.30f, 12223.31f, -6.05f, 0.0f}, // from 1 to 2
    {-11977.64f, 12268.25f, 1.39f,  0.0f}, // from 2 to 1
    {-12086.50f, 12167.23f, -2.64f, 0.0f}, // from 2 to 3
    {-12051.42f, 12218.58f, -5.93f, 0.0f}, // from 3 to 2
    {-12118.76f, 12079.09f, 2.40f,  0.0f}, // from 3 to 4
    {-12099.18f, 12147.16f, -2.64f, 0.0f}  // from 4 to 3
};

const Position CreatureSpawnPos[14] =
{
    /* NPC_DEATHWING, */            {-11903.9f, 11989.1f, -113.204f, 2.16421f},
    /* NPC_DEATHWIND_ARM_L, */      {-11967.1f, 11958.8f, -49.9822f, 2.16421f},
    /* NPC_DEATHWIND_ARM_R, */      {-11852.1f, 12036.4f, -49.9821f, 2.16421f},
    /* NPC_DEATHWIND_WING_L, */     {-11913.8f, 11926.5f, -60.3749f, 2.16421f},
    /* NPC_DEATHWIND_WING_R, */     {-11842.2f, 11974.8f, -60.3748f, 2.16421f},
    /* NPC_TAIL_TENTACLE, */        {-11857.0f, 11795.6f, -73.9549f, 2.23402f},

    /* NPC_ALEXSTRASZA_DRAGON, */   {-11957.3f, 12338.3f, 38.9364f, 5.06145f},
    /* NPC_NOZDORMU_DRAGON, */      {-12093.8f, 12312.0f, 43.228f,  5.42797f},
    /* NPC_YSERA_DRAGON, */         {-12157.4f, 12212.5f, 36.0152f, 5.75959f},
    /* NPC_KALECGOS_DRAGON, */      {-12224.0f, 12129.0f, 41.9999f, 5.82915f},

    /* NPC_ARM_TENTACLE_1, */       {-12005.8f, 12190.3f, -6.5939f, 2.1293f},
    /* NPC_ARM_TENTACLE_2, */       {-12065.0f, 12127.2f, -3.2946f, 2.33874f},
    /* NPC_WING_TENTACLE_1, */      {-11941.2f, 12248.9f, 12.1499f, 1.98968f},
    /* NPC_WING_TENTACLE_2, */      {-12097.8f, 12067.4f, 13.4888f, 2.21657f},
};

const Position BoltSpawnPos[4] =
{
    // handmade coords :X
    /* ELEMENTIUM BOLT l, */        {-12020.0f, 12210.2f, -6.2946f},    // platform 1
    /* ELEMENTIUM BOLT l c, */      {-11950.0f, 12250.f,  1.59f},       // platform 2
    /* ELEMENTIUM BOLT r c, */      {-12079.2f, 12150.9f, -2.6599f},    // platform 3
    /* ELEMENTIUM BOLT r, */        {-12100.8f, 12072.4f, 2.3888f},     // platform 4
};

const Position BoltTargetPos[4] =
{
    // handmade coords :X
    /* ELEMENTIUM BOLT l, */        {-11960.0f, 12283.2f, -6.2946f},    // platform 1
    /* ELEMENTIUM BOLT l c, */      {-12046.0f, 12234.f,  1.59f},       // platform 2
    /* ELEMENTIUM BOLT r c, */      {-12116.9f, 12170.9f, -2.6599f},    // platform 3
    /* ELEMENTIUM BOLT r, */        {-12140.8f, 12082.4f, 2.3888f},     // platform 4
};

const float _BoltTargetPos[4][2] =
{
    // handmade coords :X
    /* ELEMENTIUM BOLT l, */        {-11960.0f, 12283.2f},              // platform 1
    /* ELEMENTIUM BOLT l c, */      {-12046.0f, 12234.f},               // platform 2
    /* ELEMENTIUM BOLT r c, */      {-12116.9f, 12170.9f},              // platform 3
    /* ELEMENTIUM BOLT r, */        {-12140.8f, 12082.4f},              // platform 4
};

const Position TimeZonePos[4] =
{
    // handmade coords :X 
    {-11958.8f, 12265.4f, 0.1f},
    {-12031.4f, 12221.9f, 0.1f},
    {-12098.6f, 12160.2f, 0.1f},
    {-12119.1f, 12074.2f, 0.1f},  
};

const float _BoltTargetPos2[4][2] =
{
    // handmade coords :X 
    {-11958.8f, 12265.4f},
    {-12031.4f, 12221.9f},
    {-12098.6f, 12160.2f},
    {-12119.1f, 12074.2f},  
};


const Position CongBlood[2] =
{
    // handmade coords :X 
    {-12117.7f, 12151.4f, -2.6f},
    {-12070.7f, 12163.4f, -2.6f},
};

const float _CongBlood[2][2] =
{
    // handmade coords :X 
    {-12117.7f, 12151.4f},
    {-12070.7f, 12163.4f},
};

const Position encounterEndEvent[6] =
{
    {-12063.4f, 12144.0f, -2.700f, 2.6348f}, // NPC_AGGRA pos
    {-12065.2f, 12140.7f, -2.735f, 2.5726f}, // NPC_THRALL_3 pos

    {-12063.2f, 12145.5f, -2.735f, 0.0877f}, // NPC_NOZDORMU_2 pos
    {-12083.3f, 12152.1f, -2.735f, 6.1026f}, // NPC_ALEXSTRASZA_2 pos
    {-12082.3f, 12156.8f, -2.735f, 5.8326f}, // NPC_YSERA_2 pos
    {-12078.3f, 12162.8f, -2.735f, 5.4477f}, // NPC_KALECGOS_2 pos
};

#define tentacleHealthM RAID_MODE(27914900, 83744704, 26128346, 78385040)

bool CheckPlatforms(Creature* creature)
{
    Map* map = creature->GetMap();

    if (map && map->IsDungeon())
    {                        
        Map::PlayerList const& PlLst = map->GetPlayers();

        if (PlLst.isEmpty())
            return false;

        uint8 varA = 0;
        uint8 varB = 0;
        for (Map::PlayerList::const_iterator itr = PlLst.begin(); itr != PlLst.end(); ++itr)
        {
            varB++;
            if (creature->GetDistance2d(itr->getSource()) < 30.0f)
                varA++;
        }
        return varA / varB > 0.8f;
    }
    return false;
}

class boss_deathwing : public CreatureScript
{
public:
    boss_deathwing() : CreatureScript("boss_deathwing") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_deathwingAI(creature);
    }

    struct boss_deathwingAI : public BossAI
    {
        boss_deathwingAI(Creature* creature) : BossAI(creature, DATA_DEATHWING)
        {
            instance = creature->GetInstanceScript();
            me->SetCanFly(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

            me->ApplySpellImmune(0, IMMUNITY_EFFECT,   SPELL_EFFECT_KNOCK_BACK, true);
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
            me->ApplySpellImmune(0, IMMUNITY_STATE,    SPELL_AURA_MOD_CONFUSE, true);

            memset(bAchieve, false, sizeof(bAchieve));
            ActivedPlatform_1 = 0;
            ActivedPlatform_2 = 0;
            ActivedPlatform_3 = 0;
            ActivedPlatform_4 = 0;
            MutatedCorruption = false;
            secondPhase       = false;
            encounterEnd      = false;
            _stage            = 0;
            _stage2           = 0;
        }

        void Reset()
        {
            _Reset();

            me->SetCanFly(true);
            me->SetReactState(REACT_AGGRESSIVE);       

            memset(bAchieve, false, sizeof(bAchieve));
            ActivedPlatform_1 = 0;
            ActivedPlatform_2 = 0;
            ActivedPlatform_3 = 0;
            ActivedPlatform_4 = 0;
            MutatedCorruption = false;
            secondPhase       = false;
            encounterEnd      = false;
            _stage            = 0;
            _stage2           = 0;
        }

        void DespawnCreatures(uint32 entry)
        {
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

            if (creatures.empty())
                return;

            for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                (*iter)->DespawnOrUnsummon();
        }

        void EnterEvadeMode()
        {
            Reset();
            instance->SetBossState(DATA_DEATHWING, NOT_STARTED);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            _EnterEvadeMode();

            DespawnCreatures(NPC_DEATHWIND_ARM_L);
            DespawnCreatures(NPC_DEATHWIND_ARM_R);
            DespawnCreatures(NPC_DEATHWIND_WING_L);
            DespawnCreatures(NPC_DEATHWIND_WING_R);
            DespawnCreatures(NPC_ALEXSTRASZA_DRAGON);
            DespawnCreatures(NPC_NOZDORMU_DRAGON);
            DespawnCreatures(NPC_YSERA_DRAGON);
            DespawnCreatures(NPC_KALECGOS_DRAGON);
            DespawnCreatures(NPC_ARM_TENTACLE_1);
            DespawnCreatures(NPC_ARM_TENTACLE_2);
            DespawnCreatures(NPC_WING_TENTACLE_1);
            DespawnCreatures(NPC_WING_TENTACLE_2);
            DespawnCreatures(NPC_BLISTERING_TENTACLE);
            me->DespawnOrUnsummon(3000);

            ActivedPlatform_1 = 0;
            ActivedPlatform_2 = 0;
            ActivedPlatform_3 = 0;
            ActivedPlatform_4 = 0;
            MutatedCorruption = false;
            secondPhase       = false;
            encounterEnd      = false;
            _stage            = 0;
            _stage2           = 0;
        }

        bool AllowAchieve(uint32 Id)
        {
            return bAchieve[Id];
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();

            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            ActivedPlatform_1 = 0;
            ActivedPlatform_2 = 0;
            ActivedPlatform_3 = 0;
            ActivedPlatform_4 = 0;
            MutatedCorruption = false;
            secondPhase       = false;
            encounterEnd      = false;
            _stage            = 0;
            _stage2           = 0;

            DespawnCreatures(NPC_THE_DRAGON_SOUL_2);

            events.ScheduleEvent(PHASE_1, 3000); 
            events.ScheduleEvent(EVENT_SAY_TRALL_START, 1000);

            for (uint8 i = 0; i < 4; ++i)
                if (Creature* cPlatfrom = me->FindNearestCreature(NPC_PLATFROM, 500.0f))
                    cPlatfrom->AI()->DoAction(ACTION_PLATFROM_CHECK_START);
        }

        void DoAction(const int32 action)
        {
            switch (action)
            {
            case ACTION_PLATFROM_1_COMPLETED:
                {
                    instance->SetData(DATA_PLATFORM_1, DONE);

                    if ((instance->GetData(DATA_PLATFORM_1) == DONE) && (instance->GetData(DATA_PLATFORM_2) != DONE) &&
                        (instance->GetData(DATA_PLATFORM_3) != DONE) && (instance->GetData(DATA_PLATFORM_4) != DONE))
                    {
                        bAchieve[0] = true;
                    }

                    break;
                }
            case ACTION_PLATFROM_2_COMPLETED:
                {
                    instance->SetData(DATA_PLATFORM_2, DONE);

                    if ((instance->GetData(DATA_PLATFORM_1) != DONE) && (instance->GetData(DATA_PLATFORM_2) == DONE) &&
                        (instance->GetData(DATA_PLATFORM_3) != DONE) && (instance->GetData(DATA_PLATFORM_4) != DONE))
                    {
                        bAchieve[1] = true;
                    }
                    break;
                }
            case ACTION_PLATFROM_3_COMPLETED:
                {
                    instance->SetData(DATA_PLATFORM_3, DONE);

                    if ((instance->GetData(DATA_PLATFORM_1) != DONE) && (instance->GetData(DATA_PLATFORM_2) != DONE) &&
                        (instance->GetData(DATA_PLATFORM_3) == DONE) && (instance->GetData(DATA_PLATFORM_4) != DONE))
                    {
                        bAchieve[2] = true;
                    }
                    break;
                }
            case ACTION_PLATFROM_4_COMPLETED:
                {
                    instance->SetData(DATA_PLATFORM_4, DONE);

                    if ((instance->GetData(DATA_PLATFORM_1) != DONE) && (instance->GetData(DATA_PLATFORM_2) != DONE) &&
                        (instance->GetData(DATA_PLATFORM_3) != DONE) && (instance->GetData(DATA_PLATFORM_4) == DONE))
                    {
                        bAchieve[3] = true;
                    }
                    break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if (me->HealthBelowPct(25) && !secondPhase)
            {
                events.ScheduleEvent(HAS_20PROCENT_HEALTH_NEW_PHASE, 150);
                secondPhase = true;
            }

            if (me->HealthBelowPct(3) && !encounterEnd)
            {
                events.ScheduleEvent(EVENT_ENCOUNTER_END, 150);
                encounterEnd = true;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case PHASE_1:
                    if (IsHeroic())
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 60000);

                    events.ScheduleEvent(EVENT_BERSERK, 1200000);
                    events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 3000);
                    events.ScheduleEvent(EVENT_CATACLYSM, 15000);
                    events.ScheduleEvent(EVENT_TIME_ZONE, 3000);
                    events.ScheduleEvent(EVENT_ELEMENTIUM_BOLT, 40000);
                    events.ScheduleEvent(EVENT_SUMMON_REGENERATIVE_BLOOD, 60000);
                    break;

                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK);
                    break;

                case EVENT_SAY_TRALL_START:
                    if (Creature* trall = me->FindNearestCreature(NPC_THRALL_2, 300.0f, true))
                        trall->AI()->Talk(SAY_TRALL_BATTLE_START);
                    break;

                case EVENT_ASSAULT_ASPECTS:
                    {
                        events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 5000);
                        events.ScheduleEvent(EVENT_SUMMON_MUTATED_CORRUPTION, 11000);

                        if (instance->GetData(DATA_PLATFORM_1) == IN_PROGRESS)
                        {
                            ActivedPlatform_2 = true;

                            if (Creature* nozdormu = me->FindNearestCreature(NPC_NOZDORMU_DRAGON, 600.0f))
                                me->CastSpell(nozdormu, SPELL_ASSAULT_ASPECTS);
                        }

                        if (instance->GetData(DATA_PLATFORM_2) == IN_PROGRESS)
                        {
                            ActivedPlatform_3 = true;

                            if (Creature* ysera = me->FindNearestCreature(NPC_YSERA_DRAGON, 600.0f))
                                me->CastSpell(ysera, SPELL_ASSAULT_ASPECTS);
                        }

                        if (instance->GetData(DATA_PLATFORM_3) == IN_PROGRESS)
                        {
                            ActivedPlatform_1 = true;

                            if (Creature* alexstrasza = me->FindNearestCreature(NPC_ALEXSTRASZA_DRAGON, 600.0f))
                                me->CastSpell(alexstrasza, SPELL_ASSAULT_ASPECTS);
                        }

                        if (instance->GetData(DATA_PLATFORM_4) == IN_PROGRESS)
                        {
                            ActivedPlatform_4 = true;

                            if (Creature* kalecgos = me->FindNearestCreature(NPC_KALECGOS_DRAGON, 600.0f))
                                me->CastSpell(kalecgos, SPELL_ASSAULT_ASPECTS);
                        }
                    }
                    break;

                case EVENT_CATACLYSM:
                    {
                        DoCast(SPELL_CATACLYSM);
                        events.RescheduleEvent(EVENT_CATACLYSM, 115000);
                        break;
                    }

                case EVENT_CORRUPTING_PARASITE:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(target, SPELL_CORRUPTING_PARASITE, false);
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 30000);
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE_SUMMON, 10000);
                    }
                    break;

                case EVENT_CORRUPTING_PARASITE_SUMMON:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->SummonCreature(NPC_CORRUPTION_PARASITE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 120000);
                    }
                    break;

                case EVENT_ELEMENTIUM_BOLT:
                    {
                        if (ActivedPlatform_2)
                        {
                            DoCast(SPELL_ELEMENTIUM_BOLT);
                            //me->SummonCreature(NPC_ELEMENTIUM_BOLT, BoltSpawnPos[1]);
                            if (Creature* elemBoltAI = me->FindNearestCreature(NPC_ELEMENTIUM_BOLT, 600.0f))
                            {
                                elemBoltAI->SetSpeed(MOVE_RUN, 3.5f, true);
                                elemBoltAI->SetSpeed(MOVE_WALK, 3.5f, true);
                                elemBoltAI->GetMotionMaster()->MovePoint(POINT_ELEMENTIUM_BOLT, BoltTargetPos[1]);
                            }
                        }
                        if (ActivedPlatform_3)
                        {
                            DoCast(SPELL_ELEMENTIUM_BOLT);
                            //me->SummonCreature(NPC_ELEMENTIUM_BOLT, BoltSpawnPos[2]);
                            if (Creature* elemBoltAI = me->FindNearestCreature(NPC_ELEMENTIUM_BOLT, 600.0f))
                            {
                                elemBoltAI->SetSpeed(MOVE_RUN, 3.5f, true);
                                elemBoltAI->SetSpeed(MOVE_WALK, 3.5f, true);
                                elemBoltAI->GetMotionMaster()->MovePoint(POINT_ELEMENTIUM_BOLT, BoltTargetPos[2]);
                            }
                        }
                        if (ActivedPlatform_1)
                        {
                            DoCast(SPELL_ELEMENTIUM_BOLT);
                            //me->SummonCreature(NPC_ELEMENTIUM_BOLT, BoltSpawnPos[0]);
                            if (Creature* elemBoltAI = me->FindNearestCreature(NPC_ELEMENTIUM_BOLT, 600.0f))
                            {
                                elemBoltAI->SetSpeed(MOVE_RUN, 3.5f, true);
                                elemBoltAI->SetSpeed(MOVE_WALK, 3.5f, true);
                                elemBoltAI->GetMotionMaster()->MovePoint(POINT_ELEMENTIUM_BOLT, BoltTargetPos[0]);
                            }
                        }
                        if (ActivedPlatform_4)
                        {
                            DoCast(SPELL_ELEMENTIUM_BOLT);
                            //me->SummonCreature(NPC_ELEMENTIUM_BOLT, BoltSpawnPos[3]);
                            if (Creature* elemBoltAI = me->FindNearestCreature(NPC_ELEMENTIUM_BOLT, 600.0f))
                            {
                                elemBoltAI->SetSpeed(MOVE_RUN, 3.5f, true);
                                elemBoltAI->SetSpeed(MOVE_WALK, 3.5f, true);
                                elemBoltAI->GetMotionMaster()->MovePoint(POINT_ELEMENTIUM_BOLT, BoltTargetPos[3]);
                            }
                        }
                    }
                    break;

                case EVENT_SUMMON_MUTATED_CORRUPTION:
                    {
                        if (Creature* ARM_TENTACLE_1 = me->FindNearestCreature(NPC_ARM_TENTACLE_1, 600.0f))
                        {
                            if (ARM_TENTACLE_1->GetHealthPct() < 95 && ActivedPlatform_2)
                                if (!MutatedCorruption)
                                {
                                    MutatedCorruption = true;
                                    events.CancelEvent(EVENT_SUMMON_MUTATED_CORRUPTION);
                                    me->SummonCreature(NPC_MUTATED_CORRUPTION, MutatedCorruptionPos[1], TEMPSUMMON_CORPSE_DESPAWN);
                                }
                                else
                                    events.ScheduleEvent(EVENT_SUMMON_MUTATED_CORRUPTION, 5000);  
                        }

                        if (Creature* ARM_TENTACLE_2 = me->FindNearestCreature(NPC_ARM_TENTACLE_2, 600.0f))
                        {
                            if (ARM_TENTACLE_2->GetHealthPct() < 95 && ActivedPlatform_3)
                                if (!MutatedCorruption)
                                {
                                    MutatedCorruption = true;
                                    events.CancelEvent(EVENT_SUMMON_MUTATED_CORRUPTION);
                                    me->SummonCreature(NPC_MUTATED_CORRUPTION, MutatedCorruptionPos[2], TEMPSUMMON_CORPSE_DESPAWN);
                                }
                                else
                                    events.ScheduleEvent(EVENT_SUMMON_MUTATED_CORRUPTION, 5000);  
                        }

                        if (Creature* WING_TENTACLE_1 = me->FindNearestCreature(NPC_WING_TENTACLE_1, 600.0f))
                        {
                            if (WING_TENTACLE_1->GetHealthPct() < 95 && ActivedPlatform_1)
                                if (!MutatedCorruption)
                                {
                                    MutatedCorruption = true;
                                    events.CancelEvent(EVENT_SUMMON_MUTATED_CORRUPTION);
                                    me->SummonCreature(NPC_MUTATED_CORRUPTION, MutatedCorruptionPos[0], TEMPSUMMON_CORPSE_DESPAWN);
                                }
                                else
                                    events.ScheduleEvent(EVENT_SUMMON_MUTATED_CORRUPTION, 5000);  
                        }

                        if (Creature* WING_TENTACLE_2 = me->FindNearestCreature(NPC_WING_TENTACLE_2, 600.0f))
                        {
                            if (WING_TENTACLE_2->GetHealthPct() < 95 && ActivedPlatform_4)
                                if (!MutatedCorruption)
                                {
                                    MutatedCorruption = true;
                                    events.CancelEvent(EVENT_SUMMON_MUTATED_CORRUPTION);
                                    me->SummonCreature(NPC_MUTATED_CORRUPTION, MutatedCorruptionPos[3], TEMPSUMMON_CORPSE_DESPAWN);
                                }
                                else
                                    events.ScheduleEvent(EVENT_SUMMON_MUTATED_CORRUPTION, 5000);  
                        }
                    }
                    break;

                case EVENT_TIME_ZONE:
                    {
                        if (ActivedPlatform_1)
                        {
                            me->SummonCreature(NPC_TIME_ZONE, TimeZonePos[1]);
                        }
                        if (ActivedPlatform_2)
                        {
                            me->SummonCreature(NPC_TIME_ZONE, TimeZonePos[2]);
                        }
                        if (ActivedPlatform_3)
                        {
                            me->SummonCreature(NPC_TIME_ZONE, TimeZonePos[0]);
                        }
                        if (ActivedPlatform_4)
                        {
                            me->SummonCreature(NPC_TIME_ZONE, TimeZonePos[3]);
                        }
                    }
                    break;

                case EVENT_SUMMON_REGENERATIVE_BLOOD:
                    {
                        if (ActivedPlatform_1)
                        { // NPC_REGENERATIVE_BLOOD
                            for (uint8 i = 0; i < 5; ++i)
                                me->SummonCreature(NPC_REGENERATIVE_BLOOD, -12031.4f, 12221.9f, 2.1f);
                            //me->CastSpell(-12031.4f, 12221.9f, 2.1f, 105875 , false);
                        }
                        if (ActivedPlatform_2)
                        {
                            for (uint8 i = 0; i < 5; ++i)
                                me->SummonCreature(NPC_REGENERATIVE_BLOOD, -12098.6f, 12160.2f, 2.1f);
                            //me->CastSpell(-12098.6f, 12160.2f, 2.1f, 105875 , false);
                        }
                        if (ActivedPlatform_3)
                        {
                            for (uint8 i = 0; i < 5; ++i)
                                me->SummonCreature(NPC_REGENERATIVE_BLOOD, -11958.8f, 12265.4f, 2.1f);
                            //me->CastSpell(-11958.8f, 12265.4f, 2.1f,  105875, false);
                        }
                        if (ActivedPlatform_4)
                        {
                            for (uint8 i = 0; i < 5; ++i)
                                me->SummonCreature(NPC_REGENERATIVE_BLOOD, -12119.1f, 12074.2f, 2.1f);
                            //me->CastSpell(-12119.1f, 12074.2f, 2.1f, 105875, false); // SPELL_HEMMORHAGE
                        }
                    }
                    break;

                case HAS_20PROCENT_HEALTH_NEW_PHASE:
                    if (me->GetHealthPct() < 25)
                    {
                        events.CancelEvent(HAS_20PROCENT_HEALTH_NEW_PHASE);
                        events.ScheduleEvent(PHASE_2, 150);
                        if (Creature* trall = me->FindNearestCreature(NPC_THRALL_2, 300.0f, true))
                        {
                            //trall->AI()->Talk(SAY_TRALL_20PROCENT);
                            if (Player* players = trall->FindNearestPlayer(500.0f))
                                trall->SendPlaySound(26600, players);
                        }
                    }
                    events.ScheduleEvent(HAS_20PROCENT_HEALTH_NEW_PHASE, 5000);
                    break;

                case PHASE_2:
                    me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 600);
                    me->SetFloatValue(UNIT_FIELD_COMBATREACH, 600);

                    events.CancelEvent(EVENT_ASSAULT_ASPECTS);
                    events.CancelEvent(EVENT_CORRUPTING_PARASITE);
                    events.CancelEvent(EVENT_ELEMENTIUM_BOLT);
                    events.CancelEvent(EVENT_CATACLYSM);
                    events.CancelEvent(PHASE_1);
                    events.CancelEvent(PHASE_2);
                    events.CancelEvent(EVENT_EXPOSE); 
                    events.CancelEvent(EVENT_SUMMON_REGENERATIVE_BLOOD); 
                    events.CancelEvent(EVENT_TIME_ZONE); 
                    events.CancelEvent(EVENT_SUMMON_MUTATED_CORRUPTION); 

                    if (IsHeroic())
                        events.ScheduleEvent(EVENT_CONGEALING_BLOOD, urand(30000, 60000));

                    events.ScheduleEvent(EVENT_SLUMP, 6000);
                    events.ScheduleEvent(EVENT_ELEMENTIUM_FRAGMENT, 10000);
                    events.ScheduleEvent(EVENT_ELEMENTIUM_TERROR, 10000);
                    events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 6000);
                    break;

                case EVENT_SLUMP:
                    me->AddAura(SPELL_SLUMP, me);
                    events.ScheduleEvent(EVENT_SLUMP, 6000);
                    break;

                case EVENT_CORRUPTED_BLOOD:
                    if (me->GetHealthPct() < 15)
                    {
                        events.CancelEvent(EVENT_CORRUPTED_BLOOD);
                        events.ScheduleEvent(EVENT_ALTER_POWER, 1000);
                        me->AddAura(SPELL_CORRUPTED_BLOOD_BAR, me);
                        me->AddAura(SPELL_CORRUPTED_BLOOD, me);
                    }     
                    events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 5000);
                    break;

                case EVENT_ALTER_POWER:
                    {
                        events.CancelEvent(EVENT_CORRUPTED_BLOOD);
                        uint32 power = me->GetPower(POWER_ALTERNATE_POWER);
                        me->SetPower(POWER_ALTERNATE_POWER, power + 1);
                        events.ScheduleEvent(EVENT_ALTER_POWER, 6000);
                    }
                    break;

                case EVENT_CONGEALING_BLOOD:
                    {
                        if (Creature* MoD = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            if (((MoD->GetHealthPct() < 15) && _stage2 == 0) || ((MoD->GetHealthPct() < 10) && _stage2 == 1) || ((MoD->GetHealthPct() < 5) && _stage2 == 2))
                            { 
                                _stage2 = _stage2 + 1;

                                for (int i = 0; i < 2; i++)
                                    me->SummonCreature(NPC_CONGEALING_BLOOD, CongBlood[0]);
                            }
                        }
                        events.ScheduleEvent(EVENT_CONGEALING_BLOOD, 3000);
                    }
                    break;

                case EVENT_ELEMENTIUM_TERROR:
                    {
                        events.ScheduleEvent(EVENT_ELEMENTIUM_FRAGMENT, 1000);
                        if (Creature* MoD = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            if (((MoD->GetHealthPct() < 15) && _stage2 == 0) || ((MoD->GetHealthPct() < 10) && _stage2 == 1) || ((MoD->GetHealthPct() < 5) && _stage2 == 2))
                            { 
                                _stage2 = _stage2 + 1;

                                for (int i = 0; i < 2; i++)
                                {
                                    me->SummonCreature(NPC_ELEMENTIUM_TERROR, TimeZonePos[2]); // correct coords SPELL_HEMMORHAGE_ELEMENTIUM_TERROR
                                    //me->CastSpell(-12098.6f, 12160.2f, 10.1f, SPELL_HEMMORHAGE_ELEMENTIUM_TERROR, true);
                                }
                            } 
                        }
                    }
                    break;

                case EVENT_ELEMENTIUM_FRAGMENT:
                    {
                        events.ScheduleEvent(EVENT_ELEMENTIUM_FRAGMENT, 1000);
                        if (Creature* MoD = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            if (((MoD->GetHealthPct() < 15) && _stage == 0) || ((MoD->GetHealthPct() < 10) && _stage == 1) || ((MoD->GetHealthPct() < 5) && _stage == 2))
                            { 
                                _stage = _stage + 1;

                                for (int i = 0; i < 4; i++)
                                    me->SummonCreature(NPC_ELEMENTIUM_FRAGMENT, TimeZonePos[2]); // correct coords
                            }
                        }
                    }
                    break;

                case EVENT_ENCOUNTER_END:
                    {
                        DoCastAOE(SPELL_CALM_MAELSTROM_SKYBOX);
                        //instance->DoModifyPlayerCurrencies(CURRENCY_TYPE_VALOR_POINTS, (IsHeroic() ? 140 : 120));
                        //instance->DoModifyPlayerCurrencies(CURRENCY_TYPE_MOTE_OF_DARKNESS, (IsHeroic() ? 2 : 1));
                        //instance->DoModifyPlayerCurrencies(CURRENCY_TYPE_ESS_CORRUPTED_DW, 1);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        //instance->DoStartMovie(76); check it with other game client...
                        me->DespawnOrUnsummon(3000);
                        _JustDied();

                        DespawnCreatures(NPC_DEATHWIND_ARM_L);
                        DespawnCreatures(NPC_DEATHWIND_ARM_R);
                        DespawnCreatures(NPC_DEATHWIND_WING_L);
                        DespawnCreatures(NPC_DEATHWIND_WING_R);
                        DespawnCreatures(NPC_ALEXSTRASZA_DRAGON);
                        DespawnCreatures(NPC_NOZDORMU_DRAGON);
                        DespawnCreatures(NPC_YSERA_DRAGON);
                        DespawnCreatures(NPC_KALECGOS_DRAGON);
                        DespawnCreatures(NPC_ARM_TENTACLE_1);
                        DespawnCreatures(NPC_ARM_TENTACLE_2);
                        DespawnCreatures(NPC_WING_TENTACLE_1);
                        DespawnCreatures(NPC_WING_TENTACLE_2);
                        DespawnCreatures(NPC_THRALL_2);

                        me->SummonCreature(NPC_AGGRA,       encounterEndEvent[0]); // correct coords
                        me->SummonCreature(NPC_THRALL_3,    encounterEndEvent[1]); // correct coords
                        me->SummonCreature(NPC_NOZDORMU_2,  encounterEndEvent[2]); // correct coords
                        me->SummonCreature(NPC_NOZDORMU_2,  encounterEndEvent[3]); // correct coords
                        me->SummonCreature(NPC_YSERA_2,     encounterEndEvent[4]); // correct coords
                        me->SummonCreature(NPC_KALECGOS_2,  encounterEndEvent[5]); // correct coords
                    }
                    break;

                default:
                    break;
                }
            }
        }
    private:
        InstanceScript* instance;
        EventMap events;
        int8 ActivedPlatform_1;
        int8 ActivedPlatform_2;
        int8 ActivedPlatform_3;
        int8 ActivedPlatform_4;
        bool MutatedCorruption;
        bool secondPhase;
        bool encounterEnd;
        int8 _stage;
        int8 _stage2;
        bool bAchieve[4];
    };
};

class npc_platfrom : public CreatureScript
{
public:
    npc_platfrom() : CreatureScript("npc_platfrom") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_platfromAI(creature);
    }

    struct npc_platfromAI : public ScriptedAI
    {
        npc_platfromAI(Creature* creature) : ScriptedAI(creature) { }

        void DoAction(const int32 action)
        {
            if (action == ACTION_PLATFROM_CHECK_START)
            {
                events.ScheduleEvent(EVENT_PLATFROM_CHECK_1, 1000);
                events.ScheduleEvent(EVENT_PLATFROM_STARTING, 500);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            // NOT_STARTED -at encounter start;
            // IN_PROGRESS - if encounter at atacking
            // DONE
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PLATFROM_STARTING:
                    {
                        instance->SetData(DATA_PLATFORM_1, NOT_STARTED);
                        instance->SetData(DATA_PLATFORM_2, NOT_STARTED);
                        instance->SetData(DATA_PLATFORM_3, NOT_STARTED);
                        instance->SetData(DATA_PLATFORM_4, NOT_STARTED);
                    }
                    break;

                case EVENT_PLATFROM_CHECK_1:
                    {
                        if (Creature* ARM_TENTACLE_1 = me->FindNearestCreature(NPC_ARM_TENTACLE_1, 60.0f))
                        {
                            if (CheckPlatforms(ARM_TENTACLE_1))
                                if (((instance->GetData(DATA_PLATFORM_1) == NOT_STARTED) && (instance->GetData(DATA_PLATFORM_2) != IN_PROGRESS) &&
                                    (instance->GetData(DATA_PLATFORM_3) != IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_4) != IN_PROGRESS))
                                    ||
                                    ((instance->GetData(DATA_PLATFORM_1) == NOT_STARTED) && (instance->GetData(DATA_PLATFORM_2) == DONE) &&
                                    (instance->GetData(DATA_PLATFORM_3) == DONE) && (instance->GetData(DATA_PLATFORM_4) == DONE)))
                                {
                                    instance->SetData(DATA_PLATFORM_1, IN_PROGRESS);
                                }
                        }

                        if (Creature* ARM_TENTACLE_2 = me->FindNearestCreature(NPC_ARM_TENTACLE_2, 60.0f))
                        {
                            if (CheckPlatforms(ARM_TENTACLE_2))
                                if (((instance->GetData(DATA_PLATFORM_1) != IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_2) == NOT_STARTED) &&
                                    (instance->GetData(DATA_PLATFORM_3) != IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_4) != IN_PROGRESS))
                                    ||
                                    ((instance->GetData(DATA_PLATFORM_1) == DONE) && (instance->GetData(DATA_PLATFORM_2) == NOT_STARTED) &&
                                    (instance->GetData(DATA_PLATFORM_3) == DONE) && (instance->GetData(DATA_PLATFORM_4) == DONE)))
                                {
                                    instance->SetData(DATA_PLATFORM_2, IN_PROGRESS);
                                }
                        }

                        if (Creature* WING_TENTACLE_1 = me->FindNearestCreature(NPC_WING_TENTACLE_1, 60.0f))
                        {
                            if (CheckPlatforms(WING_TENTACLE_1))
                                if (((instance->GetData(DATA_PLATFORM_1) == IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_2) != IN_PROGRESS) &&
                                    (instance->GetData(DATA_PLATFORM_3) == NOT_STARTED) && (instance->GetData(DATA_PLATFORM_4) != IN_PROGRESS))
                                    ||
                                    ((instance->GetData(DATA_PLATFORM_1) == DONE) && (instance->GetData(DATA_PLATFORM_2) == DONE) &&
                                    (instance->GetData(DATA_PLATFORM_3) == NOT_STARTED) && (instance->GetData(DATA_PLATFORM_4) == DONE)))
                                {
                                    instance->SetData(DATA_PLATFORM_3, IN_PROGRESS);
                                }
                        }

                        if (Creature* WING_TENTACLE_2 = me->FindNearestCreature(NPC_WING_TENTACLE_2, 60.0f))
                        {
                            if (CheckPlatforms(WING_TENTACLE_2))
                                if (((instance->GetData(DATA_PLATFORM_1) != IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_2) != IN_PROGRESS) &&
                                    (instance->GetData(DATA_PLATFORM_3) != IN_PROGRESS) && (instance->GetData(DATA_PLATFORM_4) == NOT_STARTED))
                                    ||
                                    ((instance->GetData(DATA_PLATFORM_1) == DONE) && (instance->GetData(DATA_PLATFORM_2) == DONE) &&
                                    (instance->GetData(DATA_PLATFORM_3) == DONE) && (instance->GetData(DATA_PLATFORM_4) == NOT_STARTED)))
                                {
                                    instance->SetData(DATA_PLATFORM_4, IN_PROGRESS);
                                }
                        }
                        events.ScheduleEvent(EVENT_PLATFROM_CHECK_1, 2000);
                    }
                    break;
                }
            }
        }
    private:
        InstanceScript* instance;
        EventMap events;
    };
};

class npc_elementium_bolt : public CreatureScript
{
public:
    npc_elementium_bolt() : CreatureScript("npc_elementium_bolt") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elementium_boltAI(creature);
    }

    struct npc_elementium_boltAI : public ScriptedAI
    {
        npc_elementium_boltAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_CHECK_BOLT_POSITION, 2000);
            me->AddAura(SPELL_ELEMENTIUM_BLAST_AURA, me);
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
                case EVENT_CHECK_BOLT_POSITION:
                    for (uint8 i = 0; i < 4; i++)
                    {
                        if (me->GetDistance2d(_BoltTargetPos[i][0], _BoltTargetPos[i][1]) <= 1.0f)
                        {
                            me->SetSpeed(MOVE_RUN, 0.5f, true);
                            me->SetSpeed(MOVE_WALK, 0.5f, true);
                        }
                    }
                    for (uint8 i = 0; i < 4; i++)
                    {
                        if (me->GetDistance2d(_BoltTargetPos[i][0], _BoltTargetPos[i][1]) <= 1.0f)
                        {
                            events.CancelEvent(EVENT_CHECK_BOLT_POSITION);
                            me->AddAura(SPELL_ELEMENTIUM_BLAST_AURA_2, me);
                            me->SetSpeed(MOVE_RUN, 0.0f, true);
                            me->SetSpeed(MOVE_WALK, 0.0f, true);
                        }
                    }
                    events.ScheduleEvent(EVENT_CHECK_BOLT_POSITION, 1000);
                    break;
                default:
                    break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }
    private:
        InstanceScript* instance;
        EventMap events;
    };
};

class npc_regenerative_blood : public CreatureScript
{
public:
    npc_regenerative_blood() : CreatureScript("npc_regenerative_blood") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_regenerative_bloodAI(pCreature);
    }

    struct npc_regenerative_bloodAI : public ScriptedAI
    {
        npc_regenerative_bloodAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            ASSERT(me->GetVehicleKit());
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 0);
        }

        void Reset()
        {
            timerPowerTicks  = 5000;
        }

        void JustSummoned(Creature* /*summon*/)
        {
            me->AddAura(SPELL_DEGENERATIVE_BITE, me);
            timerPowerTicks  = 5000;
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (timerPowerTicks <= diff)
            {
                uint32 power = me->GetPower(POWER_ENERGY);
                if (power < 100)
                {
                    me->ModifyPower(POWER_ENERGY, power + 10);
                    timerPowerTicks = 5000;
                }
                else if (power = 100)
                {
                    //me->CastSpell(me, SPELL_REGENERATIVE_BLOOD_SCALE);
                    //me->CastSpell(me, SPELL_REGENERATIVE_BLOOD_HEAL);
                    me->SetHealth(me->GetMaxHealth());
                    me->SetPower(POWER_ENERGY, 0);
                }
            }
            else timerPowerTicks -= diff;

            DoMeleeAttackIfReady();
        }
    private:
        uint32 timerPowerTicks;
    };
};

class npc_mutated_corruption : public CreatureScript
{
public:
    npc_mutated_corruption() : CreatureScript("npc_mutated_corruption") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_mutated_corruptionAI(pCreature);
    }

    struct npc_mutated_corruptionAI : public ScriptedAI
    {
        npc_mutated_corruptionAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_CRUSH, 5000);
            events.ScheduleEvent(EVENT_IMPALE, 10000);
        }

        void Reset()
        {
            me->RemoveAllAuras();
            events.Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CRUSH:
                    {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 7.0f, true))
                            me->CastSpell(pTarget, SPELL_CRASH, true);
                        else
                            events.ScheduleEvent(EVENT_IMPALE_ASPECT, 5000);
                        events.ScheduleEvent(EVENT_CRUSH, 5000);
                        break;
                    }
                case EVENT_IMPALE:
                    {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 7.0f, true))
                            me->CastSpell(pTarget, SPELL_IMPALE, true);
                        else
                            events.ScheduleEvent(EVENT_IMPALE_ASPECT, 5000);
                        events.ScheduleEvent(EVENT_IMPALE, 34000);
                        break;
                    }
                case EVENT_IMPALE_ASPECT:
                    {
                        if (Creature* target = me->FindNearestCreature(NPC_ALEXSTRASZA_DRAGON, 50))
                            me->CastSpell(target, SPELL_IMPALE_ASPECT, true);

                        if (Creature* target = me->FindNearestCreature(NPC_NOZDORMU_DRAGON, 50))
                            me->CastSpell(target, SPELL_IMPALE_ASPECT, true);

                        if (Creature* target = me->FindNearestCreature(NPC_YSERA_DRAGON, 50))
                            me->CastSpell(target, SPELL_IMPALE_ASPECT, true);

                        if (Creature* target = me->FindNearestCreature(NPC_KALECGOS_DRAGON, 50))
                            me->CastSpell(target, SPELL_IMPALE_ASPECT, true);

                        break;
                    }
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        InstanceScript* instance;
        uint32 aspTarget;
    };
};

class npc_arm_tentacle_one : public CreatureScript
{
public:
    npc_arm_tentacle_one() : CreatureScript("npc_arm_tentacle_one") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arm_tentacle_oneAI(creature);
    }

    struct npc_arm_tentacle_oneAI : public ScriptedAI
    {
        npc_arm_tentacle_oneAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            stage = 0;
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            uint8 HealthPct = me->GetHealthPct();
            uint8 MaxPct    = 100;
            uint8 Amount    = (MaxPct - HealthPct);

            if (me->HasAura(SPELL_BURNING_BLODD))
                me->GetAura(SPELL_BURNING_BLODD)->SetStackAmount(Amount);
        }

        void Reset()
        {
            events.Reset();

            me->SetMaxHealth(tentacleHealthM);
            me->SetHealth(tentacleHealthM);

            stage = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->AddAura(SPELL_BURNING_BLODD, me);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if ((me->HealthBelowPct(75) && stage == 0) || (me->HealthBelowPct(50) && stage == 1) || (me->HealthBelowPct(25) && stage == 2))
            {
                stage = stage + 1;
                events.ScheduleEvent(EVENT_BLISTERING_TENTACLE, 5000);
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BLISTERING_TENTACLE:
                    {
                        int8 count;
                        switch (GetDifficulty())
                        {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            count = 4;
                            break;
                        case RAID_DIFFICULTY_10MAN_HEROIC:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                            count = 8;
                            break;
                        default:
                            count = 4;
                            break;
                        }

                        for (int8 i = 0; i < count; i++)
                        {
                            if (Creature* blistTent = me->SummonCreature(NPC_BLISTERING_TENTACLE, -12005.8f, 12190.3f, -6.5939f))
                            {
                                blistTent->EnterVehicle(me, i);
                                blistTent->ClearUnitState(UNIT_STATE_ONVEHICLE);
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);

            if (Creature* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f))
            {
                me->AddAura(SPELL_AGONIZING_PAIN, DeathwingTR);
                DeathwingTR->AI()->DoAction(ACTION_PLATFROM_1_COMPLETED);
            }

            Unit* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f);
            if (DeathwingTR->GetHealthPct() >= 99)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(80));
            else if (DeathwingTR->GetHealthPct() >= 79)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(60));
            else if (DeathwingTR->GetHealthPct() >= 59)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(40));
            else if (DeathwingTR->GetHealthPct() >= 39)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(20));
        }
    private:
        InstanceScript* instance;
        EventMap events;
        uint8 stage;
    };
};

class npc_arm_tentacle_two : public CreatureScript
{
public:
    npc_arm_tentacle_two() : CreatureScript("npc_arm_tentacle_two") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arm_tentacle_twoAI(creature);
    }

    struct npc_arm_tentacle_twoAI : public ScriptedAI
    {
        npc_arm_tentacle_twoAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            uint8 HealthPct = me->GetHealthPct();
            uint8 MaxPct    = 100;
            uint8 Amount    = (MaxPct - HealthPct);

            if (me->HasAura(SPELL_BURNING_BLODD))
                me->GetAura(SPELL_BURNING_BLODD)->SetStackAmount(Amount);
        }

        void Reset()
        {
            events.Reset();

            me->SetMaxHealth(tentacleHealthM);
            me->SetHealth(tentacleHealthM);

            stage = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->AddAura(SPELL_BURNING_BLODD, me);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if ((me->HealthBelowPct(75) && stage == 0) || (me->HealthBelowPct(50) && stage == 1) || (me->HealthBelowPct(25) && stage == 2))
            {
                stage = stage + 1;
                events.ScheduleEvent(EVENT_BLISTERING_TENTACLE, 5000);
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BLISTERING_TENTACLE:
                    {
                        int8 count;
                        switch (GetDifficulty())
                        {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            count = 4;
                            break;
                        case RAID_DIFFICULTY_10MAN_HEROIC:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                            count = 8;
                            break;
                        default:
                            count = 4;
                            break;
                        }

                        for (int8 i = 0; i < count; i++)
                        {
                            if (Creature* blistTent = me->SummonCreature(NPC_BLISTERING_TENTACLE, -12065.0f, 12127.2f, -3.2946f))
                            {
                                blistTent->EnterVehicle(me, i);
                                blistTent->ClearUnitState(UNIT_STATE_ONVEHICLE);
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);

            if (Creature* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f))
            {
                me->AddAura(SPELL_AGONIZING_PAIN, DeathwingTR);
                DeathwingTR->AI()->DoAction(ACTION_PLATFROM_2_COMPLETED);
            }

            Unit* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f);
            if (DeathwingTR->GetHealthPct() >= 99)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(80));
            else if (DeathwingTR->GetHealthPct() >= 79)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(60));
            else if (DeathwingTR->GetHealthPct() >= 59)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(40));
            else if (DeathwingTR->GetHealthPct() >= 39)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(20));
        }
    private:
        InstanceScript* instance;
        EventMap events;
        uint8 stage;
    };
};

class npc_wing_tentacle_one : public CreatureScript
{
public:
    npc_wing_tentacle_one() : CreatureScript("npc_wing_tentacle_one") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_wing_tentacle_oneAI(pCreature);
    }

    struct npc_wing_tentacle_oneAI : public ScriptedAI
    {
        npc_wing_tentacle_oneAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            uint8 HealthPct = me->GetHealthPct();
            uint8 MaxPct    = 100;
            uint8 Amount    = (MaxPct - HealthPct);

            if (me->HasAura(SPELL_BURNING_BLODD))
                me->GetAura(SPELL_BURNING_BLODD)->SetStackAmount(Amount);
        }

        void Reset()
        {
            events.Reset();

            me->SetMaxHealth(tentacleHealthM);
            me->SetHealth(tentacleHealthM);

            stage = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->AddAura(SPELL_BURNING_BLODD, me);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if ((me->HealthBelowPct(75) && stage == 0) || (me->HealthBelowPct(50) && stage == 1) || (me->HealthBelowPct(25) && stage == 2))
            {
                stage = stage + 1;
                events.ScheduleEvent(EVENT_BLISTERING_TENTACLE, 5000);
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BLISTERING_TENTACLE:
                    {
                        int8 count;
                        switch (GetDifficulty())
                        {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            count = 4;
                            break;
                        case RAID_DIFFICULTY_10MAN_HEROIC:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                            count = 8;
                            break;
                        default:
                            count = 4;
                            break;
                        }

                        for (int8 i = 0; i < count; i++)
                        {
                            if (Creature* blistTent = me->SummonCreature(NPC_BLISTERING_TENTACLE, -11941.2f, 12248.9f, 12.1499f))
                            {
                                blistTent->EnterVehicle(me, i);
                                blistTent->ClearUnitState(UNIT_STATE_ONVEHICLE);
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);

            if (Creature* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f))
            {
                me->AddAura(SPELL_AGONIZING_PAIN, DeathwingTR);
                DeathwingTR->AI()->DoAction(ACTION_PLATFROM_3_COMPLETED);
            }

            Unit* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f);
            if (DeathwingTR->GetHealthPct() >= 99)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(80));
            else if (DeathwingTR->GetHealthPct() >= 79)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(60));
            else if (DeathwingTR->GetHealthPct() >= 59)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(40));
            else if (DeathwingTR->GetHealthPct() >= 39)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(20));
        }
    private:
        InstanceScript* instance;
        EventMap events;
        uint8 stage;
    };
};

class npc_wing_tentacle_two : public CreatureScript
{
public:
    npc_wing_tentacle_two() : CreatureScript("npc_wing_tentacle_two") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_wing_tentacle_twoAI(pCreature);
    }

    struct npc_wing_tentacle_twoAI : public ScriptedAI
    {
        npc_wing_tentacle_twoAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            uint8 HealthPct = me->GetHealthPct();
            uint8 MaxPct    = 100;
            uint8 Amount    = (MaxPct - HealthPct);

            if (me->HasAura(SPELL_BURNING_BLODD))
                me->GetAura(SPELL_BURNING_BLODD)->SetStackAmount(Amount);
        }

        void Reset()
        {
            events.Reset();

            me->SetMaxHealth(tentacleHealthM);
            me->SetHealth(tentacleHealthM);

            stage = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->AddAura(SPELL_BURNING_BLODD, me);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if ((me->HealthBelowPct(75) && stage == 0) || (me->HealthBelowPct(50) && stage == 1) || (me->HealthBelowPct(25) && stage == 2))
            {
                stage = stage + 1;
                events.ScheduleEvent(EVENT_BLISTERING_TENTACLE, 5000);
                return;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BLISTERING_TENTACLE:
                    {
                        int8 count;
                        switch (GetDifficulty())
                        {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            count = 4;
                            break;
                        case RAID_DIFFICULTY_10MAN_HEROIC:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            count = 6;
                            break;
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                            count = 8;
                            break;
                        default:
                            count = 4;
                            break;
                        }

                        for (int8 i = 0; i < count; i++)
                        {
                            if (Creature* blistTent = me->SummonCreature(NPC_BLISTERING_TENTACLE, -12097.8f, 12067.4f, 13.4888f))
                            {
                                blistTent->EnterVehicle(me, i);
                                blistTent->ClearUnitState(UNIT_STATE_ONVEHICLE);
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);

            if (Creature* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f))
            {
                me->AddAura(SPELL_AGONIZING_PAIN, DeathwingTR);
                DeathwingTR->AI()->DoAction(ACTION_PLATFROM_4_COMPLETED);
            }

            Unit* DeathwingTR = me->FindNearestCreature(NPC_DEATHWING, 600.0f);
            if (DeathwingTR->GetHealthPct() >= 99)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(80));
            else if (DeathwingTR->GetHealthPct() >= 79)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(60));
            else if (DeathwingTR->GetHealthPct() >= 59)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(40));
            else if (DeathwingTR->GetHealthPct() >= 39)
                DeathwingTR->SetHealth(DeathwingTR->CountPctFromMaxHealth(20));
        }
    private:
        InstanceScript* instance;
        EventMap events;
        uint8 stage;
    };
};

class npc_congealing_blood : public CreatureScript
{
public:
    npc_congealing_blood() : CreatureScript("npc_congealing_blood") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_congealing_bloodAI(pCreature);
    }

    struct npc_congealing_bloodAI : public ScriptedAI
    {
        npc_congealing_bloodAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetSpeed(MOVE_RUN, 0.5f, true);
            me->SetSpeed(MOVE_WALK, 0.5f, true);
        }

        void Reset()
        {
            events.Reset();

            me->AttackStop();
            me->SetSpeed(MOVE_RUN, 0.5f, true);
            me->SetSpeed(MOVE_WALK, 0.5f, true);
            me->GetMotionMaster()->MovePoint(POINT_CONGEALING_BLOOD, CongBlood[1]);

            events.ScheduleEvent(EVENT_CONGEALING_BLOOD_CAST, 1000);
        }

        void JustSummoned(Creature* /*summon*/)
        {
            me->AttackStop();
            me->SetSpeed(MOVE_RUN, 0.5f, true);
            me->SetSpeed(MOVE_WALK, 0.5f, true);
            me->GetMotionMaster()->MovePoint(POINT_CONGEALING_BLOOD, CongBlood[1]);

            events.ScheduleEvent(EVENT_CONGEALING_BLOOD_CAST, 1000);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(1000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CONGEALING_BLOOD_CAST:
                    {
                        if (me->GetDistance2d(-12070.7f, 12163.4f) < 1.0f)
                        {
                            if (Creature* MoD = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                                me->CastSpell(MoD, SPELL_CONGEALING_BLOOD_HEAL, true);
                        }
                        events.ScheduleEvent(EVENT_CONGEALING_BLOOD_CAST, 1000);   
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_elementium_fragment : public CreatureScript
{
public:
    npc_elementium_fragment() : CreatureScript("npc_elementium_fragment") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_elementium_fragmentAI(pCreature);
    }

    struct npc_elementium_fragmentAI : public ScriptedAI
    {
        npc_elementium_fragmentAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetSpeed(MOVE_RUN, 0.0f, true);
            me->SetSpeed(MOVE_WALK, 0.0f, true);
        }

        void Reset()
        {
            events.Reset();

            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetSpeed(MOVE_RUN, 0.0f, true);
            me->SetSpeed(MOVE_WALK, 0.0f, true);
        }

        void JustSummoned(Creature* /*summon*/)
        {
            events.ScheduleEvent(EVENT_SHRAPNEL, 1000);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(1000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHRAPNEL:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            me->CastSpell(target, SPELL_SHRAPNEL_TARGET, false);
                            me->CastSpell(target, SPELL_SHRAPNEL, false);    
                        }
                        events.ScheduleEvent(EVENT_SHRAPNEL, 8000);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_elementium_terror : public CreatureScript
{
public:
    npc_elementium_terror() : CreatureScript("npc_elementium_terror") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_elementium_terrorAI(pCreature);
    }

    struct npc_elementium_terrorAI : public ScriptedAI
    {
        npc_elementium_terrorAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset()
        {
            events.Reset();
        }

        void JustSummoned(Creature* /*summon*/)
        {
            events.ScheduleEvent(EVENT_TETANUS, 2000);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(1000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TETANUS:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            me->CastSpell(target, SPELL_TETANUS, false);
                            events.ScheduleEvent(EVENT_TETANUS, 2000);    
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            DoSpellAttackIfReady(SPELL_TETANUS);
        }
    private:
        EventMap events;
    };
};

class npc_maelstrom_trall : public CreatureScript
{
public:
    npc_maelstrom_trall() : CreatureScript("npc_maelstrom_trall") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

        if (InstanceScript* instance = creature->GetInstanceScript())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Мы готовы!" : "We are ready!", GOSSIP_SENDER_MAIN, ACTION_THRALL_MENU);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 uiAction)
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (uiAction)
            {
            case ACTION_THRALL_MENU:
                if (InstanceScript* instance = creature->GetInstanceScript())
                {
                    player->CLOSE_GOSSIP_MENU();
                    creature->SummonCreature(NPC_DEATHWING,         CreatureSpawnPos[0]);
                    creature->SummonCreature(NPC_DEATHWIND_ARM_L,   CreatureSpawnPos[1]);
                    creature->SummonCreature(NPC_DEATHWIND_ARM_R,   CreatureSpawnPos[2]);
                    creature->SummonCreature(NPC_DEATHWIND_WING_L,  CreatureSpawnPos[3]);
                    creature->SummonCreature(NPC_DEATHWIND_WING_R,  CreatureSpawnPos[4]);
                    //me->SummonCreature(NPC_TAIL_TENTACLE,           CreatureSpawnPos[5]);
                    creature->SummonCreature(NPC_ALEXSTRASZA_DRAGON,CreatureSpawnPos[6]);
                    creature->SummonCreature(NPC_NOZDORMU_DRAGON,   CreatureSpawnPos[7]);
                    creature->SummonCreature(NPC_YSERA_DRAGON,      CreatureSpawnPos[8]);
                    creature->SummonCreature(NPC_KALECGOS_DRAGON,   CreatureSpawnPos[9]);
                    creature->SummonCreature(NPC_ARM_TENTACLE_1,    CreatureSpawnPos[10]);
                    creature->SummonCreature(NPC_ARM_TENTACLE_2,    CreatureSpawnPos[11]);
                    creature->SummonCreature(NPC_WING_TENTACLE_1,   CreatureSpawnPos[12]);
                    creature->SummonCreature(NPC_WING_TENTACLE_2,   CreatureSpawnPos[13]);

                    if (Creature* deathwing = creature->FindNearestCreature(NPC_DEATHWING, 400.0f))
                    {
                        deathwing->AI()->AttackStart(player);
                        deathwing->SetInCombatWithZone();
                    }

                    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    creature->CastSpell(creature, SPELL_ASTRAL_RECALL);
                }
                break;
            default:
                break;
            }
        }
        return true;
    }

    struct npc_maelstrom_trallAI : public ScriptedAI
    {
        npc_maelstrom_trallAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            if (Creature* MoD = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
            {
                if (!MoD->isInCombat())
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    DoCast(SPELL_ASTRAL_RECALL_BACK);
                }
            }

        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_maelstrom_trallAI(creature);
    }
};

class npc_corruption_parasite : public CreatureScript
{
public:
    npc_corruption_parasite() : CreatureScript("npc_corruption_parasite") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_corruption_parasiteAI(pCreature);
    }

    struct npc_corruption_parasiteAI : public ScriptedAI
    {
        npc_corruption_parasiteAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        }

        void Reset()
        {
            events.Reset();

            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetSpeed(MOVE_RUN, 0.0f, true);
            me->SetSpeed(MOVE_WALK, 0.0f, true);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(1000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                events.ScheduleEvent(EVENT_UNSTABLE_CORRUPTION, 1000);

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_UNSTABLE_CORRUPTION:
                    me->CastSpell(me, SPELL_UNSTABLE_CORRUPTION, false);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_maelstrom_alexstrasza : public CreatureScript
{
public:
    npc_maelstrom_alexstrasza() : CreatureScript("npc_maelstrom_alexstrasza") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_maelstrom_alexstraszaAI(pCreature);
    }

    struct npc_maelstrom_alexstraszaAI : public ScriptedAI
    {
        npc_maelstrom_alexstraszaAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void EnterCombat(Unit* /*who*/)
        { }

        void Reset()
        {
            me->RemoveAllAuras();
            events.Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
            {
                if (DeathWing->isInCombat())
                    events.ScheduleEvent(EVENT_ALEXSTRASZA_EXPOSE_WEAKNESS, 5000);
            }

            if (Creature* wingTent_1 = me->FindNearestCreature(NPC_WING_TENTACLE_1, 400.0f))
            {
                if (!wingTent_1->isAlive())
                    events.ScheduleEvent(EVENT_ALEXSTRASZA_STAGE_1, 10000);
            }

            if (Creature* blistTent = me->FindNearestCreature(NPC_BLISTERING_TENTACLE, 400.0f))
            {
                events.ScheduleEvent(EVENT_CAUTERIZE, 1000);
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ALEXSTRASZA_EXPOSE_WEAKNESS:
                    {
                        events.ScheduleEvent(EVENT_ALEXSTRASZA_EXPOSE_WEAKNESS, 3000);
                        if (Creature* WING_TENTACLE_1 = me->FindNearestCreature(NPC_WING_TENTACLE_1, 200.0f))
                        {
                            if (WING_TENTACLE_1->GetHealthPct() < 50)
                            {
                                me->AddAura(SPELL_EXPOSE_WEAKNESS_ALEXTRASZA, WING_TENTACLE_1);
                                me->CastSpell(WING_TENTACLE_1, SPELL_EXPOSE_WEAKNESS_ALEXTRASZA);
                            }
                        } 
                    }
                    break;
                case EVENT_ALEXSTRASZA_STAGE_1:
                    {
                        if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            me->AddAura(SPELL_CONTENCTRATION_ALEXTRASZA, me);
                            me->CastSpell(DeathWing, SPELL_CONTENCTRATION_ALEXTRASZA);
                        } 
                    }
                    break;

                case EVENT_CAUTERIZE:
                    {
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, NPC_BLISTERING_TENTACLE, 400.0f);

                        if (creatures.empty())
                            return;

                        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                        {
                            me->CastSpell(*iter, SPELL_CAUTERIZE);
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_maelstrom_kalecgos : public CreatureScript
{
public:
    npc_maelstrom_kalecgos() : CreatureScript("npc_maelstrom_kalecgos") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_maelstrom_kalecgosAI(pCreature);
    }

    struct npc_maelstrom_kalecgosAI : public ScriptedAI
    {
        npc_maelstrom_kalecgosAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void EnterCombat(Unit* /*who*/)
        { }

        void Reset()
        {
            me->RemoveAllAuras();
            events.Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
            {
                if (DeathWing->isInCombat())
                    events.ScheduleEvent(EVENT_KALECGOS_EXPOSE_WEAKNESS, 5000);
            }

            if (Creature* wingTent_2 = me->FindNearestCreature(NPC_WING_TENTACLE_2, 400.0f))
            {
                if (!wingTent_2->isAlive())
                    events.ScheduleEvent(EVENT_KALECGOS_STAGE_1, 10000);
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_KALECGOS_EXPOSE_WEAKNESS:
                    {
                        events.ScheduleEvent(EVENT_KALECGOS_EXPOSE_WEAKNESS, 3000);
                        if (Creature* WING_TENTACLE_2 = me->FindNearestCreature(NPC_WING_TENTACLE_2, 200.0f))
                        {
                            if (WING_TENTACLE_2->GetHealthPct() < 50)
                            {
                                me->AddAura(SPELL_EXPOSE_WEAKNESS_KALECGOS, WING_TENTACLE_2);
                                me->CastSpell(WING_TENTACLE_2, SPELL_EXPOSE_WEAKNESS_KALECGOS);
                            }
                        } 
                    }
                    break;

                case EVENT_KALECGOS_STAGE_1:
                    {
                        if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            me->AddAura(SPELL_CONTENCTRATION_KALECGOS, me);
                            me->CastSpell(DeathWing, SPELL_CONTENCTRATION_KALECGOS);
                        } 
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_maelstrom_ysera : public CreatureScript
{
public:
    npc_maelstrom_ysera() : CreatureScript("npc_maelstrom_ysera") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_maelstrom_yseraAI(pCreature);
    }

    struct npc_maelstrom_yseraAI : public ScriptedAI
    {
        npc_maelstrom_yseraAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void EnterCombat(Unit* /*who*/)
        { }

        void Reset()
        {
            me->RemoveAllAuras();
            events.Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
            {
                if (DeathWing->isInCombat())
                    events.ScheduleEvent(EVENT_YSERA_EXPOSE_WEAKNESS, 5000);
            }

            if (Creature* armTent_2 = me->FindNearestCreature(NPC_ARM_TENTACLE_2, 400.0f))
            {
                if (!armTent_2->isAlive())
                    events.ScheduleEvent(EVENT_YSERA_STAGE_1, 10000);
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_YSERA_EXPOSE_WEAKNESS:
                    {
                        if (Creature* ARM_TENTACLE_2 = me->FindNearestCreature(NPC_ARM_TENTACLE_2, 200.0f))
                        {
                            if (ARM_TENTACLE_2->GetHealthPct() < 50)
                            {
                                me->AddAura(SPELL_EXPOSE_WEAKNESS_YSERA, ARM_TENTACLE_2);
                                me->CastSpell(ARM_TENTACLE_2, SPELL_EXPOSE_WEAKNESS_YSERA);
                            }
                        } 
                    }
                    break;

                case EVENT_YSERA_STAGE_1:
                    {
                        if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            me->AddAura(SPELL_CONTENCTRATION_YSERA, me);
                            me->CastSpell(DeathWing, SPELL_CONTENCTRATION_YSERA);
                        } 
                    }
                    break;

                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_maelstrom_nozdormy : public CreatureScript
{
public:
    npc_maelstrom_nozdormy() : CreatureScript("npc_maelstrom_nozdormy") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_maelstrom_nozdormyAI(pCreature);
    }

    struct npc_maelstrom_nozdormyAI : public ScriptedAI
    {
        npc_maelstrom_nozdormyAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void EnterCombat(Unit* /*who*/)
        { }

        void Reset()
        {
            me->RemoveAllAuras();
            events.Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
            {
                if (DeathWing->isInCombat())
                    events.ScheduleEvent(EVENT_NOZDORMY_EXPOSE_WEAKNESS, 5000);
            }

            if (Creature* armTent_1 = me->FindNearestCreature(NPC_ARM_TENTACLE_1, 400.0f))
            {
                if (!armTent_1->isAlive())
                    events.ScheduleEvent(EVENT_NOZDORMY_STAGE_1, 10000);
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_NOZDORMY_EXPOSE_WEAKNESS:
                    {
                        events.ScheduleEvent(EVENT_NOZDORMY_EXPOSE_WEAKNESS, 3000);
                        if (Creature* ARM_TENTACLE_1 = me->FindNearestCreature(NPC_ARM_TENTACLE_1, 200.0f))
                        {
                            if (ARM_TENTACLE_1->GetHealthPct() < 50)
                            {
                                me->AddAura(SPELL_EXPOSE_WEAKNESS_NOZDORMU, ARM_TENTACLE_1);
                                me->CastSpell(ARM_TENTACLE_1, SPELL_EXPOSE_WEAKNESS_NOZDORMU);
                            }
                        } 
                    }
                    break;

                case EVENT_NOZDORMY_STAGE_1:
                    {
                        if (Creature* DeathWing = me->FindNearestCreature(NPC_DEATHWING, 400.0f))
                        {
                            me->AddAura(SPELL_CONTENCTRATION_NOZDORMU, me);
                            me->CastSpell(DeathWing, SPELL_CONTENCTRATION_NOZDORMU);
                        } 
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

class npc_time_zone : public CreatureScript
{
public:
    npc_time_zone() : CreatureScript("npc_time_zone") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_time_zoneAI(pCreature);
    }

    struct npc_time_zoneAI : public ScriptedAI
    {
        npc_time_zoneAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->AddAura(SPELL_TIME_ZONE, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasAura(SPELL_TIME_ZONE))
                me->AddAura(SPELL_TIME_ZONE, me);
        }
    };
};

class npc_blistering_tentacle : public CreatureScript
{
public:
    npc_blistering_tentacle() : CreatureScript("npc_blistering_tentacle") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_blistering_tentacleAI(pCreature);
    }

    struct npc_blistering_tentacleAI : public ScriptedAI
    {
        npc_blistering_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 50);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 50);
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->AddAura(SPELL_BLISTERING_HEAT, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasAura(SPELL_BLISTERING_HEAT))
                me->AddAura(SPELL_BLISTERING_HEAT, me);
        }
    };
};

class npc_madness_of_deathwing_jump_pad : public CreatureScript
{
public:
    npc_madness_of_deathwing_jump_pad() : CreatureScript("npc_madness_of_deathwing_jump_pad") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_madness_of_deathwing_jump_padAI(pCreature);
    }

    struct npc_madness_of_deathwing_jump_padAI : public Scripted_NoMovementAI
    {
        npc_madness_of_deathwing_jump_padAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
        {
            me->SetReactState(REACT_PASSIVE);
            checkTimer = 250;
            pos = 0;
            spellId = 0;
        }

        void Reset()
        {
            if (me->GetPositionY() >= 12270.0f && me->GetPositionY() <= 12275.0f)
            {
                pos = 1;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_1;
            } else if (me->GetPositionY() >= 12225.0f && me->GetPositionY() <= 12230.0f)
            {
                pos = 2;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_2;
            } else if (me->GetPositionY() >= 12210.0f && me->GetPositionY() <= 12215.0f)
            {
                pos = 3;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_3;
            } else if (me->GetPositionY() >= 12162.0f && me->GetPositionY() <= 12167.0f)
            {
                pos = 4;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_4;
            } else if (me->GetPositionY() >= 12150.0f && me->GetPositionY() <= 12155.0f)
            {
                pos = 5;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_5;
            } else if (me->GetPositionY() >= 12080.9f && me->GetPositionY() <= 12085.0f)
            {
                pos = 6;
                spellId = SPELL_CARRYING_WINDS_SCRIPT_6;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!spellId)
                return;

            if (checkTimer <= diff)
            {
                std::list<Player*> players;
                PlayerCheck check(me);
                Trinity::PlayerListSearcher<PlayerCheck> searcher(me, players, check);
                me->VisitNearbyObject(32.0f, searcher);
                if (!players.empty())
                    for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        DoCast((*itr), spellId, true);
                        (*itr)->AddAura(SPELL_CARRYING_WINDS_SPEED, (*itr));
                    }

                    checkTimer = 250;
            } else
                checkTimer -= diff;
        }

    private:
        uint32 checkTimer;
        uint8 pos;
        uint32 spellId;

        class PlayerCheck
        {
        public:
            PlayerCheck(WorldObject const* obj) : _obj(obj) { }
            bool operator()(Player* u)
            {
                if (!u->isAlive())
                    return false;

                if (!u->IsFalling() || _obj->GetPositionZ() < ( u->GetPositionZ() + 1.0f ))
                    return false;

                if (!_obj->IsWithinDistInMap(u, 32.0f))
                    return false;

                return true;
            }
        private:
            WorldObject const* _obj;
        };
    };
};

class spell_madness_of_deathwing_carrying_winds_script : public SpellScriptLoader
{
public:
    spell_madness_of_deathwing_carrying_winds_script(const char* name, uint8 pos) : SpellScriptLoader(name), _pos(pos) { }

    class spell_madness_of_deathwing_carrying_winds_script_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_madness_of_deathwing_carrying_winds_script_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            uint32 spellId = SPELL_CARRYING_WINDS_1;
            switch (_pos)
            {
            case 1:
                spellId = SPELL_CARRYING_WINDS_1;
                break;
            case 2:
                spellId = SPELL_CARRYING_WINDS_2;
                break;
            case 3:
                spellId = SPELL_CARRYING_WINDS_3;
                break;
            case 4:
                spellId = SPELL_CARRYING_WINDS_4;
                break;
            case 5:
                spellId = SPELL_CARRYING_WINDS_5;
                break;
            case 6:
                spellId = SPELL_CARRYING_WINDS_6;
                break;
            default:
                break;
            }
            GetHitUnit()->CastSpell(jumpPos[_pos - 1].GetPositionX(), jumpPos[_pos - 1].GetPositionY(), jumpPos[_pos - 1].GetPositionZ(), spellId, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_carrying_winds_script_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }

    public:
        spell_madness_of_deathwing_carrying_winds_script_SpellScript(uint8 pos) : SpellScript(), _pos(pos) { }

    private:
        uint8 _pos;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_madness_of_deathwing_carrying_winds_script_SpellScript(_pos);
    }

private:
    uint8 _pos;
};

class spell_agonizing_pain_dmg : public SpellScriptLoader
{
public:
    spell_agonizing_pain_dmg() : SpellScriptLoader("spell_agonizing_pain_dmg") { }

    class spell_agonizing_pain_dmg_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_agonizing_pain_dmg_SpellScript);

        void RecalculateDamage()
        {
            SetHitDamage(int32(GetHitUnit()->CountPctFromMaxHealth(20)));
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_agonizing_pain_dmg_SpellScript::RecalculateDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_agonizing_pain_dmg_SpellScript();
    }
};

class spell_burning_blood_trigger : public SpellScriptLoader
{ 
public:
    spell_burning_blood_trigger() : SpellScriptLoader("spell_burning_blood_trigger") { }

    class spell_burning_blood_trigger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_burning_blood_trigger_SpellScript);

        void RecalculateDamage()
        {
            if (!GetCaster())
                return;

            uint8 stacks = 0;

            if (Aura* burnigBlood = GetCaster()->GetAura(SPELL_BURNING_BLODD))
                stacks = burnigBlood->GetStackAmount();

            if (stacks > 1)
                SetHitDamage(GetHitDamage() * (stacks / 2));
            else
                SetHitDamage(GetHitDamage());
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_burning_blood_trigger_SpellScript::RecalculateDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_burning_blood_trigger_SpellScript();
    }
};

class spell_corrupted_blood : public SpellScriptLoader
{
public:
    spell_corrupted_blood() : SpellScriptLoader("spell_corrupted_blood") { }

    class spell_corrupted_blood_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corrupted_blood_SpellScript);

        void RecalculateDamage()
        {
            if (!GetHitUnit())
                return;

            uint32 multiplier = 1;

            if (GetCaster()->GetHealthPct() < 15)
                multiplier = 2;

            if (GetCaster()->GetHealthPct() < 10)
                multiplier = 4;

            if (GetCaster()->GetHealthPct() < 5)
                multiplier = 6;

            uint32 _damage = (GetHitDamage() * multiplier);
            PreventHitDamage();
            SetHitDamage(_damage);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_corrupted_blood_SpellScript::RecalculateDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corrupted_blood_SpellScript();
    }
};

class spell_corrupting_parasite : public SpellScriptLoader
{
public:
    spell_corrupting_parasite() : SpellScriptLoader("spell_corrupting_parasite") { }

    class spell_corrupting_parasite_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corrupting_parasite_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            GetCaster()->CastSpell(GetHitUnit(), SPELL_CORRUPTING_PARASITE_TRIGER, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_corrupting_parasite_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corrupting_parasite_SpellScript();
    }
};

class spell_corrupting_parasite_trigger : public SpellScriptLoader
{
public:
    spell_corrupting_parasite_trigger() : SpellScriptLoader("spell_corrupting_parasite_trigger") { }

    class spell_corrupting_parasite_trigger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corrupting_parasite_trigger_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            Unit* target = GetHitUnit();
            Unit* caster = GetCaster();

            if (target->HasAura(SPELL_CORRUPTING_PARASITE_TRIGER))
            {
                Creature* passenger = target->SummonCreature(NPC_BLISTERING_TENTACLE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                caster->CastSpell(target, SPELL_CORRUPTING_PARASITE_AURA, true);
                if (passenger)
                {
                    passenger->EnterVehicle(target, 0);
                    passenger->ClearUnitState(UNIT_STATE_ONVEHICLE);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_corrupting_parasite_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corrupting_parasite_trigger_SpellScript();
    }
};

class spell_blistering_heat : public SpellScriptLoader
{ 
public:
    spell_blistering_heat() : SpellScriptLoader("spell_blistering_heat") { }

    class spell_blistering_heat_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_blistering_heat_SpellScript);

        void RecalculateDamage()
        {
            Unit* target = GetHitUnit();

            if (!GetCaster() && !GetHitUnit())
                return;

            uint8 stacks = 0;
            uint8 modifier = 5;

            if (Aura* blistHeat = target->GetAura(SPELL_BLISTERING_HEAT_AURA))
                stacks = blistHeat->GetStackAmount();

            if (stacks > 1)
                SetHitDamage(GetHitDamage() * (stacks * modifier));
            else
                SetHitDamage(GetHitDamage());
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_blistering_heat_SpellScript::RecalculateDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_blistering_heat_SpellScript();
    }
};

class spell_blistering_heat_aura : public SpellScriptLoader
{
public:
    spell_blistering_heat_aura() : SpellScriptLoader("spell_blistering_heat_aura") { }

    class spell_blistering_heat_aura_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_blistering_heat_aura_SpellScript);

        void HandleApplyAura(SpellEffIndex effIndex)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            GetCaster()->CastSpell(GetHitUnit(), SPELL_BLISTERING_HEAT_AURA);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_blistering_heat_aura_SpellScript::HandleApplyAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_blistering_heat_aura_SpellScript();
    }
};


typedef boss_deathwing::boss_deathwingAI MadnessOfDeathwingAI;

class achievement_platforms : public AchievementCriteriaScript
{
public:
    achievement_platforms(char const* scriptName, uint32 Id) : AchievementCriteriaScript(scriptName), _Id(Id) { }

    bool OnCheck(Player* /*source*/, Unit* target)
    {
        if (!target)
            return false;

        if (MadnessOfDeathwingAI* deathwingAI = CAST_AI(MadnessOfDeathwingAI, target->GetAI()))
            return deathwingAI->AllowAchieve(_Id);

        return false;
    }
private:
    uint32 _Id;
};

void AddSC_madness_of_deathwing()
{
    new boss_deathwing();
    new npc_platfrom();
    new npc_elementium_bolt();
    new npc_regenerative_blood();
    new npc_mutated_corruption();
    new npc_arm_tentacle_one();
    new npc_arm_tentacle_two();
    new npc_wing_tentacle_one();
    new npc_wing_tentacle_two();
    new npc_elementium_fragment();
    new npc_elementium_terror();
    new npc_congealing_blood();
    new npc_corruption_parasite();
    new npc_maelstrom_alexstrasza();
    new npc_maelstrom_kalecgos();
    new npc_maelstrom_ysera();
    new npc_maelstrom_nozdormy();
    new npc_time_zone();
    new npc_blistering_tentacle();

    new spell_agonizing_pain_dmg();
    new spell_burning_blood_trigger();
    new spell_corrupted_blood();
    new spell_corrupting_parasite();
    new spell_corrupting_parasite_trigger();
    new spell_blistering_heat();
    new spell_blistering_heat_aura();

    // no battle
    new npc_maelstrom_trall();
    new npc_madness_of_deathwing_jump_pad();
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_1", 1);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_2", 2);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_3", 3);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_4", 4);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_5", 5);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_6", 6);

    new achievement_platforms("achievement_platform_1", 0);
    new achievement_platforms("achievement_platform_2", 1);
    new achievement_platforms("achievement_platform_3", 2);
    new achievement_platforms("achievement_platform_4", 3);
}
#ifndef DEF_DRAGONSOUL_H
#define DEF_DRAGONSOUL_H
// 109247 - ds nerf real
// 109251 - ds nerf dummy
// 108202 - temple teleport
// 108263 - ship teleport
// 106094 - eye teleport +
// 109835 - summit teleport +
// 106054 - spine teleport
// 106093 - summit teleport aoe
// 106092 - temple teleport aoe
#define DSScriptName "instance_dragon_soul"

enum Datas
{
    DATA_MORCHOK            = 0,
    DATA_YORSAHJ            = 1,
    DATA_ZONOZZ             = 2,
    DATA_HAGARA             = 3,
    DATA_ULTRAXION          = 4,
    DATA_BLACKHORN          = 5,
    DATA_KOHCROM            = 10,
    DATA_HAGARA_EVENT       = 11,
    DATA_LESSER_CACHE_10N   = 12,
    DATA_LESSER_CACHE_25N   = 13,
    DATA_LESSER_CACHE_10H   = 14,
    DATA_LESSER_CACHE_25H   = 15,
    DATA_SWAYZE             = 16,
    DATA_REEVS              = 17,
    DATA_ALLIANCE_SHIP      = 18,
};

enum CreatureIds
{
    NPC_MORCHOK                     = 55265,
    NPC_KOHCROM                     = 57773,
    NPC_YORSAHJ                     = 55312,
    NPC_ZONOZZ                      = 55308,
    NPC_HAGARA                      = 55689,
    NPC_ULTRAXION                   = 55294,
    NPC_BLACKHORN                   = 56427,
    NPC_GORIONA                     = 56781,
    NPC_SKY_CAPTAIN_SWAYZE          = 55870,
    NPC_KAANU_REEVS                 = 55891,

    NPC_NETHESTRASZ                 = 57287, // teleport upstairs
    NPC_EIENDORMI                   = 57288, // teleport to Yor'sahj
    NPC_VALEERA                     = 57289, // teleport to Zon'ozz
    NPC_TRAVEL_TO_WYRMREST_TEMPLE   = 57328, //
    NPC_TRAVEL_TO_WYRMREST_BASE     = 57882, //
    NPC_TRAVEL_TO_WYRMREST_SUMMIT   = 57379, //
    NPC_TRAVEL_TO_EYE_OF_ETERNITY   = 57377, // teleport to Hagara
    NPC_TRAVEL_TO_MAELSTORM         = 57443, //
    NPC_TRAVEL_TO_DECK              = 57378, //
    NPC_DASNURIMI                   = 58153, // trader
    NPC_YSERA_THE_AWAKENED          = 56665,
    NPC_ALEXTRASZA_THE_LIFE_BINDER  = 56630,
    NPC_KALECGOS                    = 56664,
    NPC_THRALL_1                    = 56667, // near summit
    NPC_NOZDORMU_THE_TIMELESS_ONE   = 56666,
    NPC_THE_DRAGON_SOUL             = 56668, // near summit
};

enum GameObjects
{
    GO_INNER_WALL                       = 209596,
    GO_THE_FOCUSING_IRIS                = 210132,
    GO_LESSER_CACHE_OF_THE_ASPECTS_LFR  = 210221,
    GO_LESSER_CACHE_OF_THE_ASPECTS_10N  = 210160,
    GO_LESSER_CACHE_OF_THE_ASPECTS_25N  = 210161,
    GO_LESSER_CACHE_OF_THE_ASPECTS_10H  = 210162,
    GO_LESSER_CACHE_OF_THE_ASPECTS_25H  = 210163,
    GO_ALLIANCE_SHIP                    = 210210,
};

enum SharedSpells
{
    SPELL_TELEPORT_VISUAL_ACTIVE    = 108203,
    SPELL_TELEPORT_VISUAL_DISABLED  = 108227,
    
    SPELL_CHARGING_UP_LIFE          = 108490,
    SPELL_CHARGING_UP_MAGIC         = 108491,
    SPELL_CHARGING_UP_EARTH         = 108492,
    SPELL_CHARGING_UP_TIME          = 108493,
    SPELL_CHARGING_UP_DREAMS        = 108494,
    SPELL_WARD_OF_TIME              = 108160,
    SPELL_WARD_OF_EARTH             = 108161,
    SPELL_WARD_OF_MAGIC             = 108162,
    SPELL_WARD_OF_LIFE              = 108163,
    SPELL_WARD_OF_DREAMS            = 108164,
};

const Position teleportPos[5] =
{
    {-1779.503662f, -2393.439941f, 45.61f, 3.20f},   // Wyrmrest Temple/Base
    {-1854.233154f, -3068.658691f, -178.34f, 0.46f}, // Yor'sahj The Unsleeping
    {-1743.647827f, -1835.132568f, -220.51f, 4.53f}, // Warlord Zon'ozz
    {-1781.188477f, -2375.122559f, 341.35f, 4.43f},  // Wyrmrest Summit
    {13629.356445f, 13612.099609f, 123.49f, 3.14f},  // Hagara
};

const Position ultraxionPos[2] = 
{
    {-1564.f, -2369.f, 250.083f, 3.28122f}, // spawn
    {-1699.469971f, -2388.030029f, 355.192993f, 3.21552f} // move to
};

#endif

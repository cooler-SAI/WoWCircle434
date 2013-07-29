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
    DATA_MORCHOK        = 0,
    DATA_YORSAHJ        = 1,
    DATA_ZONOZZ         = 2,
    DATA_HAGARA         = 3,
    DATA_ULTRAXION      = 4,
    DATA_KOHCROM        = 10,
    DATA_HAGARA_EVENT   = 11,
};

enum CreatureIds
{
    NPC_MORCHOK                     = 55265,
    NPC_KOHCROM                     = 57773,
    NPC_YORSAHJ                     = 55312,
    NPC_ZONOZZ                      = 55308,
    NPC_HAGARA                      = 55689,
    NPC_ULTRAXION                   = 55294,

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
};

enum GameObjects
{
    GO_INNER_WALL           = 209596,
    GO_THE_FOCUSING_IRIS    = 210132,
};

enum SharedSpells
{
    SPELL_TELEPORT_VISUAL_ACTIVE    = 108203,
    SPELL_TELEPORT_VISUAL_DISABLED  = 108227,
};

const Position teleportPos[5] =
{
    {-1779.503662f, -2393.439941f, 45.61f, 3.20f},   // Wyrmrest Temple/Base
    {-1854.233154f, -3068.658691f, -178.34f, 0.46f}, // Yor'sahj The Unsleeping
    {-1743.647827f, -1835.132568f, -220.51f, 4.53f}, // Warlord Zon'ozz
    {-1781.188477f, -2375.122559f, 341.35f, 4.43f},  // Wyrmrest Summit
    {13629.356445f, 13612.099609f, 123.49f, 3.14f},  // Hagara
};

#endif

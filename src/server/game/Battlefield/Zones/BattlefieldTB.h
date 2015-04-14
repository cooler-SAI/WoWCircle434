#ifndef BATTLEFIELD_TB_
#define BATTLEFIELD_TB_

#include "../Battlefield.h"
#include "Group.h"
#include "WorldPacket.h"
#include "World.h"

const uint32 ClockBTWorldState[2] = {5333, 5332};
const uint32 TolBaradFaction[3] = {1732, 1735, 35};

class BattlefieldTB;
class BfCapturePointTB;

struct BfTBGameObjectBuilding;
struct BfTBWorkShopData;

typedef std::set<GameObject*> GameObjectSet;
typedef std::set<BfTBGameObjectBuilding*> TbGameObjectBuilding;
typedef std::set<GameObject*> GameobjectSet;
typedef std::set<BfTBWorkShopData*> TbWorkShop;
typedef std::set<BfCapturePointTB*> BfCapturePointSet;
typedef std::set<Group*> GroupSet;

enum TolbaradData
{
    BATTLEFIELD_TB_ZONEID               = 5095,
    BATTLEFIELD_TB_MAPID                = 732
};

enum TBData32
{
    BATTLEFIELD_TB_DATA_CAPTURED,
    BATTLEFIELD_TB_DATA_DESTROYED,
    BATTLEFIELD_TB_DATA_MAX,
};

enum TBSounds
{
    BATTLEFIELD_TB_HORDE_WINS           = 8454,
    BATTLEFIELD_TB_ALLIANCE_WINS        = 8455,
    BATTLEFIELD_TB_START                = 3439,
};

enum CapturePointsWSDiff
{
    HORDE_DEFENCE = 0,
    HORDE_ATTACK,
    NEUTRAL,
    ALLIANCE_ATTACK,
    ALLIANCE_DEFENCE,
    MAX_CP_DIFF
};

enum BuildingsWSDiff
{
    BUILDING_HORDE_DEFENCE = 0,
    BUILDING_HORDE_DEFENCE_DAMAGED,
    BUILDING_DESTROYED,
    BUILDING_ALLIANCE_DEFENCE,
    BUILDING_ALLIANCE_DEFENCE_DAMAGED,
    BUILDING_MAX_DIFF
};

enum eWorldStates
{
    WS_TB_BATTLE_TIMER_ENABLED                      = 5346,
    WS_TB_BATTLE_TIMER                              = 5333,
    WS_TB_COUNTER_BUILDINGS                         = 5348,
    WS_TB_COUNTER_BUILDINGS_ENABLED                 = 5349,
    WS_TB_HORDE_DEFENCE                             = 5384,
    WS_TB_ALLIANCE_DEFENCE                          = 5385,
    WS_TB_NEXT_BATTLE_TIMER                         = 5332,
    WS_TB_NEXT_BATTLE_TIMER_ENABLED                 = 5387,

    WS_TB_SOUTH_CAPTURE_POINT                       = 5418,
    WS_TB_EAST_CAPTURE_POINT                        = 5423,
    WS_TB_WEST_CAPTURE_POINT                        = 5428,

    WS_TB_EAST_SPIRE                                = 5433,
    WS_TB_SOUTH_SPIRE                               = 5438,
    WS_TB_WEST_SPIRE                                = 5443,

    WS_TB_KEEP_HORDE_DEFENCE                        = 5469,
    WS_TB_KEEP_ALLIANCE_DEFENCE                     = 5470,

    WS_TB_ALLIANCE_ATTACK                           = 5546,
    WS_TB_HORDE_ATTACK                              = 5547,

    WS_TB_UNK                                       = 20010,
    WS_TB_UNK_2                                     = 20011,
};

enum TBpell
{
    SPELL_TB_SPIRITUAL_IMMUNITY                     = 95332,
    SPELL_TB_ALLIANCE_FLAG                          = 14268,
    SPELL_TB_HORDE_FLAG                             = 14267,

    SPELL_TB_VICTORY_REWARD_ALLIANCE                = 89789,
    SPELL_TB_VICTORY_REWARD_HORDE                   = 89791,
    SPELL_TB_LOOSER_REWARD                          = 89793,

    SPELL_TB_TOL_BARAD_TOWER_DESTROYED              = 89796,
    SPELL_TB_TOL_BARAD_TOWER_DAMAGED                = 89795,
    SPELL_TB_TOL_BARAD_TOWER_DEFENDED               = 89794,
    SPELL_TB_TOL_BARAD_SLOW_FALL                    = 88473,

    SPELL_TB_TOL_BARAD_TOWER_BONUS                  = 82629,
    SPELL_TB_TOL_BARAD_VETERAN                      = 84655,
    SPELL_TB_TOL_BARAD_HICK_LAYER_OF_RUST           = 95330,
    SPELL_TB_TOL_BARAD_DEPLOY_SIEGE_MODE            = 84974,
};

enum TBAchievement
{
    ACHIEVEMENT_TOL_BARAD_VICTORY                   = 5412,
};

const Position TbDefencerStartPosition[5] = 
{
    {-1300.45f, 1034.45f, 145.11f, 5.88f},
    {-1285.40f, 923.85f,  145.11f, 5.88f},
    {-1174.90f, 925.72f,  145.11f, 2.93f},
    {-1182.56f, 1040.25f, 145.11f, 2.93f},
    {-802.728f, 1187.91f, 110.65f, 3.12f},
};

/*#########################
*####### Graveyards ######
*#########################*/

class BfGraveyardTB: public BfGraveyard
{
public:
    BfGraveyardTB(BattlefieldTB* Bf);

    void SetTextId(uint32 textid) { m_GossipTextId=textid; }
    uint32 GetTextId() { return m_GossipTextId; }
protected:
    uint32 m_GossipTextId;
};

enum TBGraveyardId
{
    BATTLEFIELD_TB_GY_BARADIN_HOLD,
    BATTLEFIELD_TB_GY_EAST_SPIRE,
    BATTLEFIELD_TB_GY_IRONCLAD_GAMSON,
    BATTLEFIELD_TB_GY_SLAGWORKS,
    BATTLEFIELD_TB_GY_SOUTH_SPIRE,
    BATTLEFIELD_TB_GY_WARDENS_VIGIL,
    BATTLEFIELD_TB_GY_WEST_SPIRE,
    BATTLEFIELD_TB_GY_MAX,
};

enum TBGossipText // TODO
{
    BATTLEFIELD_TB_GOSSIPTEXT_GY_WARDENS_VIGIL    = -1850501,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_IRONGLAD         = -1850502,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_SLAGWORKS        = -1850504,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_NORTH_WEST       = -1850503,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_KEEP             = -1850500,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_SOUTH            = -1850505,
    BATTLEFIELD_TB_GOSSIPTEXT_GY_NORTH_EAST       = -1850506,
};

enum TBNpc
{
    // basic faction npc
    BATTLEFIELD_TB_NPC_GUARD_A                      = 51165,
    BATTLEFIELD_TB_NPC_BARADIN_GUARD_A              = 47324,	
    BATTLEFIELD_TB_NPC_HUNTER_A                     = 47595,
    BATTLEFIELD_TB_NPC_MAGE_A                       = 47598,
    BATTLEFIELD_TB_NPC_PALADIN_A                    = 47600,
    BATTLEFIELD_TB_NPC_WARRIOR_A                    = 47599,

    BATTLEFIELD_TB_NPC_GUARD_H                      = 51166,
    BATTLEFIELD_TB_NPC_BARADIN_GRUNT_H              = 47336,	
    BATTLEFIELD_TB_NPC_DRUID_H                      = 47607,
    BATTLEFIELD_TB_NPC_MAGE_H                       = 47608,
    BATTLEFIELD_TB_NPC_ROGUE_H                      = 47609,
    BATTLEFIELD_TB_NPC_SHAMAN_H                     = 47610,
    BATTLEFIELD_TB_NPC_COMMANDER_ZANOTH             = 48069, // General H
    BATTLEFIELD_TB_NPC_PRIVATE_GARNOTH              = 48071, // Cursed Depths H
    BATTLEFIELD_TB_NPC_DRILLMASTER_RAZGOTH          = 48070, // D-Block H
    BATTLEFIELD_TB_NPC_STAFF_SERGEANT_LAZGAR        = 48062, // Prison H

    BATTLEFIELD_TB_NPC_SERGEANT_PARKER              = 48066, // General A
    BATTLEFIELD_TB_NPC_COMMANDER_STEVENS            = 48039, // Cursed Depths A
    BATTLEFIELD_TB_NPC_2ND_LIENTINANT_WANSWORTH     = 48061, // D-Block A
    BATTLEFIELD_TB_NPC_MARSHAL_FALLOWS              = 48074, // Prison A

    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_1             = 45078,
    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_2             = 45074,
    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_3             = 45076,
    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_4             = 45079,
    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_5             = 45075,
    BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_6             = 45077,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_1             = 45070,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_2             = 45068,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_3             = 45073,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_4             = 45071,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_5             = 45069,
    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_6             = 45072,

    BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE       = 45344,
    NPC_TOL_BARAD_SIEGE_ENGINE_TURRET               = 45564,

    BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT               = 47531,

    BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE               = 47534,
    BATTLEFIELD_TB_NPC_ARCHMAGE_GALLUST             = 47537,
    BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER             = 47540,
    BATTLEFIELD_TB_NPC_CELL_WATCHER                 = 47542,
    BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD            = 47548,
    BATTLEFIELD_TB_NPC_SVARNOS                      = 47544,
    BATTLEFIELD_TB_NPC_EXILED_MAGE                  = 47552,
    BATTLEFIELD_TB_NPC_IMPRISONED_WORKER            = 47550,
    BATTLEFIELD_TB_NPC_WARDEN_SILVA                 = 48036,
    BATTLEFIELD_TB_NPC_GHASTLY_CONVICT              = 47590,
    BATTLEFIELD_TBNPC_CROCOLISK                     = 47591,
    BATTLEFIELD_TB_NPC_PROBLIM                      = 47593,
    BATTLEFIELD_TB_NPC_BATTLE_GUARD                 = 47561,
    BATTLEFIELD_TB_NPC_SIEGE_ENGINE_TURRET          = 45564,
};

struct BfTBCoordGY
{
    float x;
    float y;
    float z;
    float o;
    uint32 gyid;
    uint8 type;
    uint32 entrya;
    uint32 entryh;
};

const uint32 TBQuest[2][6]= // TODO!
{
    { 13186, 28882, 13222, 13538, 13177, 13179 },
    { 13185, 28884, 13223, 13539, 13178, 13180 },
};

const BfTBCoordGY TBGraveYard[BATTLEFIELD_TB_GY_MAX] =
{
    { -1244.58f, 981.233f, 155.425f, 0.733038f, 1789, BATTLEFIELD_TB_GY_BARADIN_HOLD,    BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_1, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_1 },
    { -944.34f,  576.111f, 157.543f, 5.317802f, 1788, BATTLEFIELD_TB_GY_EAST_SPIRE,      BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_1, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_1 },
    { -970.465f, 1088.33f, 132.992f, 6.268125f, 1783, BATTLEFIELD_TB_GY_IRONCLAD_GAMSON, BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_2, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_2 },
    { -1343.65f, 568.823f, 139.158f, 0.008500f, 1787, BATTLEFIELD_TB_GY_SLAGWORKS,       BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_3, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_3 },
    { -1600.28f, 869.21f,  193.948f, 4.819249f, 1786, BATTLEFIELD_TB_GY_SOUTH_SPIRE,     BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_4, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_4 },
    { -1572.14f, 1169.94f, 159.501f, 3.971623f, 1785, BATTLEFIELD_TB_GY_WARDENS_VIGIL,   BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_5, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_5 },
    { -1052.1f,  1490.65f, 191.407f, 1.972220f, 1332, BATTLEFIELD_TB_GY_WEST_SPIRE,      BATTLEFIELD_TB_NPC_A_SPIRIT_GUIDE_6, BATTLEFIELD_TB_NPC_H_SPIRIT_GUIDE_6 },
};

/*#########################
* BfCapturePointTB       *
#########################*/

class BfCapturePointTB: public BfCapturePoint
{
public:
    BfCapturePointTB(BattlefieldTB* battlefield, TeamId control);

    void LinkToWorkShop(BfTBWorkShopData* workshop) { m_WorkShop = workshop; }

    void ChangeTeam(TeamId oldteam);
    TeamId GetTeam() const { return m_team; }

    void UpdateCapturePointValue()
    {
        if (m_team == TEAM_ALLIANCE)
        {
            m_value = m_maxValue;
            m_State = BF_CAPTUREPOINT_OBJECTIVESTATE_ALLIANCE;
        }
        else 
        {
            m_value = -m_maxValue;
            m_State = BF_CAPTUREPOINT_OBJECTIVESTATE_HORDE;
        }
    }

protected:
    BfTBWorkShopData* m_WorkShop;
};

/*#########################
* TolBarad Battlefield    *
#########################*/

class BattlefieldTB: public Battlefield
{
public:
    bool SetupBattlefield();

    void OnBattleStart();
    void OnBattleEnd(bool endbytimer);
    void OnPlayerJoinWar(Player* plr);
    void OnPlayerLeaveZone(Player* plr);
    void OnPlayerLeaveWar(Player* plr);

    bool Update(uint32 diff);
    void OnCreatureCreate(Creature *creature, bool add);
    void SendInitWorldStatesTo(Player* player);
    void SendInitWorldStatesToAll();
    void AddBrokenTower(TeamId team);
    void AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid);

    void RewardMarkOfHonor(Player *plr, uint32 count);

    void FillInitialWorldStates(WorldPacket& data);

    void PromotePlayer(Player *killer);
    void CapturePoint(uint32 team);

    void OnDestroyed();
    void OnDamaged();

    void OnPlayerEnterZone(Player* plr);

    void ProcessEvent(GameObject *obj, uint32 eventId);

protected:
    TbGameObjectBuilding BuildingsInZone;
    GuidSet Vehicles;
    GuidSet questgiversA;
    GuidSet questgiversH;
    GuidSet npcAlliance;
    GuidSet npcHorde;
    GuidSet npcOld;
    GameobjectSet goDoors;
    GuidSet OutsideCreature[2];
    TbWorkShop WorkShopList; 
    BfCapturePointSet CapturePoints;
    GuidSet CanonList;
    GameObjectSet DefenderPortalList;
    GameObjectSet m_KeepGameObject[2];
    GuidSet m_PlayersIsSpellImu;
    uint32 m_saveTimer;
};

enum eTBGameObjectBuildingType // Correct this.
{
    BATTLEFIELD_TB_OBJECTTYPE_TOWER,
};

enum eTBGameObjectState // Correct this.
{
    BATTLEFIELD_TB_OBJECTSTATE_HORDE_INTACT,
    BATTLEFIELD_TB_OBJECTSTATE_HORDE_DAMAGE,
    BATTLEFIELD_TB_OBJECTSTATE_HORDE_DESTROY,
    BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT,
    BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DAMAGE,
    BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DESTROY,
};

enum eTBTeamControl
{
    BATTLEFIELD_TB_TEAM_ALLIANCE,
    BATTLEFIELD_TB_TEAM_HORDE,
    BATTLEFIELD_TB_TEAM_NEUTRAL,    
};

enum eTBText // TODO!
{
    BATTLEFIELD_TB_TEXT_WORKSHOP_NAME_NE    = 13100, //?
    BATTLEFIELD_TB_TEXT_WORKSHOP_NAME_NW    = 13101, //?
    BATTLEFIELD_TB_TEXT_WORKSHOP_NAME_SE    = 13102, //?
    BATTLEFIELD_TB_TEXT_WORKSHOP_NAME_SW    = 13103, //?
    BATTLEFIELD_TB_TEXT_WORKSHOP_ATTACK     = 13104, //?
    BATTLEFIELD_TB_TEXT_WORKSHOP_TAKEN      = 13105, //?
    BATTLEFIELD_TB_TEXT_ALLIANCE            = 13106, // 
    BATTLEFIELD_TB_TEXT_HORDE               = 13107, // 
    BATTLEFIELD_TB_TEXT_WILL_START          = 13108, //?
    BATTLEFIELD_TB_TEXT_START               = 13109, //
    BATTLEFIELD_TB_TEXT_FIRSTRANK           = 13110, //?
    BATTLEFIELD_TB_TEXT_SECONDRANK          = 13111,
    BATTLEFIELD_TB_TEXT_KEEPTOWER_NAME_NE   = 13112,
    BATTLEFIELD_TB_TEXT_KEEPTOWER_NAME_NW   = 13113,
    BATTLEFIELD_TB_TEXT_KEEPTOWER_NAME_SE   = 13114,
    BATTLEFIELD_TB_TEXT_KEEPTOWER_NAME_SW   = 13115,
    BATTLEFIELD_TB_TEXT_TOWER_DAMAGE        = 13115, //
    BATTLEFIELD_TB_TEXT_TOWER_DESTROY       = 13117, //
    BATTLEFIELD_TB_TEXT_TOWER_NAME_S        = 13118, //?
    BATTLEFIELD_TB_TEXT_TOWER_NAME_E        = 13119, //?
    BATTLEFIELD_TB_TEXT_TOWER_NAME_W        = 13120, //?
    BATTLEFIELD_TB_TEXT_DEFEND_KEEP         = 13121, //?
    BATTLEFIELD_TB_TEXT_WIN_KEEP            = 13122, //
};

enum eTBObject
{
    GAMEOBJECT_TB_NORTH_CAPTURE_POINT_AD            = 205068,
    GAMEOBJECT_TB_WEST_CAPTURE_POINT_AD             = 205101,
    GAMEOBJECT_TB_EAST_CAPTURE_POINT_AD             = 205138,
    GAMEOBJECT_TB_NORTH_CAPTURE_POINT_HD            = 205096,
    GAMEOBJECT_TB_WEST_CAPTURE_POINT_HD             = 205103,
    GAMEOBJECT_TB_EAST_CAPTURE_POINT_HD             = 205139,
    GAMEOBJECT_TB_SOUTH_SPIRE                       = 204590,
    GAMEOBJECT_TB_WEST_SPIRE                        = 204588,
    GAMEOBJECT_TB_EAST_SPIRE                        = 204589,
    GAMEOBJECT_TB_DOOR                              = 206576,
    GAMEOBJECT_TB_GATES                             = 206598,
    GAMEOBJECT_TB_GATE_D_BLOCK                      = 206844,
    GAMEOBJECT_TB_CURSED_DEPTHS_GATE                = 206843,
    GAMEOBJECT_TB_BARADINS_WARDEN_BANNER            = 207391,
    GAMEOBJECT_TB_HELLSCREAM_REACH_BANNER           = 207400,
    GAMEOBJECT_TB_CRATE_OF_CELLBLOCK_RATIONS        = 206996,
    GAMEOBJECT_TB_CURSED_SHACKLES                   = 206905,
    GAMEOBJECT_TB_DUSTY_PRISON_JOURNAL              = 206890,
};

struct BfTBObjectPosition
{
    float x;
    float y;
    float z;
    float o;
    uint32 entrya;
    uint32 entryh;
};

//*********************************************************
//************Destructible (Wall,Tower..)******************
//*********************************************************

struct BfTBBuildingSpawnData
{
    uint32 entry;
    uint32 WorldState;
    float x;
    float y;
    float z;
    float o;
    uint32 type;
    uint32 nameid;
};

#define TB_MAX_OBJ  3
const BfTBBuildingSpawnData TBGameObjectBuillding[TB_MAX_OBJ] = 
{
    // Wall
    // Entry                    WS    X          Y         Z         O          type                             NameID
    {GAMEOBJECT_TB_SOUTH_SPIRE, 5438, -1618.91f, 954.542f, 168.601f, 0.069812f, BATTLEFIELD_TB_OBJECTTYPE_TOWER, 0},
    {GAMEOBJECT_TB_WEST_SPIRE,  5433, -950.41f,  1469.1f,  176.596f, -2.10312f, BATTLEFIELD_TB_OBJECTTYPE_TOWER, 0},
    {GAMEOBJECT_TB_EAST_SPIRE,  5443, -1013.28f, 529.538f, 146.427f, 1.97222f,  BATTLEFIELD_TB_OBJECTTYPE_TOWER, 0},
};

const BfTBObjectPosition TBGameobjectsDoor[4] =
{
    {-1086.686f,   1150.33f,     125.7371f,   1.535887f,     GAMEOBJECT_TB_GATE_D_BLOCK,        0},
    {-1233.360f,   783.5536f,    125.2064f,   -0.008726f,    GAMEOBJECT_TB_CURSED_DEPTHS_GATE,  0},
    {-1204.353f,   1075.035f,    123.6819f,   0.026179f,     GAMEOBJECT_TB_DOOR,                0},
    {-834.2009f,   1188.17f,     114.125f,    -1.58825f,     GAMEOBJECT_TB_GATES,               0},
};

#define TB_MAX_DESTROY_MACHINE_NPC  6
const BfTBObjectPosition TBDestroyMachineNPC[TB_MAX_DESTROY_MACHINE_NPC] =
{
    {-1438.3f,     1095.24f,     121.136f,    5.28f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
    {-1213.01f,    782.236f,     121.4473f,   1.67f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
    {-1442.3f,     1141.07f,     123.6323f,   4.24f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
    {-1108.52f,    1111.33f,     121.2783f,   1.37f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
    {-1258.26f,    780.497f,     122.4413f,   1.48f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
    {-1106.57f,    1196.34f,     121.802f,    0.40f,     BATTLEFIELD_TB_NPC_ABANDONED_SIEGE_ENGINE, 0},
};

const BfTBObjectPosition QuestGivers[4] =
{
    {-1227.28f,     974.37f,     119.63f,     6.21f,    BATTLEFIELD_TB_NPC_SERGEANT_PARKER,             BATTLEFIELD_TB_NPC_COMMANDER_ZANOTH     }, // static
    {-1227.28f,     980.55f,     119.54f,     6.21f,    BATTLEFIELD_TB_NPC_MARSHAL_FALLOWS,             BATTLEFIELD_TB_NPC_STAFF_SERGEANT_LAZGAR},
    {-1227.28f,     980.55f,     119.54f,     6.21f,    BATTLEFIELD_TB_NPC_COMMANDER_STEVENS,           BATTLEFIELD_TB_NPC_PRIVATE_GARNOTH      },
    {-1227.28f,     980.55f,     119.54f,     6.21f,    BATTLEFIELD_TB_NPC_2ND_LIENTINANT_WANSWORTH,    BATTLEFIELD_TB_NPC_DRILLMASTER_RAZGOTH  },
};

const BfTBObjectPosition CentrNPC[37] =
{
    {-1182.93f,     936.213f,     119.728f,    5.80603f,     50173, 50167},
};

const BfTBObjectPosition AllianceSpawnNPC[23] =
{

    {-1360.68f,     685.882f,     123.426f,    2.6943f,      BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-1410.67f,     626.736f,     123.423f,    0.80149f,     BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-1466.99f,     653.838f,     123.423f,    0.397003f,    BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-1528.59f,     656.61f,      123.421f,    3.93915f,     BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-830.932f,     928.307f,     121.441f,    2.83331f,     BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-883.376f,     970.492f,     121.441f,    4.62402f,     BATTLEFIELD_TB_NPC_HUNTER_A,   0},
    {-1491.68f,     692.527f,     123.422f,    3.00452f,     BATTLEFIELD_TB_NPC_MAGE_A,     0},
    {-1419.68f,     680.449f,     123.421f,    2.09346f,     BATTLEFIELD_TB_NPC_MAGE_A,     0},
    {-1424.75f,     626.612f,     123.422f,    0.97427f,     BATTLEFIELD_TB_NPC_MAGE_A,     0},
    {-955.18f,      925.935f,     121.441f,    3.13176f,     BATTLEFIELD_TB_NPC_MAGE_A,     0},
    {-880.304f,     917.123f,     121.441f,    3.46949f,     BATTLEFIELD_TB_NPC_MAGE_A,     0},
    {-1440.05f,     747.537f,     123.422f,    1.10387f,     BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-1521.92f,     673.305f,     123.422f,    0.935005f,    BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-1444.05f,     596.387f,     123.421f,    4.11194f,     BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-1372.16f,     686.296f,     123.422f,    1.33948f,     BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-961.609f,     933.868f,     121.442f,    3.28884f,     BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-834.797f,     963.605f,     121.44f,     1.40388f,     BATTLEFIELD_TB_NPC_PALADIN_A,  0},
    {-1466.75f,     582.644f,     123.421f,    0.0514269f,   BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
    {-1482.3f,      629.489f,     123.421f,    3.9313f,      BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
    {-1414.15f,     717.203f,     123.421f,    4.34756f,     BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
    {-1447.06f,     757.232f,     123.422f,    1.89319f,     BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
    {-845.793f,     922.752f,     121.44f,     0.0294366f,   BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
    {-950.795f,     960.499f,     121.443f,    1.73376f,     BATTLEFIELD_TB_NPC_WARRIOR_A,  0},
};

const BfTBObjectPosition HordeSpawnNPC[23] =
{
    {-879.097f,     1031.14f,     121.441f,    3.43414f,     BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-846.13f,      1035.92f,     121.589f,    0.104053f,    BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-1564.3f,      1320.38f,     133.584f,    5.13218f,     BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-1499.51f,     1383.7f,      133.591f,    2.26548f,     BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-1427.8f,      1306.14f,     133.584f,    1.55862f,     BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-1478.19f,     1239.36f,     133.584f,    3.35718f,     BATTLEFIELD_TB_NPC_DRUID_H,    0},
    {-957.143f,     987.354f,     121.505f,    3.36738f,     BATTLEFIELD_TB_NPC_MAGE_H,     0},
    {-1543.83f,     1286.87f,     133.584f,    2.25369f,     BATTLEFIELD_TB_NPC_MAGE_H,     0},
    {-1503.17f,     1351.55f,     152.961f,    5.60734f,     BATTLEFIELD_TB_NPC_MAGE_H,     0},
    {-1442.06f,     1334.72f,     133.754f,    1.05989f,     BATTLEFIELD_TB_NPC_MAGE_H,     0},
    {-1466.6f,      1278.0f,      133.584f,    5.60342f,     BATTLEFIELD_TB_NPC_MAGE_H,     0},
    {-953.343f,     1023.93f,     121.441f,    3.65798f,     BATTLEFIELD_TB_NPC_ROGUE_H,    0},
    {-832.976f,     1030.49f,     121.441f,    3.2103f,      BATTLEFIELD_TB_NPC_ROGUE_H,    0},
    {-1529.07f,     1256.59f,     133.816f,    4.5824f,      BATTLEFIELD_TB_NPC_ROGUE_H,    0},
    {-1426.26f,     1291.56f,     133.601f,    0.372661f,    BATTLEFIELD_TB_NPC_ROGUE_H,    0},
    {-1545.93f,     1362.95f,     133.615f,    0.906734f,    BATTLEFIELD_TB_NPC_ROGUE_H,    0},
    {-832.029f,     989.578f,     121.441f,    4.8243f,      BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-884.68f,      991.661f,     121.441f,    4.69471f,     BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-970.775f,     1031.12f,     121.441f,    5.82175f,     BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-1447.47f,     1356.02f,     133.64f,     3.5928f,      BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-1499.62f,     1232.78f,     133.585f,    6.05109f,     BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-1521.11f,     1381.87f,     133.584f,    0.180243f,    BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
    {-1515.72f,     1303.58f,     152.961f,    2.25369f,     BATTLEFIELD_TB_NPC_SHAMAN_H,   0},
};

const BfTBObjectPosition TbOldNpcStructure[130] =
{
    {-1264.56f,     831.982f,     120.542f,    5.73804f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1280.47f,     804.361f,     121.06f,     1.68538f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1260.71f,     786.074f,     121.965f,    3.81774f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1234.43f,     796.647f,     119.433f,    4.04158f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1218.08f,     837.581f,     119.709f,    5.78909f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1239.59f,     831.214f,     121.267f,    2.50219f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1213.11f,     819.229f,     122.115f,    1.32017f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1215.63f,     798.227f,     120.12f,     5.96187f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1249.62f,     807.995f,     121.467f,    3.53892f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1195.97f,     831.314f,     89.5376f,    5.73804f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1217.52f,     835.794f,     89.5595f,    0.204913f,    BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1301.66f,     829.464f,     89.5128f,    0.617247f,    BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1256.11f,     783.963f,     89.5288f,    1.53224f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1250.61f,     816.766f,     89.5303f,    4.48926f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1283.14f,     853.025f,     89.5193f,    3.23655f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1273.05f,     832.355f,     89.5312f,    3.69209f,     BATTLEFIELD_TB_NPC_CAPTIVE_SPIRIT, 0},
    {-1240.83f,     851.644f,     89.5303f,    1.66183f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1301.03f,     862.596f,     89.528f,     1.69718f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1289.52f,     834.897f,     89.5285f,    3.26012f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1262.48f,     837.571f,     89.5285f,    1.43407f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1249.73f,     799.812f,     89.5251f,    4.75237f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1229.85f,     834.627f,     89.5275f,    3.24048f,     BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1205.1f,      831.685f,     89.5606f,    3.4329f,      BATTLEFIELD_TB_NPC_CELLBLOCK_OOZE, 0},
    {-1296.88f,     881.191f,     89.5261f,    4.75236f,     BATTLEFIELD_TB_NPC_ARCHMAGE_GALLUST, 0},
    {-1002.14f,     1213.91f,     84.5818f,    3.58997f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-932.666f,     1210.34f,     84.5822f,    1.83461f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-968.921f,     1211.58f,     84.5823f,    1.61862f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-924.17f,      1128.04f,     84.5556f,    3.37792f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-990.085f,     1127.92f,     84.5363f,    3.6371f,      BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-1095.7f,      1123.29f,     121.924f,    5.2079f,      BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-1062.05f,     1200.74f,     122.685f,    6.02079f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-1086.38f,     1182.97f,     121.754f,    1.24557f,     BATTLEFIELD_TB_NPC_SHIVAN_DESTROYER, 0},
    {-1070.31f,     1169.91f,     120.837f,    5.83699f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-1096.75f,     1155.58f,     119.834f,    1.18351f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-1101.96f,     1088.16f,     124.403f,    2.05137f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-1078.32f,     1105.58f,     121.3f,      4.74529f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-969.531f,     1103.38f,     84.5825f,    4.29368f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-953.166f,     1129.88f,     84.5825f,    5.72703f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-968.312f,     1179.87f,     84.582f,     4.81204f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-984.658f,     1218.03f,     84.582f,     3.76354f,     BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-948.037f,     1215.34f,     84.5823f,    3.3944f,      BATTLEFIELD_TB_NPC_CELL_WATCHER, 0},
    {-976.039f,     1017.46f,     84.8929f,    6.27605f,     BATTLEFIELD_TB_NPC_SVARNOS, 0},
    {-929.194f,     1222.0f,      84.5822f,    4.55099f,     BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-969.092f,     1141.62f,     84.578f,     3.25508f,     BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-936.774f,     1125.04f,     84.578f,     5.59949f,     BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-974.035f,     1042.5f,      84.5447f,    2.34794f,     BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-1098.07f,     1140.17f,     119.642f,    1.2267f,      BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-971.654f,     1072.41f,     84.5365f,    4.771f,       BATTLEFIELD_TB_NPC_JAILED_WRATHGUARD, 0},
    {-1446.8f,      1140.18f,     123.549f,    2.19982f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1475.83f,     1101.72f,     127.652f,    5.28643f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1485.06f,     1138.47f,     126.336f,    2.72211f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1459.23f,     1134.34f,     123.671f,    1.12383f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1486.95f,     1110.12f,     126.841f,    3.03234f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1463.41f,     1114.58f,     123.764f,    5.21575f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1668.01f,     1084.96f,     95.0151f,    2.33882f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1654.09f,     1071.48f,     95.0149f,    4.66753f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1611.11f,     1099.95f,     95.0195f,    6.2776f,      BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1608.73f,     1128.0f,      95.1421f,    0.748398f,    BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1661.56f,     1193.13f,     101.833f,    2.80379f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1559.28f,     1252.51f,     101.801f,    3.52635f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1520.52f,     1279.16f,     101.797f,    1.52359f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1574.06f,     1271.09f,     101.801f,    5.37596f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1599.79f,     1253.17f,     101.801f,    5.76866f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1649.34f,     1182.61f,     97.8413f,    3.46667f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1603.02f,     1183.13f,     96.0109f,    0.00698853f,  BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1606.48f,     1157.01f,     95.1265f,    3.16429f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1580.63f,     1149.79f,     95.0777f,    2.83835f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1623.92f,     1125.3f,      95.0154f,    1.88802f,     BATTLEFIELD_TB_NPC_IMPRISONED_WORKER, 0},
    {-1475.67f,     1146.29f,     126.267f,    2.39617f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1445.92f,     1123.32f,     123.422f,    0.0988761f,   BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1491.86f,     1094.17f,     131.91f,     1.20629f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1629.13f,     1105.64f,     95.0225f,    5.67675f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1675.38f,     1191.96f,     101.949f,    5.69246f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1509.72f,     1247.05f,     104.762f,    4.65181f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1520.69f,     1266.3f,      101.801f,    0.591307f,    BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1588.46f,     1263.26f,     102.042f,    5.82206f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1619.2f,      1257.04f,     101.801f,    0.544187f,    BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1610.43f,     1221.53f,     98.8233f,    4.87959f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1623.31f,     1185.54f,     95.016f,     3.31664f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1609.26f,     1142.02f,     95.0146f,    1.41205f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1574.77f,     1140.7f,      95.2083f,    0.194685f,    BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1654.04f,     1083.07f,     95.0148f,    1.21176f,     BATTLEFIELD_TB_NPC_EXILED_MAGE, 0},
    {-1673.65f,     1098.07f,     95.0459f,    5.7137f,      BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1671.61f,     1061.96f,     95.051f,     1.09556f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1634.77f,     1065.65f,     95.0516f,    2.72134f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1647.64f,     1099.45f,     95.0148f,    4.44136f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1594.7f,      1140.31f,     95.159f,     0.0745502f,   BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1611.17f,     1172.05f,     95.0166f,    5.43882f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1664.23f,     1182.75f,     101.82f,     0.64004f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1633.02f,     1190.14f,     95.0148f,    4.57488f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1610.54f,     1192.66f,     95.0148f,    5.01864f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1606.67f,     1232.26f,     101.801f,    4.02903f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1609.38f,     1263.14f,     101.769f,    4.68484f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1571.44f,     1252.53f,     101.801f,    2.67422f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1558.57f,     1270.52f,     101.801f,    4.67699f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1520.74f,     1236.27f,     101.797f,    1.64142f,     BATTLEFIELD_TB_NPC_BATTLE_GUARD, 0},
    {-1195.59f,     844.879f,     89.5303f,    1.60685f,     BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1225.32f,     845.138f,     89.5259f,    1.70503f,     BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1240.19f,     793.08f,      89.5224f,    0.0124924f,   BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1209.78f,     822.205f,     89.523f,     4.71311f,     BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1311.55f,     838.133f,     89.5184f,    3.11483f,     BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1264.55f,     808.465f,     89.5244f,    3.11876f,     BATTLEFIELD_TB_NPC_GHASTLY_CONVICT, 0},
    {-1320.38f,     1169.57f,     118.941f,    3.19618f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1195.4f,      1218.32f,     118.875f,    2.05735f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1137.07f,     1119.8f,      118.928f,    5.02615f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1232.51f,     1211.49f,     120.715f,    2.43826f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1216.49f,     1186.38f,     120.894f,    2.92521f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1191.99f,     1177.61f,     120.06f,     0.922443f,    BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1204.47f,     1148.83f,     120.312f,    0.376591f,    BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1247.7f,      1142.87f,     120.459f,    0.722166f,    BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1281.35f,     1119.24f,     120.477f,    0.329467f,    BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1315.76f,     1115.93f,     119.694f,    5.02222f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1298.72f,     1145.35f,     120.604f,    3.53389f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1278.62f,     1159.11f,     119.98f,     5.61127f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1299.56f,     1194.47f,     120.182f,    3.9423f,      BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1277.61f,     1214.17f,     119.328f,    5.3678f,      BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1281.38f,     1249.76f,     119.36f,     4.15829f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1251.5f,      1276.29f,     119.751f,    2.36758f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1226.47f,     1247.54f,     120.527f,    4.01299f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1204.49f,     1272.11f,     118.99f,     4.38212f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1190.13f,     1308.4f,      120.012f,    3.05088f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1150.76f,     1306.51f,     120.114f,    1.91598f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1145.57f,     1270.51f,     120.586f,    1.22875f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1161.19f,     1235.93f,     120.817f,    2.92521f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1138.13f,     1237.6f,      118.883f,    1.10309f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1155.12f,     1192.63f,     121.161f,    2.52073f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1124.89f,     1168.18f,     119.383f,    0.781075f,    BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1155.61f,     1138.8f,      121.17f,     3.93838f,     BATTLEFIELD_TBNPC_CROCOLISK, 0},
    {-1354.74f,     1113.18f,     123.422f,    5.00101f,     BATTLEFIELD_TB_NPC_PROBLIM, 0},
    {-1521.64f,     1257.59f,     101.801f,    3.16902f,     BATTLEFIELD_TB_NPC_WARDEN_SILVA, 0},
};

//*********************************************************
//*****************WorkShop Data & Element*****************
//*********************************************************

enum eTBWorkShopType
{
    BATTLEFIELD_TB_NOTH_CP,
    BATTLEFIELD_TB_WEST_CP,
    BATTLEFIELD_TB_EAST_CP,
    TB_MAX_WORKSHOP,
};


struct BfTBWorkShopDataBase
{
    uint32 worldstate;
    uint32 type;
    uint32 nameid;
    BfTBObjectPosition CapturePoint;
};

const BfTBWorkShopDataBase TBWorkShopDataBase[TB_MAX_WORKSHOP]=
{
    {WS_TB_SOUTH_CAPTURE_POINT, BATTLEFIELD_TB_NOTH_CP, BATTLEFIELD_TB_TEXT_WIN_KEEP, { -896.960000f, 979.497000f, 121.441000f, 3.124123f, GAMEOBJECT_TB_NORTH_CAPTURE_POINT_AD, GAMEOBJECT_TB_NORTH_CAPTURE_POINT_HD}},
    {WS_TB_EAST_CAPTURE_POINT,  BATTLEFIELD_TB_EAST_CP, BATTLEFIELD_TB_TEXT_WIN_KEEP, { -1492.34000f, 1309.87000f, 152.961000f, -0.82030f, GAMEOBJECT_TB_EAST_CAPTURE_POINT_AD,  GAMEOBJECT_TB_EAST_CAPTURE_POINT_HD}},
    {WS_TB_WEST_CAPTURE_POINT,  BATTLEFIELD_TB_WEST_CP, BATTLEFIELD_TB_TEXT_WIN_KEEP, { -1437.00000f, 685.556000f, 123.421000f, 0.802851f, GAMEOBJECT_TB_WEST_CAPTURE_POINT_AD,  GAMEOBJECT_TB_WEST_CAPTURE_POINT_HD}},
};


//********************************************************************
//*         Structs using for Building, Graveyard, Workshop
//********************************************************************

struct BfTBGameObjectBuilding
{
    BfTBGameObjectBuilding(BattlefieldTB* TB)
    {
        m_TB = TB;
        m_Team = 0;
        m_Build = NULL;
        m_Type = 0;
        m_WorldState = 0;
        m_State = 0;
        m_NameId = 0;

    }
    uint8 m_Team;
    BattlefieldTB* m_TB;
    GameObject* m_Build;
    uint32 m_Type;
    uint32 m_WorldState;
    uint32 m_State;
    uint32 m_NameId;

    void Rebuild()
    {
        if (!m_Build)
            return;

        m_Team = m_TB->GetDefenderTeam();
        m_Build->Rebuild();
        m_State = BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT - (m_Team * 3);
        m_Build->SetUInt32Value(GAMEOBJECT_FACTION,TolBaradFaction[m_Team]);
    }

    void Damaged()
    {
        m_TB->OnDamaged();

        for (int i = 0; i < BUILDING_MAX_DIFF; i++)
        {
            if (i == BUILDING_HORDE_DEFENCE_DAMAGED)
            {
                if (m_TB->GetDefenderTeam() == TEAM_HORDE)
                    m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                else
                    m_TB->SendUpdateWorldState(m_WorldState + i, 0);
            }
            else if (i == BUILDING_ALLIANCE_DEFENCE_DAMAGED)
            {
                if (m_TB->GetDefenderTeam() == TEAM_ALLIANCE)
                    m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                else
                    m_TB->SendUpdateWorldState(m_WorldState + i, 0);
            }
            else
                m_TB->SendUpdateWorldState(m_WorldState + i, 0);
        }

        m_State = BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DAMAGE - (m_Team * 3);

        if (m_NameId)
            m_TB->SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_TOWER_DAMAGE, sObjectMgr->GetTrinityStringForDBCLocale(m_NameId));
    }

    void Destroyed()
    {
        m_TB->OnDestroyed();

        m_TB->SetTimer(m_TB->GetTimer() + 5 * MINUTE * IN_MILLISECONDS);
        m_TB->SendUpdateWorldState(WS_TB_BATTLE_TIMER, uint32(time(NULL) + m_TB->GetTimer() / 1000));

        for (int i = 0; i < BUILDING_MAX_DIFF; i++)
        {
            if (i == BUILDING_DESTROYED)
                m_TB->SendUpdateWorldState(m_WorldState + i, 1);
            else
                m_TB->SendUpdateWorldState(m_WorldState + i, 0);
        }

        m_State = BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DESTROY - (m_Team * 3);

        if (m_NameId)
            m_TB->SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_TOWER_DESTROY, sObjectMgr->GetTrinityStringForDBCLocale(m_NameId));

        m_TB->AddBrokenTower(TeamId(m_Team));
    }

    void Init(GameObject* go, uint32 type, uint32 worldstate, uint32 nameid)
    {
        m_Build = go;
        m_Type = type;
        m_WorldState = worldstate;
        m_NameId = nameid;
        m_Team = m_TB->GetDefenderTeam();

        switch (m_State)
        {
        case BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT:
        case BATTLEFIELD_TB_OBJECTSTATE_HORDE_INTACT:
            if (m_Build)
                m_Build->Rebuild();
            break;
        case BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DESTROY:
        case BATTLEFIELD_TB_OBJECTSTATE_HORDE_DESTROY:
            if (m_Build)
            {
                m_Build->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                m_Build->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                m_Build->SetUInt32Value(GAMEOBJECT_DISPLAYID, m_Build->GetGOInfo()->building.destroyedDisplayId);
            }
            break;
        case BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_DAMAGE:
        case BATTLEFIELD_TB_OBJECTSTATE_HORDE_DAMAGE:
            if (m_Build)
            {
                m_Build->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                m_Build->SetUInt32Value(GAMEOBJECT_DISPLAYID, m_Build->GetGOInfo()->building.damagedDisplayId);
            }
            break;
        }
    }

    void Save()
    {
        sWorld->setWorldState(m_WorldState, m_State);
    }
};

struct BfTBWorkShopData
{
    BattlefieldTB* m_TB;
    GameObject* m_Build;
    uint32 m_Type;
    uint32 m_State;
    uint32 m_WorldState;
    uint32 m_TeamControl;
    uint32 m_NameId;

    BfTBWorkShopData(BattlefieldTB* TB)
    {
        m_TB = TB;
        m_Build = NULL;
        m_Type = 0;
        m_State = 0;
        m_WorldState = 0;
        m_TeamControl = 0;
        m_NameId = 0;
    }

    void Init(uint32 worldstate, uint32 type,uint32 nameid)
    {
        m_WorldState = worldstate;
        m_Type = type;
        m_NameId = nameid;
    }

    void ChangeControl(uint8 team, bool init)
    {
        if (!init)
            m_TB->CapturePoint(team);

        switch (team)
        {
        case BATTLEFIELD_TB_TEAM_NEUTRAL:
            {
                if (m_TeamControl == BATTLEFIELD_TB_TEAM_ALLIANCE)
                {
                    for (int i = 0; i < MAX_CP_DIFF; i++)
                    {
                        if (i == HORDE_ATTACK)
                            m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                        else
                            m_TB->SendUpdateWorldState(m_WorldState + i, 0);
                    }
                }
                else
                {
                    for (int i = 0; i < MAX_CP_DIFF; i++)
                    {
                        if (i == ALLIANCE_ATTACK)
                            m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                        else
                            m_TB->SendUpdateWorldState(m_WorldState + i, 0);
                    }
                }

                m_TB->SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_WORKSHOP_ATTACK, sObjectMgr->GetTrinityStringForDBCLocale(m_NameId),
                    sObjectMgr->GetTrinityStringForDBCLocale(m_TeamControl ? BATTLEFIELD_TB_TEXT_ALLIANCE:BATTLEFIELD_TB_TEXT_HORDE));
                break;
            }
        case BATTLEFIELD_TB_TEAM_ALLIANCE:
            {
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == ALLIANCE_DEFENCE)
                        m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                    else
                        m_TB->SendUpdateWorldState(m_WorldState + i, 0);
                }

                m_State = BATTLEFIELD_TB_OBJECTSTATE_ALLIANCE_INTACT;

                if (!init)
                    m_TB->SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_WORKSHOP_TAKEN, sObjectMgr->GetTrinityStringForDBCLocale(m_NameId),
                    sObjectMgr->GetTrinityStringForDBCLocale(BATTLEFIELD_TB_TEXT_ALLIANCE));

                m_TeamControl = team;
                break;
            }
        case BATTLEFIELD_TB_TEAM_HORDE:
            {
                for (int i = 0; i < MAX_CP_DIFF; i++)
                {
                    if (i == HORDE_DEFENCE)
                        m_TB->SendUpdateWorldState(m_WorldState + i, 1);
                    else
                        m_TB->SendUpdateWorldState(m_WorldState + i, 0);
                }

                m_State = BATTLEFIELD_TB_OBJECTSTATE_HORDE_INTACT;

                if (!init)
                    m_TB->SendWarningToAllInZone(BATTLEFIELD_TB_TEXT_WORKSHOP_TAKEN, sObjectMgr->GetTrinityStringForDBCLocale(m_NameId),
                    sObjectMgr->GetTrinityStringForDBCLocale(BATTLEFIELD_TB_TEXT_HORDE));

                m_TeamControl = team;
                break;
            }
        }
    }

    void UpdateWorkshop()
    {
        ChangeControl(m_TB->GetDefenderTeam(), true);
    }

    void Save()
    {
        sWorld->setWorldState(m_WorldState, m_State);
    }
};

enum Quests
{
    QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_A   = 28882,
    QUEST_TB_TOL_BARAD_VICTORY_IN_TOL_BARAD_H   = 28884,

};


#endif
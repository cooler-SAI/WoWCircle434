 /*
 *
 * Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
 *
 */

#include "MovementGenerator.h"
#include "DestinationHolder.h"
#include "WaypointManager.h"
#include "Traveller.h"
#include "ScriptSystem.h"

#ifndef SC_VEHICLE_ESCORTAI_H
#define SC_VEHICLE_ESCORTAI_H

#define TIMEDIFF_NEXT_WP      250

struct  VehicleEscortWaypoint
{
    VehicleEscortWaypoint(uint32 _id, float _x, float _y, float _z, uint32 _w, bool _j)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
        WaitTimeMs = _w;
        jump = _j;
    }

    uint32 id;
    float x;
    float y;
    float z;
    uint32 WaitTimeMs;
    bool jump;
};

enum eVehicleEscortState
{
    VEHICLE_STATE_NONE = 0x01,
    VEHICLE_STATE_ESCORTING = 0x02,
    VEHICLE_STATE_JUMPING = 0x04,
    VEHICLE_STATE_PAUSED = 0x08,
};

struct npc_vehicle_escortAI : public ScriptedAI
{
public:
    explicit npc_vehicle_escortAI(Creature* creature);

    ~npc_vehicle_escortAI() { }

    void UpdateAI(const uint32);

    virtual void MovementInform(uint32, uint32);

    void AddWaypoint(uint32 id, float x, float y, float z, uint32 WaitTimeMs = 0, bool jump = false);

    virtual void WaypointReached(uint32 uiPointId) = 0;
    virtual void FinishEscort() { }
    virtual void StartMoveTo(float x, float y, float z) { }

    void JustRespawned();

    void Start(bool bRun = false, bool repeat = false);

    void SetRun(bool bRun = true);

    void SetRepeat(bool repeat) { Repeat = repeat; }

    void SetEscortPaused(bool uPaused);

    bool HasEscortState(uint32 uiEscortState) { return (uiVehicleEscortState & uiEscortState); }

    void SetSpeedXY(float speed) { speedXY = speed; }

    void SetSpeedZ(float speed) { speedZ = speed; }

    void AddEscortState(uint32 uiEscortState) { uiVehicleEscortState |= uiEscortState; }

    void RemoveEscortState(uint32 uiEscortState) { uiVehicleEscortState &= ~uiEscortState; }

    uint32 GetEscortState() { return uiVehicleEscortState; }

    bool GetDestination(float &x, float &y, float &z) const;

    MovementGeneratorType GetMovementGeneratorType() { return WAYPOINT_MOTION_TYPE; }

private:

    DestinationHolder< Traveller<Creature> > destinationHolder;
    TimeTrackerSmall nextMoveTime;
    uint32 uiVehicleEscortState;
    uint32 CurrentWP_Id;
    float speedXY;
    float speedZ;
    bool IsRunning;
    bool Repeat;

    std::list< VehicleEscortWaypoint> WaypointList;
    std::list< VehicleEscortWaypoint>::iterator CurrentWP;
};
#endif

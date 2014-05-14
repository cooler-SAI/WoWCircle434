/*
*
* Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
*
*/

#include "ScriptPCH.h"
#include "ScriptedVehicleEscortAI.h"
#include "DestinationHolderImp.h"

npc_vehicle_escortAI::npc_vehicle_escortAI(Creature* creature) : ScriptedAI(creature),
uiVehicleEscortState(VEHICLE_STATE_NONE),
CurrentWP_Id(0),
IsRunning(false),
speedXY(25.0f),
speedZ(10.0f),
nextMoveTime(0),
Repeat(false)
{ }

void npc_vehicle_escortAI::JustRespawned()
{
    float x, y, z, o;
    me->GetRespawnPosition(x, y, z, &o);
    me->SetHomePosition(x, y, z, o);
    me->Relocate(x, y, z, o);
}

bool npc_vehicle_escortAI::GetDestination(float &x, float &y, float &z) const
{
    if (destinationHolder.HasArrived())
        return false;

    destinationHolder.GetDestination(x, y, z);
    return true;
}

void npc_vehicle_escortAI::Start(bool run, bool repeat)
{
    run ? me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING) : me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
    me->StopMoving();
    IsRunning = run;
    Repeat = repeat;
    uiVehicleEscortState = VEHICLE_STATE_ESCORTING;

    if (!WaypointList.empty())
    {
        me->AddUnitState(UNIT_STATE_ROAMING);
        me->AddUnitState(UNIT_STATE_MOVING);
        CurrentWP = WaypointList.begin();
        CurrentWP_Id = 0;
        Traveller<Creature> traveller(*me);
        destinationHolder.SetDestination(traveller, CurrentWP->x, CurrentWP->y, CurrentWP->z);
        StartMoveTo(CurrentWP->x, CurrentWP->y, CurrentWP->z);
        nextMoveTime.Reset(destinationHolder.GetTotalTravelTime());
    } else
        sLog->outError(LOG_FILTER_TSCR, "TSCR: VehicleEscortAI try to start when WaypointList is empty");
}

void npc_vehicle_escortAI::UpdateAI(const uint32 diff)
{
    if (GetEscortState() != VEHICLE_STATE_ESCORTING)
        return;

    if (me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return;

    if (WaypointList.empty())
        return;

    Traveller<Creature> traveller(*me);
    nextMoveTime.Update(diff);
    destinationHolder.UpdateTraveller(traveller, diff, true);

    if (nextMoveTime.GetExpiry() < TIMEDIFF_NEXT_WP)
    {
        if (me->IsStopped())
        {
            if (CurrentWP_Id == WaypointList.size() - 1)
            {
                me->SetHomePosition(CurrentWP->x, CurrentWP->y, CurrentWP->z, me->GetOrientation());
                me->GetMotionMaster()->Initialize();

                if (!Repeat)
                {
                    WaypointList.clear();
                    FinishEscort();
                    return;
                } else
                    Start(IsRunning, Repeat);
            } else
            {
                ++CurrentWP;
                ++CurrentWP_Id;
            }

            IsRunning ? me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING) : me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);

            if (CurrentWP->jump)
            {
                me->GetMotionMaster()->MoveJump(CurrentWP->x, CurrentWP->y, CurrentWP->z, speedXY, speedZ, CurrentWP->id);
                StartMoveTo(CurrentWP->x, CurrentWP->y, CurrentWP->z);
                AddEscortState(VEHICLE_STATE_JUMPING);
            } else
            {
                me->AddUnitState(UNIT_STATE_ROAMING);
                me->AddUnitState(UNIT_STATE_MOVING);
                destinationHolder.SetDestination(traveller, CurrentWP->x, CurrentWP->y, CurrentWP->z);
                StartMoveTo(CurrentWP->x, CurrentWP->y, CurrentWP->z);
                nextMoveTime.Reset(destinationHolder.GetTotalTravelTime());
            }
        } else
        {
            if (CurrentWP->WaitTimeMs)
                nextMoveTime.Reset(CurrentWP->WaitTimeMs);

            destinationHolder.ResetTravelTime();
            me->ClearUnitState(UNIT_STATE_ROAMING);
            me->ClearUnitState(UNIT_STATE_MOVING);
            me->Relocate(CurrentWP->x, CurrentWP->y, CurrentWP->z);
            MovementInform(WAYPOINT_MOTION_TYPE, CurrentWP->id);
        }
    }

    return;
}

void npc_vehicle_escortAI::MovementInform(uint32 uiMoveType, uint32 uiPointId)
{
    if (uiMoveType == WAYPOINT_MOTION_TYPE)
        WaypointReached(uiPointId);

    if (HasEscortState(VEHICLE_STATE_JUMPING))
    {
        WaypointReached(uiPointId);
        RemoveEscortState(VEHICLE_STATE_JUMPING);
    }
}

void npc_vehicle_escortAI::AddWaypoint(uint32 id, float x, float y, float z, uint32 WaitTimeMs, bool jump)
{
    VehicleEscortWaypoint t(id, x, y, z, WaitTimeMs, jump);
    WaypointList.push_back(t);
}

void npc_vehicle_escortAI::SetRun(bool bRun)
{
    if (bRun)
    {
        if (!IsRunning)
            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
        else
            sLog->outDebug(LOG_FILTER_TSCR, "TSCR: VehicleEscortAI attempt to set run mode, but is already running.");
    } else
    {
        if (IsRunning)
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
        else
            sLog->outDebug(LOG_FILTER_TSCR, "TSCR: VehicleEscortAI attempt to set walk mode, but is already walking.");
    }
    IsRunning = bRun;
}

void npc_vehicle_escortAI::SetEscortPaused(bool bPaused)
{
    if (!HasEscortState(VEHICLE_STATE_ESCORTING))
        return;

    if (bPaused)
        AddEscortState(VEHICLE_STATE_PAUSED);
    else
        RemoveEscortState(VEHICLE_STATE_PAUSED);
}

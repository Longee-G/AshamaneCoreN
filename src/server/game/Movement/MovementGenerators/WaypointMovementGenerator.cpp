/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WaypointMovementGenerator.h"
#include "CreatureAI.h"
#include "CreatureGroups.h"
#include "Log.h"
#include "MapManager.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "ObjectMgr.h"
#include "Transport.h"
#include "WaypointManager.h"
#include "World.h"

void WaypointMovementGenerator<Creature>::LoadPath(Creature* creature)
{
    if (_isLoadedFromDB)
    {
        if (!_pathId)
            _pathId = creature->GetWaypointPath();

        _path = sWaypointMgr->GetPath(_pathId);
    }

    if (!_path)
    {
        // No path id found for entry
        TC_LOG_ERROR("sql.sql", "WaypointMovementGenerator::LoadPath: creature %s (%s DB GUID: " UI64FMTD ") doesn't have waypoint path id: %u", creature->GetName().c_str(), creature->GetGUID().ToString().c_str(), creature->GetSpawnId(), _pathId);
        return;
    }

    if (!Stopped())
        StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::DoInitialize(Creature* creature)
{
    LoadPath(creature);
}

void WaypointMovementGenerator<Creature>::DoFinalize(Creature* creature)
{
    creature->ClearUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE);
    creature->SetWalk(false);
}

void WaypointMovementGenerator<Creature>::DoReset(Creature* creature)
{
    if (!Stopped())
        StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::OnArrived(Creature* creature)
{
    if (!_path || _path->nodes.empty())
        return;

    WaypointNode const &waypoint = _path->nodes.at(_currentNode);
    if (waypoint.delay)
    {
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        Stop(waypoint.delay);
    }

    if (waypoint.eventId && urand(0, 99) < waypoint.eventChance)
    {
        TC_LOG_DEBUG("maps.script", "Creature movement start script %u at point %u for %s.", waypoint.eventId, _currentNode, creature->GetGUID().ToString().c_str());
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        creature->GetMap()->ScriptsStart(sWaypointScripts, waypoint.eventId, creature, nullptr);
    }

    // Inform script
    MovementInform(creature);

    // [WIP]
    if (CreatureAI* AI = creature->AI())
        AI->OnWaypointReached(waypoint.id, _path->id);   // 当某path的某个waypoint到底目的地


    creature->UpdateWaypointID(_currentNode);

    creature->SetWalk(waypoint.moveType != WAYPOINT_MOVE_TYPE_RUN);
}

void WaypointMovementGenerator<Creature>::FormationMove(Creature* creature)
{
    bool transportPath = creature->GetTransport() != nullptr;

    WaypointNode const &waypoint = _path->nodes.at(_currentNode);

    Movement::Location formationDest(waypoint.x, waypoint.y, waypoint.z, 0.0f);

    //! If creature is on transport, we assume waypoints set in DB are already transport offsets
    if (transportPath)
    {
        if (TransportBase* trans = creature->GetDirectTransport())
            trans->CalculatePassengerPosition(formationDest.x, formationDest.y, formationDest.z, &formationDest.orientation);
    }

    // Call for creature group update
    if (creature->GetFormation() && creature->GetFormation()->getLeader() == creature)
        creature->GetFormation()->LeaderMoveTo(formationDest.x, formationDest.y, formationDest.z);
}

bool WaypointMovementGenerator<Creature>::StartMove(Creature* creature)
{
    if (!creature || !creature->IsAlive())
        return false;

    if (!_path || _path->nodes.empty())
        return false;

    bool transportPath = creature->GetTransport() != nullptr;

    if (_isArrivalDone) // 标记是否已经发出arrived 通知, 这个标记用于point的到达，而不是整个path的到达
    {
        if ((_currentNode == _path->nodes.size() - 1) && !_isRepeating) // If that's our last waypoint
        {
            // get last waypoint
            WaypointNode const &waypoint = _path->nodes.at(_currentNode);

            float x = waypoint.x;
            float y = waypoint.y;
            float z = waypoint.z;
            float o = creature->GetOrientation();

            if (!transportPath)
                creature->SetHomePosition(x, y, z, o);
            else
            {
                if (Transport* trans = creature->GetTransport())
                {
                    o -= trans->GetOrientation();
                    creature->SetTransportHomePosition(x, y, z, o);
                    trans->CalculatePassengerPosition(x, y, z, &o);
                    creature->SetHomePosition(x, y, z, o);
                }
                else
                    transportPath = false;
                // else if (vehicle) - this should never happen, vehicle offsets are const
            }

            // 完成了path中的所有point
            if (!_isInformDone)
            {
                if (CreatureAI* AI = creature->AI())
                    AI->OnWaypointPathEnded(waypoint.id, _path->id);    // PathEnd 是在最后收到的 ... OnWaypointArrived

                _isInformDone = true;
            }

            return false;
        }

        _currentNode = (_currentNode + 1) % _path->nodes.size();
    }

    float finalOrient = 0.0f;
    uint8 finalMove = WAYPOINT_MOVE_TYPE_WALK;

    Movement::PointsArray pathing;
    pathing.reserve((_path->nodes.size() - _currentNode) + 1);

    pathing.push_back(G3D::Vector3(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ()));
    for (uint32 i = _currentNode; i < _path->nodes.size(); ++i)
    {
        WaypointNode const &waypoint = _path->nodes.at(i);

        pathing.push_back(G3D::Vector3(waypoint.x, waypoint.y, waypoint.z));

        finalOrient = waypoint.orientation;
        finalMove = waypoint.moveType;

        if (waypoint.delay)
            break;
    }

    // if we have only 1 point, only current position, we shall return
    if (pathing.size() < 2)
        return false;

    if (CreatureAI* AI = creature->AI())
    {
        WaypointNode const &wp = _path->nodes.at(_currentNode);
        AI->OnWaypointStarted(wp.id, _path->id);
    }

    _isInformDone = false;
    _isArrivalDone = false;
    _isRecalculateSpeed = false;

    creature->AddUnitState(UNIT_STATE_ROAMING_MOVE);

    Movement::MoveSplineInit init(creature);

    Movement::Location formationDest(_path->nodes.at(_currentNode).x, _path->nodes.at(_currentNode).y, _path->nodes.at(_currentNode).z, 0.0f);

    //! If creature is on transport, we assume waypoints set in DB are already transport offsets
    if (transportPath)
    {
        init.DisableTransportPathTransformations();
        if (TransportBase* trans = creature->GetDirectTransport())
            trans->CalculatePassengerPosition(formationDest.x, formationDest.y, formationDest.z, &formationDest.orientation);
    }
    if (finalMove == WAYPOINT_MOVE_TYPE_TAKEOFF)
        init.MoveTo(formationDest.x, formationDest.y, formationDest.z);
    else
        init.MovebyPath(pathing, _currentNode);

    switch (finalMove)
    {
        case WAYPOINT_MOVE_TYPE_LAND:
            init.SetAnimation(Movement::ToGround);
            break;
        case WAYPOINT_MOVE_TYPE_TAKEOFF:
            init.SetAnimation(Movement::ToFly);
            break;
        case WAYPOINT_MOVE_TYPE_RUN:
            init.SetWalk(false);
            break;
        case WAYPOINT_MOVE_TYPE_WALK:
            init.SetWalk(true);
            break;
    }

    if (finalOrient != 0.0f)
        init.SetFacing(finalOrient);

    init.Launch();

    //Call for creature group update
    if (creature->GetFormation() && creature->GetFormation()->getLeader() == creature)
        creature->GetFormation()->LeaderMoveTo(formationDest.x, formationDest.y, formationDest.z);

    return true;
}

bool WaypointMovementGenerator<Creature>::DoUpdate(Creature* creature, uint32 diff)
{
    if (!creature || !creature->IsAlive())
        return false;

    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (creature->HasUnitState(UNIT_STATE_NOT_MOVE))
    {
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        return true;
    }

    // prevent a crash at empty waypoint path.
    if (!_path || _path->nodes.empty())
        return false;

    if (Stopped())
    {
        if (CanMove(diff))
            return StartMoveNow(creature);
    }
    else
    {
        // Set home position at place on waypoint movement.
        if (!creature->GetTransGUID())
            creature->SetHomePosition(creature->GetPosition());

        if (creature->IsStopped())
            Stop(_isLoadedFromDB ? sWorld->getIntConfig(CONFIG_CREATURE_STOP_FOR_PLAYER) : 2 * HOUR * IN_MILLISECONDS);
        else if (creature->movespline->Finalized())
        {
            OnArrived(creature);

            _isArrivalDone = true;

            if (!Stopped())
            {
                if (creature->IsStopped())
                    Stop(_isLoadedFromDB ? sWorld->getIntConfig(CONFIG_CREATURE_STOP_FOR_PLAYER) : 2 * HOUR * IN_MILLISECONDS);
                else
                    return StartMove(creature);
            }
        }
        else
        {
            // speed changed during path execution, calculate remaining path and launch it once more
            if (_isRecalculateSpeed)
            {
                _isRecalculateSpeed = false;

                if (!Stopped())
                    return StartMove(creature);
            }
            else
            {
                uint32 pointId = uint32(creature->movespline->currentPathIdx());
                if (pointId > _currentNode)
                {
                    OnArrived(creature);
                    _currentNode = pointId;
                    FormationMove(creature);
                }
            }
        }
    }
    return true;
}

void WaypointMovementGenerator<Creature>::MovementInform(Creature* creature)
{
    if (creature->AI())
        creature->AI()->MovementInform(WAYPOINT_MOTION_TYPE, _currentNode);
}

bool WaypointMovementGenerator<Creature>::GetResetPos(Creature*, float& x, float& y, float& z)
{
    // prevent a crash at empty waypoint path.
    if (!_path || _path->nodes.empty())
        return false;

    WaypointNode const &waypoint = _path->nodes.at(_currentNode);

    x = waypoint.x;
    y = waypoint.y;
    z = waypoint.z;
    return true;
}


//----------------------------------------------------//

uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (_currentNode >= _path.size())
        return _path.size();

    uint32 curMapId = _path[_currentNode]->ContinentID;
    for (uint32 i = _currentNode; i < _path.size(); ++i)
        if (_path[i]->ContinentID != curMapId)
            return i;

    return _path.size();
}

#define SKIP_SPLINE_POINT_DISTANCE_SQ (40.0f * 40.0f)

bool IsNodeIncludedInShortenedPath(TaxiPathNodeEntry const* p1, TaxiPathNodeEntry const* p2)
{
    return p1->ContinentID != p2->ContinentID || std::pow(p1->Loc.X - p2->Loc.X, 2) + std::pow(p1->Loc.Y - p2->Loc.Y, 2) > SKIP_SPLINE_POINT_DISTANCE_SQ;
}

void FlightPathMovementGenerator::LoadPath(Player* player, uint32 startNode /*= 0*/)
{
    _path.clear();
    _currentNode = startNode;
    _pointsForPathSwitch.clear();
    std::deque<uint32> const& taxi = player->_taxi.GetPath();
    float discount = player->GetReputationPriceDiscount(player->_taxi.GetFlightMasterFactionTemplate());
    for (uint32 src = 0, dst = 1; dst < taxi.size(); src = dst++)
    {
        uint32 path, cost;
        sObjectMgr->GetTaxiPath(taxi[src], taxi[dst], path, cost);
        if (path > sTaxiPathNodesByPath.size())
            return;

        TaxiPathNodeList const& nodes = sTaxiPathNodesByPath[path];
        if (!nodes.empty())
        {
            TaxiPathNodeEntry const* start = nodes[0];
            TaxiPathNodeEntry const* end = nodes[nodes.size() - 1];
            bool passedPreviousSegmentProximityCheck = false;
            for (uint32 i = 0; i < nodes.size(); ++i)
            {
                if (passedPreviousSegmentProximityCheck || !src || _path.empty() || IsNodeIncludedInShortenedPath(_path.back(), nodes[i]))
                {
                    if ((!src || (IsNodeIncludedInShortenedPath(start, nodes[i]) && i >= 2)) &&
                        (dst == taxi.size() - 1 || (IsNodeIncludedInShortenedPath(end, nodes[i]) && i < nodes.size() - 1)))
                    {
                        passedPreviousSegmentProximityCheck = true;
                        _path.push_back(nodes[i]);
                    }
                }
                else
                {
                    _path.pop_back();
                    --_pointsForPathSwitch.back().PathIndex;
                }
            }
        }

        _pointsForPathSwitch.push_back({ uint32(_path.size() - 1), int64(ceil(cost * discount)) });
    }
}

void FlightPathMovementGenerator::DoInitialize(Player* player)
{
    Reset(player);
    InitEndGridInfo();
}

void FlightPathMovementGenerator::DoFinalize(Player* player)
{
    // remove flag to prevent send object build movement packets for flight state and crash (movement generator already not at top of stack)
    player->ClearUnitState(UNIT_STATE_IN_FLIGHT);

    player->Dismount();
    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    if (player->_taxi.empty())
    {
        player->getHostileRefManager().setOnlineOfflineState(true);
        // update z position to ground and orientation for landing point
        // this prevent cheating with landing  point at lags
        // when client side flight end early in comparison server side
        player->StopMoving();
    }

    player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK);
    player->RestoreDisplayId();
}

#define PLAYER_FLIGHT_SPEED 30.0f

void FlightPathMovementGenerator::DoReset(Player* player)
{
    player->getHostileRefManager().setOnlineOfflineState(false);
    player->AddUnitState(UNIT_STATE_IN_FLIGHT);
    player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    Movement::MoveSplineInit init(player);
    uint32 end = GetPathAtMapEnd();

    if (_currentNode > end)
        _currentNode = end;

    for (uint32 i = _currentNode; i != end; ++i)
    {
        G3D::Vector3 vertice(_path[i]->Loc.X, _path[i]->Loc.Y, _path[i]->Loc.Z);
        init.Path().push_back(vertice);
    }
    init.SetFirstPointId(_currentNode);
    init.SetFly();
    init.SetSmooth();
    init.SetUncompressed();
    init.SetWalk(true);
    init.SetVelocity(PLAYER_FLIGHT_SPEED * player->GetTotalAuraMultiplier(SPELL_AURA_MOD_TAXI_FLIGHT_SPEED));
    init.Launch();
}

bool FlightPathMovementGenerator::DoUpdate(Player* player, uint32 /*diff*/)
{
    uint32 pointId = (uint32)player->movespline->currentPathIdx();
    if (pointId > _currentNode)
    {
        bool departureEvent = true;
        do
        {
            DoEventIfAny(player, _path[_currentNode], departureEvent);
            while (!_pointsForPathSwitch.empty() && _pointsForPathSwitch.front().PathIndex <= _currentNode)
            {
                _pointsForPathSwitch.pop_front();
                player->_taxi.NextTaxiDestination();
                if (!_pointsForPathSwitch.empty())
                {
                    player->UpdateCriteria(CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING, _pointsForPathSwitch.front().Cost);
                    player->ModifyMoney(-_pointsForPathSwitch.front().Cost);
                }
            }

            if (pointId == _currentNode)
                break;

            if (_currentNode == _preloadTargetNode)
                PreloadEndGrid();
            _currentNode += (uint32)departureEvent;
            departureEvent = !departureEvent;
        }
        while (true);
    }

    return _currentNode < (_path.size() - 1);
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (_path.empty() || _currentNode >= _path.size())
        return;

    uint32 map0 = _path[_currentNode]->ContinentID;
    for (size_t i = _currentNode + 1; i < _path.size(); ++i)
    {
        if (_path[i]->ContinentID != map0)
        {
            _currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::DoEventIfAny(Player* player, TaxiPathNodeEntry const* node, bool departure)
{
    if (uint32 eventid = departure ? node->DepartureEventID : node->ArrivalEventID)
    {
        TC_LOG_DEBUG("maps.script", "Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node->NodeIndex, node->PathID, player->GetName().c_str());
        player->GetMap()->ScriptsStart(sEventScripts, eventid, player, player);
    }
}

bool FlightPathMovementGenerator::GetResetPos(Player*, float& x, float& y, float& z)
{
    TaxiPathNodeEntry const* node = _path[_currentNode];
    x = node->Loc.X;
    y = node->Loc.Y;
    z = node->Loc.Z;
    return true;
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    /*! Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
       be reinitialized for each flightmaster at the end of each spline (or stop) in the flight. */
    uint32 nodeCount = _path.size();        //! Number of nodes in path.
    _endMapId = _path[nodeCount - 1]->ContinentID; //! MapId of last node
    _preloadTargetNode = nodeCount - 3;
    _endGridX = _path[nodeCount - 1]->Loc.X;
    _endGridY = _path[nodeCount - 1]->Loc.Y;
}

void FlightPathMovementGenerator::PreloadEndGrid()
{
    // used to preload the final grid where the flightmaster is
    Map* endMap = sMapMgr->FindBaseNonInstanceMap(_endMapId);

    // Load the grid
    if (endMap)
    {
        TC_LOG_DEBUG("misc", "Preloading rid (%f, %f) for map %u at node index %u/%u", _endGridX, _endGridY, _endMapId, _preloadTargetNode, (uint32)(_path.size() - 1));
        endMap->LoadGrid(_endGridX, _endGridY);
    }
    else
        TC_LOG_DEBUG("misc", "Unable to determine map to preload flightmaster grid");
}

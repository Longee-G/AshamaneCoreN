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

#ifndef TRINITY_WAYPOINTMOVEMENTGENERATOR_H
#define TRINITY_WAYPOINTMOVEMENTGENERATOR_H

/** @page PathMovementGenerator is used to generate movements
 * of waypoints and flight paths.  Each serves the purpose
 * of generate activities so that it generates updated
 * packets for the players.
 */

#include "MovementGenerator.h"
#include "Creature.h"
#include "DB2Stores.h"
#include "Player.h"
#include "Timer.h"

#define FLIGHT_TRAVEL_UPDATE  100
#define TIMEDIFF_NEXT_WP      250

template<class T, class P>
class PathMovementBase
{
public:
    PathMovementBase() : _path(), _currentNode(0) { }
    virtual ~PathMovementBase() { };

    uint32 GetCurrentNode() const { return _currentNode; }

protected:
    P _path;
    uint32 _currentNode;    // _currentNode != `waypoint_data.point`
};

template<class T>
class WaypointMovementGenerator;

template<>
class WaypointMovementGenerator<Creature> : public MovementGeneratorMedium< Creature, WaypointMovementGenerator<Creature> >,
    public PathMovementBase<Creature, WaypointPath const*>
{
    public:
        WaypointMovementGenerator(uint32 path_id = 0, bool repeating = true)
            : _nextMoveTime(0), _isArrivalDone(false), _pathId(path_id), _isRepeating(repeating), _isLoadedFromDB(true)  { }

        WaypointMovementGenerator(WaypointPath& path, bool repeating = true)
            : _nextMoveTime(0), _isArrivalDone(false), _pathId(0), _isRepeating(repeating), _isLoadedFromDB(false)
        {
            _path = &path;
        }

        ~WaypointMovementGenerator() { _path = nullptr; }

        void DoInitialize(Creature*);
        void DoFinalize(Creature*);
        void DoReset(Creature*);
        bool DoUpdate(Creature*, uint32 diff);

        void MovementInform(Creature*);

        MovementGeneratorType GetMovementGeneratorType() const override { return WAYPOINT_MOTION_TYPE; }

        // now path movement implmementation
        void LoadPath(Creature*);

        bool GetResetPos(Creature*, float& x, float& y, float& z);

        TimeTrackerSmall & GetTrackerTimer() { return _nextMoveTime; }

        void UnitSpeedChanged() { _isRecalculateSpeed = true; }

    private:

        void Stop(int32 time) { _nextMoveTime.Reset(time);}

        bool Stopped() { return !_nextMoveTime.Passed();}

        bool CanMove(int32 diff)
        {
            _nextMoveTime.Update(diff);
            return _nextMoveTime.Passed();
        }

        void OnArrived(Creature*);
        bool StartMove(Creature*);
        void FormationMove(Creature*);

        bool StartMoveNow(Creature* creature)
        {
            _nextMoveTime.Reset(0);
            return StartMove(creature);
        }

        TimeTrackerSmall _nextMoveTime;
        uint32 _pathId;
        bool _isRecalculateSpeed;
        bool _isArrivalDone;        
        bool _isRepeating;
        bool _isLoadedFromDB;
};

/** FlightPathMovementGenerator generates movement of the player for the paths
 * and hence generates ground and activities for the player.
 */
class FlightPathMovementGenerator : public MovementGeneratorMedium< Player, FlightPathMovementGenerator >,
    public PathMovementBase<Player, TaxiPathNodeList>
{
    public:
        explicit FlightPathMovementGenerator()
        {
            _currentNode = 0;
            _endGridX = 0.0f;
            _endGridY = 0.0f;
            _endMapId = 0;
            _preloadTargetNode = 0;
        }
        void LoadPath(Player* player, uint32 startNode = 0);
        void DoInitialize(Player*);
        void DoReset(Player*);
        void DoFinalize(Player*);
        bool DoUpdate(Player*, uint32);
        MovementGeneratorType GetMovementGeneratorType() const override { return FLIGHT_MOTION_TYPE; }

        TaxiPathNodeList const& GetPath() { return _path; }
        uint32 GetPathAtMapEnd() const;
        bool HasArrived() const { return (_currentNode >= _path.size()); }
        void SetCurrentNodeAfterTeleport();
        void SkipCurrentNode() { ++_currentNode; }
        void DoEventIfAny(Player* player, TaxiPathNodeEntry const* node, bool departure);

        bool GetResetPos(Player*, float& x, float& y, float& z);

        void InitEndGridInfo();
        void PreloadEndGrid();

    private:

        float _endGridX;                            //! X coord of last node location
        float _endGridY;                            //! Y coord of last node location
        uint32 _endMapId;                           //! map Id of last node location
        uint32 _preloadTargetNode;                  //! node index where preloading starts

        struct TaxiNodeChangeInfo
        {
            uint32 PathIndex;
            int64 Cost;
        };

        std::deque<TaxiNodeChangeInfo> _pointsForPathSwitch;    //! node indexes and costs where TaxiPath changes
};
#endif

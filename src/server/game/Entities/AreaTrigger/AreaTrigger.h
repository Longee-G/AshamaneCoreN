﻿/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
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

#ifndef TRINITYCORE_AREATRIGGER_H
#define TRINITYCORE_AREATRIGGER_H

#include "Object.h"
#include "MapObject.h"
#include "AreaTriggerTemplate.h"

class AuraEffect;
class AreaTriggerAI;
class SpellInfo;
class Unit;

namespace G3D
{
    class Vector2;
    class Vector3;
}
namespace Movement
{
    template<typename length_type>
    class Spline;
}

typedef std::list<AreaTrigger*> AreaTriggerList;

// 区域触发器...
class TC_GAME_API AreaTrigger : public WorldObject, public GridObject<AreaTrigger>, public MapObject
{
public:
    AreaTrigger();
    ~AreaTrigger();

    void AddToWorld() override;
    void RemoveFromWorld() override;

    void AI_Initialize();
    void AI_Destroy();

    AreaTriggerAI* AI() { return _ai.get(); }

private:
    bool Create(uint32 spellMiscId, Unit* caster, Unit* target, SpellInfo const* spell, Position const& pos, int32 duration, uint32 spellXSpellVisualId, ObjectGuid const& castId, AuraEffect const* aurEff);
    bool CreateStaticAreaTrigger(uint32 entry, ObjectGuid::LowType guidLow, Position const& p_Pos, Map* p_Map, uint32 scriptId = 0);
public:
    static AreaTrigger* CreateAreaTrigger(uint32 spellMiscId, Unit* caster, Unit* target, SpellInfo const* spell, Position const& pos, int32 duration, uint32 spellXSpellVisualId, ObjectGuid const& castId = ObjectGuid::Empty, AuraEffect const* aurEff = nullptr);
    bool LoadFromDB(ObjectGuid::LowType guid, Map* map);

    void Update(uint32 diff) override;
    void Remove();
    bool IsRemoved() const { return _isRemoved; }
    uint32 GetSpellId() const { return GetUInt32Value(AREATRIGGER_SPELLID); }
    AuraEffect const* GetAuraEffect() const { return _aurEff; }
    uint32 GetTimeSinceCreated() const { return _timeSinceCreated; }
    uint32 GetTimeToTarget() const { return GetUInt32Value(AREATRIGGER_TIME_TO_TARGET); }
    uint32 GetTimeToTargetScale() const { return GetUInt32Value(AREATRIGGER_TIME_TO_TARGET_SCALE); }
    void UpdateTimeToTarget(uint32 timeToTarget);
    int32 GetDuration() const { return _duration; }
    int32 GetTotalDuration() const { return _totalDuration; }
    void SetDuration(int32 newDuration);
    void Delay(int32 delaytime) { SetDuration(GetDuration() - delaytime); }
    void SetPeriodicProcTimer(uint32 periodicProctimer) { _basePeriodicProcTimer = periodicProctimer; _periodicProcTimer = periodicProctimer; }

    GuidUnorderedSet const& GetInsideUnits() const { return _insideUnits; }

    AreaTriggerMiscTemplate const* GetMiscTemplate() const { return _areaTriggerMiscTemplate; }
    AreaTriggerTemplate const* GetTemplate() const;
    ObjectGuid::LowType GetSpawnId() const { return _spawnId; }
    uint32 GetScriptId() const;

    ObjectGuid const& GetCasterGuid() const { return GetGuidValue(AREATRIGGER_CASTER); }
    Unit* GetCaster() const;
    Unit* GetTarget() const;

    Position const& GetRollPitchYaw() const;
    Position const& GetTargetRollPitchYaw() const;
    void InitSplineOffsets(std::vector<Position> const& offsets, uint32 timeToTarget);
    void InitSplines(std::vector<G3D::Vector3> splinePoints, uint32 timeToTarget);
    bool HasSplines() const;
    bool SetDestination(Position const& pos, uint32 timeToTarget);
    ::Movement::Spline<int32> const& GetSpline() const { return *_spline; }
    uint32 GetElapsedTimeForMovement() const { return GetTimeSinceCreated(); } /// @todo: research the right value, in sniffs both timers are nearly identical

    void InitCircularMovement(AreaTriggerCircularMovementInfo const& cmi, uint32 timeToTarget);
    bool HasCircularMovement() const;
    Optional<AreaTriggerCircularMovementInfo> const& GetCircularMovementInfo() const { return _circularMovementInfo; }

    void UpdateShape();

protected:
    void _UpdateDuration(int32 newDuration);
    float GetProgress() const;

    void UpdateTargetList();
    void SearchUnitInSphere(std::list<Unit*>& targetList);
    void SearchUnitInBox(std::list<Unit*>& targetList);
    void SearchUnitInPolygon(std::list<Unit*>& targetList);
    void SearchUnitInCylinder(std::list<Unit*>& targetList);
    bool CheckIsInPolygon2D(Position const* pos) const;
    void HandleUnitEnterExit(std::list<Unit*> const& targetList);

    float GetCurrentTimePercent();

    void DoActions(Unit* unit);
    void UndoActions(Unit* unit);

    void UpdatePolygonOrientation();
    void UpdateCircularMovementPosition(uint32 diff);
    void UpdateSplinePosition(uint32 diff);

    Position const* GetCircularMovementCenterPosition() const;
    Position CalculateCircularMovementPosition() const;

    void DebugVisualizePosition(); // Debug purpose only

    ObjectGuid _targetGuid;

    AuraEffect const* _aurEff;

    int32 _duration;
    int32 _totalDuration;
    uint32 _timeSinceCreated;
    uint32 _periodicProcTimer;
    uint32 _basePeriodicProcTimer;
    float _previousCheckOrientation;
    bool _isBeingRemoved;
    bool _isRemoved;

    std::vector<Position> _polygonVertices;
    std::unique_ptr<::Movement::Spline<int32>> _spline;

    bool _reachedDestination;
    int32 _lastSplineIndex;
    uint32 _movementTime;

    AreaTriggerTemplate const* _areaTriggerTemplate;
    Optional<AreaTriggerCircularMovementInfo> _circularMovementInfo;
    AreaTriggerMiscTemplate const* _areaTriggerMiscTemplate;
    GuidUnorderedSet _insideUnits;

    ObjectGuid::LowType _spawnId;
    uint32 _guidScriptId;
    std::unique_ptr<AreaTriggerAI> _ai;

    ObjectGuid _circularMovementCenterGUID;
    Position _circularMovementCenterPosition;
};

#endif

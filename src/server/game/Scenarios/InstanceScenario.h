/*
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

#ifndef InstanceScenario_h__
#define InstanceScenario_h__

#include "Scenario.h"

class Map;

typedef std::unordered_map<uint8, CriteriaProgressMap> StepCriteriaProgressMap;

// `场景战役` 副本？
// 这个类在什么地方使用

// 这个类应该叫 `副本剧情` 关联副本中的剧情数据的
class TC_GAME_API InstanceScenario : public Scenario
{
public:
    InstanceScenario(Map const* map, ScenarioData const* scenarioData);

    void SaveToDB();
    void LoadInstanceData(uint32 instanceId);

    void CompleteScenario() override;

protected:
    std::string GetOwnerInfo() const override;
    void SendPacket(WorldPacket const* data) const override;

    Map const* _map;
    ScenarioData const* _data;
    StepCriteriaProgressMap _stepCriteriaProgress;
};

#endif // InstanceScenario_h__

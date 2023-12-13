/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
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
#include "broken_shore_scenario.h"

// spell:217781     `phase update`

// 场景动画和脚本的关联在[world.scene_template]中

// ScriptPackageID: 1531 `Broken Shore Scenario - Alliance - Intro`
// ScriptPackageID: 1716 `Broken Shore Scenario - Horde - Intro`

// 这个cutscene的动画是由谁来触发的呢？
class scene_broken_shore_intro : public SceneScript
{
public:
    scene_broken_shore_intro() : SceneScript("scene_broken_shore_intro") {}
    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName) override
    {
    }
    void OnSceneEnd(Player* /*player*/, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
    }
};

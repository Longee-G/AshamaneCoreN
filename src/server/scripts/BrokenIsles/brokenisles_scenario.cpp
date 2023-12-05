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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "LFGMgr.h"
#include "Group.h"

// Horder NPC - Captain Russo [113118]
// quest [40518]
// quest objective [113118]
class npc_captain_russo : public CreatureScript
{
public:
    npc_captain_russo() : CreatureScript("npc_captain_russo") {}
    // 
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->playerTalkClass->ClearMenus();
        
        // make quest objective completed 
        player->KilledMonsterCredit(113118);
        // play cutscene
        player->CastSpell(player, 225147, false);

        CloseGossipMenuFor(player);
        return true;
    }
};

// Alliance NPC - Captain angelica[108920]
class npc_captain_angelica : public CreatureScript
{
public:
    npc_captain_angelica() : CreatureScript("npc_captain_angelica") {}
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->playerTalkClass->ClearMenus();
        player->KilledMonsterCredit(108920);        // angelica
        player->CastSpell(player, 216356, false);   // alliance spell
        CloseGossipMenuFor(player);
        return true;
    }
};

// 获取角色的类型
uint8 GetRoleMask(Player* player)
{
    uint8 roleMask = lfg::LfgRoles::PLAYER_ROLE_NONE;
    if (auto const& group = player->GetGroup())
        if (group->GetLeaderGUID() == player->GetGUID())
            roleMask |= lfg::LfgRoles::PLAYER_ROLE_LEADER;


    // TODO:
    return roleMask | lfg::LfgRoles::PLAYER_ROLE_DAMAGE;
}


// 当播放完cutscene动画之后，将player加入到场景战役的副本队列中...
class scene_face_the_legion : public SceneScript
{
public:
    scene_face_the_legion() : SceneScript("scene_face_the_legion") {}
    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        // The Battle For Broken Shore [908] from `LFGDungeons.db2`
        lfg::LfgDungeonSet dungeons = { 908 };

        sLFGMgr->JoinLfg(player, GetRoleMask(player), dungeons);
    }
};



void AddSC_scenario_scripts()
{
    new npc_captain_russo();
    //new npc_captain_angelica();

    new scene_face_the_legion();
}

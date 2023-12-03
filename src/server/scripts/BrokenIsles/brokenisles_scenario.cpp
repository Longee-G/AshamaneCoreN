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


// Captain Russo [113118]
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
        // 通过spell来切换
        player->CastSpell(player, 225147, false);
        return true;
    }

};

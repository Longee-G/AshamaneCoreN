/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Shadowfang_Keep
SD%Complete: 0%
SDComment:
SDCategory: Shadowfang Keep
EndScriptData */

/* ContentData
npc_shadowfang_prisoner
EndContentData */

#include "ScriptedGossip.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "shadowfang_keep.h"
#include "ScriptedEscortAI.h"

enum Yells
{
    SAY_FREE_AS = 0,
    SAY_OPEN_DOOR_AS = 1,
    SAY_POST_DOOR_AS = 2,
    SAY_FREE_AD = 0,
    SAY_OPEN_DOOR_AD = 1,
    SAY_POST1_DOOR_AD = 2,
    SAY_POST2_DOOR_AD = 3
};

enum Spells
{
    SPELL_UNLOCK = 6421,
    SPELL_DARK_OFFERING = 7154
};

enum Creatures
{
    NPC_ASH = 3850
};

// 3849, 3850 -- 
class npc_shadowfang_prisoner : public CreatureScript
{
public:
    npc_shadowfang_prisoner() : CreatureScript("npc_shadowfang_prisoner") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetInstanceAI<npc_shadowfang_prisonerAI>(creature, "instance_shadowfang_keep");
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        return true;
    }
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        return true;
    }
    // 继承了npc护送AI
    struct npc_shadowfang_prisonerAI : public npc_escortAI
    {
        npc_shadowfang_prisonerAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
            case 0:
                if (me->GetEntry() == NPC_ASH)
                    Talk(SAY_FREE_AS);
                else
                    Talk(SAY_FREE_AD);
                break;
            case 10:
                if (me->GetEntry() == NPC_ASH)
                    Talk(SAY_OPEN_DOOR_AS);
                else
                    Talk(SAY_OPEN_DOOR_AD);
                break;
            case 11:
                if (me->GetEntry() == NPC_ASH)
                    DoCast(me, SPELL_UNLOCK);
                break;
            case 12:
                if (me->GetEntry() == NPC_ASH)
                    Talk(SAY_POST_DOOR_AS);
                else
                    Talk(SAY_POST1_DOOR_AD);

                instance->SetData(1, DONE);
                break;
            case 13:
                if (me->GetEntry() != NPC_ASH)
                    Talk(SAY_POST2_DOOR_AD);
                break;
            }
        }

        void Reset() override { }
        void EnterCombat(Unit* /*who*/) override { }
    };
};



class npc_haunted_stable_hand : public CreatureScript
{
public:
    npc_haunted_stable_hand() : CreatureScript("npc_haunted_stable_hand") { }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 Sender, uint32 action) override
    {
        player->playerTalkClass->ClearMenus();

        if (Sender != GOSSIP_SENDER_MAIN)
            return true;
        if (!player->getAttackers().empty())
            return true;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->TeleportTo(33, -225.70f, 2269.67f, 74.999f, 2.76f);
                CloseGossipMenuFor(player);
            break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->TeleportTo(33, -260.66f, 2246.97f, 100.89f, 2.43f);
                CloseGossipMenuFor(player);
            break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->TeleportTo(33, -171.28f, 2182.020f, 129.255f, 5.92f);
                CloseGossipMenuFor(player);
            break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        InstanceScript* instance = creature->GetInstanceScript();

        if (instance && instance->GetData(DATA_BARON_SILVERLAINE_EVENT)==DONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport me to Baron Silberlein", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        if (instance && instance->GetData(DATA_COMMANDER_SPRINGVALE_EVENT)==DONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport me to Commander Springvale", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        if (instance && instance->GetData(DATA_LORD_WALDEN_EVENT)==DONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport me to Lord Walden", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

        SendGossipMenuFor(player, 2475, creature->GetGUID());
        return true;
    }
};

void AddSC_shadowfang_keep()
{
    new npc_shadowfang_prisoner();
    new npc_haunted_stable_hand();
}

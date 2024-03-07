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
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "QuestPackets.h"
#include "ScenePackets.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "MotionMaster.h"
#include "WorldSession.h"
#include "PhasingHandler.h"
#include "CombatAI.h"
#include "Log.h"

enum eQuests
{
    QUEST_BREAKING_OUT = 38672,
    QUEST_RISE_OF_THE_ILLIDARI = 38690,
    QUEST_FEL_INFUSION = 38689,
    QUEST_VAULT_OF_THE_WARDENS = 39742, // optional bonus objective
    QUEST_STOP_GULDAN_H = 38723,
    QUEST_STOP_GULDAN_A = 40253,
    QUEST_GRAND_THEFT_FELBAT = 39682,
    QUEST_FROZEN_IN_TIME = 39685,
    QUEST_BEAM_ME_UP = 39684,
    QUEST_FORGED_IN_FIRE_H = 39683,
    QUEST_FORGED_IN_FIRE_V = 40254,
    QUEST_ALL_THE_WAY_UP = 39686,
    QUEST_A_NEW_DIRECTION = 40373,
    QUEST_BETWEEN_US_AND_FREEDOM_HH = 39694,
    QUEST_BETWEEN_US_AND_FREEDOM_HA = 39688,
    QUEST_BETWEEN_US_AND_FREEDOM_VA = 40255,
    QUEST_BETWEEN_US_AND_FREEDOM_VH = 40256,
    QUEST_ILLIDARI_LEAVING_A = 39689,
    QUEST_ILLIDARI_LEAVING_H = 39690,
};
enum eSpells
{
    SPELL_FEL_INFUSION = 133508,
    SPELL_CANCEL_FEL_BAR = 133510,
    SPELL_UNLOCKING_ALTRUIS = 184012,
    SPELL_UNLOCKING_KAYN = 177803,
    SPELL_PRISON_EXPLOSION = 232248,
};

enum eKillCredits
{
    KILL_CREDIT_KAYN_PICKED_UP_WEAPONS = 112276,
    KILL_CREDIT_ALTRUIS_PICKED_UP_WEAPONS = 112277,
    KILL_CREDIT_REUNION_FINISHED_KAYN = 99326,
    KILL_CREDIT_REUNION_FINISHED_ALTRUIS = 112287,
};

// quest:38672 `Breaking Out`
// 92718 - npc_maiev_shadowsong
class npc_maiev_shadowsong_welcome : public CreatureScript
{
public:
    npc_maiev_shadowsong_welcome() : CreatureScript("npc_maiev_shadowsong_welcome") { }
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BREAKING_OUT)
        {
            player->SummonCreature(92718, creature->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 10000, 0, true);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_maiev_shadowsong_welcome_AI(creature);
    }

    struct npc_maiev_shadowsong_welcome_AI : public ScriptedAI
    {
        uint32 timer;
        bool movein;

        npc_maiev_shadowsong_welcome_AI(Creature* creature) : ScriptedAI(creature)
        {
            timer = 0;
            movein = false;
        }
        void IsSummonedBy(Unit* summoner) override
        {
            if (Player* player = summoner->ToPlayer())
            {
                timer = 2000;
                movein = true;
                Talk(0);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (movein)
            {
                if (timer <= diff)
                {
                    // Ward of the Hunters[244925]
                    if (GameObject* go = me->FindNearestGameObject(244925, me->GetValuesCount()))
                        go->UseDoorOrButton();

                    // path defines in table `waypoint_data`
                    me->GetMotionMaster()->MovePath(1092718, false);                    
                    me->GetScheduler().Schedule(4s, [this](TaskContext context)
                    {
                        Creature* maiev = GetContextCreature();
                        if (GameObject* go = maiev->FindNearestGameObject(244925, maiev->GetVisibilityRange()))
                            go->ResetDoorOrButton();

                    });

                    movein = false;
                    me->DespawnOrUnsummon(5s);
                }
                else
                    timer -= diff;
            }
        }
    };
};



// 103658 - Kayn's cell
class npc_kayn_cell : public CreatureScript
{
public:
    npc_kayn_cell() : CreatureScript("npc_kayn_cell") {}
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        // 检查任务目标是否已经完成 ...
        if (player->GetQuestStatus(QUEST_BREAKING_OUT) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->GetQuestObjectiveCounter(92848) > 0)
                return true;

            if (creature == nullptr)
                return false;

            player->CastSpell(creature, SPELL_UNLOCKING_KAYN, true);
            creature->DespawnOrUnsummon(60);
            // 99631 - Kayn Sunfury
            player->SummonCreature(99631, 4343.16f, -589.57f, -281.40f, 3.38f, TEMPSUMMON_TIMED_DESPAWN, 60000, false);
        }

        return true;
    }
};

// 103655 - Altruis's cell
class npc_altruis_cell : public CreatureScript
{
public:
    npc_altruis_cell() : CreatureScript("npc_altruis_cell") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_BREAKING_OUT) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->GetQuestObjectiveCounter(92849) > 0)
                return true;

            if (creature == nullptr)
                return false;

            player->CastSpell(creature, SPELL_UNLOCKING_ALTRUIS, true);
            creature->DespawnOrUnsummon(60);
            // 99632 - Altruis the Sufferer
            player->SummonCreature(99632, 4309.94f, -589.618f, -281.407f, 6.13126f, TEMPSUMMON_TIMED_DESPAWN, 60000, false);
        }

        return true;
    }
};


// 
void AddSC_zone_vault_of_wardens()
{
    new npc_maiev_shadowsong_welcome();
    new npc_kayn_cell();
    new npc_altruis_cell();

}

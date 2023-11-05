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

#include "ScriptMgr.h"
#include "CreatureAIImpl.h"
#include "GameObject.h"
#include "MotionMaster.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "ScriptedGossip.h"

/*######
## Quest 37446: Lazy Peons
## npc_lazy_peon
######*/

enum LazyPeonYells
{
    SAY_SPELL_HIT                                 = 0
};

enum LazyPeon
{
    QUEST_LAZY_PEONS    = 37446,
    GO_LUMBERPILE       = 175784,
    SPELL_BUFF_SLEEP    = 17743,
    SPELL_AWAKEN_PEON   = 19938
};

class npc_lazy_peon : public CreatureScript
{
public:
    npc_lazy_peon() : CreatureScript("npc_lazy_peon") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lazy_peonAI(creature);
    }

    struct npc_lazy_peonAI : public ScriptedAI
    {
        npc_lazy_peonAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            RebuffTimer = 0;
            work = false;
        }

        uint32 RebuffTimer;
        bool work;

        void Reset() override
        {
            Initialize();
        }

        void MovementInform(uint32 /*type*/, uint32 id) override
        {
            if (id == 1)
                work = true;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id != SPELL_AWAKEN_PEON)
                return;

            Player* player = caster->ToPlayer();
            if (player && player->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                Talk(SAY_SPELL_HIT, caster);
                me->RemoveAllAuras();
                if (GameObject* Lumberpile = me->FindNearestGameObject(GO_LUMBERPILE, 20))
                    me->GetMotionMaster()->MovePoint(1, Lumberpile->GetPositionX()-1, Lumberpile->GetPositionY(), Lumberpile->GetPositionZ());
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (work == true)
                me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);
            if (RebuffTimer <= diff)
            {
                DoCast(me, SPELL_BUFF_SLEEP);
                RebuffTimer = 300000; //Rebuff agian in 5 minutes
            }
            else
                RebuffTimer -= diff;
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };
};

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716, // Launch (Whee!)
};

// 17009
class spell_voodoo : public SpellScriptLoader
{
    public:
        spell_voodoo() : SpellScriptLoader("spell_voodoo") { }

        class spell_voodoo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_voodoo_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return ValidateSpellInfo(
                {
                    SPELL_BREW,
                    SPELL_GHOSTLY,
                    SPELL_HEX1,
                    SPELL_HEX2,
                    SPELL_HEX3,
                    SPELL_GROW,
                    SPELL_LAUNCH
                });
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, spellid, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_voodoo_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_voodoo_SpellScript();
        }
};

/*
  npc_durotar_duelist
  实现任务`整装待发`的决斗, 
*/

// 这个字符串应该定义在数据库中，然后通过id来进行转换...
#define GOSSIP_LETS_DUEL    "Let's duel"

enum eDuelist
{
    QUEST_TO_BE_PREPARED = 44281,
    KILL_CREDIT_WARM_DUEL = 108722,         // 这个id关联的是什么呢？ 这个关联到了任务目标ID，table `quest_objectives`中定义的...
    QUEST_OBJECTIVE_ID = 286487,        // `Warmed up with a duel`
    EVENT_DO_CAST = 1,
    EVENT_STOP_DUEL = 2,
    DATA_START_DUEL = 10,
    SPELL_DUEL = 52996,         // 开始决斗使用的法术？谁使用的？
    SPELL_DUEL_TRIGGERED = 52990,       // 这个值是定义在什么地方的？
    SPELL_DUEL_VICTORY = 52994, // 这个Id是什么呢？
    SPELL_DUEL_FLAG = 52991,        // 用在什么地方呢？定义在什么地方
};

// <creatureEntry, spellId>?
// spellId defines in spell.db2

std::map<uint32, uint32> const creatureAbilities
{
    { 113955,  172675 }, // Utona Wolfeye : Lighting Bolt
    { 113951,  171884 }, // Sahale : Denounce
    { 113947,  172769 }, // Maska : Mortal Strike
    { 113545,  171764 }, // Dawn Merculus : Fireball
    { 113961,  172028 }, // Pinkee Rizzo : Sinister Strike
    { 113956,  171858 }, // Taela Shatterborne : Frostbolt
    { 113952,  171957 }, // Aila Dourblade : Hemorrhage
    { 113948,  172673 }, // Arienne Black : Holy Smite
    { 113542,  11538 },  // Marius Sunshard : Frostbolt + Ice Barrier (33245)
    { 113954,  171919 }, // Argonis Solheart : Crusader Strike
    { 113544,  171777 }, // Neejala : Starfire
    { 113950,  172779 }, // Lonan : Stormstrike
    { 113546,  172673 }, // Yaalo : Holy Smite
};

// 决斗的npc的脚本，当前如果战胜npc有正确的处理，如果战败了有争取处理吗？

class npc_durotar_duelist : public CreatureScript
{
public:
    npc_durotar_duelist() : CreatureScript("npc_durotar_duelist") {}
    // npc的AI，用来操控npc的战斗逻辑...
    struct  npc_durotar_duelist_AI : public ScriptedAI
    {
        npc_durotar_duelist_AI(Creature* creature) : ScriptedAI(creature) {}
        void Reset() override
        {
            _events.Reset();
            me->RestoreFaction();
            me->SetReactState(REACT_DEFENSIVE);
        }

        // 重载进入战斗的处理，who是战斗对象吗？
        void EnterCombat(Unit* who) override
        {
            _events.ScheduleEvent(EVENT_DO_CAST, 1000);
        }

        // 重载自己出现伤害，用来处理自己的生命值..
        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (damage >= me->GetHealth())
            { // 伤害溢出
                damage = me->GetHealth() - 1;       // 因为要处理事件，所以不能打死？只能打到剩下1点血，然后处理击败事件
                _events.Reset();
                me->RemoveAllAuras();
                // 阵营35是什么呢？
                // 在faction.db2中35的定义是Villian, 不明定义啊... 是这个阵营属于特殊的中立的吗？
                me->setFaction(35);            
                me->AttackStop();

                // 和npc进行决斗的逻辑是击败即可完成任务
                attacker->AttackStop();
                attacker->ClearInCombat();  // 这个函数的作用是什么？
                // 直接调用击杀monster的凭证？ 用来触发击杀某个monster将要处理的条件判断
                // 参数是monster的Id吗？
                attacker->ToPlayer()->KilledMonsterCredit(KILL_CREDIT_WARM_DUEL);

                // 将决斗的插旗移除, 是因为决斗旗是player插的，所以也是由player来移除吗？
                attacker->RemoveGameObject(SPELL_DUEL_FLAG, true);
                // player向npc施放决斗胜利
                attacker->CastSpell(me, SPELL_DUEL_VICTORY, true);
                me->CastSpell(me, 7267, true);

                // 这个函数是让1秒后停止决斗吗？
                _events.ScheduleEvent(EVENT_STOP_DUEL, 3000);
            }
        }

        // diff -- elapsed time in milliseconds?
        // 处理AI的更新...
        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_DO_CAST:
                        // 处理cast事件，让AI控制的npc施放指定的法术
                        DoCastVictim( creatureAbilities.at(me->GetEntry()));
                        _events.RescheduleEvent(EVENT_DO_CAST, 4000);   // 4秒后再执行此事件..
                        break;
                    case EVENT_STOP_DUEL:
                        me->Say("Fine duel.", LANG_UNIVERSAL, nullptr);
                        me->GetMotionMaster()->MoveTargetedHome();  // npc回到原位...
                        break;
                    default:
                        break;
                }
            }
            // 执行近战攻击？
            DoMeleeAttackIfReady();
        }

        void SetGUID(ObjectGuid guid, int32 /*id*/) override
        {
            _playerGuid = guid;
        }
             


    private:
        EventMap _events;
        ObjectGuid _playerGuid = ObjectGuid::Empty;
    };

    // Q：这个回调处理的是什么呢？
    // A：这个函数是处理玩家右键点击Npc进行交互的第1步
    // 是通过`sScriptMgr->OnGossipHello(...)`来调用的...
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        // player或者npc不能处于战斗状态
        if (player->IsInCombat() || creature->IsInCombat())
            return true;

        // 检查任务的状态..
        // 如果Player接到了任务`To Be Prepared` 并且没有完成，那么就添加菜单项，让Player可以选择和Npc进行决斗
        // FIXME: 是否可以检查任务的某一个目标是否已经完成 
        if (player->GetQuestStatus(QUEST_TO_BE_PREPARED) == QUEST_STATUS_INCOMPLETE &&
            player->GetQuestObjectiveCounter(286487) < 1)   // `quest objective: Warmed up with a duel` 
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_LETS_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        // 这个调用的作用的时候？是让 `OnGossipSelect`的回调被触发吗？
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    // 当player选择了一个对话选项的回调吗？
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->playerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1) {
            creature->AI()->SetGUID(player->GetGUID());
            creature->setFaction(FACTION_TEMPLATE_FLAG_PVP);

            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);       // 这个代码的作用未知...


            creature->SetReactState(REACT_AGGRESSIVE);

            player->CastSpell(creature, SPELL_DUEL, false);     // 和npc进行插旗决斗？
            player->CastSpell(player, SPELL_DUEL_FLAG, true);   // 这个是往地上插旗吗？ 好像是 52991 是决斗旗 spell的id

            // 让AI开始攻击player...
            creature->AI()->AttackStart(player);
            CloseGossipMenuFor(player);
        }
        return true;
    }

    // 获取npc的AI
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_durotar_duelist_AI(creature);
    }
};




void AddSC_durotar()
{
    new npc_lazy_peon();
    new spell_voodoo();

    new npc_durotar_duelist();
}

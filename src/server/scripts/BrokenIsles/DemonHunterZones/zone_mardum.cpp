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

/*
 * the Shattered Abyss
 * Map: 1481 Mardum
 * Zone: 7705 MardumtheShatteredAbyss
 */


#include "Creature.h"
#include "GameObject.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SceneMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Conversation.h"
#include "GameObjectAI.h"
#include "CreatureTextMgr.h"
#include "GossipDef.h"


enum eQuests
{
    QUEST_INVASION_BEGIN        = 40077,            // DH starting quest: The Invasion Begins
    QUEST_ASHTONGUE_FORCES      = 40378,
    QUEST_COILSKAR_FORCES       = 40379,
    QUEST_MEETING_WITH_QUEEN    = 39050,
    QUEST_SHIVARRA_FORCES       = 38765,
    QUEST_BEFORE_OVERRUN        = 38766,
    QUEST_HIDDEN_NO_MORE        = 39495,
    QUEST_ON_FELBAT_WINGS       = 39663,
    QUEST_THE_KEYSTONE          = 38728,


    QUEST_SetThemFree = 38759,
};

enum eScenes
{
    SPELL_SCENE_MARDUM_WELCOME          = 193525,   // (sceneId: 1106) 
    SPELL_SCENE_MARDUM_LEGION_BANNER    = 191677,   // (sceneId: 1116)
    SPELL_SCENE_MARDUM_ASHTONGUE_FORCES = 189261,   // (sceneId: 1053)
    SPELL_SCENE_MARDUM_COILSKAR_FORCES  = 190793,   // (sceneId: 1077)
    SPELL_SCENE_MARDUM_SHIVARRA_FORCES  = 190851,   // (sceneId: 1078)
    SPELL_SCENE_MEETING_WITH_QUEEN      = 188539,   // 
};

// phase id defines in `phase.db2`
enum ePhaseSpells
{
    SPELL_PHASE_170 = 59073,
    SPELL_PHASE_171 = 59074,
    SPELL_PHASE_172 = 59087,
    SPELL_PHASE_173 = 54341,

    SPELL_PHASE_175 = 57569,        // ASHTONGUE_FORCES
    SPELL_PHASE_176 = 74789,        // Coilskar forces
    SPELL_PHASE_177 = 64576,        // Shivarra forces

    SPELL_PHASE_179 = 67789,        // 关联到 Felguard Butcher

    SPELL_PHASE_180 = 68480,
    SPELL_PHASE_181 = 68481,
    SPELL_PHASE_182 = 68482,
    SPELL_PHASE_183 = 68483,
    SPELL_PHASE_184 = 69077,

    // temp phase
    TEMP_PHASE_10 = 69078,  // phase-185
    TEMP_PHASE_11 = 69484,  // phase-186
    TEMP_PHASE_12 = 69485,  // phase-187
    TEMP_PHASE_13 = 69486,  // phase-188
    TEMP_PHASE_14 = 70695,  // phase-189
    TEMP_PHASE_15 = 70696,  // phase-190
    TEMP_PHASE_16 = 74093,  // phase-191
    TEMP_PHASE_17 = 74094,  // phase-192
    TEMP_PHASE_18 = 74095,  // phase-193
    TEMP_PHASE_19 = 74096,  // phase-194
    TEMP_PHASE_20 = 74097,  // phase-195
    TEMP_PHASE_21 = 79041   // phase-196
};

enum ePhases
{
    SPELL_PHASE_MARDUM_WELCOME              = SPELL_PHASE_170,
    SPELL_PHASE_MARDUM_FELSABBER            = SPELL_PHASE_172,

    SPELL_PHASE_ILLIDARI_OUTPOST_ASHTONGUE  = SPELL_PHASE_175,
    SPELL_PHASE_ILLIDARI_OUTPOST_COILSKAR   = SPELL_PHASE_176,      
    SPELL_PHASE_ILLIDARI_OUTPOST_SHIVARRA   = SPELL_PHASE_177,


    // 灰舌秘术师，通过相位 180 控制
    PHASE_ASHTONGUE_MYSTIC = SPELL_PHASE_180,
};

enum eMisc
{
    PLAYER_CHOICE_DH_SPEC_SELECTION             = 231,
    PLAYER_CHOICE_DH_SPEC_SELECTION_DEVASTATION = 478,
    PLAYER_CHOICE_DH_SPEC_SELECTION_VENGEANCE   = 479,

    SPELL_VISUAL_KIT_KAYN_GLIDE = 59738,
    SPELL_VISUAL_KIT_KAYN_WINGS = 59406,
    SPELL_VISUAL_KIT_KAYN_DOUBLE_JUMP = 58110,
    SPELL_VISUAL_KIT_KORVAS_JUMP = 63071,
    SPELL_VISUAL_KIT_WRATH_WARRIOR_DIE = 58476,
};


enum eEnums
{
    MAP_MARDUM = 1481,           // Mardum Map Id
    ZONE_MARDUM = 7705,
    STARTING_QUEST = QUEST_INVASION_BEGIN,

    // Kayn Team
    NPC_Kayn = 93011,
    NPC_Jayce = 98228,
    NPC_Allari = 98227,

    // Korvas Team
    NPC_Korvas = 98292,
    NPC_Cyana = 98290,
    NPC_Sevis = 99918,


    // Quest: Set Them Free NPCs
    NPC_Belath = 94400,
    NPC_Mannethrel = 93230,
    NPC_Izal = 93117,
    NPC_Cyana_2 = 94377,
       
    SAY_Belath = 96643,
    SAY_Mannethrel = 96202,
    SAY_Izal = 96644,
    SAY_Cyana_2 = 95081
};

// starting quest check...
void SummonKaynSunfuryForStarting(Player* player)
{
    if (player->GetQuestStatus(STARTING_QUEST) == QUEST_STATUS_NONE
        && !player->HasAura(SPELL_SCENE_MARDUM_WELCOME))
    {
        // Summon a team of NPCs & add phase:170 to player
        if (!player->FindNearestCreature({ NPC_Kayn }, 30.f))
            player->CastSpell(player, SPELL_SCENE_MARDUM_WELCOME, true);
    }
}


// table `scene_template`, sceneId: 1106, ScriptPackageID: 1487
class scene_mardum_welcome : public SceneScript
{
public:
    scene_mardum_welcome() : SceneScript("scene_mardum_welcome") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/)
    {
        
    }

    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        // player enter phase-170
        player->AddAura(SPELL_PHASE_MARDUM_WELCOME);
    }

    void OnSceneTriggerEvent(Player* player, uint32 sceneInstanceID, SceneTemplate const* sceneTemplate, std::string const& triggerName) override
    {
        if (triggerName == "CUEILLIDANTH")
        {
            // Illidan Stormrage says
            Conversation::CreateConversation(705, player, *player, { player->GetGUID() }, nullptr);
        }
    }
};

// FIXME: This is a global Script, every player will trigger this script
// Perhaps other scripts can be used instead
class PlayerScript_mardum_welcome_scene_trigger : public PlayerScript
{
public:
    PlayerScript_mardum_welcome_scene_trigger() : PlayerScript("PlayerScript_mardum_welcome_scene_trigger") {}
    uint32 checkTimer = 1000;

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (player->getClass() == CLASS_DEMON_HUNTER && player->GetZoneId() == ZONE_MARDUM)
        {
            if(player->GetQuestStatus(STARTING_QUEST) == QUEST_STATUS_NONE)
                player->AddAura(SPELL_PHASE_MARDUM_WELCOME);
            else
                player->RemoveAurasDueToSpell(SPELL_PHASE_MARDUM_WELCOME);
            
            // Quest: Enter the Illidari: Ashtongue
            if (player->GetQuestStatus(40378) == QUEST_STATUS_INCOMPLETE)
            {
                // Accept Illidan's gift
                if (player->GetQuestObjectiveData(40378, 4) && !player->GetQuestObjectiveData(40378, 3))    
                    player->AddAura(SPELL_PHASE_MARDUM_FELSABBER);
            }

            // Quest: Enter the Illidari: Coilskar
            if (player->GetQuestStatus(QUEST_COILSKAR_FORCES) == QUEST_STATUS_NONE)
                player->AddAura(SPELL_PHASE_173);
        }
    }
    
    void OnQuestAbandon(Player* player, Quest const* quest) override
    {
        if (player->GetMapId() != MAP_MARDUM || player->GetZoneId() != ZONE_MARDUM)
            return;

        if (quest->ID == STARTING_QUEST)
        {
            player->RemoveAurasDueToSpell(SPELL_PHASE_171);

            // must be in the right area 
            if (player->GetAreaId() != 0 && player->GetAreaId() != ZONE_MARDUM)
                return;

            player->AddAura(SPELL_PHASE_170);


            player->RemoveRewardedQuest(quest->ID);

            // Recall Kayn
            SummonKaynSunfuryForStarting(player);
        }
        else if (quest->ID == QUEST_COILSKAR_FORCES)
        {
            player->AddAura(SPELL_PHASE_173);
        }
    }

    void OnQuestComplete(Player* player, Quest const* quest) override
    {
        if (quest && quest->ID == STARTING_QUEST)
            player->CastSpell(player, SPELL_PHASE_171, true);
    }

    // When new DH character finishes watching the intro movie.
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId != 469 && movieId != 497 && movieId != 478)
            return;
        if (player->GetMapId() != MAP_MARDUM)
            return;
        if (player->getClass() != CLASS_DEMON_HUNTER)
            return;

        if(movieId == 469) // intro movie
            SummonKaynSunfuryForStarting(player);
        else if (movieId == 497)
        {
            // Return to the Black Temple
            // TODO:
        }
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 oldArea)
    {
        if (player->getClass() != CLASS_DEMON_HUNTER)
            return;

        if (player->GetMapId() != MAP_MARDUM || player->GetZoneId() != ZONE_MARDUM)
            return;

        if ( newArea == 0 || newArea == ZONE_MARDUM)
            SummonKaynSunfuryForStarting(player);
    }
};


Position const WrathWarriorSpawnPosition = { 1081.9166f, 3183.8716f, 26.335993f };
Position const KaynDoubleJumpPosition = { 1094.2384f, 3186.058f, 28.81562f };

Position const KaynJumpPos = { 1172.17f, 3202.55f, 54.3479f };
Position const JayceJumpPos = { 1119.24f, 3203.42f, 38.1061f };
Position const AllariJumpPos = { 1120.08f, 3197.2f, 36.8502f };
Position const KorvasJumpPos = { 1117.89f, 3196.24f, 36.2158f };
Position const SevisJumpPos = { 1120.74f, 3199.47f, 37.5157f };
Position const CyanaJumpPos = { 1120.34f, 3194.28f, 36.4321f };


// npc: 93011 `Kayn SunFury`
// phaseId: 50/170

// 这个npc是出现在一开始的时候...
class npc_kayn_sunfury_welcome : public CreatureScript
{
public:
    npc_kayn_sunfury_welcome() : CreatureScript("npc_kayn_sunfury_welcome") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_INVASION_BEGIN)
        {
            /*
            if (Creature* Korvas = creature->FindNearestCreature({ NPC_Korvas }, 30.0f))
                Korvas->GetMotionMaster()->MoveCharge(&KorvasJumpPos);

            if (Creature* Cyana = creature->FindNearestCreature({ NPC_Cyana }, 30.f))
                Cyana->GetMotionMaster()->MoveCharge(&CyanaJumpPos);

            if (Creature* Sevis = creature->FindNearestCreature({ NPC_Sevis }, 30.f))
                Sevis->GetMotionMaster()->MoveCharge(&SevisJumpPos);
            */



            // TODO: 当接完任务之后，凯恩将带着小队冲进怪区，由于npc是对所有玩家的，因此正确的应该是
            // 召唤一个clone出来
            // 召唤可以通过SAI来完成？
            // player->SummonCreature(93011, creature->GetPosition());
            // 当player接任务之后，添加新的phase

            // 接了任务之后，就不要170相位，这样看不见接任务的npc...
            player->RemoveAurasDueToSpell(SPELL_PHASE_MARDUM_WELCOME);
            // if (TempSummon* npc =
            player->SummonCreature(93011, creature->GetPosition().GetPositionX(), creature->GetPosition().GetPositionY(), creature->GetPosition().GetPositionZ(),
                creature->GetPosition().GetAngle(player) , TEMPSUMMON_MANUAL_DESPAWN, 17000, true);
                //npc->SetFacingToObject(player);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kayn_sunfury_welcomeAI(creature);
    }

    struct npc_kayn_sunfury_welcomeAI : public ScriptedAI
    {
        const uint32 DespawnTimes = 16000;
        npc_kayn_sunfury_welcomeAI(Creature* creature) : ScriptedAI(creature)
        {

        }
        void IsSummonedBy(Unit* summoner) override
        {
            if (Player* player = summoner->ToPlayer())
            {
                Talk(0);
                me->RemoveFlag64(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                me->SummonCreature(NPC_Jayce, 1182.36f, 3202.91f, 51.6049f, 4.88566f, TEMPSUMMON_MANUAL_DESPAWN, 16000, true);
                me->SummonCreature(NPC_Allari, 1177.00f, 3203.07f, 51.3637f, 4.88746f, TEMPSUMMON_MANUAL_DESPAWN, 16000, true);
                me->SummonCreature(NPC_Sevis, 1172.92f, 3207.82f, 52.3935f, 3.73185f, TEMPSUMMON_MANUAL_DESPAWN, 16000, true);
                me->SummonCreature(NPC_Cyana, 1171.49f, 3203.69f, 51.3145f, 3.4564f, TEMPSUMMON_MANUAL_DESPAWN, 16000, true);
                me->SummonCreature(NPC_Korvas, 1170.74f, 3204.71f, 51.5426f, 3.36744f, TEMPSUMMON_MANUAL_DESPAWN, 16000, true);

                me->GetScheduler().Schedule(5s, [this](TaskContext context)
                {
                    me->AI()->Talk(1);
                }).Schedule(11s, [this](TaskContext context)
                {
                    // Kor'vas Yells
                    if (Creature* npc = me->FindNearestCreature(NPC_Korvas, 10.0f, true))
                        npc->AI()->Talk(0);
                }).Schedule(13s, [this, player](TaskContext context)
                {
                    me->GetMotionMaster()->MovePath(9301100, false);
                    me->DespawnOrUnsummon(16s);

                    if (Creature* npc = me->FindNearestCreature(NPC_Jayce, 10.0f, true))
                    {
                        //npc->GetMotionMaster()->MovePath(10267108, false);
                        npc->DespawnOrUnsummon(16s);
                    }
                    if (Creature* npc = me->FindNearestCreature(NPC_Allari, 10.0f, true))
                    {
                        npc->SetAIAnimKitId(9180);  // DH Run Allari
                        //npc->GetMotionMaster()->MovePath(10267109, false);
                        npc->DespawnOrUnsummon(16s);
                    }
                    if (Creature* npc = me->FindNearestCreature(NPC_Sevis, 10.0f, true))
                    {
                        npc->SetAIAnimKitId(9767);  // DH Run
                        //npc->GetMotionMaster()->MovePath(10267110, false);
                        npc->DespawnOrUnsummon(16s);
                    }
                    if (Creature* npc = me->FindNearestCreature(NPC_Korvas, 10.0f, true))
                    {
                        npc->SetAIAnimKitId(9767);  // DH Run
                        //npc->GetMotionMaster()->MovePath(10267111, false);
                        npc->DespawnOrUnsummon(16s);
                    }
                    if (Creature* npc = me->FindNearestCreature(NPC_Cyana, 10.0f, true))
                    {
                        //npc->GetMotionMaster()->MovePath(10267112, false);
                        npc->DespawnOrUnsummon(16s);
                    }                    
                });
            }
        }


        void MovementInform(uint32 moveType, uint32 data) override
        {
            // 用pointId判断会出现错误，因为需要将path分成两段，两端都有point ...
            // 因此需要添加新的回调，pathEnd 才能实现 ...

            if (data == EVENT_JUMP) // 跳到空中完成
            {
                std::string tmp;
                tmp.append("jump in the air...moveType:").append(TOSTR(moveType)).append(" data:").append(TOSTR(data));;
                me->Say(tmp.c_str(), LANG_UNIVERSAL);

                me->GetScheduler().Schedule(3000ms, [this](TaskContext /*context*/)
                {
                    me->SetAIAnimKitId(9767);
                    //me->SetAIAnimKitId(0);

                    //me->GetMotionMaster()->MoveLand(1998, { 1117.65f,3191.72f,35.4776f });

                    // 落地之后，需要移除spell:滑翔的效果

                    //me->RemoveAurasDueToSpell(199303);


                    me->SendCancelSpellVisualKit(SPELL_VISUAL_KIT_KAYN_WINGS);
                    me->SendCancelSpellVisualKit(SPELL_VISUAL_KIT_KAYN_DOUBLE_JUMP);


                    me->GetMotionMaster()->MovePath(9301101, false);
                });
            }
        }

        void OnWaypointPathEnded(uint32 pointId, uint32 pathId) override
        {
            // 什么时候能收到这个回调？
            std::string tmp;
            tmp.append("pointId:").append(TOSTR(pointId)).append(" pathId:").append(TOSTR(pathId));
            me->Say(tmp.c_str(), LANG_UNIVERSAL);

            if (pathId == 9301100)
            {
                //Position jumpPos = { 1131.93f,3194.71f,43.2108f };
                Position jumpPos = { 1117.65f, 3191.72f, 35.4776f };

                TempSummon* summon = me->ToTempSummon();
                if (!summon)
                    return;
                WorldObject* summoner = summon->GetSummoner();
                if (!summoner)
                    return;

                //me->SetAIAnimKitId(0);
                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_KAYN_WINGS, 4, 3000); // DH Wings
                me->PlayObjectSound(53780, me->GetGUID(), summoner->ToPlayer());  // double jump
                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_KAYN_DOUBLE_JUMP, 0, 0);

                // 恶魔猎手滑翔状态 ...
                //me->CastSpell(nullptr, 199303, true);   // SPELL_DEMON_HUNTER_GLIDE_STATE

                // EVENT_JUMP被触发的时间不对，预想是在到达目的地触发，实际上开始move的时候就触发了

                // 以跳跃的方式移动，垂直速度越大，跳得越高

                me->GetMotionMaster()->MoveJumpEx(jumpPos, 10.0f, 12.0f, EVENT_JUMP);
            }
        }
    };
};


// Legion版本使用 `go-244898` 是一个旗子

// 任务目标，激活会烧掉军团的旗帜..
class go_mardum_legion_banner_1 : public GameObjectScript
{
public:
    go_mardum_legion_banner_1() : GameObjectScript("go_mardum_legion_banner_1") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (!player->GetQuestObjectiveData(QUEST_INVASION_BEGIN, 0))
            return true;

        if (!player->GetQuestObjectiveData(QUEST_INVASION_BEGIN, 1))
            player->CastSpell(player, SPELL_SCENE_MARDUM_LEGION_BANNER, true);

        // Do NOT return true here.
        return false;
    }
};

// 进入第2个任务


// 灰舌..  激活器会在完成任务目标后消失应该是和phase相关..
// LegionCore 是怎么来控制phase的？ GO对某个phase可见...
// GO-241751
class go_mardum_portal_ashtongue : public GameObjectScript
{
public:
    go_mardum_portal_ashtongue() : GameObjectScript("go_mardum_portal_ashtongue") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if (!player->GetQuestObjectiveData(QUEST_ASHTONGUE_FORCES, 0))
        {
            player->KilledMonsterCredit(88872); // QUEST_ASHTONGUE_FORCES storageIndex 0 `Ashtongue forces`
            player->KilledMonsterCredit(97831); // QUEST_ASHTONGUE_FORCES storageIndex 1 `First Summoned Guardian`
            player->CastSpell(player, SPELL_SCENE_MARDUM_ASHTONGUE_FORCES, true);
        }

        return false;
    }
};

// 播放灰舌加入的场景动画...
class scene_mardum_welcome_ashtongue : public SceneScript
{
public:
    scene_mardum_welcome_ashtongue() : SceneScript("scene_mardum_welcome_ashtongue") { }

    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName)
    {
        if (triggerName == "SEEFELSABERCREDIT")
        {
            // QUEST_ASHTONGUE_FORCES storageIndex 4 `See Felsaber -- Hidden Objective`
            player->KilledMonsterCredit(101534);
        }
        else if (triggerName == "UPDATEPHASE")  // felsaber step out
        {
            // AddAura直接添加spell关联的aura，不需要判断spell的cast条件
            // 添加172相位，这样就可以看到邪刃豹坐骑完成任务目标了..
            player->AddAura(SPELL_PHASE_MARDUM_FELSABBER);
        }
    }
};

// 200176 - Learn felsaber   学习DH的职业坐骑，邪刃豹
class spell_learn_felsaber : public SpellScript
{
    PrepareSpellScript(spell_learn_felsaber);

    void HandleMountOnHit(SpellEffIndex /*effIndex*/)
    {
        // 移除了172 相位，这样就不会一直看到邪刃豹了
        GetCaster()->RemoveAurasDueToSpell(SPELL_PHASE_MARDUM_FELSABBER);

        // We schedule this to let hover animation pass
        GetCaster()->GetScheduler().Schedule(Milliseconds(900), [](TaskContext context)
        {
            GetContextUnit()->CastSpell(GetContextUnit(), 200175, true); // Felsaber mount
        });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_learn_felsaber::HandleMountOnHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 94410 - Allari the Souleater
// 这个脚本让玩家不需要和npc交互就可以完成任务--靠近的时候就完成
// 这个类型的AI是怎么和Creature进行绑定的呢？

// SAI 已经实现了这个功能，不需要这个脚本的支持了
/*
struct r : public ScriptedAI
{
    npc_mardum_allari(Creature* creature) : ScriptedAI(creature) { }
    // 当靠近npc的时候，完成quest-40378 最后一步，找到阿莱利
    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 5.0f)
                if (!player->GetQuestObjectiveData(QUEST_ASHTONGUE_FORCES, 2))
                    player->KilledMonsterCredit(me->GetEntry());    // QUEST_ASHTONGUE_FORCES storageIndex 2 `Find Allari to the southeast`
    }
};
*/
// Jailer Cage
// GO-244916, NPC-94377
class go_mardum_cage : public GameObjectScript
{
public:
    go_mardum_cage(const char* name, uint32 insideNpc, uint32 killCredit = 0) : GameObjectScript(name), _insideNpc(insideNpc), _killCredit(killCredit)
    {
        if (_killCredit == 0)
            _killCredit = insideNpc;
    }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        QuestStatus status = player->GetQuestStatus(QUEST_SetThemFree);        
        if (status == QUEST_STATUS_NONE || status == QUEST_STATUS_REWARDED)
            return true;

        if (_insideNpc == NPC_Belath && player->GetQuestObjectiveData(QUEST_SetThemFree, 0))
            return true;
        else if (_insideNpc == NPC_Mannethrel && player->GetQuestObjectiveData(QUEST_SetThemFree, 1))
            return true;
        else if (_insideNpc == NPC_Izal && player->GetQuestObjectiveData(QUEST_SetThemFree, 2))
            return true;
        else if (_insideNpc == NPC_Cyana_2 && player->GetQuestObjectiveData(QUEST_SetThemFree, 4))
            return true;

        if (Creature* creature = go->FindNearestCreature(_insideNpc, 10.0f))
        {
            if (TempSummon* personalCreature = player->SummonCreature(_insideNpc, creature->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 2000, 0, true))
            {
                float x, y, z;
                personalCreature->GetClosePoint(x, y, z, personalCreature->GetObjectSize() / 3, 15.0f);
                personalCreature->GetMotionMaster()->MovePoint(0, x, y, z);

                if (NPC_Belath == _insideNpc)
                    personalCreature->Say(SAY_Belath);
                else if (NPC_Mannethrel == _insideNpc)
                    personalCreature->Say(SAY_Mannethrel);
                else if (NPC_Izal == _insideNpc)
                    personalCreature->Say(SAY_Izal);
                else if(NPC_Cyana_2 == _insideNpc)
                    personalCreature->Say(SAY_Cyana_2);
            }

            // TODO : Remove this line when phasing is done properly
            //creature->DestroyForPlayer(player);
            creature->DespawnOrUnsummon(300ms);
            creature->SetRespawnTime(3000);

            player->KilledMonsterCredit(_killCredit);
        }

        return false;
    }

    uint32 _insideNpc;
    uint32 _killCredit;
};

// NPC: 93105
struct npc_mardum_inquisitor_pernissius : public ScriptedAI
{
    npc_mardum_inquisitor_pernissius(Creature* creature) : ScriptedAI(creature) { }

    enum Spells
    {
        SPELL_INCITE_MADNESS    = 194529,
        SPELL_INFERNAL_SMASH    = 192709,

        SPELL_LEARN_EYE_BEAM    = 195447
    };

    enum Creatures
    {
        NPC_COLOSSAL_INFERNAL   = 96159
    };

    enum Text
    {
        SAY_ONDEATH = 0,
        SAY_ONCOMBAT = 1,
        SAY_60PCT = 2,
    };

    ObjectGuid colossalInfernalguid;

    void Reset() override
    {
        if (Creature* infernal = me->SummonCreature(NPC_COLOSSAL_INFERNAL, 523.404f, 2428.41f, -117.087f, 0.108873f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
            colossalInfernalguid = infernal->GetGUID();
    }

    Creature* GetInfernal() const
    {
        return ObjectAccessor::GetCreature(*me, colossalInfernalguid);
    }

    void EnterCombat(Unit*) override
    {
        Talk(SAY_ONCOMBAT);

        me->GetScheduler().Schedule(Seconds(15), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                me->CastSpell(target, SPELL_INCITE_MADNESS);

            context.Repeat(Seconds(15));
        })
        .Schedule(Seconds(10), [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                if (Creature* infernal = GetInfernal())
                    infernal->CastSpell(target, SPELL_INFERNAL_SMASH);

            if (me->GetHealthPct() <= 60)
            {
                Talk(SAY_60PCT);
            }

            context.Repeat(Seconds(10));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_ONDEATH);

        if (Creature* infernal = GetInfernal())
            infernal->KillSelf();

        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 50.0f);

        for (Player* player : players)
        {
            player->KilledMonsterCredit(105946);
            player->KilledMonsterCredit(96159);

            if (!player->HasSpell(SPELL_LEARN_EYE_BEAM))
                player->CastSpell(player, SPELL_LEARN_EYE_BEAM);
        }
    }
};

// 192709 Infernal Smash
class spell_mardum_infernal_smash : public SpellScript
{
    PrepareSpellScript(spell_mardum_infernal_smash);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (!GetCaster() || !GetHitUnit())
            return;

        GetCaster()->CastSpell(GetHitUnit(), GetEffectValue(), true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mardum_infernal_smash::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};


// 100982 - Sevis Brightflame
struct npc_sevis_enter_coilskar : public ScriptedAI
{
    npc_sevis_enter_coilskar(Creature* creature) : ScriptedAI(creature) {}


    void sQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_COILSKAR_FORCES)
            player->SummonCreature(100982, 826.9f, 2758.64f, -30.5f, 1.62f, TEMPSUMMON_MANUAL_DESPAWN, 0, true);

        player->RemoveAurasDueToSpell(SPELL_PHASE_173);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetTypeId() == TYPEID_PLAYER)
        {
            sCreatureTextMgr->SendChat(me, 0, summoner);

            me->Mount(64385);
            me->GetMotionMaster()->MovePath(1009820, false);
            me->DespawnOrUnsummon(10s);
        }
        else
            me->DespawnOrUnsummon();
    }
};

// 99917 - Sevis Brightflame

// 正确的流程
// 当靠近npc，Talk(0)
// 然后当和npc进行交互的时候，打开菜单，显示1对话，然后做出2的emote..

struct npc_mardum_sevis_99917 : public ScriptedAI
{
    npc_mardum_sevis_99917(Creature* creature) : ScriptedAI(creature) {}

    void sGossipHello(Player* player) override
    {
        //sCreatureTextMgr->SendChat(me, 1, player);
        //sCreatureTextMgr->SendChat(me, 2, player);
    }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_COILSKAR_FORCES) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveData(QUEST_COILSKAR_FORCES, 0))
            {
                float dist = player->GetDistance(me);
                if (dist < 25.0f)
                {
                    // use temp phase to avoid repeated triggering  TEMP_PHASE_10

                    if (!player->HasAura(PHASE_ASHTONGUE_MYSTIC))
                    {
                        sCreatureTextMgr->SendChat(me, 0, player);
                        /*
                        me->GetScheduler().Schedule(3s, [this, player](TaskContext context)
                        {
                            sCreatureTextMgr->SendChat(me, 1, player);
                            sCreatureTextMgr->SendChat(me, 2, player);
                        });
                        */

                        player->AddAura(PHASE_ASHTONGUE_MYSTIC);
                    }
                }
                else if (dist > 41.0f)
                {
                    player->RemoveAurasDueToSpell(PHASE_ASHTONGUE_MYSTIC);
                }
            }
        }
    }
};

// 99914 - Ashtongue Mystic
class npc_mardum_ashtongue_mystic : public CreatureScript
{
public:
    npc_mardum_ashtongue_mystic() : CreatureScript("npc_mardum_ashtongue_mystic") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        
        return false;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
    {
        sCreatureTextMgr->SendChat(creature, 0, player);

        player->playerTalkClass->ClearMenus();
        if (player->GetQuestStatus(QUEST_COILSKAR_FORCES) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveData(QUEST_COILSKAR_FORCES, 0))
        {   
            player->KilledMonsterCredit(creature->GetEntry());

            // TODO : Remove this line when phasing is done properly
            //creature->DestroyForPlayer(player);
            player->RemoveAurasDueToSpell(PHASE_ASHTONGUE_MYSTIC);

            // 召唤出来的npc的相位是怎么设置的？
            if (TempSummon* mystic = player->SummonCreature(creature->GetEntry(), creature->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 14000, 0, true))
            {
                // player attack mytic
                player->CastSpell(mystic, 196724, false);
                // mystic down
                mystic->KillSelf();
            }
        }

        player->playerTalkClass->SendCloseGossip();
        return false;
    }
};

// 第2只部队加入...
class go_mardum_portal_coilskar : public GameObjectScript
{
public:
    go_mardum_portal_coilskar() : GameObjectScript("go_mardum_portal_coilskar") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if (!player->GetQuestObjectiveData(QUEST_COILSKAR_FORCES, 1))
        {
            player->KilledMonsterCredit(94406); // QUEST_COILSKAR_FORCES storageIndex 0 KillCredit
            player->KilledMonsterCredit(97831); // QUEST_COILSKAR_FORCES storageIndex 1 KillCredit
            player->CastSpell(player, SPELL_SCENE_MARDUM_COILSKAR_FORCES, true);
        }

        return false;
    }
};

// 
class go_meeting_with_queen_ritual : public GameObjectScript
{
public:
    go_meeting_with_queen_ritual() : GameObjectScript("go_meeting_with_queen_ritual") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if (player->HasQuest(QUEST_MEETING_WITH_QUEEN) &&
            !player->GetQuestObjectiveData(QUEST_MEETING_WITH_QUEEN, 0))
        {
            player->CastSpell(player, SPELL_SCENE_MEETING_WITH_QUEEN, true);
        }

        return false;
    }
};

class scene_mardum_meeting_with_queen : public SceneScript
{
public:
    scene_mardum_meeting_with_queen() : SceneScript("scene_mardum_meeting_with_queen") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->KilledMonsterCredit(100722);
    }
};

//93221 Doom Commander Beliash
struct npc_mardum_doom_commander_beliash : public ScriptedAI
{
    npc_mardum_doom_commander_beliash(Creature* creature) : ScriptedAI(creature){ }

    enum Spells
    {
        SPELL_SHADOW_BOLT_VOLLEY    = 196403,
        SPELL_SHADOW_RETREAT        = 196625,
        SPELL_SHADOW_RETREAT_AT     = 195402,

        SPELL_LEARN_CONSUME_MAGIC   = 195439
    };

    enum texts
    {
        SAY_ONCOMBAT_BELIASH = 0,
        SAY_ONDEATH = 1,
    };

    void EnterCombat(Unit*) override
    {
        Talk(SAY_ONCOMBAT_BELIASH);

        me->GetScheduler().Schedule(Milliseconds(2500), [this](TaskContext context)
        {
            me->CastSpell(me, SPELL_SHADOW_BOLT_VOLLEY, true);
            context.Repeat(Milliseconds(2500));
        });

        me->GetScheduler().Schedule(Seconds(10), [this](TaskContext context)
        {
            me->CastSpell(me, SPELL_SHADOW_RETREAT);
            context.Repeat(Seconds(15));

            // During retreat commander make blaze appear
            me->GetScheduler().Schedule({ Milliseconds(500), Milliseconds(1000) }, [this](TaskContext /*context*/)
            {
                me->CastSpell(me, SPELL_SHADOW_RETREAT_AT, true);
            });
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_ONDEATH);

        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 50.0f);

        for (Player* player : players)
        {
            player->ForceCompleteQuest(QUEST_BEFORE_OVERRUN);

            if (!player->HasSpell(SPELL_LEARN_CONSUME_MAGIC))
                player->CastSpell(player, SPELL_LEARN_CONSUME_MAGIC);
        }
    }
};

// 99915 - Sevis Brightflame
struct npc_mardum_sevis_brightflame_shivarra : public ScriptedAI
{
    npc_mardum_sevis_brightflame_shivarra(Creature* creature) : ScriptedAI(creature) { }

    // TEMP FIX, will need gossip
    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 5.0f)
                if (!player->GetQuestObjectiveData(QUEST_SHIVARRA_FORCES, 0))
                    player->KilledMonsterCredit(me->GetEntry());
    }
};





// quest: 38765
class go_mardum_portal_shivarra : public GameObjectScript
{
public:
    go_mardum_portal_shivarra() : GameObjectScript("go_mardum_portal_shivarra") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if (player->GetQuestObjectiveData(QUEST_SHIVARRA_FORCES, 0) &&
            !player->GetQuestObjectiveData(QUEST_SHIVARRA_FORCES, 2) )
        {
            player->ForceCompleteQuest(QUEST_SHIVARRA_FORCES);
            player->CastSpell(player, SPELL_SCENE_MARDUM_SHIVARRA_FORCES, true);
        }

        return false;
    }
};

// 90247, 93693, 94435
class npc_mardum_captain : public CreatureScript
{
public:
    npc_mardum_captain() : CreatureScript("npc_mardum_captain") { }

    enum
    {
        NPC_ASHTONGUE_CAPTAIN   = 90247,
        NPC_COILSKAR_CAPTAIN    = 93693,
        NPC_SHIVARRA_CAPTAIN    = 94435,

        SCENE_ASHTONGUE         = 191315,
        SCENE_COILSKAR          = 191400,
        SCENE_SHIVARRA          = 191402
    };
    // quest: 38813
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
    {
        if (player->GetQuestStatus(38813) == QUEST_STATUS_INCOMPLETE)
        {
            uint32 sceneSpellId = 0;
            uint32 phaseSpellId = 0;
            switch (creature->GetEntry())
            {
                case NPC_ASHTONGUE_CAPTAIN:
                    if (player->GetQuestObjectiveData(38813, 0))
                        return true;
                    sceneSpellId = SCENE_ASHTONGUE;
                    phaseSpellId = SPELL_PHASE_ILLIDARI_OUTPOST_ASHTONGUE;
                    break;
                case NPC_COILSKAR_CAPTAIN:
                    if (player->GetQuestObjectiveData(38813, 1))
                        return true;
                    sceneSpellId = SCENE_COILSKAR;  phaseSpellId = SPELL_PHASE_ILLIDARI_OUTPOST_COILSKAR;   break;
                case NPC_SHIVARRA_CAPTAIN:
                    if (player->GetQuestObjectiveData(38813, 2))
                        return true;
                    sceneSpellId = SCENE_SHIVARRA;  phaseSpellId = SPELL_PHASE_ILLIDARI_OUTPOST_SHIVARRA;   break;
                default: break;
            }

            player->KilledMonsterCredit(creature->GetEntry());

            if (sceneSpellId)
                player->CastSpell(player, sceneSpellId, true);

            if (phaseSpellId)
                player->RemoveAurasDueToSpell(phaseSpellId);

            player->GetScheduler().Schedule(Seconds(15), [player, creature](TaskContext /*context*/)
            {
                creature->DestroyForPlayer(player);
            });
        }
        
        return true;
    }
};

// 96436
class npc_mardum_jace_darkweaver : public CreatureScript
{
public:
    npc_mardum_jace_darkweaver() : CreatureScript("npc_mardum_jace_darkweaver") { }

    enum
    {
        GOB_CAVERN_STONES = 245045
    };

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->KilledMonsterCredit(creature->GetEntry());
        return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == 39495)
        {
            if (Creature* demonHunter1 = creature->FindNearestCreature(101790, 50.0f))
                demonHunter1->CastSpell(demonHunter1, 194326, true);
            if (Creature* demonHunter2 = creature->FindNearestCreature(101787, 50.0f))
                demonHunter2->CastSpell(demonHunter2, 194326, true);
            if (Creature* demonHunter3 = creature->FindNearestCreature(101788, 50.0f))
                demonHunter3->CastSpell(demonHunter3, 194326, true);
            if (Creature* demonHunter4 = creature->FindNearestCreature(101789, 50.0f))
                demonHunter4->CastSpell(demonHunter4, 194326, true);

            if (GameObject* go = creature->FindNearestGameObject(245045, 50.0f))
                go->UseDoorOrButton();

            player->KilledMonsterCredit(98755, ObjectGuid::Empty);
        }

        if (GameObject* personnalCavernStone = player->SummonGameObject(GOB_CAVERN_STONES, 1237.150024f, 1642.619995f, 103.152f, 5.80559f, QuaternionData(0, 0, 20372944, 20372944), 0, true))
        {
            personnalCavernStone->GetScheduler().
            Schedule(2s, [](TaskContext context)
            {
                GetContextGameObject()->SetLootState(GO_READY);
                GetContextGameObject()->UseDoorOrButton(10000);
            }).Schedule(10s, [](TaskContext context)
            {
                GetContextGameObject()->Delete();
            });
        }

        return true;
    }
};

// 188501 spectral sight
class spell_mardum_spectral_sight : public SpellScript
{
    PrepareSpellScript(spell_mardum_spectral_sight);

    void HandleOnCast()
    {
        if (GetCaster()->IsPlayer() && GetCaster()->GetAreaId() == 7754)
            GetCaster()->ToPlayer()->KilledMonsterCredit(96437);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mardum_spectral_sight::HandleOnCast);
    }
};

// 197180
struct npc_mardum_fel_lord_caza : public ScriptedAI
{
    npc_mardum_fel_lord_caza(Creature* creature) : ScriptedAI(creature) { }

    enum Spells
    {
        SPELL_FEL_INFUSION          = 197180,
        SPELL_LEARN_CONSUME_MAGIC   = 195441
    };

    void EnterCombat(Unit*) override
    {
        me->GetScheduler().Schedule(Seconds(10), [this](TaskContext context)
        {
            me->CastSpell(me->GetVictim(), SPELL_FEL_INFUSION, true);
            context.Repeat(Seconds(10));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 50.0f);

        for (Player* player : players)
        {
            player->ForceCompleteQuest(QUEST_HIDDEN_NO_MORE);

            if (!player->HasSpell(SPELL_LEARN_CONSUME_MAGIC))
                player->CastSpell(player, SPELL_LEARN_CONSUME_MAGIC);
        }
    }
};

// 243968 - Banner near 96732 - Destroyed by Ashtongue - KillCredit 96734
// 243967 - Banner near 96731 - Destroyed by Shivarra - KillCredit 96733
// 243965 - Banner near 93762 - Destroyed by Coilskar - KillCredit 96692
//
// quest - 38727
class go_mardum_illidari_banner : public GameObjectScript
{
public:
    go_mardum_illidari_banner() : GameObjectScript("go_mardum_illidari_banner") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (player->GetQuestStatus(38727) == QUEST_STATUS_INCOMPLETE)
        {
            uint32 devastatorEntry = 0;
            uint32 killCreditEntry = 0;
            switch (go->GetEntry())
            {
                case 243968:
                    if (player->GetQuestObjectiveData(38727, 2))
                        return true;
                    devastatorEntry = 96732; killCreditEntry = 96734;  // Doom Fortress
                    break;
                case 243967: 
                    if (player->GetQuestObjectiveData(38727, 4))
                        return true;
                    devastatorEntry = 96731; killCreditEntry = 96733;  // Forge Of Corruption
                    break;
                case 243965:
                    if (player->GetQuestObjectiveData(38727, 0))
                        return true;
                    devastatorEntry = 93762; killCreditEntry = 96692;  // Soul Engine
                    break;
                default: break;
            }

            if (Creature* devastator = player->FindNearestCreature(devastatorEntry, 50.0f))
            {
                if (Creature* personnalCreature = player->SummonCreature(devastatorEntry, devastator->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN, 5000, 0, true))
                {
                    player->KilledMonsterCredit(devastatorEntry);
                    player->KilledMonsterCredit(killCreditEntry);
                    devastator->DestroyForPlayer(player);

                    //TODO : Script destruction event
                    personnalCreature->GetScheduler().Schedule(Seconds(2), [](TaskContext context)
                    {
                        GetContextUnit()->KillSelf();
                    });
                }
            }
        }

        return false;
    }
};

class go_mardum_tome_of_fel_secrets : public GameObjectScript
{
public:
    go_mardum_tome_of_fel_secrets() : GameObjectScript("go_mardum_tome_of_fel_secrets") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        player->CastSpell(player, 194938, true); // Display player spec choice
        return false;
    }
};

class PlayerScript_mardum_spec_choice : public PlayerScript
{
public:
    PlayerScript_mardum_spec_choice() : PlayerScript("PlayerScript_mardum_spec_choice") {}

    void OnPlayerChoiceResponse(Player* player, uint32 choiceID, uint32 responseID) override
    {
        if (choiceID != PLAYER_CHOICE_DH_SPEC_SELECTION)
            return;

        player->LearnSpell(200749, false); // Allow to choose specialization

        switch (responseID)
        {
            case PLAYER_CHOICE_DH_SPEC_SELECTION_DEVASTATION:
                player->CastSpell(player, 194940, true);
                break;
            case PLAYER_CHOICE_DH_SPEC_SELECTION_VENGEANCE:
                player->CastSpell(player, 194939, true);

                if (ChrSpecializationEntry const* spec = sChrSpecializationStore.AssertEntry(581))
                    player->ActivateTalentGroup(spec);

                break;
            default:
                break;
        }
    }
};

// 96655, 93127, 99045, 96420, 96652
class npc_mardum_dh_learn_spec : public CreatureScript
{
public:
    npc_mardum_dh_learn_spec() : CreatureScript("npc_mardum_dh_learn_spec") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->KilledMonsterCredit(creature->GetEntry());

        if (creature->GetEntry() == 96652)
        {
            // TODO Animation power overwhel & kill creature
        }
        else
        {
            // TODO Animation
        }

        return false;
    }
};

// 96653
class npc_mardum_izal_whitemoon : public CreatureScript
{
public:
    npc_mardum_izal_whitemoon() : CreatureScript("npc_mardum_izal_whitemoon") { }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) override
    {
        if (player->HasQuest(QUEST_ON_FELBAT_WINGS) || player->GetQuestStatus(QUEST_ON_FELBAT_WINGS) == QUEST_STATUS_REWARDED)
            player->CastSpell(player, 192136, true); // KillCredit & SendTaxi

        return true;
    }
};

// 93802
struct npc_mardum_tyranna : public ScriptedAI
{
    npc_mardum_tyranna(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* /*killer*/) override
    {
        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 50.0f);

        for (Player* player : players)
            player->KilledMonsterCredit(101760);
    }
};

// 97303
class npc_mardum_kayn_sunfury_end : public CreatureScript
{
public:
    npc_mardum_kayn_sunfury_end() : CreatureScript("npc_mardum_kayn_sunfury_end") { }

    bool OnQuestReward(Player* /*player*/, Creature* /*creature*/, Quest const* /*quest*/, uint32 /*opt*/) override
    {
        // This Scene make the mobs disappear ATM
        //if (quest->GetQuestId() == QUEST_THE_KEYSTONE)
        //    player->CastSpell(player, 193387, true); // Scene

        return true;
    }
};

// 245728
// Quest - 38729
class go_mardum_the_keystone : public GameObjectScript
{
public:
    go_mardum_the_keystone() : GameObjectScript("go_mardum_the_keystone") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if(player->GetQuestStatus(38729) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(100651);
            return false;
        }        

        return true;
    }
};

// 

// spell[192140] - Return to the Black Temple: Quest Completion

class spell_mardum_back_to_black_temple : public SpellScript
{
    PrepareSpellScript(spell_mardum_back_to_black_temple);

    void HandleOnCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            // Should be spell 192141 but we can't cast after a movie right now
            //player->AddMovieDelayedTeleport(471, 1468, 4325.94, -620.21, -281.41, 1.658936);l

            // 为什么传送到主城呢？ 并且播放的动画是471动画吗？

            if (player->GetTeam() == ALLIANCE)
                player->AddMovieDelayedTeleport(471, 0, -8838.72f,   616.29f, 93.06f, 0.779564f);
            else
                player->AddMovieDelayedTeleport(471, 1,  1569.96f, -4397.41f, 16.05f, 0.527317f);

            player->GetScheduler().Schedule(Seconds(2), [](TaskContext context)
            {
                GetContextUnit()->RemoveAurasDueToSpell(192140); // Remove black screen
            });

            // TEMPFIX - Spells learned in next zone
            if (player->GetSpecializationId() == TALENT_SPEC_DEMON_HUNTER_HAVOC)
            {
                player->LearnSpell(188499, false);
                player->LearnSpell(198793, false);
                player->LearnSpell(198589, false);
                player->LearnSpell(179057, false);
            }
            else
            {
                player->LearnSpell(204596, false);
                player->LearnSpell(203720, false);
                player->LearnSpell(204021, false);
                player->LearnSpell(185245, false);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mardum_back_to_black_temple::HandleOnCast);
    }
};

void AddSC_zone_mardum()
{
    new PlayerScript_mardum_welcome_scene_trigger();
    new scene_mardum_welcome();
    new npc_kayn_sunfury_welcome();
    new go_mardum_legion_banner_1();
    new go_mardum_portal_ashtongue();
    new scene_mardum_welcome_ashtongue();
    RegisterSpellScript(spell_learn_felsaber);
    //RegisterCreatureAI(npc_mardum_allari);
    new go_mardum_cage("go_mardum_cage_belath",     94400);
    new go_mardum_cage("go_mardum_cage_cyana",      94377);
    new go_mardum_cage("go_mardum_cage_izal",       93117);
    new go_mardum_cage("go_mardum_cage_mannethrel", 93230);
    RegisterCreatureAI(npc_mardum_inquisitor_pernissius);
    RegisterSpellScript(spell_mardum_infernal_smash);
    new npc_mardum_ashtongue_mystic();
    new go_mardum_portal_coilskar();
    new go_meeting_with_queen_ritual();
    new scene_mardum_meeting_with_queen();
    RegisterCreatureAI(npc_mardum_doom_commander_beliash);
    RegisterCreatureAI(npc_mardum_sevis_brightflame_shivarra);
    RegisterCreatureAI(npc_sevis_enter_coilskar);
    RegisterCreatureAI(npc_mardum_sevis_99917);
    new go_mardum_portal_shivarra();
    new npc_mardum_captain();
    new npc_mardum_jace_darkweaver();
    RegisterSpellScript(spell_mardum_spectral_sight);
    RegisterCreatureAI(npc_mardum_fel_lord_caza);
    new go_mardum_illidari_banner();
    new go_mardum_tome_of_fel_secrets();
    new PlayerScript_mardum_spec_choice();
    new npc_mardum_dh_learn_spec();
    new npc_mardum_izal_whitemoon();
    RegisterCreatureAI(npc_mardum_tyranna);
    new npc_mardum_kayn_sunfury_end();
    new go_mardum_the_keystone();
    RegisterSpellScript(spell_mardum_back_to_black_temple);
}

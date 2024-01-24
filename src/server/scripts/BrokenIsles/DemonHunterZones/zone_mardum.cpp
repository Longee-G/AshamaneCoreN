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
#include "SpellScript.h"
#include "GridNotifiers.h"

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

    SPELL_PHASE_175 = 57569,        // Ashtongue forces
    SPELL_PHASE_176 = 74789,        // Coilskar forces
    SPELL_PHASE_177 = 64576,        // Shivarra forces

    SPELL_PHASE_179 = 67789,        // Felguard Butcher

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

    PATH_Kayn = 9301100,
    PATH_Jayce = 9822800,
    PATH_Allari = 9822700,
    PATH_Korvas = 9829200,
    PATH_Cyana = 9829000,
    PATH_Sevis = 9991800,

    // Quest: Set Them Free NPCs
    NPC_Belath = 94400,
    NPC_Mannethrel = 93230,
    NPC_Izal = 93117,
    NPC_Cyana_2 = 94377,

    SAY_Belath = 96643,
    SAY_Mannethrel = 96202,
    SAY_Izal = 96644,
    SAY_Cyana_2 = 95081,

    SPELL_DemonHunterGlideState = 199303,

    ANIM_DH_Wings = 58110,
    ANIM_DH_Run = 9767,
    ANIM_DH_RunAllari = 9180,
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
            player->RemoveRewardedQuest(quest->ID);
            player->AddAura(SPELL_PHASE_MARDUM_WELCOME);
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

        if (newArea == 0 || newArea == ZONE_MARDUM)
        {
            if (player->GetQuestStatus(STARTING_QUEST) == QUEST_STATUS_NONE
                && !player->HasAura(SPELL_SCENE_MARDUM_WELCOME))
                player->AddAura(SPELL_PHASE_MARDUM_WELCOME);

        }
    }
};

Position const KaynLandPos = { 1117.65f, 3191.72f, 35.4776f };
Position const JayceLandPos = { 1119.6f, 3202.27f, 37.9456f };
Position const AllariLandPos = { 1118.68f, 3198.24f, 36.7279f };
Position const KorvasLandPos = { 1112.27f, 3190.34f, 34.0611f };
Position const SevisLandPos = { 1119.06f, 3192.96f, 35.9639f };
Position const CyanaLandPos = { 1100.82f, 3185.39f, 30.8637f };

// 93011 - `Kayn SunFury`
class npc_kayn_sunfury_welcome : public CreatureScript
{
public:
    npc_kayn_sunfury_welcome() : CreatureScript("npc_kayn_sunfury_welcome") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_INVASION_BEGIN)
        {
            player->RemoveAurasDueToSpell(SPELL_PHASE_MARDUM_WELCOME);
            player->SummonCreature(NPC_Kayn, creature->GetPosition().GetPositionX(), creature->GetPosition().GetPositionY(), creature->GetPosition().GetPositionZ(),
                creature->GetPosition().GetAngle(player) , TEMPSUMMON_MANUAL_DESPAWN, 17000, true);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kayn_sunfury_welcomeAI(creature);
    }

    struct npc_kayn_sunfury_welcomeAI : public ScriptedAI
    {
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
                    if (Creature* npc = me->FindNearestCreature(NPC_Korvas, 10.0f, true))
                        npc->AI()->Talk(0);
                }).Schedule(13s, [this, player](TaskContext context)
                {
                    me->GetMotionMaster()->MovePath(PATH_Kayn, false);

                    if (Creature* npc = me->FindNearestCreature(NPC_Jayce, 10.0f, true))
                        npc->GetMotionMaster()->MovePath(PATH_Jayce, false);

                    if (Creature* npc = me->FindNearestCreature(NPC_Allari, 10.0f, true))
                        npc->GetMotionMaster()->MovePath(PATH_Allari, false);

                    if (Creature* npc = me->FindNearestCreature(NPC_Sevis, 10.0f, true))
                        npc->GetMotionMaster()->MovePath(PATH_Sevis, false);

                    if (Creature* npc = me->FindNearestCreature(NPC_Korvas, 10.0f, true))
                        npc->GetMotionMaster()->MovePath(PATH_Korvas, false);

                    if (Creature* npc = me->FindNearestCreature(NPC_Cyana, 10.0f, true))
                        npc->GetMotionMaster()->MovePath(PATH_Cyana, false);
                });
            }
        }

        void MovementInform(uint32 moveType, uint32 data) override
        {
            if (data == EVENT_JUMP)
            {   
                me->SendCancelSpellVisualKit(SPELL_VISUAL_KIT_KAYN_DOUBLE_JUMP);
                me->SendCancelSpellVisualKit(SPELL_VISUAL_KIT_KAYN_WINGS);
                me->SetAIAnimKitId(ANIM_DH_Run);
                me->GetMotionMaster()->MovePath(PATH_Kayn+1, false);
            }
        }

        void OnWaypointPathEnded(uint32 pointId, uint32 pathId) override
        { 
            if (pathId == PATH_Kayn)
            {
                TempSummon* summon = me->ToTempSummon();
                if (!summon)
                    return;
                WorldObject* summoner = summon->GetSummoner();
                if (!summoner)
                    return;

                me->PlayObjectSound(53780, me->GetGUID(), summoner->ToPlayer());
                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_KAYN_DOUBLE_JUMP, 0, 0);
                me->GetScheduler().Schedule(600ms, [this](TaskContext context)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_KAYN_WINGS, 4, 3000);
                });

                me->SetAIAnimKitId(ANIM_DH_Run);
                me->GetMotionMaster()->MoveJump(KaynLandPos, 10.0f, 10.0f, EVENT_JUMP);
            }
            else if (pathId == PATH_Kayn + 1)
                me->DespawnOrUnsummon();
        }
    };
};

// 98227 - Allari the Souleater
struct npc_allari_the_souleater_invasion_begins : public ScriptedAI
{
    npc_allari_the_souleater_invasion_begins(Creature* creature) : ScriptedAI(creature) {}
    void OnWaypointPathEnded(uint32 /*pointId*/, uint32 pathId) override
    {
        if (pathId == PATH_Allari)
        {
            me->CastSpell(nullptr, SPELL_DemonHunterGlideState, true);
            me->GetScheduler().Schedule(600ms, [this](TaskContext /*context*/)
            {
                me->GetMotionMaster()->MoveJump(AllariLandPos, 10.0f, 10.0f, EVENT_JUMP);
            });
        }
        else if (pathId == PATH_Allari + 1)
            me->DespawnOrUnsummon();
    }
    void MovementInform(uint32 type, uint32 data) override
    {
        if (type != EFFECT_MOTION_TYPE) // only catch MoveJump Event
            return;
        if (data == EVENT_JUMP)
        {
            me->RemoveAurasDueToSpell(SPELL_DemonHunterGlideState);
            me->SetAIAnimKitId(ANIM_DH_RunAllari);
            me->GetMotionMaster()->MovePath(PATH_Allari + 1, false);
        }
    }
};

// 98228 - Jayce Darkweaver
struct npc_jayce_darkweaver_invasion_begins : public ScriptedAI
{
    npc_jayce_darkweaver_invasion_begins(Creature* creature) : ScriptedAI(creature) {}
    void OnWaypointPathEnded(uint32 /*pointId*/, uint32 pathId) override
    {
        if (pathId == PATH_Jayce)
        {
            me->CastSpell(nullptr, SPELL_DemonHunterGlideState, true);
            me->GetMotionMaster()->MoveJump(JayceLandPos, 10.0f, 10.0f, EVENT_JUMP);
        }
        else if (pathId == PATH_Jayce + 1)
            me->DespawnOrUnsummon();
    }
    void MovementInform(uint32 type, uint32 data) override
    {
        if (type != EFFECT_MOTION_TYPE)
            return;
        if (data == EVENT_JUMP)
        {
            me->RemoveAurasDueToSpell(SPELL_DemonHunterGlideState);
            me->SetSheath(SHEATH_STATE_MELEE);  // show weapon
            me->SetAIAnimKitId(ANIM_DH_Run);            
            me->GetMotionMaster()->MovePath(PATH_Jayce + 1, false);
        }
    }
};

// 98292 - Korvas Bloodthorn
struct npc_korvas_bloodthorn_invasion_begins : public ScriptedAI
{
    npc_korvas_bloodthorn_invasion_begins(Creature* creature) : ScriptedAI(creature) {}
    void OnWaypointPathEnded(uint32 /*pointId*/, uint32 pathId) override
    {
        if (pathId == PATH_Korvas)
        {
            me->CastSpell(nullptr, SPELL_DemonHunterGlideState, true);
            me->GetMotionMaster()->MoveJump(KorvasLandPos, 10.0f, 10.0f, EVENT_JUMP);
        }
        else if (pathId == PATH_Korvas + 1)
            me->DespawnOrUnsummon();
    }
    void MovementInform(uint32 type, uint32 data) override
    {
        if (type != EFFECT_MOTION_TYPE)
            return;
        if (data == EVENT_JUMP)
        {
            me->RemoveAurasDueToSpell(SPELL_DemonHunterGlideState);
            me->SetAIAnimKitId(ANIM_DH_Run);
            me->GetMotionMaster()->MovePath(PATH_Korvas + 1, false);
        }
    }
};

// 99918 - Sevis Brightflame
struct npc_sevis_brightflame_invasion_begins : public ScriptedAI
{
    npc_sevis_brightflame_invasion_begins(Creature* creature) : ScriptedAI(creature) {}
    void OnWaypointPathEnded(uint32 /*pointId*/, uint32 pathId) override
    {
        if (pathId == PATH_Sevis)
        {
            me->CastSpell(nullptr, SPELL_DemonHunterGlideState, true);
            me->GetMotionMaster()->MoveJump(SevisLandPos, 10.0f, 10.0f, EVENT_JUMP);
        }
        else if (pathId == PATH_Sevis + 1)
            me->DespawnOrUnsummon();
    }
    void MovementInform(uint32 type, uint32 data) override
    {
        if (type != EFFECT_MOTION_TYPE)
            return;
        if (data == EVENT_JUMP)
        {
            me->RemoveAurasDueToSpell(SPELL_DemonHunterGlideState);
            me->SetAIAnimKitId(ANIM_DH_Run);
            me->GetMotionMaster()->MovePath(PATH_Sevis + 1, false);
        }
    }
};

// 98290 - Cyana Nightglaive
struct npc_cyana_nightglaive_invasion_begins : public ScriptedAI
{
    npc_cyana_nightglaive_invasion_begins(Creature* creature) : ScriptedAI(creature) {}
    void OnWaypointPathEnded(uint32 /*pointId*/, uint32 pathId) override
    {
        if (pathId == PATH_Cyana)
        {
            me->CastSpell(nullptr, SPELL_DemonHunterGlideState, true);
            me->GetMotionMaster()->MoveJump(CyanaLandPos, 10.0f, 10.0f, EVENT_JUMP);
        }
        else if (pathId == PATH_Cyana + 1)
            me->DespawnOrUnsummon();
    }
    void MovementInform(uint32 type, uint32 data) override
    {
        if (type != EFFECT_MOTION_TYPE)
            return;
        if (data == EVENT_JUMP)
        {
            me->RemoveAurasDueToSpell(SPELL_DemonHunterGlideState);
            //me->SetAIAnimKitId(ANIM_DH_Run);
            me->GetMotionMaster()->MovePath(PATH_Cyana + 1, false);
        }
    }
};


// 98459 - Kayn Sunfury
// 98458 - Jayce Darkweaver
// 98456 - Allari the Souleater
// 98460 - Korvas Bloodthorn
// 99919 - Sevis Brightflame
// 98457 - Cyana Nightglaive
struct npc_illidari_fighting_invasion_begins : public ScriptedAI
{
    npc_illidari_fighting_invasion_begins(Creature* creature) : ScriptedAI(creature) {}

    enum FightingEvents
    {
        EVENT_CHAOS_STRIKE = 1,
        EVENT_FEL_RUSH
    };

    Unit* GetNextTarget()
    {
        std::list<Unit*> targetList;        
        Trinity::AnyUnfriendlyUnitInObjectRangeCheck checker(me, me, 35.0f);
        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targetList, checker);
        Cell::VisitAllObjects(me, searcher, 100.0f);
        targetList.remove_if([](Unit* possibleTarget)
        {
            if (possibleTarget->GetGUID().IsBattlePet())
                return true;

            return possibleTarget->isAttackingPlayer();
        });

        return Trinity::Containers::SelectRandomContainerElement(targetList);;
    }

    void ScheduleTargetSelection()
    {
        _scheduler.Schedule(200ms, [this](TaskContext context)
        {
            Unit* target = GetNextTarget();
            if (!target)
            {
                context.Repeat(1500ms);
                return;
            }
            AttackStart(target);
        });
    }

    void JustRespawned() override
    {
        ScheduleTargetSelection();
    }

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*victim*/) override
    {
        events.ScheduleEvent(EVENT_CHAOS_STRIKE, 5s);
        events.ScheduleEvent(EVENT_FEL_RUSH, 7s);
    }
    // leave combat
    void EnterEvadeMode(EvadeReason why) override
    {
        // manualling calling it to not move to home position but move to next target instead
        _EnterEvadeMode(why);
        Reset();
        ScheduleTargetSelection();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            _scheduler.Update(diff);
            return;
        }

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_CHAOS_STRIKE:
            DoCastVictim(197639); // SPELL_ILLIDARI_CHAOS_STRIKE
            events.ScheduleEvent(EVENT_CHAOS_STRIKE, 5s);
            break;
        case EVENT_FEL_RUSH:
            DoCastVictim(200879);   // SPELL_ILLIDARI_FEL_RUSH
            events.ScheduleEvent(EVENT_FEL_RUSH, 7s);
            break;
        default:
            break;
        }
    }
private:
    TaskScheduler _scheduler;
};


// 244439/244440/244441 - Legion Communicator
// 39279 - quest "Asault on Mardum"
class go_legion_communicator : public GameObjectScript
{
public:
    go_legion_communicator() : GameObjectScript("go_legion_communicator") {}

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (go->GetEntry() == 244439)   // Legion Communicator #1
        {
            UpdateObjective(player, go, 281333, 102223, 558);
        }
        else if (go->GetEntry() == 244440) // Legion Communicator #2
        {
            return !UpdateObjective(player, go, 281334, 102224, 583);
        }
        else if (go->GetEntry() == 244441) // Legion Communicator #3
        {
            // Brood Queen Tyranna says: Whoever this is, you're caught in my web now.
            return !UpdateObjective(player, go, 281335, 102225, 583);   // what conversation id?
        }
        return false;
    }
private:
    bool UpdateObjective(Player* player, GameObject* go, uint32 objectiveId, uint32 killCredit, uint32 conversationId)
    {
        // 需要调用这个接口，才能用GO关联的spell触发
        go->AddUniqueUse(player);


        //if (!player->GetQuestObjectiveCounter(objectiveId))
        {
            // FIXME: ... 这个有问题 ... 不能输入guid参数，这个参数好像是用来找creature的 ...

            player->KilledMonsterCredit(killCredit, go->GetGUID());
            Conversation::CreateConversation(conversationId, player, *player, { player->GetGUID() }, nullptr);

            player->KilledMonsterCredit(go->GetEntry());
            return true;
        }
        return false;
    }
};

// Fel Spreader is NPC - 97142
// in table `npc_spellclick_spells`, associated with spell_id:191827
// 当player和npc交互，会自动释放法术
class spell_destroying_fel_spreader : public SpellScript
{
    PrepareSpellScript(spell_destroying_fel_spreader);
    void HandleHitDefaultEffect(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                if (Unit* target = GetHitUnit())
                {
                    // 将npc添加到隐藏列表，让player不能再看到这个npc
                    // 不使用相位让某个creature或者gameobject只对某个player不可见...
                    // TODO: 还没有实现 ... 怎么实现这个功能
                                       

                    target->DestroyForPlayer(player);
                    // 通过这个方式来增加突袭马顿的进度 ...
                    player->KilledMonsterCredit(target->GetEntry());
                    Conversation::CreateConversation(581, player, *player, { player->GetGUID() }, nullptr);
                }
            }
        }

    }
    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_destroying_fel_spreader::HandleHitDefaultEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};


// Legion版本使用 `go-244898` 是一个旗子

// 任务目标，激活会烧掉军团的旗帜..
class go_mardum_legion_banner_1 : public GameObjectScript
{
public:
    go_mardum_legion_banner_1() : GameObjectScript("go_mardum_legion_banner_1") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (!player->IsQuestObjectiveComplete(QUEST_INVASION_BEGIN, 0))
            return true;

        // bugfix: 当任务目标第1步没有完成，不能和go进行交互 ...

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

// TODO: 激活器需要通过phase来控制，当被点击之后，需要消失掉 ...

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
struct npc_mardum_allari : public ScriptedAI
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

    // TEMP FIX, need gossip?
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
    RegisterCreatureAI(npc_jayce_darkweaver_invasion_begins);
    RegisterCreatureAI(npc_allari_the_souleater_invasion_begins);
    RegisterCreatureAI(npc_korvas_bloodthorn_invasion_begins);
    RegisterCreatureAI(npc_sevis_brightflame_invasion_begins);
    RegisterCreatureAI(npc_cyana_nightglaive_invasion_begins);
    RegisterCreatureAI(npc_illidari_fighting_invasion_begins);

    new go_legion_communicator();
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

    RegisterSpellScript(spell_destroying_fel_spreader);
}

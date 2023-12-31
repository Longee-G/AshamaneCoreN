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
#include "BattlePayMgr.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "SpellMgr.h"



namespace BattlePay
{
    namespace ProfessionBookSpells
    {
        enum
        {
            Alchemy = 156614,
            Blacksmithing = 169923,
            Enchanting = 161788,
            Engineering = 161787,
            Inscription = 161789,
            JewelCrafting = 169926,
            LeatherWorking = 169925,
            Tailoring = 169924,
            FirstAid = 160329,
            Cooking = 160360,
            Herbalism = 158745,
            Mining = 158754,
            Skinning = 158756,
            Archaeology = 158762,
            Fishing = 160326
        };
    }
    // <skillid, spellid> pair
    static std::map<uint32, uint32> SkillLearningSpells =
    {
        { SKILL_ALCHEMY,        ProfessionBookSpells::Alchemy        },
        { SKILL_BLACKSMITHING,  ProfessionBookSpells::Blacksmithing  },
        { SKILL_ENCHANTING,     ProfessionBookSpells::Enchanting     },
        { SKILL_ENGINEERING,    ProfessionBookSpells::Engineering    },
        { SKILL_INSCRIPTION,    ProfessionBookSpells::Inscription    },
        { SKILL_JEWELCRAFTING,  ProfessionBookSpells::JewelCrafting  },
        { SKILL_LEATHERWORKING, ProfessionBookSpells::LeatherWorking },
        { SKILL_TAILORING,      ProfessionBookSpells::Tailoring      },
        { SKILL_FIRST_AID,      ProfessionBookSpells::FirstAid       },
        { SKILL_COOKING,        ProfessionBookSpells::Cooking        },
        { SKILL_HERBALISM,      ProfessionBookSpells::Herbalism      },
        { SKILL_MINING,         ProfessionBookSpells::Mining         },
        { SKILL_SKINNING,       ProfessionBookSpells::Skinning       },
        { SKILL_ARCHAEOLOGY,    ProfessionBookSpells::Archaeology    },
        { SKILL_FISHING,        ProfessionBookSpells::Fishing        }
    };
}

template<uint32 t_SkillID, uint32 t_Value>
class BattlePay_Profession : BattlePayProductScript
{
public:
    explicit BattlePay_Profession(std::string scriptName) : BattlePayProductScript(scriptName) {}

    void OnProductDelivery(WorldSession* session, BattlePay::Product const& /*product*/) override
    {
        auto player = session->GetPlayer();
        if (!player)
            return;

        auto spellID = BattlePay::SkillLearningSpells[t_SkillID];

        // 为什么要cast？在这里
        player->CastSpell(player, spellID, true);
        player->SetSkill(t_SkillID, player->GetSkillStep(t_SkillID), t_Value, t_Value);

        // 一个skill会关联若干的spell...
        for (auto itr : sSpellMgr->GetTradeSpellFromSkill(t_SkillID))
            player->LearnSpell(itr->Spell, false);

        // 特殊处理？为什么需要这样做呢？ ... 数据库配置有问题吗？
        switch (t_SkillID)
        {
        case SKILL_JEWELCRAFTING:
            player->LearnSpell(31252 /*Prospecting*/, false);   // 
            break;
        case SKILL_HERBALISM:   // 草药学..
        case SKILL_MINING:      // 采矿 为什么只有这两个技能需要学习关联的spell
            player->LearnSpell(spellID, false);
            break;
        default:
            break;
        }

        player->SaveToDB();
    }

    bool CanBuy(WorldSession* session, BattlePay::Product const& /*product*/, std::string& reason) override
    {
        auto player = session->GetPlayer();
        if (!player)
        {
            reason = sObjectMgr->GetTrinityString(BattlePay::String::NeedToBeInGame, session->GetSessionDbLocaleIndex());
            return false;
        }

        if (BattlePay::SkillLearningSpells.find(t_SkillID) == BattlePay::SkillLearningSpells.end())
            return false;

        if (player->getLevel() < 90)
        {
            reason = sObjectMgr->GetTrinityString(BattlePay::String::Level90Required, session->GetSessionDbLocaleIndex());
            return false;
        }

        if (IsPrimaryProfessionSkill(t_SkillID) && !player->HasSkill(t_SkillID) && player->GetFreePrimaryProfessionPoints() == 0)
        {
            reason = sObjectMgr->GetTrinityString(BattlePay::String::ReachPrimaryProfessionLimit, session->GetSessionDbLocaleIndex());
            return false;
        }

        if (player->HasSkill(t_SkillID) && player->GetSkillValue(t_SkillID) == t_Value)
        {
            reason = sObjectMgr->GetTrinityString(BattlePay::String::YouAlreadyOwnThat, session->GetSessionDbLocaleIndex());
            return false;
        }

        return true;
    }

    std::string GetCustomData(BattlePay::Product const& /*product*/) override
    {
        return R"({\"skill_id\": )" + std::to_string(t_SkillID) + "}";
    }
};

void AddSC_BattlePay_Professions()
{
    new BattlePay_Profession<SKILL_ALCHEMY, 800>("battlepay_profession_alchemy");
    new BattlePay_Profession<SKILL_BLACKSMITHING, 800>("battlepay_profession_blacksmithing");
    new BattlePay_Profession<SKILL_ENCHANTING, 800>("battlepay_profession_enchanting");
    new BattlePay_Profession<SKILL_ENGINEERING, 800>("battlepay_profession_engineering");
    new BattlePay_Profession<SKILL_INSCRIPTION, 800>("battlepay_profession_inscription");
    new BattlePay_Profession<SKILL_JEWELCRAFTING, 800>("battlepay_profession_jewelcrafting");
    new BattlePay_Profession<SKILL_LEATHERWORKING, 800>("battlepay_profession_leatherworking");
    new BattlePay_Profession<SKILL_TAILORING, 800>("battlepay_profession_tailoring");
    new BattlePay_Profession<SKILL_FIRST_AID, 800>("battlepay_profession_first_aid");
    new BattlePay_Profession<SKILL_COOKING, 800>("battlepay_profession_cooking");
    new BattlePay_Profession<SKILL_HERBALISM, 800>("battlepay_profession_herbalism");
    new BattlePay_Profession<SKILL_MINING, 800>("battlepay_profession_mining");
    new BattlePay_Profession<SKILL_SKINNING, 800>("battlepay_profession_skinning");
    new BattlePay_Profession<SKILL_ARCHAEOLOGY, 800>("battlepay_profession_archaeology");
    new BattlePay_Profession<SKILL_FISHING, 800>("battlepay_profession_fishing");
}

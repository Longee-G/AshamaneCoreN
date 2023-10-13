/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
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

#include "BattlePet.h"
#include "BattlePetDataStore.h"
#include "DB2Stores.h"
#include "Containers.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldSession.h"
#include "ObjectGuid.h"
#include "AchievementMgr.h"

// 根据宠物的各种系数来计算宠物战斗属性...
void BattlePet::CalculateStats()
{
    float health = 0.0f;
    float power = 0.0f;
    float speed = 0.0f;
    int8 gender = 0;

    // get base breed stats
    BattlePetStateMap* breedState = sBattlePetDataStore->GetPetBreedStats(Breed);
    if (!breedState) // non existing breed id
        return;

    // 通过表来获得pet的基础属性...
    health = (*breedState)[STATE_STAT_STAMINA];
    power = (*breedState)[STATE_STAT_POWER];
    speed = (*breedState)[STATE_STAT_SPEED];
    gender = (*breedState)[STATE_GENDER];


    int32 hpBonus = 0;

    // modify stats depending on species - not all pets have this
    BattlePetStateMap* speciesState = sBattlePetDataStore->GetPetSpeciesStats(Species);
    if (speciesState)
    {
        health += (*speciesState)[STATE_STAT_STAMINA];
        power += (*speciesState)[STATE_STAT_POWER];
        speed += (*speciesState)[STATE_STAT_SPEED];

        hpBonus = (*speciesState)[STATE_MAX_HEALTH_BONUS];
    }

    // modify stats by quality
    for (BattlePetBreedQualityEntry const* battlePetBreedQuality : sBattlePetBreedQualityStore)
    {
        if (battlePetBreedQuality->QualityEnum == Quality)
        {
            health *= battlePetBreedQuality->StateMultiplier;
            power *= battlePetBreedQuality->StateMultiplier;
            speed *= battlePetBreedQuality->StateMultiplier;

            break;
        }
        // TOOD: add check if pet has existing quality
    }

    // scale stats depending on level
    health *= Level;
    power *= Level;
    speed *= Level;

    // set stats
    // round, ceil or floor? verify this
    MaxHealth = uint32((round(health / 20) + 100))  + hpBonus;
    Power = uint32(round(power / 100));
    Speed = uint32(round(speed / 100));
    Health = MaxHealth;
    PetGender = gender;
}

BattlePetSpeciesEntry const* BattlePet::GetSpecies()
{
    return sBattlePetSpeciesStore.LookupEntry(Species);
}


// 将宠物给player..
ObjectGuid::LowType BattlePet::AddToPlayer(Player * player)
{
    AccountID = player->GetSession()->GetAccountId();

    auto guidLow = sObjectMgr->GetGenerator<HighGuid::BattlePet>().Generate();
    Guid = ObjectGuid::Create<HighGuid::BattlePet>(guidLow);
    // 构建存储Pet到数据库的语句, 这个是一条update语句...
    PreparedStatement* statement = CharacterDatabase.GetPreparedStatement(CHAR_REP_PETBATTLE);
    // 将数据库查询结构填入..
    statement->setUInt64(0, guidLow);
    statement->setInt32(1, Slot);
    statement->setString(2, Name);
    statement->setUInt32(3, NameTimeStamp);
    statement->setUInt32(4, Species);
    statement->setUInt32(5, Quality);
    statement->setUInt32(6, Breed);
    statement->setUInt32(7, Level);
    statement->setUInt32(8, Exp);
    statement->setUInt32(9, DisplayModelID);
    statement->setInt32(10, Health);
    statement->setUInt32(11, Flags);
    statement->setInt32(12, Power);
    statement->setInt32(13, MaxHealth);
    statement->setInt32(14, Speed);
    statement->setInt32(15, PetGender);
    statement->setInt32(16, AccountID);
    statement->setString(17, "");     // DeclinedNames[0] TODO:
    statement->setString(18, "");
    statement->setString(19, "");
    statement->setString(20, "");
    statement->setString(21, "");

    CharacterDatabase.Execute(statement);

    // 当得到宠物之后，需要更新成就？成就的完成是靠这个方式来触发的吗？
    player->GetAchievementMgr()->UpdateCriteria(CRITERIA_TYPE_OWN_BATTLE_PET_COUNT, 1);



    return guidLow;
}

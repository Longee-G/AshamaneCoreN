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

#include "GarrisonPackets.h"
#include "DB2Structure.h"

WorldPacket const* WorldPackets::Garrison::GarrisonCreateResult::Write()
{
    _worldPacket << uint32(Result);
    _worldPacket << uint32(GarrSiteLevelID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonDeleteResult::Write()
{
    _worldPacket << uint32(Result);
    _worldPacket << uint32(GarrSiteID);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonPlotInfo& plotInfo)
{
    data << uint32(plotInfo.GarrPlotInstanceID);
    data << plotInfo.PlotPos;
    data << uint32(plotInfo.PlotType);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonBuildingInfo const& buildingInfo)
{
    data << uint32(buildingInfo.GarrPlotInstanceID);
    data << uint32(buildingInfo.GarrBuildingID);
    data << uint32(buildingInfo.TimeBuilt);
    data << uint32(buildingInfo.CurrentGarSpecID);
    data << uint32(buildingInfo.TimeSpecCooldown);
    data.WriteBit(buildingInfo.Active);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonFollower const& follower)
{
    data << uint64(follower.DbID);
    data << uint32(follower.GarrFollowerID);
    data << uint32(follower.Quality);
    data << uint32(follower.FollowerLevel);
    data << uint32(follower.ItemLevelWeapon);
    data << uint32(follower.ItemLevelArmor);
    data << uint32(follower.Xp);
    data << uint32(follower.Durability);
    data << uint32(follower.CurrentBuildingID);
    data << uint32(follower.CurrentMissionID);
    data << uint32(follower.AbilityID.size());
    data << uint32(follower.ZoneSupportSpellID);
    data << uint32(follower.FollowerStatus);
    for (GarrAbilityEntry const* ability : follower.AbilityID)
        data << uint32(ability->ID);

    data.WriteBits(follower.CustomName.length(), 7);
    data.FlushBits();
    data.WriteString(follower.CustomName);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonMission const& mission)
{
    data << uint64(mission.DbID);
    data << uint32(mission.MissionRecID);
    data << uint32(mission.OfferTime);
    data << uint32(mission.OfferDuration);
    data << uint32(mission.StartTime);
    data << uint32(mission.TravelDuration);
    data << uint32(mission.MissionDuration);
    data << uint32(mission.MissionState);
    data << uint32(mission.SuccessChance);
    data << uint32(mission.Unknown2);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonMissionReward const& missionRewardItem)
{
    data << int32(missionRewardItem.ItemID);
    data << uint32(missionRewardItem.ItemQuantity);
    data << int32(missionRewardItem.CurrencyID);
    data << uint32(missionRewardItem.CurrencyQuantity);
    data << uint32(missionRewardItem.FollowerXP);
    data << uint32(missionRewardItem.BonusAbilityID);
    data << int32(missionRewardItem.Unknown);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonMissionAreaBonus const& areaBonus)
{
    data << uint32(areaBonus.GarrMssnBonusAbilityID);
    data << uint32(areaBonus.StartTime);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonTalent const& talent)
{
    data << int32(talent.GarrTalentID);
    data << int32(talent.ResearchStartTime);
    data << int32(talent.Flags);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonInfo const& garrison)
{
    ASSERT(garrison.Missions.size() == garrison.MissionRewards.size());
    ASSERT(garrison.Missions.size() == garrison.MissionBonusRewards.size());
    ASSERT(garrison.Missions.size() == garrison.CanStartMission.size());

    data << int32(garrison.GarrTypeID);
    data << int32(garrison.GarrSiteID);
    data << int32(garrison.GarrSiteLevelID);
    data << uint32(garrison.Buildings.size());
    data << uint32(garrison.Plots.size());
    data << uint32(garrison.Followers.size());
    data << uint32(garrison.Missions.size());
    data << uint32(garrison.MissionRewards.size());
    data << uint32(garrison.MissionBonusRewards.size());
    data << uint32(garrison.MissionAreaBonuses.size());
    data << uint32(garrison.Talents.size());
    data << uint32(garrison.CanStartMission.size());
    data << uint32(garrison.ArchivedMissions.size());
    data << int32(garrison.NumFollowerActivationsRemaining);
    data << uint32(garrison.NumMissionsStartedToday);

    for (WorldPackets::Garrison::GarrisonPlotInfo* plot : garrison.Plots)
        data << *plot;

    for (WorldPackets::Garrison::GarrisonMission const* mission : garrison.Missions)
        data << *mission;

    for (std::vector<WorldPackets::Garrison::GarrisonMissionReward> const& missionReward : garrison.MissionRewards)
    {
        data << uint32(missionReward.size());
        for (WorldPackets::Garrison::GarrisonMissionReward const& missionRewardItem : missionReward)
            data << missionRewardItem;
    }

    for (std::vector<WorldPackets::Garrison::GarrisonMissionReward> const& missionReward : garrison.MissionBonusRewards)
    {
        data << uint32(missionReward.size());
        for (WorldPackets::Garrison::GarrisonMissionReward const& missionRewardItem : missionReward)
            data << missionRewardItem;
    }

    for (WorldPackets::Garrison::GarrisonMissionAreaBonus const* areaBonus : garrison.MissionAreaBonuses)
        data << *areaBonus;

    for (WorldPackets::Garrison::GarrisonTalent const& talent : garrison.Talents)
        data << talent;

    if (!garrison.ArchivedMissions.empty())
        data.append(garrison.ArchivedMissions.data(), garrison.ArchivedMissions.size());

    for (WorldPackets::Garrison::GarrisonBuildingInfo const* building : garrison.Buildings)
        data << *building;

    for (bool canStartMission : garrison.CanStartMission)
        data.WriteBit(canStartMission);

    data.FlushBits();

    for (WorldPackets::Garrison::GarrisonFollower const* follower : garrison.Followers)
        data << *follower;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::FollowerSoftCapInfo const& followerSoftCapInfo)
{
    data << int32(followerSoftCapInfo.GarrFollowerTypeID);
    data << uint32(followerSoftCapInfo.Count);
    return data;
}

WorldPacket const* WorldPackets::Garrison::GetGarrisonInfoResult::Write()
{
    _worldPacket << int32(FactionIndex);
    _worldPacket << uint32(Garrisons.size());
    _worldPacket << uint32(FollowerSoftCaps.size());
    for (FollowerSoftCapInfo const& followerSoftCapInfo : FollowerSoftCaps)
        _worldPacket << followerSoftCapInfo;

    for (GarrisonInfo const& garrison : Garrisons)
        _worldPacket << garrison;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonRemoteBuildingInfo const& building)
{
    data << uint32(building.GarrPlotInstanceID);
    data << uint32(building.GarrBuildingID);

    return data;
}

// 重载操作符 `<< `
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonRemoteSiteInfo const& site)
{
    data << uint32(site.GarrSiteLevelID);
    data << uint32(site.Buildings.size());
    for (WorldPackets::Garrison::GarrisonRemoteBuildingInfo const& building : site.Buildings)
        data << building;

    return data;
}

WorldPacket const* WorldPackets::Garrison::GarrisonRemoteInfo::Write()
{
    _worldPacket << uint32(Sites.size());
    for (GarrisonRemoteSiteInfo const& site : Sites)
        _worldPacket << site;           // [Longee] 结构并没有实现 << 运算符，这个是怎么实现的？ ... 在前面的代码中实现了

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonPurchaseBuilding::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> PlotInstanceID;
    _worldPacket >> BuildingID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonPlaceBuildingResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << BuildingInfo;
    _worldPacket.WriteBit(PlayActivationCinematic);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonCancelConstruction::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> PlotInstanceID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonBuildingRemoved::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(GarrPlotInstanceID);
    _worldPacket << uint32(GarrBuildingID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonLearnBlueprintResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(BuildingID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonUnlearnBlueprintResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(BuildingID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonCheckUpgradeableResult::Write()
{
    _worldPacket << uint32(!IsUpgradeable);

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonUpgrade::Read()
{
    _worldPacket >> NpcGUID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonUpgradeResult::Write()
{
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonRequestBlueprintAndSpecializationDataResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(BlueprintsKnown ? BlueprintsKnown->size() : 0);
    _worldPacket << uint32(SpecializationsKnown ? SpecializationsKnown->size() : 0);
    if (BlueprintsKnown)
        for (uint32 blueprint : *BlueprintsKnown)
            _worldPacket << uint32(blueprint);

    if (SpecializationsKnown)
        for (uint32 specialization : *SpecializationsKnown)
            _worldPacket << uint32(specialization);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::GarrisonBuildingLandmark& landmark)
{
    data << uint32(landmark.GarrBuildingPlotInstID);
    data << landmark.Pos;

    return data;
}

WorldPacket const* WorldPackets::Garrison::GarrisonBuildingLandmarks::Write()
{
    _worldPacket << uint32(Landmarks.size());
    for (GarrisonBuildingLandmark& landmark : Landmarks)
        _worldPacket << landmark;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonPlotPlaced::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << *PlotInfo;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonPlotRemoved::Write()
{
    _worldPacket << uint32(GarrPlotInstanceID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonAddFollowerResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << Follower;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonRemoveFollowerResult::Write()
{
    _worldPacket << uint64(FollowerDBID);
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(Destroyed);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonBuildingActivated::Write()
{
    _worldPacket << uint32(GarrPlotInstanceID);

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonOpenMissionNpcClient::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> GarrTypeID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonOpenMissionNpc::Write()
{
    _worldPacket << int32(garrType);
    _worldPacket << int32(result);
    _worldPacket << uint32(Missions.size());

    for (auto const& missionId : Missions)
        _worldPacket << int32(missionId);

    _worldPacket.WriteBit(unk4);
    _worldPacket.WriteBit(preventXmlOpenMissionEvent);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::ShowAdventureMap::Write()
{
    _worldPacket << Unit;

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonRequestScoutingMap::Read()
{
    _worldPacket >> ID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonScoutingMapResult::Write()
{
    _worldPacket << ID;
    _worldPacket.WriteBit(Active);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonAddMissionResult::Write()
{
    _worldPacket << GarrType;
    _worldPacket << Result;
    _worldPacket << State;
    _worldPacket << Mission;

    _worldPacket << uint32(Rewards.size());
    _worldPacket << uint32(BonusRewards.size());

    for (WorldPackets::Garrison::GarrisonMissionReward const& missionRewardItem : Rewards)
        _worldPacket << missionRewardItem;

    for (WorldPackets::Garrison::GarrisonMissionReward const& missionRewardItem : BonusRewards)
        _worldPacket << missionRewardItem;

    _worldPacket.WriteBit(Success);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonStartMission::Read()
{
    _worldPacket >> NpcGUID;

    uint32 followerCount = 0;
    _worldPacket >> followerCount;
    _worldPacket >> MissionID;

    for (uint32 i = 0; i < followerCount; ++i)
    {
        uint64 followerDbID;
        _worldPacket >> followerDbID;
        Followers.push_back(followerDbID);
    }
}

WorldPacket const* WorldPackets::Garrison::GarrisonStartMissionResult::Write()
{
    _worldPacket << Result;
    _worldPacket << FailReason;
    _worldPacket << Mission;

    _worldPacket << Followers.size();
    for (uint64 followerDbID : Followers)
        _worldPacket << followerDbID;

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonCompleteMission::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> MissionID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonCompleteMissionResult::Write()
{
    _worldPacket << Result;
    _worldPacket << Mission;
    _worldPacket << Mission.MissionRecID;
    _worldPacket << uint32(Followers.size());

    for (auto itr : Followers)
    {
        _worldPacket << itr.first;
        _worldPacket << itr.second;
    }

    _worldPacket.WriteBit(Succeed);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonMissionBonusRoll::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> MissionID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonMissionBonusRollResult::Write()
{
    _worldPacket << Mission;
    _worldPacket << Mission.MissionRecID;
    _worldPacket << Result;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonFollowerChangeXP::Write()
{
    _worldPacket << XP;
    _worldPacket << Unk;
    _worldPacket << OldFollower;
    _worldPacket << NewFollower;

    return &_worldPacket;
}

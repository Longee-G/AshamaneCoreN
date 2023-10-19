-- fixed incorrect faction 
DELETE FROM `achievement` WHERE `ID` IN (11210, 11211);

SET @FACTION_HORDE := 0;
SET @FACTION_ALLIA := 1;


INSERT INTO `achievement` 
(`Title`, `Description`, `Reward`, `Flags`, `InstanceID`, `Supercedes`, `Category`, `UiOrder`, `SharesCriteria`, `Faction`, `Points`, `MinimumCriteria`, `ID`,  `IconFileID`, `CriteriaTree`, `VerifiedBuild`) VALUES
("为联盟而战","在《魔兽》全球首映后的两个月内登录过游戏。","奖励：仿制的雄狮之牙和雄狮之心",133120,-1,0,15268,39,0,@FACTION_ALLIA,0,0,11210,1447596,53123,39),			-- alliance 1
("为部落而战","在《魔兽》全球首映后的两个月内登录过游戏。","奖励：仿制的血卫士斩斧和古尔丹之杖",133120,-1,0,15268,40,0,@FACTION_HORDE,0,0,11211,1447599,53125,40); 		-- horde 0
-- ----------------------------
-- Table structure for character_loadout
-- ----------------------------
DROP TABLE IF EXISTS `character_loadout`;
CREATE TABLE `character_loadout` (
  `ID` int(11) NOT NULL DEFAULT '0',
  `RaceMask` bigint(20) NOT NULL DEFAULT '0',
  `ChrClassID` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `Purpose` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `VerifiedBuild` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for character_loadout_item
-- ----------------------------
DROP TABLE IF EXISTS `character_loadout_item`;
CREATE TABLE `character_loadout_item` (
  `ID` int(11) NOT NULL DEFAULT '0',
  `ItemID` int(11) NOT NULL DEFAULT '0',
  `CharacterLoadoutID` smallint(5) unsigned NOT NULL DEFAULT '0',
  `VerifiedBuild` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

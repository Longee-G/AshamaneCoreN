-- ----------------------------
-- Table structure for account_battlepet
-- ----------------------------
DROP TABLE IF EXISTS `account_battlepet`;
CREATE TABLE `account_battlepet` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `account` int(10) unsigned NOT NULL DEFAULT '0',
  `slot` int(11) NOT NULL DEFAULT '-1',
  `name` varchar(50) NOT NULL,
  `nameTimeStamp` int(10) unsigned NOT NULL DEFAULT '0',
  `species` int(10) unsigned NOT NULL DEFAULT '0',
  `quality` int(10) unsigned NOT NULL DEFAULT '0',
  `breed` int(10) unsigned NOT NULL DEFAULT '0',
  `level` int(10) unsigned NOT NULL DEFAULT '0',
  `xp` int(10) unsigned NOT NULL DEFAULT '0',
  `display` int(10) unsigned NOT NULL DEFAULT '0',
  `health` int(11) NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `infoPower` int(11) NOT NULL DEFAULT '0',
  `infoMaxHealth` int(11) NOT NULL DEFAULT '0',
  `infoSpeed` int(11) NOT NULL DEFAULT '0',
  `infoGender` int(11) NOT NULL DEFAULT '0',
  `declinedGenitive` varchar(50) NOT NULL,
  `declinedNative` varchar(50) NOT NULL,
  `declinedAccusative` varchar(50) NOT NULL,
  `declinedInstrumental` varchar(50) NOT NULL,
  `declinedPrepositional` varchar(50) NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `account` (`account`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=latin1 ROW_FORMAT=COMPACT;

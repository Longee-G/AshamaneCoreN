-- ----------------------------
-- Table structure for battlepay_display_info_locales
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_display_info_locales`;
CREATE TABLE `battlepay_display_info_locales` (
  `Id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Locale` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Name1` varchar(1024) DEFAULT '',
  `Name2` varchar(1024) DEFAULT '',
  `Name3` varchar(1024) DEFAULT '',
  `Name4` varchar(1024) DEFAULT '',
  PRIMARY KEY (`Id`,`Locale`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of battlepay_display_info_locales
-- ----------------------------

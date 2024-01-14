-- ----------------------------
-- Table structure for battlepay_product_display_info
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_display_info`;
CREATE TABLE `battlepay_product_display_info` (
  `ProductID` int(11) unsigned NOT NULL,
  `Name1` varchar(1024) CHARACTER SET utf8 DEFAULT NULL,
  `Name2` varchar(1024) CHARACTER SET utf8 DEFAULT NULL,
  `Name3` varchar(1024) CHARACTER SET utf8 DEFAULT NULL,
  `Name4` varchar(1024) CHARACTER SET utf8 DEFAULT NULL,
  `Type` smallint(3) unsigned NOT NULL DEFAULT '0',
  `CreatureDisplayInfoID` int(11) unsigned NOT NULL DEFAULT '0',
  `FileDataID` int(11) unsigned NOT NULL DEFAULT '0',
  `Flags` smallint(3) unsigned NOT NULL DEFAULT '0',
  `UnkInt1` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkInt2` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkInt3` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ProductID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of battlepay_product_display_info
-- ----------------------------
INSERT INTO `battlepay_product_display_info` VALUES ('50', 'Koro\'Tyshka', null, 'Koro\'Tyshka freezes opponents and laughs merrily.', null, '0', '134514', '76', '0', '0', '0', '0');
INSERT INTO `battlepay_product_display_info` VALUES ('147', 'Change of appearance', 'Change of Appearence', 'Change the appearance of the character (facial features, skin color, gender, but not race). An optional name change function is included.', null, '0', '1126582', '10', '0', '0', '0', '0');
INSERT INTO `battlepay_product_display_info` VALUES ('148', 'Change of Name', 'Change of Name', 'Change character name.', null, '0', '1126584', '10', '0', '0', '0', '0');


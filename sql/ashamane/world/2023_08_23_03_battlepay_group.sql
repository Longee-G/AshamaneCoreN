-- ----------------------------
-- Table structure for battlepay_group
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_group`;
CREATE TABLE `battlepay_group` (
  `GroupID` int(11) unsigned NOT NULL DEFAULT '0',
  `IconFileDataID` int(11) unsigned NOT NULL DEFAULT '0',
  `DisplayType` smallint(3) unsigned NOT NULL DEFAULT '0',
  `Ordering` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkInt` int(11) unsigned NOT NULL DEFAULT '0',
  `Name` varchar(255) CHARACTER SET utf8 NOT NULL,
  `IsAvailableDescription` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  PRIMARY KEY (`GroupID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of battlepay_group
-- ----------------------------
INSERT INTO `battlepay_group` VALUES ('13', '656556', '0', '2', '0', 'Pets', '');
INSERT INTO `battlepay_group` VALUES ('34', '132596', '2', '60', '1', 'Kits', 'No Kits Available');

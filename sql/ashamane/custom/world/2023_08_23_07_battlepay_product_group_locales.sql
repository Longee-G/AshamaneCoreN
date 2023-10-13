-- ----------------------------
-- Table structure for battlepay_product_group_locales
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_group_locales`;
CREATE TABLE `battlepay_product_group_locales` (
  `GroupID` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Locale` text,
  `Name` text,
  PRIMARY KEY (`GroupID`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of battlepay_product_group_locales
-- ----------------------------

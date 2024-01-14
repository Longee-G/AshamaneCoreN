-- ----------------------------
-- Table structure for battlepay_product_info
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_info`;
CREATE TABLE `battlepay_product_info` (
  `ProductID` int(11) unsigned NOT NULL,
  `NormalPriceFixedPoint` bigint(20) unsigned NOT NULL DEFAULT '0',
  `CurrentPriceFixedPoint` bigint(20) unsigned NOT NULL DEFAULT '0',
  `UnkInt2` int(11) unsigned NOT NULL DEFAULT '0',
  `ChoiceType` int(11) unsigned NOT NULL DEFAULT '0',
  `ProductIDs` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `UnkInts` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  PRIMARY KEY (`ProductID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of battlepay_product_info
-- ----------------------------
INSERT INTO `battlepay_product_info` VALUES ('50', '555000', '555000', '47', '2', '260', null);


-- ----------------------------
-- Table structure for battlepay_product_visuals
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_visuals`;
CREATE TABLE `battlepay_product_visuals` (
  `ProductID` int(11) unsigned NOT NULL,
  `DisplayId` int(11) unsigned NOT NULL DEFAULT '0',
  `IconFileDataId` int(11) unsigned NOT NULL DEFAULT '0',
  `ProductName` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  PRIMARY KEY (`ProductID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of battlepay_product_visuals
-- ----------------------------
INSERT INTO `battlepay_product_visuals` VALUES ('50', '30507', '76', 'Koro\'Tyshka');

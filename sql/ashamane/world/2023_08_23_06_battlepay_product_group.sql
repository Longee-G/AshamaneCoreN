SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `battlepay_product_group`
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_group`;
CREATE TABLE `battlepay_product_group` (
  `GroupID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) NOT NULL,
  `IconFileDataID` int(11) unsigned NOT NULL,
  `DisplayType` tinyint(3) unsigned NOT NULL,
  `Ordering` int(11) unsigned NOT NULL,
  `Flags` int(11) unsigned NOT NULL DEFAULT '0',
  `TokenType` tinyint(3) unsigned NOT NULL,
  `IngameOnly` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `OwnsTokensOnly` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`GroupID`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of battlepay_product_group
-- ----------------------------
INSERT INTO `battlepay_product_group` VALUES ('1', 'Mount', '939379', '0', '1', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('2', 'Pets', '939380', '0', '2', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('3', 'Services', '939382', '0', '3', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('4', 'Golds', '940857', '0', '4', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('5', 'Professions', '940858', '0', '5', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('7', 'Armors', '940856', '0', '7', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('8', 'Weapons', '940868', '0', '9', 0, 0, 0, 0);
INSERT INTO `battlepay_product_group` VALUES ('9', 'Toys', '940867', '0', '10', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('10', 'Boosts', '939378', '0', '11', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('11', 'Bags', '940857', '0', '12', 0, 0, 1, 0);
INSERT INTO `battlepay_product_group` VALUES ('12', 'Heirlooms', '940856', '0', '8', 0, 0, 1, 0);
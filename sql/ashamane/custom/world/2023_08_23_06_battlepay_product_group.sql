-- ----------------------------
-- Table structure for battlepay_product_group
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_group`;
CREATE TABLE `battlepay_product_group` (
  `GroupID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) NOT NULL,
  `IconFileDataID` int(11) NOT NULL,
  `DisplayType` tinyint(3) unsigned NOT NULL,
  `Ordering` int(11) NOT NULL,
  PRIMARY KEY (`GroupID`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of battlepay_product_group
-- ----------------------------
INSERT INTO `battlepay_product_group` VALUES ('1', 'Mount', '939379', '0', '1');
INSERT INTO `battlepay_product_group` VALUES ('2', 'Pets', '939380', '0', '2');
INSERT INTO `battlepay_product_group` VALUES ('3', 'Services', '939382', '0', '3');
INSERT INTO `battlepay_product_group` VALUES ('4', 'Golds', '940857', '0', '4');
INSERT INTO `battlepay_product_group` VALUES ('5', 'Professions', '940858', '0', '5');
INSERT INTO `battlepay_product_group` VALUES ('7', 'Armors', '940856', '0', '7');
INSERT INTO `battlepay_product_group` VALUES ('8', 'Weapons', '940868', '0', '9');
INSERT INTO `battlepay_product_group` VALUES ('9', 'Toys', '940867', '0', '10');
INSERT INTO `battlepay_product_group` VALUES ('10', 'Boosts', '939378', '0', '11');
INSERT INTO `battlepay_product_group` VALUES ('11', 'Bags', '940857', '0', '12');
INSERT INTO `battlepay_product_group` VALUES ('12', 'Heirlooms', '940856', '0', '8');

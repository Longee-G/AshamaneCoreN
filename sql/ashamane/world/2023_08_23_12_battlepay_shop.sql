-- ----------------------------
-- Table structure for battlepay_shop
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_shop`;
/*
CREATE TABLE `battlepay_shop` (
  `EntryID` int(11) unsigned NOT NULL,
  `GroupID` int(11) unsigned NOT NULL,
  `ProductID` int(11) unsigned NOT NULL,
  `Ordering` int(11) NOT NULL DEFAULT '0',
  `Flags` int(11) unsigned NOT NULL DEFAULT '0',
  `BannerType` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`EntryID`),
  KEY `with_battlepay_group` (`GroupID`),
  KEY `with_battlepay_product` (`ProductID`),
  CONSTRAINT `battlepay_shop_ibfk_1` FOREIGN KEY (`GroupID`) REFERENCES `battlepay_group` (`GroupID`),
  CONSTRAINT `battlepay_shop_ibfk_2` FOREIGN KEY (`ProductID`) REFERENCES `battlepay_product_display_info` (`ProductID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of battlepay_shop
-- ----------------------------
INSERT INTO `battlepay_shop` VALUES ('6', '13', '50', '96', '0', '0');
*/
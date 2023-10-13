-- ----------------------------
-- Table structure for battlepay_product_items
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_product_items`;
CREATE TABLE `battlepay_product_items` (
  `ProductID` int(11) unsigned NOT NULL,
  `ID` int(11) unsigned NOT NULL DEFAULT '0',
  `ItemID` int(11) unsigned NOT NULL DEFAULT '0',
  `Quantity` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkInt1` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkInt2` int(11) unsigned NOT NULL DEFAULT '0',
  `UnkByte` int(11) unsigned NOT NULL DEFAULT '0',
  `HasPet` int(11) unsigned NOT NULL DEFAULT '0',
  `PetResult` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ProductID`,`ID`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

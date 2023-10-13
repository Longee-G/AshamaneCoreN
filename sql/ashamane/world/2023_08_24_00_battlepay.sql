-- add TokenType column for each group
ALTER TABLE `battlepay_product_group`
	ADD COLUMN `TokenType` TINYINT(3) UNSIGNED NOT NULL AFTER `Ordering`,
	ADD COLUMN `IngameOnly` TINYINT(1) UNSIGNED NOT NULL DEFAULT '1' AFTER `TokenType`,
	ADD COLUMN `OwnsTokensOnly` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' AFTER `IngameOnly`;

-- set default group to use tokenType 1
UPDATE `battlepay_product_group` SET TokenType = 1 WHERE GroupID = 1;

ALTER TABLE `battlepay_product_group`
	CHANGE COLUMN `IconFileDataID` `IconFileDataID` INT(11) UNSIGNED NOT NULL AFTER `Name`,
	CHANGE COLUMN `Ordering` `Ordering` INT(11) UNSIGNED NOT NULL AFTER `DisplayType`,
	ADD COLUMN `Flags` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `Ordering`;
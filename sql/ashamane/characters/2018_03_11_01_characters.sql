ALTER TABLE `item_instance_artifact` DROP COLUMN `artifactTierId`;
ALTER TABLE `item_instance_artifact` ADD COLUMN `artifactTierId` INT(10) UNSIGNED DEFAULT 0 NOT NULL AFTER `artifactAppearanceId`;

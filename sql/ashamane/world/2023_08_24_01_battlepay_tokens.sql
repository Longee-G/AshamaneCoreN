CREATE TABLE `battlepay_tokens` (
	`tokenType` TINYINT(3) NOT NULL,
	`name` VARCHAR(255) NOT NULL DEFAULT '',
	`loginMessage` TEXT NULL DEFAULT NULL,
	`listIfNone` TINYINT(1) UNSIGNED NOT NULL DEFAULT '1',
	PRIMARY KEY (`tokenType`)
)
COLLATE='latin1_swedish_ci';

-- insert default token type
DELETE FROM battlepay_tokens WHERE `tokenType` = 1;
INSERT INTO battlepay_tokens (`tokenType`, `name`) VALUES (1, 'Battle Coins');

DELETE FROM `trinity_string` WHERE `entry` = 14092;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(14092, 'Your level is too high to buy this service.');
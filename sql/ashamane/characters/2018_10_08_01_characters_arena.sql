﻿DROP TABLE IF EXISTS `character_arena_stats`;

DROP TABLE IF EXISTS `character_arena_data`;
CREATE TABLE `character_arena_stats` (
  `guid` BIGINT (20) UNSIGNED NOT NULL,
  `slot` TINYINT (3) UNSIGNED NOT NULL,
  `rating` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `bestRatingOfWeek` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `bestRatingOfSeason` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `matchMakerRating` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `weekGames` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `weekWins` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `prevWeekGames` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `prevWeekWins` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `seasonGames` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  `seasonWins` INT (10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`, `slot`)
);

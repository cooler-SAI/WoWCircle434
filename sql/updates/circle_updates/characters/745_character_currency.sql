ALTER TABLE `character_currency`
	ADD COLUMN `season_count` INT(11) NOT NULL DEFAULT 0 AFTER `week_count`;

UPDATE `character_currency` SET `season_count` = total_count;
-- Clear all valor and conquest points
UPDATE `character_currency` SET `character_currency`.`total_count`=0 WHERE `character_currency`.`currency` IN (484, 483, 396, 390);

-- Drop all arena teams
TRUNCATE `arena_team`;
TRUNCATE `arena_team_member`;
TRUNCATE `arena_team_stats`;
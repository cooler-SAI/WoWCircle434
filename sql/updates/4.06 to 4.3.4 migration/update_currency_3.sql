-- Clear all valor and conquest points
DELETE FROM `character_currency` WHERE `character_currency`.`currency` IN (484, 483, 396, 390);

-- Drop all arena teams
TRUNCATE `arena_team`;
TRUNCATE `arena_team_member`;
TRUNCATE `arena_team_stats`;
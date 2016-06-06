-- Clear all valor and conquest points
DELETE FROM `character_currency` WHERE `character_currency`.`currency` IN (484, 483, 396, 390);

-- Drop all arena teams
TRUNCATE `arena_team`;
TRUNCATE `arena_team_member`;
TRUNCATE `character_arena_stats`;

-- Drop progress 4.2 quests
DELETE FROM `character_queststatus_rewarded` WHERE `quest` in (29391,29390,29387,29388,29389,29145,29195,29196,29197,29198,29201,29202,29200,29199);
DELETE FROM `character_queststatus_rewarded` WHERE `quest` in (29439,29440,29326,29335,29327,29336,29328,29337,29329,29338,29330,29331);
DELETE FROM `character_currency` WHERE `character_currency`.`currency`=416;

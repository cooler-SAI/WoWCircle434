ALTER TABLE `character_currency_cap`
CHANGE `highestArenaRating` `highestArenaRating` smallint(5) UNSIGNED NOT NULL default '0' COMMENT 'Highest rating of all arena brakets',
CHANGE `highestRBgRating` `highestRBgRating` smallint(5) UNSIGNED NOT NULL default '0' COMMENT 'Highest rating in rated battleground',
CHANGE `currentArenaCap` `currentArenaCap` smallint(5) UNSIGNED NOT NULL default '1350' COMMENT 'Week cap',
CHANGE `currentRBgCap` `currentRBgCap` smallint(5) UNSIGNED NOT NULL default '0' COMMENT 'Battleground week cap';
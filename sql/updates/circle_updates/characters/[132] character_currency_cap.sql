-- ----------------------------
-- Table structure for `character_currency_cap`
-- ----------------------------
DROP TABLE IF EXISTS `character_currency_cap`;
CREATE TABLE `character_currency_cap` (
  `guid` int(10) NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `highestArenaRating` smallint(5) NOT NULL default '0' COMMENT 'Highest rating of all arena brakets',
  `highestRBgRating` smallint(5) NOT NULL default '0' COMMENT 'Highest rating in rated battleground',
  `currentArenaCap` smallint(5) NOT NULL default '1350' COMMENT 'Week cap',
  `currentRBgCap` smallint(5) NOT NULL default '0' COMMENT 'Battleground week cap',
  `requireReset` tinyint(3) NOT NULL default '0' COMMENT 'Need reset once a week',
  PRIMARY KEY  (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE guild_member ADD BankResetTimeMoney INT(10) UNSIGNED DEFAULT '0' AFTER offnote;
ALTER TABLE guild_member ADD BankRemMoney INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeMoney;
ALTER TABLE guild_member ADD BankResetTimeTab0 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemMoney;
ALTER TABLE guild_member ADD BankRemSlotsTab0 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab0;
ALTER TABLE guild_member ADD BankResetTimeTab1 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab0;
ALTER TABLE guild_member ADD BankRemSlotsTab1 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab1;
ALTER TABLE guild_member ADD BankResetTimeTab2 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab1;
ALTER TABLE guild_member ADD BankRemSlotsTab2 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab2;
ALTER TABLE guild_member ADD BankResetTimeTab3 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab2;
ALTER TABLE guild_member ADD BankRemSlotsTab3 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab3;
ALTER TABLE guild_member ADD BankResetTimeTab4 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab3;
ALTER TABLE guild_member ADD BankRemSlotsTab4 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab4;
ALTER TABLE guild_member ADD BankResetTimeTab5 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab4;
ALTER TABLE guild_member ADD BankRemSlotsTab5 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab5;
ALTER TABLE guild_member ADD BankResetTimeTab6 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab5;
ALTER TABLE guild_member ADD BankRemSlotsTab6 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab6;
ALTER TABLE guild_member ADD BankResetTimeTab7 INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab6;
ALTER TABLE guild_member ADD BankRemSlotsTab7 INT(10) UNSIGNED DEFAULT '0' AFTER BankResetTimeTab7;
ALTER TABLE guild_member ADD achievementPoints INT(10) UNSIGNED DEFAULT '0' AFTER BankRemSlotsTab7;
ALTER TABLE guild_member ADD FirstProffLevel INT(10) UNSIGNED DEFAULT '0' AFTER achievementPoints;
ALTER TABLE guild_member ADD FirstProffSkill INT(10) UNSIGNED DEFAULT '0' AFTER FirstProffLevel;
ALTER TABLE guild_member ADD FirstProffRank INT(10) UNSIGNED DEFAULT '0' AFTER FirstProffSkill;
ALTER TABLE guild_member ADD SecondProffLevel INT(10) UNSIGNED DEFAULT '0' AFTER FirstProffRank;
ALTER TABLE guild_member ADD SecondProffSkill INT(10) UNSIGNED DEFAULT '0' AFTER SecondProffLevel;
ALTER TABLE guild_member ADD SecondProffRank INT(10) UNSIGNED DEFAULT '0' AFTER SecondProffSkill;

ALTER TABLE `guild_member`
CHANGE `RepWeek` `week_rep` INT(11) UNSIGNED DEFAULT '0' AFTER `achievementPoints`,
CHANGE `XpContribWeek` `week_activity` BIGINT(20) UNSIGNED DEFAULT '0' AFTER `SecondProffRank`,
CHANGE `XpContrib` `total_activity` BIGINT(20) UNSIGNED DEFAULT '0' AFTER `week_activity`;

DROP TABLE IF EXISTS `guild_member_withdraw`;

DROP TABLE IF EXISTS `guild_news_log`;
CREATE TABLE `guild_news_log` (
  `guild` int(10) unsigned NOT NULL,
  `id` int(10) unsigned NOT NULL,
  `eventType` int(10) unsigned NOT NULL,
  `playerGuid` bigint(20) unsigned NOT NULL,
  `data` int(10) unsigned NOT NULL,
  `flags` int(10) unsigned NOT NULL,
  `date` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guild`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `character_archaelogy`;
DROP TABLE IF EXISTS `character_archproject`;

CREATE TABLE `character_archaeology` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `sites0` text,
  `sites1` text,
  `sites2` text,
  `sites3` text,
  `counts` text,
  `projects` text,
  `completed` text,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `character_currency_cap`;
CREATE TABLE `character_currency_cap` (
  `guid` int(10) NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `highestArenaRating` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Highest rating of all arena brakets',
  `highestRBgRating` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Highest rating in rated battleground',
  `currentArenaCap` smallint(5) unsigned NOT NULL DEFAULT '1350' COMMENT 'Week cap',
  `currentRBgCap` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Battleground week cap',
  `requireReset` tinyint(3) NOT NULL DEFAULT '0' COMMENT 'Need reset once a week',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `currency_reset_save`;
CREATE TABLE `currency_reset_save` (
  `id` tinyint(3) NOT NULL,
 `lastguid` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

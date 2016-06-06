
--
-- Table structure for table `account_data`
--

ALTER TABLE `account_data`
    CHANGE COLUMN `account` `accountId` INT(10) UNSIGNED NOT NULL DEFAULT '0' FIRST;

--
-- Table structure for table `account_tutorial`
--

DROP TABLE IF EXISTS `account_tutorial`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `account_tutorial` (
  `accountId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Identifier',
  `tut0` int(10) unsigned NOT NULL DEFAULT '0',
  `tut1` int(10) unsigned NOT NULL DEFAULT '0',
  `tut2` int(10) unsigned NOT NULL DEFAULT '0',
  `tut3` int(10) unsigned NOT NULL DEFAULT '0',
  `tut4` int(10) unsigned NOT NULL DEFAULT '0',
  `tut5` int(10) unsigned NOT NULL DEFAULT '0',
  `tut6` int(10) unsigned NOT NULL DEFAULT '0',
  `tut7` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`accountId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters`    
--
    
ALTER TABLE `characters`
    ADD COLUMN `slot` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `name`,
    CHANGE COLUMN `map` `map` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Map Identifier' AFTER `position_z`,
    ADD COLUMN `talentTree` VARCHAR(10) NOT NULL DEFAULT '0 0' AFTER `resettalents_time`,
    DROP COLUMN `power6`,
    DROP COLUMN `power7`,
    DROP COLUMN `power8`,
    DROP COLUMN `power9`,
    DROP COLUMN `power10`,
    DROP COLUMN `ammoId`,
    ADD COLUMN `grantableLevels` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `actionBars`,
    CHANGE COLUMN `petSlotUsed` `petSlotUsed` INT(10) UNSIGNED NULL DEFAULT NULL AFTER `currentPetSlot`;

--
-- Table structure for table `character_achievement`
--

ALTER TABLE `character_achievement`
    CHANGE COLUMN `achievement` `achievement` SMALLINT(5) UNSIGNED NOT NULL AFTER `guid`;

--
-- Table structure for table `character_arena_stats`
--

ALTER TABLE `character_arena_stats` DROP COLUMN `personalRating`;

--
-- Table structure for table `character_cuf_profiles`
--

DROP TABLE IF EXISTS `character_cuf_profiles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `character_cuf_profiles` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Character Guid',
  `id` tinyint(3) unsigned NOT NULL COMMENT 'Profile Id (0-4)',
  `name` varchar(12) NOT NULL COMMENT 'Profile Name',
  `frameHeight` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Frame Height',
  `frameWidth` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Frame Width',
  `sortBy` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Frame Sort By',
  `healthText` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Frame Health Text',
  `boolOptions` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Many Configurable Bool Options',
  `unk146` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int8',
  `unk147` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int8',
  `unk148` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int8',
  `unk150` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int16',
  `unk152` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int16',
  `unk154` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Profile Unk Int16',
  PRIMARY KEY (`guid`,`id`),
  KEY `index` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_currency`
--

ALTER TABLE `character_currency`
    CHANGE COLUMN `count` `total_count` INT(11) UNSIGNED NOT NULL AFTER `currency`,
    CHANGE COLUMN `thisweek` `week_count` INT(11) UNSIGNED NOT NULL AFTER `total_count`,
    ADD COLUMN `season_count` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `week_count`;

--
-- Table structure for table `character_currency_cap`
--

DROP TABLE IF EXISTS `character_currency_cap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `character_currency_cap` (
  `guid` int(10) NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `highestArenaRating` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Highest rating of all arena brakets',
  `highestRBgRating` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Highest rating in rated battleground',
  `currentArenaCap` smallint(5) unsigned NOT NULL DEFAULT '1350' COMMENT 'Week cap',
  `currentRBgCap` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Battleground week cap',
  `requireReset` tinyint(3) NOT NULL DEFAULT '0' COMMENT 'Need reset once a week',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_equipmentsets`
--

ALTER TABLE `character_equipmentsets`
    ADD COLUMN `ignore_mask` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `iconname`;

--
-- Table structure for table `character_pet`
--

ALTER TABLE `character_pet` 
    DROP COLUMN `curhappiness`;

--
-- Table structure for table `character_queststatus`
--

ALTER TABLE `character_queststatus` 
    ADD COLUMN `playerCount` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' AFTER `itemcount4`;

--
-- Table structure for table `character_queststatus_seasonal`
--

DROP TABLE IF EXISTS `character_queststatus_seasonal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `character_queststatus_seasonal` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  `event` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Event Identifier',
  PRIMARY KEY (`guid`,`quest`),
  KEY `idx_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_stats`
--

ALTER TABLE `character_stats`
    ADD COLUMN `resilience` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `spellPower`,
    DROP COLUMN `maxpower6`,
    DROP COLUMN `maxpower7`,
    DROP COLUMN `maxpower8`,
    DROP COLUMN `maxpower9`,
    DROP COLUMN `maxpower10`;

--
-- Table structure for table `character_void_storage`
--

DROP TABLE IF EXISTS `character_void_storage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `character_void_storage` (
  `itemId` bigint(20) unsigned NOT NULL,
  `playerGuid` int(10) unsigned NOT NULL,
  `itemEntry` mediumint(8) unsigned NOT NULL,
  `slot` tinyint(3) unsigned NOT NULL,
  `creatorGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `randomProperty` int(10) unsigned NOT NULL DEFAULT '0',
  `suffixFactor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`itemId`),
  UNIQUE KEY `idx_player_slot` (`playerGuid`,`slot`),
  KEY `idx_player` (`playerGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `corpse`
--

ALTER TABLE `corpse`
    DROP COLUMN `guildId`;

--
-- Table structure for table `creature_respawn`
--

ALTER TABLE `creature_respawn`
    ADD COLUMN `mapId` SMALLINT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `respawnTime`;

--
-- Table structure for table `currency_reset_save`
--


DROP TABLE IF EXISTS `currency_reset_save`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `currency_reset_save` (
  `id` tinyint(3) NOT NULL,
  `lastguid` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_respawn`
--

ALTER TABLE `gameobject_respawn`
    ADD COLUMN `mapId` SMALLINT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `respawnTime`;

--
-- Table structure for table `game_event_condition_save`
--
    
ALTER TABLE `game_event_condition_save`
    CHANGE COLUMN `event_id` `eventEntry` SMALLINT(5) UNSIGNED NOT NULL FIRST;

--
-- Table structure for table `game_event_save`
--

ALTER TABLE `game_event_save`
    CHANGE COLUMN `event_id` `eventEntry` MEDIUMINT(8) UNSIGNED NOT NULL FIRST;

--
-- Table structure for table `gm_surveys`
--

ALTER TABLE `gm_surveys`
    CHANGE COLUMN `player` `guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `surveyid`,
    CHANGE COLUMN `overall_comment` `overallComment` LONGTEXT NOT NULL AFTER `mainSurvey`,
    CHANGE COLUMN `timestamp` `createTime` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `overallComment`;

--
-- Table structure for table `gm_tickets`
--
    
ALTER TABLE `gm_tickets`
    CHANGE COLUMN `guid` `ticketId` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT FIRST,
    CHANGE COLUMN `playerGuid` `guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `ticketId`,
    CHANGE COLUMN `map` `mapId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' AFTER `createtime`,
    CHANGE COLUMN `timestamp` `lastModifiedTime` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `posZ`,
    CHANGE COLUMN `closed` `closedBy` INT(11) NOT NULL DEFAULT '0' AFTER `lastModifiedTime`,
    ADD COLUMN `response` TEXT NOT NULL AFTER `comment`,
    ADD COLUMN `haveTicket` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `viewed`;

--
-- Table structure for table `guild`
--
    
ALTER TABLE `guild`
    CHANGE COLUMN `level` `level` INT(10) UNSIGNED NOT NULL DEFAULT '1' AFTER `BankMoney`,
    CHANGE COLUMN `xp` `experience` BIGINT(20) UNSIGNED NOT NULL AFTER `level`,
    CHANGE COLUMN `day_xp` `todayExperience` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0' AFTER `experience`;

--
-- Table structure for table `guild_achievement`
--
    
ALTER TABLE `guild_achievement`
    CHANGE COLUMN `achievement` `achievement` SMALLINT(5) UNSIGNED NOT NULL AFTER `guildid`,
    ADD COLUMN `guids` TEXT NOT NULL AFTER `date`;    

--
-- Table structure for table `guild_achievement_progress`
--
    
ALTER TABLE `guild_achievement_progress`
    ADD COLUMN `completedGuid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `date`;

--
-- Table structure for table `guild_finder_applicant`
--

DROP TABLE IF EXISTS `guild_finder_applicant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_finder_applicant` (
  `guildId` int(10) unsigned DEFAULT NULL,
  `playerGuid` int(10) unsigned DEFAULT NULL,
  `availability` tinyint(3) unsigned DEFAULT '0',
  `classRole` tinyint(3) unsigned DEFAULT '0',
  `interests` tinyint(3) unsigned DEFAULT '0',
  `comment` varchar(255) DEFAULT NULL,
  `submitTime` int(10) unsigned DEFAULT NULL,
  UNIQUE KEY `guildId` (`guildId`,`playerGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_finder_guild_settings`
--

DROP TABLE IF EXISTS `guild_finder_guild_settings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_finder_guild_settings` (
  `guildId` int(10) unsigned NOT NULL,
  `availability` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `classRoles` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `interests` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `level` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `listed` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`guildId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_news_log`
--

DROP TABLE IF EXISTS `guild_news_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lag_reports`
--

ALTER TABLE `lag_reports`
    CHANGE COLUMN `report_id` `reportId` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT FIRST,
    CHANGE COLUMN `player` `guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `reportId`,
    CHANGE COLUMN `lag_type` `lagtype` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `guid`,
    CHANGE COLUMN `map` `mapId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' AFTER `lagtype`,
    ADD COLUMN `latency` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `posZ`,
    ADD COLUMN `createTime` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `latency`;
    
--
-- Table structure for table `lfg_data`
--

DROP TABLE IF EXISTS `lfg_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `lfg_data` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `dungeon` int(10) unsigned NOT NULL DEFAULT '0',
  `state` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='LFG Data';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reserved_name`
--

DROP TABLE IF EXISTS `reserved_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `reserved_name` (
  `name` varchar(12) NOT NULL DEFAULT '',
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Player Reserved Names';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `warden_action`
--

DROP TABLE IF EXISTS `warden_action`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `warden_action` (
  `wardenId` smallint(5) unsigned NOT NULL,
  `action` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`wardenId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

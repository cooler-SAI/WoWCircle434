/*
Navicat MySQL Data Transfer

Source Server         : local db
Source Server Version : 50615
Source Host           : localhost:3306
Source Database       : auth_uwow

Target Server Type    : MYSQL
Target Server Version : 50615
File Encoding         : 65001

Date: 2014-06-26 15:05:12
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL DEFAULT '',
  `sha_pass_hash` varchar(40) NOT NULL DEFAULT '',
  `gmlevel` tinyint(3) NOT NULL DEFAULT '0',
  `sessionkey` longtext,
  `v` longtext,
  `s` longtext,
  `email` text,
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `failed_logins` int(11) unsigned NOT NULL DEFAULT '0',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `lock_country` varchar(2) NOT NULL DEFAULT '00',
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` int(11) unsigned NOT NULL DEFAULT '0',
  `expansion` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mutetime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `recruiter` int(11) NOT NULL DEFAULT '0',
  `premium` int(255) NOT NULL DEFAULT '0',
  `premium_time` int(255) NOT NULL DEFAULT '0',
  `realmgm` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `pass_q` varchar(32) DEFAULT '',
  `pass_a` varchar(20) DEFAULT '',
  `wac_state` tinyint(4) DEFAULT '0',
  `os` varchar(4) NOT NULL DEFAULT '',
  `donate` int(255) NOT NULL DEFAULT '0',
  `l2top` varchar(20) NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_username` (`username`),
  KEY `recruiter` (`recruiter`),
  KEY `donate` (`donate`)
) ENGINE=InnoDB AUTO_INCREMENT=1901034 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Account System';

-- ----------------------------
-- Records of account
-- ----------------------------

-- ----------------------------
-- Table structure for account_access
-- ----------------------------
DROP TABLE IF EXISTS `account_access`;
CREATE TABLE `account_access` (
  `id` int(11) unsigned NOT NULL,
  `gmlevel` tinyint(3) unsigned NOT NULL,
  `RealmID` int(11) NOT NULL,
  `gmlevel2` float unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`RealmID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of account_access
-- ----------------------------

-- ----------------------------
-- Table structure for account_banned
-- ----------------------------
DROP TABLE IF EXISTS `account_banned`;
CREATE TABLE `account_banned` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `bandate` bigint(40) NOT NULL DEFAULT '0',
  `unbandate` bigint(40) NOT NULL DEFAULT '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`bandate`),
  KEY `unbandate` (`unbandate`),
  KEY `bandate` (`bandate`),
  KEY `active` (`active`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Ban List';

-- ----------------------------
-- Records of account_banned
-- ----------------------------

-- ----------------------------
-- Table structure for account_history
-- ----------------------------
DROP TABLE IF EXISTS `account_history`;
CREATE TABLE `account_history` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `accid` int(12) DEFAULT NULL,
  `data` int(12) DEFAULT NULL,
  `run` varchar(255) DEFAULT NULL,
  `comment` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account_history
-- ----------------------------

-- ----------------------------
-- Table structure for account_log_ip
-- ----------------------------
DROP TABLE IF EXISTS `account_log_ip`;
CREATE TABLE `account_log_ip` (
  `accountid` int(11) unsigned NOT NULL,
  `ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `date` datetime DEFAULT NULL,
  PRIMARY KEY (`accountid`,`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of account_log_ip
-- ----------------------------

-- ----------------------------
-- Table structure for account_muted
-- ----------------------------
DROP TABLE IF EXISTS `account_muted`;
CREATE TABLE `account_muted` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `bandate` bigint(40) NOT NULL DEFAULT '0',
  `unbandate` bigint(40) NOT NULL DEFAULT '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`bandate`),
  KEY `bandate` (`bandate`),
  KEY `unbandate` (`unbandate`),
  KEY `active` (`active`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Ban List';

-- ----------------------------
-- Records of account_muted
-- ----------------------------

-- ----------------------------
-- Table structure for account_premium
-- ----------------------------
DROP TABLE IF EXISTS `account_premium`;
CREATE TABLE `account_premium` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `setdate` bigint(40) NOT NULL DEFAULT '0',
  `unsetdate` bigint(40) NOT NULL DEFAULT '0',
  `premium_type` tinyint(4) unsigned NOT NULL DEFAULT '1',
  `active` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`setdate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Premium Accounts';

-- ----------------------------
-- Records of account_premium
-- ----------------------------

-- ----------------------------
-- Table structure for account_reputation
-- ----------------------------
DROP TABLE IF EXISTS `account_reputation`;
CREATE TABLE `account_reputation` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL DEFAULT '',
  `reputation` varchar(5) NOT NULL DEFAULT '0',
  `reason` varchar(255) NOT NULL DEFAULT '',
  `date` varchar(32) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account_reputation
-- ----------------------------

-- ----------------------------
-- Table structure for autobroadcast
-- ----------------------------
DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast` (
  `realmid` int(11) NOT NULL DEFAULT '-1',
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `weight` tinyint(3) unsigned DEFAULT '1',
  `text` longtext NOT NULL,
  PRIMARY KEY (`id`,`realmid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of autobroadcast
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_accounts
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_accounts`;
CREATE TABLE `battlenet_accounts` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `email` varchar(320) NOT NULL,
  `sha_pass_hash` varchar(64) NOT NULL DEFAULT '',
  `v` varchar(256) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `sessionKey` varchar(128) NOT NULL DEFAULT '',
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int(10) unsigned NOT NULL DEFAULT '0',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `lock_country` varchar(2) NOT NULL DEFAULT '00',
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `os` varchar(3) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8 COMMENT='Account System';

-- ----------------------------
-- Records of battlenet_accounts
-- ----------------------------
-- ----------------------------
-- Table structure for battlenet_account_bans
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_account_bans`;
CREATE TABLE `battlenet_account_bans` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Account id',
  `bandate` int(10) unsigned NOT NULL DEFAULT '0',
  `unbandate` int(10) unsigned NOT NULL DEFAULT '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`bandate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Ban List';

-- ----------------------------
-- Records of battlenet_account_bans
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_components
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_components`;
CREATE TABLE `battlenet_components` (
  `Program` varchar(4) NOT NULL,
  `Platform` varchar(4) NOT NULL,
  `Build` int(11) unsigned NOT NULL,
  PRIMARY KEY (`Program`,`Platform`,`Build`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of battlenet_components
-- ----------------------------
INSERT INTO `battlenet_components` VALUES ('Bnet', 'Cmp1', '3');
INSERT INTO `battlenet_components` VALUES ('Bnet', 'Win', '21719');
INSERT INTO `battlenet_components` VALUES ('Bnet', 'Win', '26487');
INSERT INTO `battlenet_components` VALUES ('Bnet', 'Wn64', '26487');
INSERT INTO `battlenet_components` VALUES ('Tool', 'Win', '1569');
INSERT INTO `battlenet_components` VALUES ('Tool', 'Win', '2736');
INSERT INTO `battlenet_components` VALUES ('WoW', 'base', '12340');
INSERT INTO `battlenet_components` VALUES ('WoW', 'base', '15595');
INSERT INTO `battlenet_components` VALUES ('WoW', 'enGB', '12340');
INSERT INTO `battlenet_components` VALUES ('WoW', 'enGB', '15595');
INSERT INTO `battlenet_components` VALUES ('WoW', 'enUS', '12340');
INSERT INTO `battlenet_components` VALUES ('WoW', 'enUS', '15595');
INSERT INTO `battlenet_components` VALUES ('WoW', 'Win', '12340');
INSERT INTO `battlenet_components` VALUES ('WoW', 'Win', '15595');
INSERT INTO `battlenet_components` VALUES ('WoW', 'Wn64', '15595');

-- ----------------------------
-- Table structure for battlenet_modules
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_modules`;
CREATE TABLE `battlenet_modules` (
  `Hash` varchar(64) NOT NULL,
  `Name` varchar(64) NOT NULL DEFAULT '',
  `Type` varchar(8) NOT NULL,
  `System` varchar(8) NOT NULL,
  `Data` text,
  PRIMARY KEY (`Hash`),
  UNIQUE KEY `uk_name_type_system` (`Name`,`Type`,`System`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of battlenet_modules
-- ----------------------------
INSERT INTO `battlenet_modules` VALUES ('00ffd88a437afbb88d7d4b74be2e3b43601605ee229151aa9f4bebb29ef66280', 'Resume', 'auth', 'Mac', null);
INSERT INTO `battlenet_modules` VALUES ('19c91b68752b7826df498bf73aca1103c86962a9a55a0a7033e5ad895f4d927c', 'Password', 'auth', 'Mc64', null);
INSERT INTO `battlenet_modules` VALUES ('1af5418a448f8ad05451e3f7dbb2d9af9cb13458eea2368ebfc539476b954f1c', 'RiskFingerprint', 'auth', 'Mc64', null);
INSERT INTO `battlenet_modules` VALUES ('207640724f4531d3b2a21532224d1486e8c4d2d805170381cbc3093264157960', 'SelectGameAccount', 'auth', 'Mac', null);
INSERT INTO `battlenet_modules` VALUES ('2e6d53adab37a41542b38e01f62cd365eab8805ed0de73c307cc6d9d1dfe478c', 'Password', 'auth', 'Win', null);
INSERT INTO `battlenet_modules` VALUES ('304627d437c38500c0b5ca0c6220eeade91390e52a2b005ff3f7754afa1f93cd', 'Resume', 'auth', 'Mc64', null);
INSERT INTO `battlenet_modules` VALUES ('36b27cd911b33c61730a8b82c8b2495fd16e8024fc3b2dde08861c77a852941c', 'Thumbprint', 'auth', 'Win', 'E716F4F0A01EB9C032A6C1393356A4F766F067949D71023C0CFC0613718966EF814E65CC6EE70C432A7F8AFD8A062B52603A2697E851D231D72C0277614181D713369B1E8E4BEEAB72045A9AAD45F319DB918ECDDB83C8EF8B7510600D391D45E7FEC0BEEAE904A5F9FA620F1CCDAD699D84A4739CE669B5A551831E396214E13B4C88F573F5CDC784CD01530C086B674C03BEB66403A0F87ED17ABBB403DE54CF31BE828A20C566C22E4D4263AA77220B0644D99245345BCAC276EA06925EB984D664725C3CB757140AFE12E27CB996F17159B1057E9B58B78BBB5A139C9FF6215A0D250B75FC9DD435655DDEADCD6CFD84800792C146B3633188ECEB53D2038C185E0BD51A9E6C70FD38ADF530F8DF50FB62053C5E894897AB7DD65C7AC80665F18E7989BE6E30F15E939751123D6D8A44F033175301D15AAAD2AEA06FAC60BA4065846AE938F32B1CB15F16DC0E76792A7332346896048065D17C059899E1D2300E402BD0EA74265DA6A42B1C854E2470D7B21AE4A2DAE90E602A759B2CA0EE610B50D5389DB89335D5451FE76DD85B09FD5297D6F9EFB6C34CE885007F7DF20D6A524E0C3E772FA04B3DD2E014D3A337A790943DAD523CBB5453F4FDF8E74DFE361BD5F25AB31952B478148B570DF5762643F32B994FEC99A747E4A265A66EE84A53509EC285C84679606049314FC526C61B537AC8061C788F8B86F52208');
INSERT INTO `battlenet_modules` VALUES ('393ca8d75aff6f04f79532cf95a88b91e5743bc59121520ac31fc4508019fe60', 'Token', 'auth', 'Mac', null);
INSERT INTO `battlenet_modules` VALUES ('3c2f63e5949aa6fd6cf330b109ca5b9adcd215beac846b7462ed3aa01d5ad6bb', 'Password', 'auth', 'Mac', null);
INSERT INTO `battlenet_modules` VALUES ('52e2978db6468dfade7c61da89513f443c9225692b5085fbe956749870993703', 'SelectGameAccount', 'auth', 'Mc64', null);
INSERT INTO `battlenet_modules` VALUES ('548b5ef9e0dd5c2f89f59c3e0979249b27505c51f0c77d2b27133726eaee0ad0', 'Thumbprint', 'auth', 'Mac', 'E716F4F0A01EB9C032A6C1393356A4F766F067949D71023C0CFC0613718966EF814E65CC6EE70C432A7F8AFD8A062B52603A2697E851D231D72C0277614181D713369B1E8E4BEEAB72045A9AAD45F319DB918ECDDB83C8EF8B7510600D391D45E7FEC0BEEAE904A5F9FA620F1CCDAD699D84A4739CE669B5A551831E396214E13B4C88F573F5CDC784CD01530C086B674C03BEB66403A0F87ED17ABBB403DE54CF31BE828A20C566C22E4D4263AA77220B0644D99245345BCAC276EA06925EB984D664725C3CB757140AFE12E27CB996F17159B1057E9B58B78BBB5A139C9FF6215A0D250B75FC9DD435655DDEADCD6CFD84800792C146B3633188ECEB53D2038C185E0BD51A9E6C70FD38ADF530F8DF50FB62053C5E894897AB7DD65C7AC80665F18E7989BE6E30F15E939751123D6D8A44F033175301D15AAAD2AEA06FAC60BA4065846AE938F32B1CB15F16DC0E76792A7332346896048065D17C059899E1D2300E402BD0EA74265DA6A42B1C854E2470D7B21AE4A2DAE90E602A759B2CA0EE610B50D5389DB89335D5451FE76DD85B09FD5297D6F9EFB6C34CE885007F7DF20D6A524E0C3E772FA04B3DD2E014D3A337A790943DAD523CBB5453F4FDF8E74DFE361BD5F25AB31952B478148B570DF5762643F32B994FEC99A747E4A265A66EE84A53509EC285C84679606049314FC526C61B537AC8061C788F8B86F52208');
INSERT INTO `battlenet_modules` VALUES ('5e298e530698af905e1247e51ef0b109b352ac310ce7802a1f63613db980ed17', 'RiskFingerprint', 'auth', 'Win', null);
INSERT INTO `battlenet_modules` VALUES ('851c1d2ef926e9b9a345a460874e65517195129b9e3bdec7cc77710fa0b1fad6', 'Password', 'auth', 'Wn64', null);
INSERT INTO `battlenet_modules` VALUES ('894d25d3219d97d085ea5a8b98e66df5bd9f460ec6f104455246a12b8921409d', 'SelectGameAccount', 'auth', 'Wn64', null);
INSERT INTO `battlenet_modules` VALUES ('898166926805f897804bdbbf40662c9d768590a51a0b26c40dbcdf332ba11974', 'Resume', 'auth', 'Wn64', null);
INSERT INTO `battlenet_modules` VALUES ('8c43bda10be33a32abbc09fb2279126c7f5953336391276cff588565332fcd40', 'RiskFingerprint', 'auth', 'Wn64', null);
INSERT INTO `battlenet_modules` VALUES ('a1edab845d9e13e9c84531369be2f61b930a37c8e7a9c66d11ef8963380e178a', 'Token', 'auth', 'Mc64', null);
INSERT INTO `battlenet_modules` VALUES ('a330f388b6687f8a42fe8fbb592a3df724b20b65fb0989342bb8261f2f452318', 'Token', 'auth', 'Wn64', null);
INSERT INTO `battlenet_modules` VALUES ('abc6bb719a73ec1055296001910e26afa561f701ad9995b1ecd7f55f9d3ca37c', 'SelectGameAccount', 'auth', 'Win', null);
INSERT INTO `battlenet_modules` VALUES ('b37136b39add83cfdbafa81857de3dd8f15b34e0135ec6cd9c3131d3a578d8c2', 'Thumbprint', 'auth', 'Mc64', 'E716F4F0A01EB9C032A6C1393356A4F766F067949D71023C0CFC0613718966EF814E65CC6EE70C432A7F8AFD8A062B52603A2697E851D231D72C0277614181D713369B1E8E4BEEAB72045A9AAD45F319DB918ECDDB83C8EF8B7510600D391D45E7FEC0BEEAE904A5F9FA620F1CCDAD699D84A4739CE669B5A551831E396214E13B4C88F573F5CDC784CD01530C086B674C03BEB66403A0F87ED17ABBB403DE54CF31BE828A20C566C22E4D4263AA77220B0644D99245345BCAC276EA06925EB984D664725C3CB757140AFE12E27CB996F17159B1057E9B58B78BBB5A139C9FF6215A0D250B75FC9DD435655DDEADCD6CFD84800792C146B3633188ECEB53D2038C185E0BD51A9E6C70FD38ADF530F8DF50FB62053C5E894897AB7DD65C7AC80665F18E7989BE6E30F15E939751123D6D8A44F033175301D15AAAD2AEA06FAC60BA4065846AE938F32B1CB15F16DC0E76792A7332346896048065D17C059899E1D2300E402BD0EA74265DA6A42B1C854E2470D7B21AE4A2DAE90E602A759B2CA0EE610B50D5389DB89335D5451FE76DD85B09FD5297D6F9EFB6C34CE885007F7DF20D6A524E0C3E772FA04B3DD2E014D3A337A790943DAD523CBB5453F4FDF8E74DFE361BD5F25AB31952B478148B570DF5762643F32B994FEC99A747E4A265A66EE84A53509EC285C84679606049314FC526C61B537AC8061C788F8B86F52208');
INSERT INTO `battlenet_modules` VALUES ('bfe4ceb47700aa872e815e007e27df955d4cd4bc1fe731039ee6498ce209f368', 'Resume', 'auth', 'Win', null);
INSERT INTO `battlenet_modules` VALUES ('c0f38d05aa1b83065e839c9bd96c831e9f7e42477085138752657a6a9bb9c520', 'RiskFingerprint', 'auth', 'Mac', null);
INSERT INTO `battlenet_modules` VALUES ('c3a1ac0694979e709c3b5486927e558af1e2be02ca96e5615c5a65aacc829226', 'Thumbprint', 'auth', 'Wn64', 'E716F4F0A01EB9C032A6C1393356A4F766F067949D71023C0CFC0613718966EF814E65CC6EE70C432A7F8AFD8A062B52603A2697E851D231D72C0277614181D713369B1E8E4BEEAB72045A9AAD45F319DB918ECDDB83C8EF8B7510600D391D45E7FEC0BEEAE904A5F9FA620F1CCDAD699D84A4739CE669B5A551831E396214E13B4C88F573F5CDC784CD01530C086B674C03BEB66403A0F87ED17ABBB403DE54CF31BE828A20C566C22E4D4263AA77220B0644D99245345BCAC276EA06925EB984D664725C3CB757140AFE12E27CB996F17159B1057E9B58B78BBB5A139C9FF6215A0D250B75FC9DD435655DDEADCD6CFD84800792C146B3633188ECEB53D2038C185E0BD51A9E6C70FD38ADF530F8DF50FB62053C5E894897AB7DD65C7AC80665F18E7989BE6E30F15E939751123D6D8A44F033175301D15AAAD2AEA06FAC60BA4065846AE938F32B1CB15F16DC0E76792A7332346896048065D17C059899E1D2300E402BD0EA74265DA6A42B1C854E2470D7B21AE4A2DAE90E602A759B2CA0EE610B50D5389DB89335D5451FE76DD85B09FD5297D6F9EFB6C34CE885007F7DF20D6A524E0C3E772FA04B3DD2E014D3A337A790943DAD523CBB5453F4FDF8E74DFE361BD5F25AB31952B478148B570DF5762643F32B994FEC99A747E4A265A66EE84A53509EC285C84679606049314FC526C61B537AC8061C788F8B86F52208');
INSERT INTO `battlenet_modules` VALUES ('fbe675a99fb5f4b7fb3eb5e5a22add91a4cabf83e3c5930c6659ad13c457ba18', 'Token', 'auth', 'Win', null);

-- ----------------------------
-- Table structure for ip2nation
-- ----------------------------
DROP TABLE IF EXISTS `ip2nation`;
CREATE TABLE `ip2nation` (
  `ip` int(11) unsigned NOT NULL DEFAULT '0',
  `country` char(2) NOT NULL DEFAULT '',
  KEY `ip` (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip2nation
-- ----------------------------

-- ----------------------------
-- Table structure for ip2nationcountries
-- ----------------------------
DROP TABLE IF EXISTS `ip2nationcountries`;
CREATE TABLE `ip2nationcountries` (
  `code` varchar(4) NOT NULL DEFAULT '',
  `iso_code_2` varchar(2) NOT NULL DEFAULT '',
  `iso_code_3` varchar(3) DEFAULT '',
  `iso_country` varchar(255) NOT NULL DEFAULT '',
  `country` varchar(255) NOT NULL DEFAULT '',
  `lat` float NOT NULL DEFAULT '0',
  `lon` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`code`),
  KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip2nationcountries
-- ----------------------------

-- ----------------------------
-- Table structure for ip_banned
-- ----------------------------
DROP TABLE IF EXISTS `ip_banned`;
CREATE TABLE `ip_banned` (
  `ip` varchar(32) NOT NULL DEFAULT '127.0.0.1',
  `bandate` bigint(40) NOT NULL,
  `unbandate` bigint(40) NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(255) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`,`bandate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Banned IPs';

-- ----------------------------
-- Records of ip_banned
-- ----------------------------

-- ----------------------------
-- Table structure for ip_ddos
-- ----------------------------
DROP TABLE IF EXISTS `ip_ddos`;
CREATE TABLE `ip_ddos` (
  `ip` varchar(32) NOT NULL,
  PRIMARY KEY (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip_ddos
-- ----------------------------

-- ----------------------------
-- Table structure for logs
-- ----------------------------
DROP TABLE IF EXISTS `logs`;
CREATE TABLE `logs` (
  `time` int(14) NOT NULL,
  `realm` int(4) NOT NULL,
  `type` int(4) NOT NULL,
  `level` int(11) NOT NULL DEFAULT '0',
  `string` text
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of logs
-- ----------------------------

-- ----------------------------
-- Table structure for online
-- ----------------------------
DROP TABLE IF EXISTS `online`;
CREATE TABLE `online` (
  `realmID` int(11) unsigned NOT NULL DEFAULT '0',
  `online` int(11) unsigned NOT NULL DEFAULT '0',
  `diff` int(11) unsigned NOT NULL DEFAULT '0',
  `uptime` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`realmID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of online
-- ----------------------------
INSERT INTO `online` VALUES ('1', '1', '51', '1802');

-- ----------------------------
-- Table structure for realmcharacters
-- ----------------------------
DROP TABLE IF EXISTS `realmcharacters`;
CREATE TABLE `realmcharacters` (
  `realmid` int(10) unsigned NOT NULL DEFAULT '0',
  `acctid` int(10) unsigned NOT NULL,
  `numchars` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`realmid`,`acctid`),
  KEY `acctid` (`acctid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Realm Character Tracker';

-- ----------------------------
-- Records of realmcharacters
-- ----------------------------

-- ----------------------------
-- Table structure for realmlist
-- ----------------------------
DROP TABLE IF EXISTS `realmlist`;
CREATE TABLE `realmlist` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(32) NOT NULL DEFAULT '127.0.0.1',
  `localAddress` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `localSubnetMask` varchar(255) NOT NULL DEFAULT '255.255.255.0',
  `port` int(11) NOT NULL DEFAULT '8085',
  `icon` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `color` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `allowedSecurityLevel` float unsigned NOT NULL DEFAULT '0',
  `population` float unsigned NOT NULL DEFAULT '0',
  `gamebuild` int(11) unsigned NOT NULL DEFAULT '11723',
  `flag` tinyint(3) unsigned NOT NULL DEFAULT '2',
  `online` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Realm System';

-- ----------------------------
-- Records of realmlist
-- ----------------------------

-- ----------------------------
-- Table structure for realm_transfer
-- ----------------------------
DROP TABLE IF EXISTS `realm_transfer`;
CREATE TABLE `realm_transfer` (
  `from_realm` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `to_realm` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`from_realm`,`to_realm`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of realm_transfer
-- ----------------------------

-- ----------------------------
-- Table structure for server
-- ----------------------------
DROP TABLE IF EXISTS `server`;
CREATE TABLE `server` (
  `id` int(11) NOT NULL,
  `groupId` tinyint(4) DEFAULT NULL,
  `subGroupId` tinyint(4) DEFAULT NULL,
  `type` tinyint(4) DEFAULT NULL,
  `host` varchar(15) DEFAULT NULL,
  `port` int(11) DEFAULT NULL,
  `name` varchar(45) DEFAULT NULL,
  `dbId` tinyint(4) DEFAULT NULL,
  `online` int(11) DEFAULT NULL,
  `comment` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of server
-- ----------------------------

-- ----------------------------
-- Table structure for transferts
-- ----------------------------
DROP TABLE IF EXISTS `transferts`;
CREATE TABLE `transferts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `account` int(11) NOT NULL DEFAULT '0',
  `perso_guid` int(11) NOT NULL DEFAULT '0',
  `from` int(11) NOT NULL DEFAULT '0',
  `to` int(11) NOT NULL DEFAULT '0',
  `toacc` int(11) NOT NULL DEFAULT '0',
  `dump` longtext NOT NULL,
  `nb_attempt` int(11) NOT NULL DEFAULT '0',
  `state` int(10) DEFAULT '0',
  `error` int(10) DEFAULT '0',
  `revision` int(10) DEFAULT '0',
  `transferId` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `account` (`account`),
  KEY `perso_guid` (`perso_guid`),
  KEY `from` (`from`),
  KEY `to` (`to`),
  KEY `state` (`state`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of transferts
-- ----------------------------

-- ----------------------------
-- Table structure for transferts_logs
-- ----------------------------
DROP TABLE IF EXISTS `transferts_logs`;
CREATE TABLE `transferts_logs` (
  `id` int(11) DEFAULT NULL,
  `account` int(11) DEFAULT '0',
  `perso_guid` int(11) DEFAULT '0',
  `from` int(2) DEFAULT '0',
  `to` int(2) DEFAULT '0',
  `dump` longtext,
  `toacc` int(11) NOT NULL DEFAULT '0',
  `newguid` int(11) NOT NULL DEFAULT '0',
  `transferId` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of transferts_logs
-- ----------------------------

-- ----------------------------
-- Table structure for transfert_dump
-- ----------------------------
DROP TABLE IF EXISTS `transfert_dump`;
CREATE TABLE `transfert_dump` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `acid` int(11) unsigned NOT NULL,
  `guid` int(11) unsigned DEFAULT '0',
  `realm` int(11) unsigned NOT NULL,
  `realm_test` int(11) unsigned NOT NULL,
  `file` varchar(50) NOT NULL,
  `client_build` smallint(5) unsigned NOT NULL,
  `client_locale` varchar(4) NOT NULL,
  `server_site` varchar(50) NOT NULL DEFAULT '',
  `server_realmlist` varchar(50) NOT NULL DEFAULT '',
  `account_login` varchar(32) NOT NULL DEFAULT '',
  `account_pass` varchar(32) NOT NULL DEFAULT '',
  `server_realm` varchar(50) NOT NULL DEFAULT '',
  `char_name` varchar(20) NOT NULL,
  `char_level` tinyint(2) unsigned NOT NULL,
  `dump_version` varchar(255) NOT NULL,
  `dump_dt` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `upload_dt` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `moderator` int(11) unsigned DEFAULT NULL,
  `comment` varchar(100) DEFAULT '',
  `order` int(11) unsigned DEFAULT '0',
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx` (`acid`,`file`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of transfert_dump
-- ----------------------------

-- ----------------------------
-- Table structure for transfert_factions
-- ----------------------------
DROP TABLE IF EXISTS `transfert_factions`;
CREATE TABLE `transfert_factions` (
  `id` int(10) unsigned NOT NULL,
  `name` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of transfert_factions
-- ----------------------------

-- ----------------------------
-- Table structure for transfert_history
-- ----------------------------
DROP TABLE IF EXISTS `transfert_history`;
CREATE TABLE `transfert_history` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `dump_id` int(11) unsigned NOT NULL,
  `account_login` varchar(32) NOT NULL DEFAULT '',
  `char_name` varchar(20) NOT NULL,
  `moderator` int(11) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `comment` varchar(100) DEFAULT '',
  `dt` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of transfert_history
-- ----------------------------

-- ----------------------------
-- Table structure for transfert_items
-- ----------------------------
DROP TABLE IF EXISTS `transfert_items`;
CREATE TABLE `transfert_items` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `item` int(11) unsigned NOT NULL,
  `item_transfert` int(11) unsigned NOT NULL,
  `count` int(11) unsigned NOT NULL,
  `note` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx__item_id__new_item_id` (`item`,`item_transfert`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of transfert_items
-- ----------------------------

-- ----------------------------
-- Table structure for twitch_streams
-- ----------------------------
DROP TABLE IF EXISTS `twitch_streams`;
CREATE TABLE `twitch_streams` (
  `accountId` int(11) NOT NULL,
  `realmId` smallint(6) NOT NULL,
  `playerId` bigint(20) NOT NULL,
  `channelName` varchar(45) NOT NULL,
  `playerName` varchar(45) NOT NULL,
  `class` tinyint(4) NOT NULL,
  `mmr2x2` smallint(6) NOT NULL,
  `mmr3x3` smallint(6) NOT NULL,
  `status` tinyint(4) NOT NULL,
  PRIMARY KEY (`accountId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of twitch_streams
-- ----------------------------

-- ----------------------------
-- Table structure for twitch_vip
-- ----------------------------
DROP TABLE IF EXISTS `twitch_vip`;
CREATE TABLE `twitch_vip` (
  `accountId` int(11) NOT NULL,
  PRIMARY KEY (`accountId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of twitch_vip
-- ----------------------------

-- ----------------------------
-- Table structure for uptime
-- ----------------------------
DROP TABLE IF EXISTS `uptime`;
CREATE TABLE `uptime` (
  `realmid` int(11) unsigned NOT NULL,
  `starttime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `startstring` varchar(64) NOT NULL DEFAULT '',
  `uptime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `maxplayers` smallint(5) unsigned NOT NULL DEFAULT '0',
  `revision` varchar(255) NOT NULL DEFAULT 'Trinitycore',
  PRIMARY KEY (`realmid`,`starttime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Uptime system';

-- ----------------------------
-- Records of uptime
-- ----------------------------

-- ----------------------------
-- Table structure for warden_action
-- ----------------------------
DROP TABLE IF EXISTS `warden_action`;
CREATE TABLE `warden_action` (
  `wardenId` smallint(5) unsigned NOT NULL,
  `action` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`wardenId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of warden_action
-- ----------------------------

-- ----------------------------
-- Table structure for warden_checks
-- ----------------------------
DROP TABLE IF EXISTS `warden_checks`;
CREATE TABLE `warden_checks` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `type` tinyint(3) unsigned DEFAULT NULL,
  `data` varchar(48) DEFAULT NULL,
  `str` varchar(20) DEFAULT NULL,
  `address` int(10) unsigned DEFAULT NULL,
  `length` tinyint(3) unsigned DEFAULT NULL,
  `result` varchar(24) DEFAULT NULL,
  `comment` varchar(50) DEFAULT NULL,
  `bantime` int(12) unsigned DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=32 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of warden_checks
-- ----------------------------

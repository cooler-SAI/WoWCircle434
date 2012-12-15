/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `battlemaster_entry`
--

DROP TABLE IF EXISTS `battlemaster_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `battlemaster_entry` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT 'Entry of a creature',
  `bg_template` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT 'Battleground template id',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `battlemaster_entry`
--

LOCK TABLES `battlemaster_entry` WRITE;
/*!40000 ALTER TABLE `battlemaster_entry` DISABLE KEYS */;
INSERT INTO `battlemaster_entry` VALUES (347,1),(5118,1),(7410,1),(7427,1),(12197,1),(14942,1),(16695,1),(19906,1),(19907,1),(20119,1),(20271,1),(20276,1),(35019,30),(2302,2),(2804,2),(3890,2),(10360,2),(14981,2),(14982,2),(16696,2),(19908,2),(19910,2),(20002,2),(20118,2),(20269,2),(20272,2),(35027,30),(857,3),(907,3),(12198,3),(15006,3),(15007,3),(15008,3),(16694,3),(35020,30),(19905,3),(20120,3),(20273,3),(20274,3),(35021,30),(19915,6),(19859,6),(25991,6),(20499,6),(20497,6),(18895,6),(19923,6),(19911,6),(21235,6),(20362,7),(20374,7),(20381,7),(20382,7),(20383,7),(20384,7),(20385,7),(20386,7),(20388,7),(20390,7),(22013,7),(22015,7),(14990,3),(14991,3),(15102,2),(15105,2),(15106,1),(15103,1),(29568,6),(19858,6),(32333,6),(32332,6),(29672,2),(29673,3),(29674,1),(29675,7),(29676,9),(34989,32),(34988,32),(34986,32),(35007,32),(34991,32),(34997,32),(34998,32),(35000,32),(34999,32),(35002,32),(35001,32),(19925,6),(32330,6),(19909,6),(29533,6),(30610,6),(19912,6),(22656,1),(22647,1),(26007,6),(30567,9),(30566,9),(30231,3),(29667,2),(29668,3),(29669,1),(29670,7),(29671,9),(30578,9),(30579,9),(32617,1),(32619,3),(32620,7),(32623,9),(32625,2),(32616,1),(32622,9),(32621,7),(32618,3),(32624,2),(30581,9),(30587,9),(30590,9),(30586,9),(30584,9),(30580,9),(30583,9),(30582,9),(34983,32),(34985,32),(35008,32),(34955,32),(34987,32),(34972,32),(34973,32),(34976,32),(34978,32),(35023,30),(35024,30),(35026,30),(35022,30),(35017,30),(35025,30),(34952,30),(34953,30),(34950,30),(34948,30),(34949,30),(34951,30);
/*!40000 ALTER TABLE `battlemaster_entry` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `battleground_template`
--

DROP TABLE IF EXISTS `battleground_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `battleground_template` (
  `id` mediumint(8) unsigned NOT NULL,
  `MinPlayersPerTeam` smallint(5) unsigned NOT NULL DEFAULT '0',
  `MaxPlayersPerTeam` smallint(5) unsigned NOT NULL DEFAULT '0',
  `MinLvl` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `MaxLvl` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `AllianceStartLoc` mediumint(8) unsigned NOT NULL,
  `AllianceStartO` float NOT NULL,
  `HordeStartLoc` mediumint(8) unsigned NOT NULL,
  `HordeStartO` float NOT NULL,
  `StartMaxDist` float NOT NULL DEFAULT '0',
  `Weight` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `ScriptName` char(64) NOT NULL DEFAULT '',
  `Comment` char(32) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `battleground_template`
--

LOCK TABLES `battleground_template` WRITE;
/*!40000 ALTER TABLE `battleground_template` DISABLE KEYS */;
INSERT INTO `battleground_template` VALUES (1,20,40,51,85,611,3.16312,610,0.715504,100,1,'','Alterac Valley'),(2,5,10,10,85,769,3.14159,770,0.151581,75,1,'','Warsong Gulch'),(3,8,15,20,85,890,3.91571,889,0.813671,75,1,'','Arathi Basin'),(7,8,15,61,85,1103,3.03123,1104,0.055761,75,1,'','Eye of The Storm'),(4,0,2,10,85,929,0,936,3.14159,0,1,'','Nagrand Arena'),(5,0,2,10,85,939,0,940,3.14159,0,1,'','Blades\'s Edge Arena'),(6,0,2,10,85,0,0,0,0,0,1,'','All Arena'),(8,0,2,10,85,1258,0,1259,3.14159,0,1,'','Ruins of Lordaeron'),(9,7,15,71,85,1367,0,1368,0,0,1,'','Strand of the Ancients'),(10,0,2,10,85,1362,0,1363,0,0,1,'','Dalaran Sewers'),(11,5,5,10,85,1364,0,1365,0,0,1,'','The Ring of Valor'),(30,20,40,71,85,1485,0,1486,3.16124,200,1,'','Isle of Conquest'),(32,10,10,1,85,0,0,0,0,0,1,'','Random battleground'),(108,5,10,85,85,1726,2.53684,1727,6.27533,55,1,'','Twin Peaks'),(120,5,10,85,85,1798,5.95725,1799,1.56687,60,1,'','The Battle for Gilneas');
/*!40000 ALTER TABLE `battleground_template` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;



INSERT IGNORE INTO `game_event` VALUES (18,'2010-10-21 12:01:00','2020-12-30 11:59:00',60480,5758,283,'Call to Arms: Alterac Valley!',1);
INSERT IGNORE INTO `game_event` VALUES (19,'2010-09-16 12:01:00','2020-12-30 11:59:00',60480,5758,284,'Call to Arms: Warsong Gulch!',1);
INSERT IGNORE INTO `game_event` VALUES (20,'2010-10-07 12:01:00','2020-12-30 11:59:00',60480,5758,285,'Call to Arms: Arathi Basin!',1);
INSERT IGNORE INTO `game_event` VALUES (21,'2010-10-14 12:01:00','2020-12-30 11:59:00',60480,5758,353,'Call to Arms: Eye of the Storm!',1);
INSERT IGNORE INTO `game_event` VALUES (30,'2010-09-23 12:01:00','2020-12-30 11:59:00',60480,5758,400,'Call to Arms: Strand of the Ancients!',1);
INSERT IGNORE INTO `game_event` VALUES (42,'2010-09-30 12:01:00','2020-12-30 11:59:00',60480,5758,420,'Call to Arms: Isle of Conquest!',1);
INSERT IGNORE INTO `game_event` VALUES (75,'2012-05-11 12:01:00','2020-12-30 11:59:00',60480,5758,436,'Call to Arms: Twin Peaks!',1);
INSERT IGNORE INTO `game_event` VALUES (78,'2012-05-04 12:01:00','2020-12-30 11:59:00',60480,5758,288,'Call to Arms: The Battle for Gilneas',1);


-- Strings
DELETE FROM trinity_string WHERE entry IN (1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263, 1264);
INSERT INTO trinity_string (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8) VALUES 
(1250, 'The Battle for Gilneas begins in 2 minutes.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1251, 'The Battle for Gilneas begins in 1 minute.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1252, 'The Battle for Gilneas begins in 30 seconds. Prepare yourselves!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1253, 'The Battle for Gilneas has begun!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1254, 'Alliance', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1255, 'Horde', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1256, 'lighthouse', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1257, 'waterworks', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1258, 'mine', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1259, 'The %s has taken the %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1260, '$n has defended the %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1261, '$n has assaulted the %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1262, '$n claims the %s! If left unchallenged, the %s will control it in 1 minute!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1263, 'The Alliance has gathered $1776W resources, and is near victory!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(1264, 'The Horde has gathered $1777W resources, and is near victory!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

-- HORDE GATE
DELETE FROM gameobject_template WHERE entry=207178;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(207178, 0, 10215, 'Horde Gate', 0, 0, 13623);

-- ALLIANCE GATE
DELETE FROM gameobject_template WHERE entry=205496;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(205496, 0, 6354, 'Alliance Gate', 0, 0, 13623);

-- ALLIANCE DOOR
DELETE FROM gameobject_template WHERE entry=207177;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES 
(207177, 0, 9062, 'Alliance Door', 0, 0, 13623);

-- MINE BANNER
DELETE FROM gameobject_template WHERE entry=208782;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208782, 10, 6271, 'Mine Banner', 0, 0, 13623);

-- WATERWORKS BANNER
DELETE FROM gameobject_template WHERE entry=208785;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208785, 10, 6271, 'Waterworks Banner', 0, 0, 13623);

-- LIGHTHOUSE BANNER
DELETE FROM gameobject_template WHERE entry=205557;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(205557, 10, 6271, 'Lighthouse Banner', 0, 0, 13623);

-- CONTESTED BANNER
DELETE FROM gameobject_template WHERE entry=208733;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208733, 1, 6254, 'Contested Banner', 0, 0, 13623);

-- CONTESTED BANNER
DELETE FROM gameobject_template WHERE entry=208754;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208754, 1, 6254, 'Contested Banner', 0, 0, 13623);

-- CONTESTED BANNER
DELETE FROM gameobject_template WHERE entry=208724;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208724, 1, 6252, 'Contested Banner', 0, 0, 13623);

-- ALLIANCE BANNER
DELETE FROM gameobject_template WHERE entry=208718;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208718, 1, 6251, 'Alliance Banner', 0, 0, 13623);

-- ALLIANCE BANNER
DELETE FROM gameobject_template WHERE entry=208673;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208673, 1, 6251, 'Alliance Banner', 0, 0, 13623);

-- ALLIANCE BANNER
DELETE FROM gameobject_template WHERE entry=208739;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208739, 1, 6251, 'Alliance Banner', 0, 0, 13623);

-- HORDE BANNER
DELETE FROM gameobject_template WHERE entry=208766;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208766, 1, 6253, 'Horde Banner', 0, 0, 13623);

-- HORDE BANNER
DELETE FROM gameobject_template WHERE entry=208748;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208748, 1, 6253, 'Horde Banner', 0, 0, 13623);

-- HORDE BANNER
DELETE FROM gameobject_template WHERE entry=208727;
INSERT INTO gameobject_template (entry, TYPE, displayId, NAME, data1, data6, WDBVerified) VALUES
(208727, 1, 6253, 'Horde Banner', 0, 0, 13623);

-- TwinPeaks Gates
DELETE FROM `gameobject_template` WHERE `entry` BETWEEN '402189' AND '402191';
DELETE FROM `gameobject_template` WHERE `entry` BETWEEN '402364' AND '402366';
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES
('402189','3','10122','TWINPEAKS_DWARVEN_GATE_03','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623'),
('402190','3','10123','TWINPEAKS_DWARVEN_GATE_01','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623'),
('402191','3','10124','TWINPEAKS_DWARVEN_GATE_02','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623'),
('402364','3','10442','TWINPEAKS_ORC_GATE_01','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623'),
('402365','3','10443','TWINPEAKS_ORC_GATE_02','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623'),
('402366','3','10444','TWINPEAKS_ORC_GATE_03','','','','0','0','1','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','13623');

REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (206653, 0, 10122, 'Twin Peaks Alliance Gate 3 and 4', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (206654, 0, 10123, 'Twin Peaks Alliance Gate 2', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (206655, 0, 10124, 'Twin Peaks Alliance Gate 1', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208205, 0, 10442, 'Twin Peaks Horde Gate 1', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208206, 0, 10443, 'Twin Peaks Horde Gate 2 and 3', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208207, 0, 10444, 'Twin Peaks Horde Gate 4', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (207177, 0, 9062, 'Alliance Door', '', '', '', 114, 32, 0.35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '', '', 13623);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (207178, 0, 10215, 'Horde Gate', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '', '', 13623);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195223, 0, 411, 'Horde Keep Portcullis', '', '', '', 114, 32, 3.16334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195437, 0, 7482, 'Doodad_VR_Portcullis01', '', '', '', 114, 32, 0.27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195452, 0, 411, 'Doodad_PortcullisActive02', '', '', '', 114, 32, 2.15132, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195451, 0, 411, 'Doodad_PortcullisActive01', '', '', '', 114, 32, 2.15132, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195703, 0, 9087, 'Doodad_ND_Human_Gate_ClosedFX_Door01', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195491, 0, 9035, 'Doodad_ND_WinterOrc_Wall_GateFX_Door01', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195436, 0, 7869, 'Doodad_HU_Portcullis01', '', '', '', 114, 32, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (195223, 0, 411, 'Horde Keep Portcullis', '', '', '', 114, 32, 3.16334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208779, 10, 6271, 'Lighthouse Banner', '', 'Capturing', '', 35, 0, 1, 0, 0, 0, 0, 0, 0, 1479, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 23936, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208780, 10, 6271, 'Waterworks Banner', '', 'Capturing', '', 35, 0, 1, 0, 0, 0, 0, 0, 0, 1479, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 23936, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);
REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `AIName`, `ScriptName`, `WDBVerified`) VALUES (208781, 10, 6271, 'Mines Banner', '', 'Capturing', '', 35, 0, 1, 0, 0, 0, 0, 0, 0, 1479, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 23936, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);


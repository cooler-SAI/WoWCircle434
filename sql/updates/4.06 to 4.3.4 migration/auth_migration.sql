
--
-- Table structure for table `account`
--

ALTER TABLE `account`
	ADD COLUMN `os` VARCHAR(3) NOT NULL DEFAULT '' AFTER `locale`;

--
-- Table structure for table `logs`
--
	
ALTER TABLE `logs`
	ADD COLUMN `level` INT(4) UNSIGNED NOT NULL DEFAULT '0' AFTER `type`;
	
--
-- Table structure for table `realmlist`
--

ALTER TABLE `realmlist`
	CHANGE COLUMN `id` `id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT FIRST,
	CHANGE COLUMN `address` `address` VARCHAR(255) NOT NULL DEFAULT '127.0.0.1' AFTER `name`,
	CHANGE COLUMN `port` `port` SMALLINT(5) NOT NULL DEFAULT '8085' AFTER `address`,
	ADD COLUMN `flag` TINYINT(3) UNSIGNED NOT NULL DEFAULT '2' AFTER `color`,
	CHANGE COLUMN `gamebuild` `gamebuild` INT(10) UNSIGNED NOT NULL DEFAULT '15595' AFTER `online`;
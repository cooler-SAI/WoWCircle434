INSERT INTO `access_requirement` (`mapId`, `difficulty`, `level_min`, `quest_failed_text`, `comment`) VALUES (33, 1, 85, NULL, 'Shadowfang Keep Entrance');
INSERT INTO `access_requirement` (`mapId`, `difficulty`, `level_min`, `quest_failed_text`, `comment`) VALUES (36, 1, 85, NULL, 'DeadMines Entrance');
UPDATE `access_requirement` SET `level_min`=80 WHERE `mapId`=645 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `level_min`=84 WHERE `mapId`=644 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `level_min`=80 WHERE `mapId`=643 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `level_min`=82 WHERE `mapId`=657 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `level_min`=84 WHERE `mapId`=670 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `level_min`=84 WHERE `mapId`=755 AND `difficulty`=0 LIMIT 1;
UPDATE `access_requirement` SET `comment`='Zul\'Aman' WHERE `mapId`=568;
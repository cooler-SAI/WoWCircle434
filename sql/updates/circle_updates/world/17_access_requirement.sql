ALTER TABLE `access_requirement`  ADD COLUMN `leader_achievement` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `completed_achievement`;

UPDATE `access_requirement` SET `leader_achievement`=5802 WHERE `mapId`=720 AND `difficulty`=2;
UPDATE `access_requirement` SET `leader_achievement`=5802 WHERE `mapId`=720 AND `difficulty`=3;
UPDATE `access_requirement` SET `leader_achievement`=6177 WHERE `mapId`=967 AND `difficulty`=3;
UPDATE `access_requirement` SET `leader_achievement`=6177 WHERE `mapId`=967 AND `difficulty`=2;
UPDATE `access_requirement` SET `leader_achievement`=4850 WHERE `mapId`=671 AND `difficulty`=3;
UPDATE `access_requirement` SET `leader_achievement`=4850 WHERE `mapId`=671 AND `difficulty`=2;
UPDATE `access_requirement` SET `leader_achievement`=4851 WHERE `mapId`=754 AND `difficulty`=3;
UPDATE `access_requirement` SET `leader_achievement`=4851 WHERE `mapId`=754 AND `difficulty`=2;
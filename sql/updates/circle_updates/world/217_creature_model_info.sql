ALTER TABLE `creature_model_info` ADD COLUMN `negative_displayid` MEDIUMINT(8) NOT NULL DEFAULT 0;
UPDATE `creature_model_info` SET `negative_displayid`='34997' WHERE (`modelid`='38203');
UPDATE `creature_model_info` SET `bounding_radius`='0', `combat_reach`='0' WHERE (`modelid`='38203');
UPDATE `creature_model_info` SET `bounding_radius`='0', `combat_reach`='0' WHERE (`modelid`='34997');
DELETE FROM `creature` WHERE `id`=23191;
DELETE FROM `creature` WHERE `id` IN (23418, 23419, 23420, 23469);
DELETE FROM `creature` WHERE `id` IN (23451, 23429);

UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=22856;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23418;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23419 ;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23420;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23469;
UPDATE `creature_template` SET `unit_flags`=33554434 WHERE `entry`=22856;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=23469;

DELETE FROM `creature` WHERE `id`=23089;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(92817, 23089, 564, 1, 1, 0, 0, 752.419, 304.313, 312.077, 6.19369, 600, 0, 0, 303500, 33870, 0, 0, 0, 0);

DELETE FROM `creature` WHERE `id` IN 
(23259, 22996, 22997, 23197, 23375, 23069, 23336, 23226, 23292, 23070);

UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=22917;
UPDATE `creature_template` SET `unit_flags`=0, `dynamicflags`=8 WHERE `entry`=22996;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=22997;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23069;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=23259;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=23197;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=23226;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=23336;
UPDATE `creature_template` SET `unit_flags`=0, `dynamicflags`=8 WHERE `entry`=23498;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23412;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23411;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23410;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23336;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23259;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23197;
UPDATE `creature_template` SET `dynamicflags`=8 WHERE `entry`=23226;
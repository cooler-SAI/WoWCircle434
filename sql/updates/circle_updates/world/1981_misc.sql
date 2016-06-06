DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=3684 AND `type`=5;
DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=3879 AND `type`=7;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=16848;
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(16848, 11, 0, 0, 'achievement_ohganot_so_fast');

UPDATE `gameobject` SET `spawnMask`=0 WHERE `guid`=191206 AND `id`=186658;
UPDATE `gameobject` SET `spawntimesecs`=-86400 WHERE `guid`=191158 AND `id`=186648;
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=186667;
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=186672;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=186648;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=186667;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=186672;
UPDATE `gameobject` SET `spawnMask`=3 WHERE `guid`=191157;
UPDATE `gameobject` SET `spawnMask`=3 WHERE `guid`=191158;
UPDATE `gameobject` SET `spawnMask`=3 WHERE `guid`=191160;
UPDATE `creature_template` SET `faction_A`=7, `faction_H`=7 WHERE `entry`=52945;
UPDATE `gameobject_template` SET `flags`=36 WHERE `entry`=186306;
DELETE FROM `creature` WHERE `id`=24246 AND `map`=568;
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=186728;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=16835;
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(16835, 11, 0, 0, 'achievement_ring_out');
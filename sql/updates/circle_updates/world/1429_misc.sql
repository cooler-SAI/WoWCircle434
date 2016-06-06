UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=17307 LIMIT 1;
UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=17536 LIMIT 1;
DELETE FROM `creature` WHERE `guid`=355523;
DELETE FROM `creature` WHERE `guid`=350753;
DELETE FROM `creature` WHERE `guid`=350752;
DELETE FROM `creature` WHERE `guid`=355527;
DELETE FROM `creature` WHERE `guid`=350225;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=185168;
DELETE FROM `creature` WHERE `id`=22515;
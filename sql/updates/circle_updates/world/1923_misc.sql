UPDATE `creature_template` SET `lootid`=16152 WHERE `entry`=15550;

UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=176907;

DELETE FROM `gameobject` WHERE `guid`=101339 AND `id`=180636 AND `map`=531;
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=180636;
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=180635;

UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=184277;
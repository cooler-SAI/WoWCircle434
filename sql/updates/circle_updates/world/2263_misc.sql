UPDATE `creature_template` SET `Health_mod`=20 WHERE `entry`=57838;
UPDATE `creature_template` SET `Health_mod`=60 WHERE `entry`=57839;

UPDATE `gameobject_template` SET `flags`=1056 WHERE `entry`=210210;

UPDATE `creature_template_addon` SET `auras`='109247' WHERE `entry` IN (56923, 57704, 57717, 57718);
UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=146084;
DELETE FROM `gameobject` WHERE `guid`=189741 AND `id`=146084;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry` IN (7267, 7797);
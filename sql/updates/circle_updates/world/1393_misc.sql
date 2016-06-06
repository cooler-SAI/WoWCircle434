DELETE FROM `creature` WHERE `id` IN (52593, 53216);

UPDATE `creature` SET `spawnmask`=15, `position_x`=512.36, `position_y`=-61.625, `position_z`=84 WHERE `id`=52571;
UPDATE `creature` SET `spawnmask`=0 WHERE `id`=53619;

UPDATE `creature_template` SET `lootid`=0 WHERE `entry` IN (52571, 53856, 53857, 53858);
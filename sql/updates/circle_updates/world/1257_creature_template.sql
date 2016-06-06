UPDATE `creature` SET `position_x`=-193.090, `position_y`=2111.942, `position_z`=100.153, `orientation`=2.56 WHERE `id`=3870 AND `guid`=325957;
UPDATE `creature` SET `orientation`=2.56 WHERE `id`=3870 AND `guid`=325958;
UPDATE `creature` SET `position_x`=-221.325, `position_y`=2097.479, `position_z`=100.153, `orientation`=1.14 WHERE `id`=3870 AND `guid`=325955;
UPDATE `creature` SET `position_x`=-226.399, `position_y`=2099.321, `position_z`=100.153, `orientation`=1.14 WHERE `id`=3870 AND `guid`=325956;

UPDATE `creature` SET `spawnmask`=2, `position_x`=-223.937, `position_y`=2106.082, `position_z`=100.153, `orientation`=4.27 WHERE `id`=3869 AND `guid`=327182;
UPDATE `creature` SET `spawnmask`=2, `position_x`=-218.249, `position_y`=2103.814, `position_z`=100.153, `orientation`=4.27 WHERE `id`=3869 AND `guid`=327183;
UPDATE `creature` SET `spawnmask`=2, `position_x`=-206.575, `position_y`=2117.208, `position_z`=100.153, `orientation`=5.99 WHERE `id`=3869 AND `guid`=327185;
UPDATE `creature` SET `spawnmask`=2, `position_x`=-204.567, `position_y`=2122.450, `position_z`=100.153, `orientation`=5.99 WHERE `id`=3869 AND `guid`=327184;

UPDATE `creature_addon` SET `emote`=0 WHERE `guid` IN
(327182, 327183, 327184, 327185,
325955, 325956, 325957, 325958);

UPDATE `creature_template_addon` SET `emote`=0 WHERE `entry` IN (3869, 3870);

UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry` IN (3869, 3870);

DELETE FROM `creature` WHERE `id`=3873 AND `guid`=51228;
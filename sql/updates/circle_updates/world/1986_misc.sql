UPDATE `creature_template` SET `flags_extra`=128, `ScriptName`='' WHERE `entry`=55544 LIMIT 1;
UPDATE `creature_template` SET `MovementType`=1 WHERE `entry`=56231 LIMIT 1;
UPDATE `creature_template` SET `MovementType`=1 WHERE `entry`=57437 LIMIT 1;
UPDATE `creature_template` SET `MovementType`=1 WHERE `entry`=57438 LIMIT 1;
UPDATE `creature_template` SET `MovementType`=1 WHERE `entry`=57439 LIMIT 1;
UPDATE `creature_template` SET `modelid2`=0 WHERE `entry`=56231 LIMIT 1;
UPDATE `creature_template` SET `modelid2`=0 WHERE `entry`=57437 LIMIT 1;
UPDATE `creature_template` SET `modelid2`=0 WHERE `entry`=57438 LIMIT 1;
UPDATE `creature_template` SET `modelid2`=0 WHERE `entry`=57439 LIMIT 1;

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry` IN (53500, 53813, 53814, 53815);

UPDATE `creature` SET `spawnmask`=15 WHERE `id`=58153 AND `map`=967;
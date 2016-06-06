UPDATE `creature_template` SET `ScriptName`='npc_slipstream_raid', `InhabitType`=7 WHERE `entry`=47066 LIMIT 1;


UPDATE `creature_template` SET `ScriptName` = 'boss_anshal' WHERE `entry` = 45870;
UPDATE `creature_template` SET `ScriptName` = 'boss_nezir' WHERE `entry` = 45871;
UPDATE `creature_template` SET `ScriptName` = 'boss_rohash' WHERE `entry` = 45872;
UPDATE `creature_template` SET `ScriptName` = 'npc_hurricane' WHERE `entry` = 46419;
UPDATE `creature_template` SET `ScriptName` = 'npc_tornado' WHERE `entry` = 46207;
UPDATE `creature_template` SET `ScriptName` = 'npc_ravenous_creeper_trigger' WHERE `entry` = 45813;
UPDATE `creature_template` SET `ScriptName` = 'npc_ravenous_creeper' WHERE `entry` = 45812;

DELETE FROM `spell_script_names` WHERE `spell_id` IN
(86492);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(86492, 'spell_hurricane');

UPDATE `creature_template` SET `ScriptName` = 'boss_alakir' WHERE `entry` =46753;
UPDATE `creature_template` SET `ScriptName` = 'npc_stormling' WHERE `entry` =47175;

DELETE FROM `creature` WHERE `guid` IN (1300595,1300596,1300597,1300598,1300599,1300600,1300601,1300602);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(1300595, 47066, 754, 15, 1, 0, 0, -240.162, 769.122, 195.846, 5.49923, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300596, 47066, 754, 15, 1, 0, 0, -101.552, 626.445, 195.848, 2.35436, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300597, 47066, 754, 15, 1, 0, 0, -3.43208, 626.723, 195.846, 0.816079, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300598, 47066, 754, 15, 1, 0, 0, 138.67, 764.981, 195.846, 3.92453, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300599, 47066, 754, 15, 1, 0, 0, 138.693, 862.983, 195.847, 2.36697, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300600, 47066, 754, 15, 1, 0, 0, 0.51813, 1004.95, 195.846, 5.50298, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300601, 47066, 754, 15, 1, 0, 0, -97.3398, 1005.53, 195.846, 3.93802, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(1300602, 47066, 754, 15, 1, 0, 0, -239.742, 867.066, 195.846, 0.784143, 300, 0, 0, 42, 0, 0, 0, 0, 0);

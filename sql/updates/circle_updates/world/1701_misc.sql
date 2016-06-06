UPDATE `creature_template` SET `modelid1`=38696, `modelid2`=0 WHERE `entry`=54233;

DELETE FROM `creature` WHERE `id` IN (53009, 54233) and `map`=571;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(53009, 571, 1, 65535, 0, 0, 3546.56, 5882.51, 143.915, 1.18275, 300, 0, 0, 77490, 0, 0, 0, 0, 0),
(54233, 571, 1, 65535, 0, 0, 3558.02, 5919.65, 139.203, 4.32434, 300, 0, 0, 77490, 0, 0, 0, 0, 0);

DELETE FROM `spell_area` WHERE `spell`=98069;
REPLACE INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(98069, 4024, 29239, 29269, 0, 0, 2, 1, 64, 9);

DELETE FROM `creature_template_addon` WHERE `entry` IN (53149, 53098);
INSERT INTO `creature_template_addon` (`entry`, `bytes1`, `auras`) VALUES (53098, 8, '98425');
UPDATE `creature_template` SET `unit_flags`=33554434 WHERE `entry`=53098;

DELETE FROM `creature` WHERE `id` IN (53098, 53149, 53210) and `map`=571;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(53098, 571, 1, 256, 0, 0, 3970.81, 6783.17, 156.541, 3.83346, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(53149, 571, 1, 256, 0, 0, 3956.35, 6770.45, 153.565, 0.868587, 300, 0, 0, 77490, 0, 0, 0, 0, 0),
(53210, 571, 1, 1, 0, 0, 3956.35, 6770.45, 153.565, 0.868587, 300, 0, 0, 77490, 0, 0, 0, 0, 0);

UPDATE `quest_template` SET `flags`=`flags`&~65536 WHERE `id` IN (29240, 29269);

UPDATE `creature_template` SET `ScriptName`='npc_tarecgosa_53098' WHERE `entry`=53098;
UPDATE `creature_template` SET `ScriptName`='npc_kalecgos_53149' WHERE `entry`=53149;
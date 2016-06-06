DELETE FROM `creature` WHERE `id`=52835;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(52835, 571, 1, 65535, 0, 0, 3801.14, 6551.9, 171.866, 3.60117, 300, 0, 0, 503685, 0, 0, 0, 0, 0);

UPDATE `creature_template` SET `faction_a`=35, `faction_h`=35, `ScriptName`='npc_tarecgosa_52835' WHERE `entry`=52835;
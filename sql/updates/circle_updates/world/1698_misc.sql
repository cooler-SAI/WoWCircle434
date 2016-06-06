
REPLACE INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(99488, 4265, 29193, 29225, 0, 0, 2, 1, 64, 11);

DELETE FROM `creature` WHERE `id` IN (53472, 53567, 52995);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(53472, 576, 3, 256, 0, 0, 618.939880, -315.667389, -2.895350, 0.93, 86400, 0, 0, 3874500, 0, 0, 0, 0, 0),
(53567, 576, 3, 256, 0, 0, 638.334, -291.412, -9.25012, 4.73203, 86400, 0, 0, 503685, 0, 0, 0, 0, 0),
(52995, 1, 1, 65535, 0, 0, 5543.773438, -3565.131104, 1570.066406, 3.79, 300, 0, 0, 77490, 0, 0, 0, 0, 0);

REPLACE INTO `creature_template_addon` (`entry`, `auras`) VALUES (53567, '99561');
UPDATE `creature_template` SET `Inhabittype`=5, `ScriptName`='npc_thyrinar' WHERE `entry`=53472;

UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=209033;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=209035;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=209036;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=209037;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=209100;

UPDATE `creature_template` SET `npcflag`=0, `unit_flags`=33554434, `IconName`='Pickup', `ScriptName`='npc_firelands_dull_focus' WHERE `entry` IN (53951, 53968, 53970, 53963);

UPDATE `creature_template` SET `unit_flags`=`unit_flags`|0x02000000, `IconName`='Interact', `ScriptName`='npc_firelands_circle_of_thorns_portal' WHERE `entry`=54247;

UPDATE `gameobject_template` SET `flags`=4 WHERE `entry`=209098;

UPDATE `creature_template` SET `flags_extra`=`flags_extra`|1, `ScriptName`='npc_firelands_volcanus' WHERE `entry`=53833;

DELETE FROM `creature` WHERE `id` IN (54247, 53833) AND `map`=720;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(54247, 720, 1, 65535, 0, 0, 510.224, 469.048, 246.768, 2.27408, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(54247, 720, 1, 65535, 0, 0, 182.557, 287.61, 85.7451, 3.64067, 300, 0, 0, 42, 0, 0, 0, 0, 0),
(53833, 720, 1, 65535, 0, 0, 453.196, 521.454, 244.325, 5.27431, 604800, 0, 0, 12449100, 0, 0, 0, 0, 0);

DELETE FROM `gameobject` WHERE `id`=209098 AND `map`=720;
INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(209098, 720, 1, 65535, 182.254, 288.76, 85.7784, 3.85273, 0, 0, 0.937448, -0.348124, 300, 0, 1),
(209098, 720, 1, 65535, 511.048, 469.516, 246.906, 2.19555, 0, 0, 0.890195, 0.45558, 300, 0, 1);

UPDATE `creature_template` SET `type_flags`=`type_flags`|4 WHERE `entry` IN (52409, 53797, 53798, 53799);

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(101384, 'spell_firelands_siphon_essence');

UPDATE `creature_template` SET `ScriptName`='npc_kalecgos_53349' WHERE `entry`=53349;

DELETE FROM `creature` WHERE `id` IN (53215, 53349);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(53215, 571, 1, 1, 0, 0, 3646.57, 7158.98, 229.204, 6.21137, 300, 0, 0, 77490, 0, 0, 0, 0, 0),
(53349, 571, 1, 1, 0, 0, 4002.97, 7215.61, 340.563, 1.14162, 300, 0, 0, 77490, 0, 0, 0, 0, 0);
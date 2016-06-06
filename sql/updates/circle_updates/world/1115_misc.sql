DELETE FROM `event_scripts` WHERE `id`=24005;
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`) VALUES
(24005, 5, 10, 39446, 120000);

UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_lycanthoth' WHERE `entry`=39446;

DELETE FROM `smart_scripts` WHERE `entryorguid`=39446;

REPLACE INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES
(74077, 1, 5146, -2302, 1278, 0);

UPDATE `creature_template` SET `faction_a`=35, `faction_h`=35, `minlevel`=85, `maxlevel`=85 WHERE `entry`=39622;
-- Fix spell script name on startup error
INSERT INTO `spell_script_names` VALUES
(66299,'spell_radio'),
(83883,'spell_q26861_door_dummy'),
(19593,'spell_egg_explosion');

-- Исправление битых НИП
UPDATE `creature_template` SET `npcflag` = 0 WHERE `entry` = 34865;
UPDATE `creature_template` SET `unit_flags` = 1 WHERE `entry` = 35120;

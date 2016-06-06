DELETE FROM `creature_template_addon` WHERE `entry` IN (53784, 53489);
UPDATE `creature_template` SET `speed_run`=2.0 WHERE `entry` IN (53509, 54052, 54053, 54054, 53898, 54061, 54062);
UPDATE `creature_template` SET `ScriptName`='npc_alysrazor_molten_meteor' WHERE `entry` IN (53489, 53784);
REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(100745, 'spell_alysrazor_firestorm'),
(101664, 'spell_alysrazor_firestorm'),
(101665, 'spell_alysrazor_firestorm'),
(101666, 'spell_alysrazor_firestorm');
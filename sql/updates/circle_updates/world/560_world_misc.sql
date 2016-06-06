DELETE FROM `spell_bonus_data` WHERE `entry`=33745;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (33745, 0, 0, 0.0552, 0.00369, 'Druid - Lacerate');

DELETE FROM `spell_bonus_data` WHERE `entry`=6807;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (6807, 0, 0, 0.25, 0, 'Druid - Maul');

DELETE FROM `spell_proc_event` WHERE `entry`=33603;
DELETE FROM `spell_proc_event` WHERE `entry`=33604;
DELETE FROM `spell_proc_event` WHERE `entry`=33605;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33603, 0, 7, 2, 0, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33604, 0, 7, 2, 0, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33605, 0, 7, 2, 0, 0, 65536, 1027, 0, 100, 0);

DELETE FROM `spell_script_names` WHERE `spell_id`=8921;
DELETE FROM `spell_script_names` WHERE `spell_id`=93402;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (8921, 'spell_dru_lunar_shower_energize'); 
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (93402, 'spell_dru_lunar_shower_energize'); 
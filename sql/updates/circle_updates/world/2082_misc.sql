DELETE FROM `spell_script_names` WHERE `spell_id`=89962;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(89962, 'spell_pet_stat_calculate');

REPLACE INTO `pet_scaling_data` (`spell_id`, `class`, `comment`) VALUES 
(89962, 5, 'All Critical Chance, Magic Resistance, Critical Damage Bonus');

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(105788, 0, 3, 541065249, 4096, 4104, 0, 0, 0, 100, 0),
(105797, 0, 4, 0, 0, 536870912, 1024, 0, 0, 13, 0),
(11213, 0, 3, 0, 0, 0, 86288, 2051, 0, 13, 15),
(12574, 0, 3, 0, 0, 0, 86288, 2051, 0, 27, 15),
(12575, 0, 3, 0, 0, 0, 86288, 2051, 0, 40, 15);
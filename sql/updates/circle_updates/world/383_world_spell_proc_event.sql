DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_pal_guarded_by_the_light';
DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_pal_blessing_of_sanctuary';
DELETE FROM `spell_proc_event` WHERE `entry`=85646;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85646, 0, 0, 0, 0, 16384, 0, 1027, 0, 0, 0);
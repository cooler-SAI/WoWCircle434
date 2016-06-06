DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_mage_cone_of_cold';

DELETE FROM `spell_proc_event` WHERE `entry` IN (11190, 12489);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(11190, 0, 3, 512, 0, 0, 65536, 1027, 0, 100, 0),
(12489, 0, 3, 512, 0, 0, 65536, 1027, 0, 100, 0);
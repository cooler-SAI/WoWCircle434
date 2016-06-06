DELETE FROM `spell_proc_event` WHERE `entry` IN (34485, 34486, 34487);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(34485, 0, 9, 0, 1, 0, 256, 1027, 0, 20, 0),
(34486, 0, 9, 0, 1, 0, 256, 1027, 0, 40, 0),
(34487, 0, 9, 0, 1, 0, 256, 1027, 0, 60, 0);

DELETE FROM `spell_proc_event` WHERE `entry` IN (83340, 83356);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(83340, 0, 9, 133120, 2147483648, 0, 0, 1026, 0, 100, 0),
(83356, 0, 9, 133120, 2147483648, 0, 0, 1026, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry` IN (35104, 35110);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(35104, 0, 9, 4096, 0, 0, 4416, 1026, 0, 100, 0),
(35110, 0, 9, 4096, 0, 0, 4416, 1026, 0, 100, 0);
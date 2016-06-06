DELETE FROM `spell_proc_event` WHERE `entry`=81913; 
DELETE FROM `spell_proc_event` WHERE `entry`=81914;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81913, 0, 0, 0, 0, 0, 1048576, 0, 0, 100, 120);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81914, 0, 0, 0, 0, 0, 1048576, 0, 0, 100, 120);


DELETE FROM `spell_proc_event` WHERE `entry`=46913;
DELETE FROM `spell_proc_event` WHERE `entry`=46914;
DELETE FROM `spell_proc_event` WHERE `entry`=46915;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (46913, 0, 4, 0, 1024, 0, 16, 0, 0, 10, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (46914, 0, 4, 0, 1024, 0, 16, 0, 0, 20, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (46915, 0, 4, 0, 1024, 0, 16, 0, 0, 30, 0);

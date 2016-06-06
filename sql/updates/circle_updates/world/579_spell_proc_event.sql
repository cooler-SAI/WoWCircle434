DELETE FROM `spell_proc_event` WHERE `entry`=16880;
DELETE FROM `spell_proc_event` WHERE `entry`=61345;
DELETE FROM `spell_proc_event` WHERE `entry`=61346;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (16880, 0, 7, 2097218, 0, 0, 0, 1027, 0, 0, 60);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (61345, 0, 7, 2097218, 0, 0, 0, 1027, 0, 0, 60);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (61346, 0, 7, 2097218, 0, 0, 0, 1027, 0, 0, 60);
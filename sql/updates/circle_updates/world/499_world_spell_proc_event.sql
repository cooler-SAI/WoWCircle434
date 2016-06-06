DELETE FROM `spell_proc_event` WHERE `entry`=18119;
DELETE FROM `spell_proc_event` WHERE `entry`=18120;
DELETE FROM `spell_proc_event` WHERE `entry`=85112;
DELETE FROM `spell_proc_event` WHERE `entry`=91986;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (18119, 0, 5, 0, 128, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (18120, 0, 5, 0, 128, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85112, 0, 5, 4096, 128, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (91986, 0, 5, 4096, 128, 0, 65536, 1027, 0, 100, 0);
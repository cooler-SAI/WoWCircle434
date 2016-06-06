DELETE FROM `spell_proc_event` WHERE `entry`=33881;
DELETE FROM `spell_proc_event` WHERE `entry`=33882;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33881, 0, 0, 0, 0, 0, 0, 1027, 0, 50, 6);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33882, 0, 0, 0, 0, 0, 0, 1027, 0, 100, 6);
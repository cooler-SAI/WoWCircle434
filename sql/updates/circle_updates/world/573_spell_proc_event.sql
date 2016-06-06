DELETE FROM `spell_proc_event` WHERE `entry`=34151;
DELETE FROM `spell_proc_event` WHERE `entry`=81274;
DELETE FROM `spell_proc_event` WHERE `entry`=81275;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (34151, 0, 7, 0, 2, 0, 16384, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81274, 0, 7, 0, 2, 0, 16384, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81275, 0, 7, 0, 2, 0, 16384, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=92363;
DELETE FROM `spell_proc_event` WHERE `entry`=92364;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (92363, 0, 7, 0, 16, 0, 262144, 1027, 0, 2, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (92364, 0, 7, 0, 16, 0, 262144, 1027, 0, 4, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=16864;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (16864, 0, 0, 0, 0, 0, 0, 65540, 1027, 6, 10);

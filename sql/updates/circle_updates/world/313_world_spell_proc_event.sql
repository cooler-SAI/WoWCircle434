DELETE FROM `spell_proc_event` WHERE `entry`=31828;
DELETE FROM `spell_proc_event` WHERE `entry`=31829;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (31828, 0, 0, 0, 0, 0, 0, 0, 0, 50, 8);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (31829, 0, 0, 0, 0, 0, 0, 0, 0, 100, 8);

DELETE FROM `spell_proc_event` WHERE `entry`=26022;
DELETE FROM `spell_proc_event` WHERE `entry`=26023;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (26022, 0, 0, 0, 0, 0, 0, 65536, 0, 50, 8);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (26023, 0, 0, 0, 0, 0, 0, 65536, 0, 100, 8);
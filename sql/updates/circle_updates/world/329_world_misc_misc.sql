DELETE FROM `spell_proc_event` WHERE `entry`=12311;
DELETE FROM `spell_proc_event` WHERE `entry`=12958;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (12311, 0, 0, 8, 1, 0, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (12958, 0, 0, 8, 1, 0, 0, 1027, 0, 0, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=80979;
DELETE FROM `spell_proc_event` WHERE `entry`=80980;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80979, 0, 0, 128, 0, 0, 0, 1027, 0, 0, 1);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80980, 0, 0, 128, 0, 0, 0, 1027, 0, 0, 1);

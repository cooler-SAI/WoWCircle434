DELETE FROM `spell_proc_event` WHERE `entry`=80128;
DELETE FROM `spell_proc_event` WHERE `entry`=80129;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80128, 0, 4, 0, 64, 0, 16, 0, 0, 25, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80129, 0, 4, 0, 64, 0, 16, 0, 0, 50, 0);
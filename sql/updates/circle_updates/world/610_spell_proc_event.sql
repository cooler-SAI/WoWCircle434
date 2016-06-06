DELETE FROM `spell_proc_event` WHERE `entry`=14168;
DELETE FROM `spell_proc_event` WHERE `entry`=14169;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (14168, 0, 8, 524288, 0, 0, 69648, 1027, 0, 50, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (14169, 0, 8, 524288, 0, 0, 69648, 1027, 0, 100, 0);

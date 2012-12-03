DELETE FROM `spell_proc_event` WHERE `entry`=30299;
DELETE FROM `spell_proc_event` WHERE `entry`=30301;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (30299, 0, 0, 0, 0, 0, 0, 1027, 0, 50, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (30301, 0, 0, 0, 0, 0, 0, 1027, 0, 100, 0);
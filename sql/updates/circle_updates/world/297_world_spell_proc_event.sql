DELETE FROM `spell_proc_event` WHERE `entry`=84800;
DELETE FROM `spell_proc_event` WHERE `entry`=85511;
DELETE FROM `spell_proc_event` WHERE `entry`=85512;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (84800, 0, 10, 1073741824, 0, 1024, 16384, 0, 0, 33, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85511, 0, 10, 1073741824, 0, 1024, 16384, 0, 0, 66, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85512, 0, 10, 1073741824, 0, 1024, 16384, 0, 0, 100, 0);

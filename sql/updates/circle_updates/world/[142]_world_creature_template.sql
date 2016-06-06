DELETE FROM `spell_proc_event` WHERE `entry`=85117;
DELETE FROM `spell_proc_event` WHERE `entry`=86172;
DELETE FROM `spell_proc_event` WHERE `entry`=85803;
DELETE FROM `spell_proc_event` WHERE `entry`=85804;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85117, 0, 10, 8388608, 2228354, 40960, 69904, 3, 0, 7, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85803, 0, 10, 0, 0, 16384, 16384, 0, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85804, 0, 10, 0, 0, 16384, 16384, 0, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (86172, 0, 10, 8388608, 2228354, 40960, 69904, 3, 0, 15, 0);

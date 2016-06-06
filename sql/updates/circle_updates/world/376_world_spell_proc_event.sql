DELETE FROM `spell_proc_event` WHERE `entry`=75806;
DELETE FROM `spell_proc_event` WHERE `entry`=85043;
DELETE FROM `spell_proc_event` WHERE `entry`=85416;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (75806, 0, 10, 0, 294912, 0, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85043, 0, 10, 0, 294912, 0, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85416, 0, 10, 16384, 0, 0, 65552, 1027, 0, 0, 0);
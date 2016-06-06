DELETE FROM `spell_proc_event` WHERE `entry`=77755;
DELETE FROM `spell_proc_event` WHERE `entry`=77756;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77755, 0, 11, 268435456, 0, 0, 262144, 1027, 0, 10, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77756, 0, 11, 268435456, 0, 0, 262144, 1027, 0, 20, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=86183;
DELETE FROM `spell_proc_event` WHERE `entry`=86184;
DELETE FROM `spell_proc_event` WHERE `entry`=86185;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (86183, 0, 11, 3, 0, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (86184, 0, 11, 3, 0, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (86185, 0, 11, 3, 0, 0, 65536, 1027, 0, 100, 0);
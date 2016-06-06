DELETE FROM `spell_proc_event` WHERE `entry`=20502;
DELETE FROM `spell_proc_event` WHERE `entry`=20503;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (20502, 0, 4, 536870912, 0, 0, 16, 0, 0, 50, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (20503, 0, 4, 536870912, 0, 0, 16, 0, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=12329;
DELETE FROM `spell_proc_event` WHERE `entry`=12950;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (12329, 0, 4, 4194304, 4, 0, 4112, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (12950, 0, 4, 4194304, 4, 0, 4112, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=29598;
DELETE FROM `spell_proc_event` WHERE `entry`=84607;
DELETE FROM `spell_proc_event` WHERE `entry`=84608;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (29598, 0, 4, 4096, 0, 0, 0, 0, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (84607, 0, 4, 4096, 0, 0, 0, 0, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (84608, 0, 4, 4096, 0, 0, 0, 0, 0, 100, 0);
 
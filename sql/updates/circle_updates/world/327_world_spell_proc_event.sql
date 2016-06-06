DELETE FROM `spell_proc_event` WHERE `entry`=80979;
DELETE FROM `spell_proc_event` WHERE `entry`=80980;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80979, 0, 0, 0, 0, 128, 1027, 0, 0, 0, 1);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (80980, 0, 0, 0, 0, 128, 1027, 0, 0, 0, 1);

DELETE FROM `spell_proc_event` WHERE `entry`=46867;
DELETE FROM `spell_proc_event` WHERE `entry`=56611;
DELETE FROM `spell_proc_event` WHERE `entry`=56612;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (46867, 0, 4, 33554432, 0, 0, 0, 1026, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (56611, 0, 4, 33554432, 0, 0, 0, 1026, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (56612, 0, 4, 33554432, 0, 0, 0, 1026, 0, 100, 0);

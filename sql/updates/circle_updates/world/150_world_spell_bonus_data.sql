DELETE FROM `spell_bonus_data` WHERE `entry`=879;
DELETE FROM `spell_bonus_data` WHERE `entry`=24275;
DELETE FROM `spell_bonus_data` WHERE `entry`=53600;

DELETE FROM `spell_proc_event` WHERE `entry`=76672;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (76672, 1, 10, 0, 163840, 8192, 16, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=85285;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85285, 0, 0, 0, 0, 0, 1048576, 3, 0, 100, 30);

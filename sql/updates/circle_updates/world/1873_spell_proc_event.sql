-- (79577) Eclipse Mastery Driver Passive
DELETE FROM `spell_proc_event` WHERE `entry` IN (79577);
INSERT INTO `spell_proc_event` VALUES (79577, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00015400, 0x0000060F, 0, 100, 0);

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES ('46832', '0', '0', '0', '0', '0', '0', '2', '0', '0', '6');

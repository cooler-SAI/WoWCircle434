-- (31801) Печать правды 
REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) 
VALUES (31801, 0, 10, 8388608, 32898, 8192, 0, 1027, 0, 100, 0);

-- (14523) Atonement (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (14523);
INSERT INTO `spell_proc_event` VALUES (14523, 0x00, 0x06, 0x00000080, 0x00000000, 0x00000000, 0x00010000, 0x00000003, 0, 100, 0);

-- (81749) Atonement (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (81749);
INSERT INTO `spell_proc_event` VALUES (81749, 0x00, 0x06, 0x00000080, 0x00000000, 0x00000000, 0x00010000, 0x00000003, 0, 100, 0);

-- (54939) Glyph of Divinity
DELETE FROM `spell_proc_event` WHERE `entry` IN (54939);
INSERT INTO `spell_proc_event` VALUES (54939, 0x00, 0x0A, 0x00008000, 0x00000000, 0x00000000, 0x00004400, 0x00000000, 0, 100, 0);
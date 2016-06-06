-- (30299) Nether Protection (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (30299);
INSERT INTO `spell_proc_event` VALUES (30299, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00020220, 0x00000400, 0, 100, 0);

-- (30301) Nether Protection (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (30301);
INSERT INTO `spell_proc_event` VALUES (30301, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00020220, 0x00000400, 0, 100, 0);

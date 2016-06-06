-- (34485) Master Marksman (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (34485);
INSERT INTO `spell_proc_event` VALUES (34485, 0x00, 0x09, 0x00000000, 0x00000001, 0x00000000, 0x00000100, 0x00000403, 0, 20, 1);

-- (34486) Master Marksman (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (34486);
INSERT INTO `spell_proc_event` VALUES (34486, 0x00, 0x09, 0x00000000, 0x00000001, 0x00000000, 0x00000100, 0x00000403, 0, 40, 1);

-- (34487) Master Marksman (Rank 3)
DELETE FROM `spell_proc_event` WHERE `entry` IN (34487);
INSERT INTO `spell_proc_event` VALUES (34487, 0x00, 0x09, 0x00000000, 0x00000001, 0x00000000, 0x00000100, 0x00000403, 0, 60, 1);

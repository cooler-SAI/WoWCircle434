-- (49018) Sudden Doom (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (49018);
INSERT INTO `spell_proc_event` VALUES (49018, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000403, 0, 4, 0);

-- (49529) Sudden Doom (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (49529);
INSERT INTO `spell_proc_event` VALUES (49529, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000403, 0, 12, 0);

-- (49530) Sudden Doom (Rank 3)
DELETE FROM `spell_proc_event` WHERE `entry` IN (49530);
INSERT INTO `spell_proc_event` VALUES (49530, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000403, 0, 15, 0);

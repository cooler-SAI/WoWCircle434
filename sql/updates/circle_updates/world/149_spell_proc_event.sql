-- (47220) Empowered Imp (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (47220);
INSERT INTO `spell_proc_event` VALUES (47220, 0x00, 0x05, 0x00001000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 3, 0);

-- (47221) Empowered Imp (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (47221);
INSERT INTO `spell_proc_event` VALUES (47221, 0x00, 0x05, 0x00001000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 5, 0);

-- (77495) Harmony
DELETE FROM `spell_proc_event` WHERE `entry` IN (77495);
INSERT INTO `spell_proc_event` VALUES (77495, 0x00, 0x07, 0x00000060, 0x02000002, 0x00000000, 0x00004000, 0x00000000, 0, 100, 0);
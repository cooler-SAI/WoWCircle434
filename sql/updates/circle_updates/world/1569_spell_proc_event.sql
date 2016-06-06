-- (53253) Invigoration (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (53253);
INSERT INTO `spell_proc_event` VALUES (53253, 0x00, 0x0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0, 0, 0);

-- (53252) Invigoration (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (53252);
INSERT INTO `spell_proc_event` VALUES (53252, 0x00, 0x0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0, 0, 0);

-- (53397) Invigoration (Passive)
DELETE FROM `spell_proc_event` WHERE `entry` IN (53397);
INSERT INTO `spell_proc_event` VALUES (53397, 0x00, 0x0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0, 0, 0);
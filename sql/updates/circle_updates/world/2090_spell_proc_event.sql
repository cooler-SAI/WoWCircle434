-- (79096) Restless Blades (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (79096);
INSERT INTO `spell_proc_event` VALUES (79096, 0x00, 0x08, 0x00000000, 0x00000000, 0x00000000, 0x00001110, 0x00000403, 0, 100, 0);

-- (79095) Restless Blades (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (79095 );
INSERT INTO `spell_proc_event` VALUES (79095, 0x00, 0x08, 0x00000000, 0x00000000, 0x00000000, 0x00001110, 0x00000403, 0, 100, 0);

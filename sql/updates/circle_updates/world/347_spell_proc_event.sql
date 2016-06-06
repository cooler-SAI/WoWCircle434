-- (11213) Arcane Concentration (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (11213);
INSERT INTO `spell_proc_event` VALUES (11213, 0x00, 0x03, 0x00000000, 0x00000000, 0x00000000, 0x00015110, 0x00000803, 0, 3, 0);

-- (12574) Arcane Concentration (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (12574);
INSERT INTO `spell_proc_event` VALUES (12574, 0x00, 0x03, 0x00000000, 0x00000000, 0x00000000, 0x00015110, 0x00000803, 0, 6, 0);

-- (12575) Arcane Concentration (Rank 3)
DELETE FROM `spell_proc_event` WHERE `entry` IN (12575);
INSERT INTO `spell_proc_event` VALUES (12575, 0x00, 0x03, 0x00000000, 0x00000000, 0x00000000, 0x00015110, 0x00000803, 0, 10, 0);

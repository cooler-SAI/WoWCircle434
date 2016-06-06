-- (79683) Arcane Missiles!
DELETE FROM `spell_proc_event` WHERE `entry` IN (79683);
INSERT INTO `spell_proc_event` VALUES (79683, 0x00, 0x03, 0x00000800, 0x00000000, 0x00000000, 0x00001000, 0x00010000, 0, 100, 0);
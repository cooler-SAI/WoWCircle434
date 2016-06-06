-- (86209) Nether Vortex(level 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (86209);
INSERT INTO `spell_proc_event` VALUES (86209, 0x00, 0x03, 0x20000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0);

-- (86181) Nether Vortex(level 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (86181);
INSERT INTO `spell_proc_event` VALUES (86181, 0x00, 0x03, 0x20000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 50, 0);
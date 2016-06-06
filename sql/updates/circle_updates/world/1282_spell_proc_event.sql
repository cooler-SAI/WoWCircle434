-- (85100) Pandemic (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (85100);
INSERT INTO `spell_proc_event` VALUES (85100, 0x00, 0x05, 0x00004000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 100, 0);

-- (85099) Pandemic (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (85099);
INSERT INTO `spell_proc_event` VALUES (85099, 0x00, 0x05, 0x00004000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 50, 0);


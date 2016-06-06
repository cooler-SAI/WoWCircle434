-- (16235) Ancestral Healing (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (16235);
INSERT INTO `spell_proc_event` VALUES (16235, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00055554, 0x00000000, 0, 100, 0);

-- (16176) Ancestral Healing (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (16176);
INSERT INTO `spell_proc_event` VALUES (16176, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00055554, 0x00000000, 0, 100, 0);
 
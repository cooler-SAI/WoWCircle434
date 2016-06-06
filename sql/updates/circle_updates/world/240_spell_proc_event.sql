-- (88688) Surge of Light (Proc)
DELETE FROM `spell_proc_event` WHERE `entry` IN (88688);
INSERT INTO `spell_proc_event` VALUES (88688, 0x00, 0x06, 0x00000800, 0x00000000, 0x00000000, 0x00004400, 0x00000003, 0, 100, 0);

-- (88687) Surge of Light (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (88687);
INSERT INTO `spell_proc_event` VALUES (88687, 0x00, 0x06, 0x00001C80, 0x00000004, 0x00000000, 0x00014000, 0x00000003, 0, 3, 0);

-- (88690) Surge of Light (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (88690);
INSERT INTO `spell_proc_event` VALUES (88690, 0x00, 0x06, 0x00001C80, 0x00000004, 0x00000000, 0x00014000, 0x00000003, 0, 6, 0);
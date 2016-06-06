-- (76806) Main Gauche
DELETE FROM `spell_proc_event` WHERE `entry` IN (76806);
INSERT INTO `spell_proc_event` VALUES (76806, 0x00, 0x08, 0x00000000, 0x00000000, 0x00000000, 0x00000014, 0x00000000, 0, 100, 0);

-- (77222) Elemental Overload
DELETE FROM `spell_proc_event` WHERE `entry` IN (77222);
INSERT INTO `spell_proc_event` VALUES (77222, 0x00, 0x0B, 0x00000003, 0x00001000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0);

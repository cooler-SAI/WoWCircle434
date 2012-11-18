-- (44543) Ледяные пальцы (Уровень 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (44543);
INSERT INTO `spell_proc_event` VALUES (44543, 0x00, 0x03, 0x00100220, 0x00001000, 0x00004000, 0x00010000, 0x00000003, 0, 7, 0);

-- (44545) Ледяные пальцы (Уровень 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (44545);
INSERT INTO `spell_proc_event` VALUES (44545, 0x00, 0x03, 0x00100220, 0x00001000, 0x00004000, 0x00010000, 0x00000003, 0, 14, 0);

-- (83074) Ледяные пальцы (Уровень 3)
DELETE FROM `spell_proc_event` WHERE `entry` IN (83074);
INSERT INTO `spell_proc_event` VALUES (83074, 0x00, 0x03, 0x00100220, 0x00001000, 0x00004000, 0x00010000, 0x00000003, 0, 20, 0);

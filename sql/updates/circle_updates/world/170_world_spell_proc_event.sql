-- (84723) Ответные чары (Уровень 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (84723);
INSERT INTO `spell_proc_event` VALUES (84723, 0x00, 0x03, 0x00004000, 0x00000000, 0x00000000, 0x00010000, 0x00001000, 0, 100, 0);

-- (84722) Ответные чары (Уровень 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (84722);
INSERT INTO `spell_proc_event` VALUES (84722, 0x00, 0x03, 0x00004000, 0x00000000, 0x00000000, 0x00010000, 0x00001000, 0, 100, 0);

-- (61216) Грубое прерывание (Уровень 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (61216);
INSERT INTO `spell_proc_event` VALUES (61216, 0x00, 0x04, 0x00000808, 0x00000000, 0x00000000, 0x00001010, 0x00001000, 0, 100, 0);

-- (61221) Грубое прерывание (Уровень 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (61221);
INSERT INTO `spell_proc_event` VALUES (61221, 0x00, 0x04, 0x00000808, 0x00000000,0x00000000, 0x00001010, 0x00001000, 0, 100, 0);

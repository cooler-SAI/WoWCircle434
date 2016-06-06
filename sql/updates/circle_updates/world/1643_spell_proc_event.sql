-- (19573) Улучшенное лечение питомца (Уровень 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (19573);
INSERT INTO `spell_proc_event` VALUES (19573, 0x00, 0x09, 0x00800000, 0x00000000, 0x00000000, 0x00044000, 0x00000000, 0, 100, 0);

-- (19572) Улучшенное лечение питомца (Уровень 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (19572);
INSERT INTO `spell_proc_event` VALUES (19572, 0x00, 0x09, 0x00800000, 0x00000000, 0x00000000, 0x00044000, 0x00000000, 0, 100, 0);
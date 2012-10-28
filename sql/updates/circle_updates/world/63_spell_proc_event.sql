-- (51099) Разносчик черной чумы (Уровень 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (51099);
INSERT INTO `spell_proc_event` VALUES (51099, 0x00, 0x0F, 0x00000007, 0x00000000, 0x00001000, 0x00000000, 0x00000000, 0, 100, 0);

-- (51160) Разносчик черной чумы (Уровень 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (51160);
INSERT INTO `spell_proc_event` VALUES (51160, 0x00, 0x0F, 0x00000007, 0x00000000, 0x00001000, 0x00000000, 0x00000000, 0, 100, 0);
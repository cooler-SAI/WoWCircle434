UPDATE `creature_template` SET `unit_flags`=33554434, `flags_extra`=2 WHERE `entry` IN (52608);

DELETE FROM `creature` WHERE `map`=859 AND `id` IN (52137, 52154, 52157, 52156, 52324);
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN (52157, 52156, 52324);

UPDATE `gameobject_template` SET `flags`=4 WHERE `entry` IN (208850, 208845, 208846, 208847, 208848, 208849);

UPDATE `creature_template` SET `ScriptName`='boss_mandokir' WHERE `entry`=52151;
UPDATE `creature_template` SET `ScriptName`='npc_mandokir_chained_spirit' WHERE `entry`=52156;
UPDATE `creature_template` SET `ScriptName`='npc_mandokir_ohgan' WHERE `entry`=52157;

DELETE FROM `spell_script_names` WHERE `spell_id`=96776;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (96776, 'spell_mandokir_bloodletting');

DELETE FROM `creature_text` WHERE `entry`=52151;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(52151, 0, 0, 'Make peace, wormd. I be deliverin\' you to Hakkar myself!', 14, 0, 0, 0, 0, 24289, 'Mandokir aggro'),
(52151, 1, 0, 'My blood feeds Hakkar! My soul... feeds... Jin\'do!', 14, 0, 0, 0, 0, 24290, 'Mandokir death'),
(52151, 2, 0, 'Off with your head!', 14, 0, 0, 0, 0, 24291, 'Mandokir decapitate 01'),
(52151, 2, 1, 'Heads gonna roll!', 14, 0, 0, 0, 0, 24292, 'Mandokir decapitate 02'),
(52151, 2, 2, 'Ha ha ha! Is that all you got?', 14, 0, 0, 0, 0, 24293, 'Mandokir kill 01'),
(52151, 2, 3, 'Your broken body belongs to Hakkar!', 14, 0, 0, 0, 0, 24294, 'Mandokir kill 02'),
(52151, 2, 4, 'Fall to my feet. Fall to Mandokir, the bloodlord!', 14, 0, 0, 0, 0, 24295, 'Mandokir kill 03'),
(52151, 3, 0, 'Ding!', 14, 0, 0, 0, 0, 24297, 'Mandokir levelup'),
(52151, 4, 0, 'Go an\' get dem, Ohgan! We WON\'T be fooled again!', 14, 0, 0, 0, 0, 24298, 'Mandokir ohgan 01'),
(52151, 5, 0, 'Rise, Ohgan! Rise, and fight for your master!', 14, 0, 0, 0, 0, 24299, 'Mandokir ohgan 02');

DELETE FROM `locales_creature_text` WHERE `entry`=52151;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(52151, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ну все, черви! Сейчас я отправлю вас к Хаккару!'),
(52151, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Мою кровь пример Хаккар, а мою душу... Джин\'до...'),
(52151, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Голову с плеч!'),
(52151, 2, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ох и полетят головы!'),
(52151, 2, 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ха-ха-ха! И это все?'),
(52151, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Твое никчемное тело теперь принадлежит Хаккару.'),
(52151, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Пади пред Мандокиром, Повелителем Крови!'),
(52151, 3, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Опа!'),
(52151, 4, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Взять их, Оган! Нас больше не обманут!'),
(52151, 5, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вставай, Оган! Вставай и сражайся за своего хозяина!');
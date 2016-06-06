
DELETE FROM `creature_text` WHERE `entry`=54938;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(54938, 0, 0, 'I looked into the eyes of The Dragon, and despaired...', 14, 0, 100, 0, 0, 25865, 'benedictus death'),
(54938, 1, 0, 'Get inside! Quickly! I\'ll hold them off.', 12, 0, 100, 0, 0, 25866, 'benedictus event 01'),
(54938, 2, 0, 'And now, Shaman, you will give the Dragon Soul to me.', 12, 0, 100, 0, 0, 25867, 'benedictus event 02'),
(54938, 3, 0, 'The light will consume you!', 14, 0, 100, 0, 0, 25868, 'benedictus spell 01'),
(54938, 4, 0, 'Drown in Shadow!', 14, 0, 100, 0, 0, 25869, 'benedictus spell 02');

DELETE FROM `locales_creature_text` WHERE `entry`=54938;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(54938, 0, 0, 'Я заглянул в глаза Дракона, и отчаяние охватило меня...'),
(54938, 1, 0, 'Скорее, внутрь! Я задержу их!'),
(54938, 2, 0, 'А теперь, шаман, ты отдашь Душу Дракона МНЕ.'),
(54938, 3, 0, 'Свет поглотит вас!'),
(54938, 4, 0, 'Утоните во тьме!');
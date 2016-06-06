DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN 
(18495, 18496, 18497, 18498, 18458, 18459, 18447, 18482);
REPLACE INTO `achievement_criteria_data` (`criteria_id`, `type`, `ScriptName`) VALUES 
(18495, 11, 'achievement_taste_the_rainbow_BY'),
(18496, 11, 'achievement_taste_the_rainbow_RG'),
(18497, 11, 'achievement_taste_the_rainbow_BB'),
(18498, 11, 'achievement_taste_the_rainbow_PY');

REPLACE INTO `achievement_reward` (`entry`, `title_A`, `title_H`, `item`, `sender`, `subject`, `text`) VALUES 
(4845, 0, 0, 62900, 34337, 'Recovered item', 'We recovered a lost item in the twisting nether and noted that it was yours. Please find said object enclosed.'),
(4853, 0, 0, 62901, 34337, 'Recovered item', 'We recovered a lost item in the twisting nether and noted that it was yours. Please find said object enclosed.'),
(5828, 0, 0, 69230, 34337, 'Recovered item', 'We recovered a lost item in the twisting nether and noted that it was yours. Please find said object enclosed.'),
(6169, 0, 0, 77068, 34337, 'Recovered item', 'We recovered a lost item in the twisting nether and noted that it was yours. Please find said object enclosed.');

REPLACE INTO `locales_achievement_reward` (`entry`, `subject_loc1`, `subject_loc2`, `subject_loc3`, `subject_loc4`, `subject_loc5`, `subject_loc6`, `subject_loc7`, `subject_loc8`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES 
(4845, '', '', '', '', '', '', '', 'Найденная вещь', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Мы нашли это посреди Круговерти Пустоты. Кажется, вещичка ваша. Прилагаем к письму посылку.'),
(4853, '', '', '', '', '', '', '', 'Найденная вещь', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Мы нашли это посреди Круговерти Пустоты. Кажется, вещичка ваша. Прилагаем к письму посылку.'),
(6169, '', '', '', '', '', '', '', 'Найденная вещь', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Мы нашли это посреди Круговерти Пустоты. Кажется, вещичка ваша. Прилагаем к письму посылку.'),
(5828, '', '', '', '', '', '', '', 'Найденная вещь', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Мы нашли это посреди Круговерти Пустоты. Кажется, вещичка ваша. Прилагаем к письму посылку.');

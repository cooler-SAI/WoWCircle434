UPDATE `creature` SET `spawnmask`=15 WHERE `id`=52530;

UPDATE `creature_template` SET `ScriptName`='npc_alysrazor_molten_meteor' WHERE `entry`=53489;
UPDATE `creature_template` SET `ScriptName`='npc_alysrazor_herald_of_the_burning_end' WHERE `entry`=53375;

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_alysrazor_molten_feather_script';
REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(102111, 'spell_alysrazor_cataclysm'),
(98734, 'spell_alysrazor_molten_feather_script');

DELETE FROM `creature_text` WHERE `entry` = 52530;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(52530, 0, 0, 'I serve a new master now, mortals!', 14, 0, 100, 0, 0, 24426, 'Alysrazor aggro'),
(52530, 1, 0, 'The light... mustn\'t... burn out...', 14, 0, 100, 0, 0, 24429, 'Alysrazor death'),
(52530, 2, 0, 'BURN!', 14, 0, 100, 0, 0, 24430, 'Alysrazor kill 01'),
(52530, 2, 1, 'For his Glory!', 14, 0, 100, 0, 0, 24431, 'Alysrazor kill 02'),
(52530, 3, 0, 'These skies are MINE!', 14, 0, 100, 0, 0, 24434, 'Alysrazor spiral 01'),
(52530, 4, 0, 'I will burn you from the sky', 14, 0, 100, 0, 0, 24435, 'Alysrazor spiral 02'),
(52530, 5, 0, 'Fire... fire...', 14, 0, 100, 0, 0, 24436, 'Alysrazor transition 01'),
(52530, 6, 0, 'Я переродилась в огне!', 14, 0, 100, 0, 0, 24437, 'Alysrazor transition 02');

DELETE FROM `locales_creature_text` WHERE `entry` = 52530;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(52530, 0, 0, 'Теперь я служу новому господину, смертные!'),
(52530, 1, 0, 'Свет... не должен... погаснуть...'),
(52530, 2, 0, 'ГОРИ!'),
(52530, 2, 1, 'Во славу его!'),
(52530, 3, 0, 'Небо над вами принадлежит МНЕ!'),
(52530, 4, 0, 'Я буду жечь вас с неба!'),
(52530, 5, 0, 'Огонь... Огонь...'),
(52530, 6, 0, 'Я переродилась в огне!');

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (17538, 17536, 17535, 17533, 17448, 17449, 18098);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(17538, 11, 0, 0, 'achievement_do_a_barrel_roll'),
(17536, 11, 0, 0, 'achievement_do_a_barrel_roll'),
(17535, 11, 0, 0, 'achievement_do_a_barrel_roll'),
(17533, 11, 0, 0, 'achievement_do_a_barrel_roll');
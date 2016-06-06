DELETE FROM `creature` WHERE `map`=36 AND `id`=49564;
DELETE FROM `creature` WHERE `map`=36 AND `id`=49429;

UPDATE `creature_template` SET `ScriptName`='boss_vanessa_vancleef' WHERE `entry`=49541;
UPDATE `creature_template` SET `ScriptName`='npc_vanessa_vancleef_a_note_from_vanessa' WHERE `entry`=49564;
UPDATE `creature_template` SET `ScriptName`='npc_vanessa_vancleef_vanessa_sitting' WHERE `entry`=49429;

DELETE FROM `creature` WHERE `map`=36 AND `id`=49541;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(49541, 36, 2, 1, 0, 49541, -62.9399, -820.721, 41.2854, 3.12676, 86400, 0, 0, 4149700, 0, 0, 0, 0, 0);

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN
(16208, 14372, 524, 16213);

DELETE FROM `spell_script_names` WHERE `spell_id`=92620;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(92620, 'spell_vanessa_vancleef_backslash_targeting');

DELETE FROM `creature_text` WHERE `entry`=49541;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(49541, 0, 0, "I will not share my father\'s fate! Your tale ends here!", 14, 0, 100, 0, 0, 24599, 'Vanessa VanCleef aggro'),
(49541, 1, 0, "ENOUGH! I will not give you the pleasure.", 14, 0, 100, 0, 0, 24610, 'Vanessa VanCleef death 1'),
(49541, 1, 1, "If I'm going to die, I\'m taking you all with me!", 14, 0, 100, 0, 0, 24611, 'Vanessa VanCleef death 2'),
(49541, 1, 2, "MY FATE IS MY OWN!", 14, 0, 100, 0, 0, 24612, 'Vanessa VanCleef death 3'),
(49541, 2, 0, "The first of many!", 14, 0, 100, 0, 0, 24618, 'Vanessa VanCleef kill'),
(49541, 3, 0, "Did you really think I would come to this fight alone!?", 14, 0, 100, 0, 0, 24620, 'Vanessa VanCleef adds'),
(49541, 4, 0, "Fools! This entire ship is rigged with explosives! Enjoy your fiery deaths!", 14, 0, 100, 0, 0, 24621, 'Vanessa VanCleef spell 1'),
(49541, 5, 0, "You didn't honestly think I would only plant ONE set of explosives, did you?", 14, 0, 100, 0, 0, 24622, 'Vanessa VanCleef spell 2');

DELETE FROM `locales_creature_text` WHERE `entry`=49541;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(49541, 0, 0, 'Я не повторю судьбу своего отца! Рассказ окончен!'),
(49541, 1, 0, 'Я не доставлю вам такого удовольствия!'),
(49541, 1, 1, 'Если мне суждено умереть, то я заберу вас всех с собой!'),
(49541, 1, 2, 'Я – хозяйка своей судьбы!'),
(49541, 2, 0, 'Первая смерть из многих.'),
(49541, 3, 0, 'Вы и впрямь считали, что я решила сразиться с вами в одиночку?'),
(49541, 4, 0, 'Идиоты! Весь корабль начинен взрывчаткой! Наслаждайтесь вашей пламенной смертью!'),
(49541, 5, 0, 'Вы же не думали, что я заложила только одну бомбу?');
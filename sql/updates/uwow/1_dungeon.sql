

DELETE FROM `creature` WHERE `map`=967 AND `id` IN 
(53891, 56162, 56161, 53890, 53889, 53888, 56341, 56575, 53879);

DELETE FROM `creature` WHERE `map`=967  AND `guid`=345004 AND `id`=42844;

UPDATE `creature_addon` SET `auras`='109247' WHERE `guid`=344853;

UPDATE `creature_template_addon` SET `auras`='109247' WHERE `entry`=53879;

UPDATE `creature_template` SET `Inhabittype`=7 WHERE `entry` IN (53890, 53889);

UPDATE `creature_template` SET `modelid1`=39429, `modelid2`=0 WHERE `entry` IN 
(56575, 57887, 57888, 57889, 56341, 57884, 57885, 57886);

UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry` IN (53888);

UPDATE `creature_template` SET `mechanic_immune_mask`=617297887 WHERE `entry` IN
(53889, 57895, 57896, 57897);

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry` IN
(53890, 56516, 56517, 56518,
53891, 57879, 57880, 57881,
56161, 57901, 57902, 57903,
56162, 57904, 57905, 57906,
56341, 57884, 57885, 57886,
56575, 57887, 57888, 57889);

UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_deathwing' WHERE `entry`=53879;
UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_corruption' WHERE `entry`=53891;
UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_hideous_amalgamation' WHERE `entry`=53890;
UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_spawner' WHERE `entry`=53888;
UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_corrupted_blood' WHERE `entry`=53889;
UPDATE `creature_template` SET `ScriptName`='npc_spine_of_deathwing_burning_tendons' WHERE `entry` IN (56341, 56575);

DELETE FROM `spell_script_names` WHERE `spell_id` IN 
(104621, 109379, 105845, 105241, 105846, 106199, 106200, 105777);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(104621, 'spell_spine_of_deathwing_roll_control_check'),
(109379, 'spell_spine_of_deathwing_searing_plasma_aoe'),
(105845, 'spell_spine_of_deathwing_nuclear_blast'),
(105241, 'spell_spine_of_deathwing_absorb_blood'),
(105846, 'spell_spine_of_deathwing_nuclear_blast_script'),
(106199, 'spine_of_deathwing_blood_corruption'),
(106200, 'spine_of_deathwing_blood_corruption'),
(105777, 'spine_of_deathwing_roll_control');

DELETE FROM `conditions` WHERE `SourceEntry` IN 
(105241, 105846, 105363, 105385);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '105241', '0', '31', '0', '3', '53889', 'Spine of Deathwing - Absorb Blood'),
('13', '1', '105846', '0', '31', '0', '3', '56341', 'Spine of Deathwing - Nuclear Blast Script'),
('13', '1', '105846', '1', '31', '0', '3', '56575', 'Spine of Deathwing - Nuclear Blast Script'),
('13', '1', '105363', '0', '31', '0', '5', '209623', 'Spine of Deathwing - Armor Breach'),
('13', '1', '105363', '1', '31', '0', '5', '209631', 'Spine of Deathwing - Armor Breach'),
('13', '1', '105363', '2', '31', '0', '5', '209632', 'Spine of Deathwing - Armor Breach'),
('13', '1', '105385', '0', '31', '0', '5', '209623', 'Spine of Deathwing - Armor Breach'),
('13', '1', '105385', '1', '31', '0', '5', '209631', 'Spine of Deathwing - Armor Breach'),
('13', '1', '105385', '2', '31', '0', '5', '209632', 'Spine of Deathwing - Armor Breach');

DELETE FROM `creature_text` WHERE `entry`=53879;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(53879, 0, 0, 'Deathwing feels players on his left side.\nHe\'s about to roll left!', 41, 0, 100, 0, 0, 0, 'Spine of Deathwing - left roll preparation'),
(53879, 1, 0, 'Deathwing feels players on his right side.\nHe\'s about to roll right!', 41, 0, 100, 0, 0, 0, 'Spine of Deathwing - right roll preparation'),
(53879, 2, 0, 'Deathwing rolls left!', 41, 0, 100, 0, 0, 0, 'Spine of Deathwing - left roll'),
(53879, 3, 0, 'Deathwing rolls right!', 41, 0, 100, 0, 0, 0, 'Spine of Deathwing - right roll'),
(53879, 5, 0, 'Your tenacity is admirable, but pointless! You ride into the jaws of the apocalypse!', 14, 0, 100, 0, 0, 26346, 'Spine of Deathwing - backevent 01'),
(53879, 4, 0, 'Deathwing roars in agony.', 16, 0, 100, 0, 0, 26347, 'Spine of Deathwing - backevent 02'),
(53879, 4, 1, 'Deathwing roars in agony.', 16, 0, 100, 0, 0, 26348, 'Spine of Deathwing - backevent 03'),
(53879, 4, 2, 'Deathwing roars in agony.', 16, 0, 100, 0, 0, 26349, 'Spine of Deathwing - backevent 04'),
(53879, 5, 1, 'You are less than dust, fit only to be brushed from my back.', 14, 0, 100, 0, 0, 26350, 'Spine of Deathwing - slay 01'),
(53879, 5, 2, 'Ha! I didn\'t realize you fools were still there.', 14, 0, 100, 0, 0, 26351, 'Spine of Deathwing - slay 02'),
(53879, 5, 3, 'Your efforts are insignificant! I carry you to your deaths!', 14, 0, 100, 0, 0, 26352, 'Spine of Deathwing - slay 03');

DELETE FROM `locales_creature_text` WHERE `entry`=53879;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(53879, 0, 0, 'Смертокрыл чувствует игроков на своем левом боку.\nОн собирается накрениться влево!'),
(53879, 1, 0, 'Смертокрыл чувствует игроков на своем правом боку.\nОн собирается накрениться вправо!'),
(53879, 2, 0, 'Смертокрыл наклоняется влево!'),
(53879, 3, 0, 'Смертокрыл наклоняется вправо!'),
(53879, 5, 0, 'Ваше упорство ничего не изменит! Апокалипсис ждет вас!'),
(53879, 4, 0, 'Смертокрыл ревет в агонии.'),
(53879, 4, 1, 'Смертокрыл ревет в агонии.'),
(53879, 4, 2, 'Смертокрыл ревет в агонии.'),
(53879, 5, 1, 'Вы лишь пылинки, которые я стряхну со своей спины.'),
(53879, 5, 2, 'Ха! Вы все еще здесь, глупцы? Я вас и не заметил.'),
(53879, 5, 3, 'Ваши усилия ничего не изменят! Я несу вас навстречу смерти!');


REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(106199, 'spine_of_deathwing_blood_corruption_death');

UPDATE `creature_template` SET `Inhabittype`=7 WHERE `entry` IN 
(53890, 56516, 56517, 56518, 53889, 57895, 57896, 57897);

UPDATE `creature_template` SET `dmg_multiplier`=30 WHERE `entry` IN (53889, 57895);
UPDATE `creature_template` SET `dmg_multiplier`=30 WHERE `entry` IN (57896, 57897);

UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822 WHERE `entry` IN (53890, 56516, 56517, 56518);

UPDATE `creature_template` SET `dmg_multiplier`=100 WHERE `entry` IN (53890, 56516);
UPDATE `creature_template` SET `dmg_multiplier`=135 WHERE `entry` IN (56517, 56518);
UPDATE `creature_template_addon` SET `auras`='109247' WHERE `entry` IN (53879, 58862, 58863, 58864);

DELETE FROM `spell_script_names` WHERE `spell_id` IN 
(106199, 106200);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(106199, 'spell_spine_of_deathwing_blood_corruption'),
(106200, 'spell_spine_of_deathwing_blood_corruption'),
(106199, 'spell_spine_of_deathwing_blood_corruption_death');

DELETE FROM `spell_script_names` WHERE `spell_id` IN 
(106199, 106200, 105777);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(106199, 'spell_spine_of_deathwing_blood_corruption'),
(106200, 'spell_spine_of_deathwing_blood_corruption'),
(106199, 'spell_spine_of_deathwing_blood_corruption_death'),
(105777, 'spell_spine_of_deathwing_roll_control');


DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN
(18486, 18502, 18451, 18466, 18467);

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN
(18487, 18658, 18659, 18660, 18661, 18452, 18468, 18469, 18489, 18490, 18491, 18492);

UPDATE `creature` SET `spawnmask`=0 WHERE `map`=967 AND `id` IN 
(56099, 56102, 56100, 56101, 56168, 56846, 56167, 56471, 56173, 57693, 57962, 56188,
58211, 58232, 58210, 58208, 58207, 58209);

UPDATE `gameobject` SET `spawnmask`=0 WHERE `id`=210222;

UPDATE `creature_template_addon` SET `auras`=109247 WHERE `entry` IN
(56341, 57884, 57885, 57886,
56575, 57887, 57888, 57889);

UPDATE `creature_template` SET `ScriptName`='npc_madness_of_deathwing_jump_pad' WHERE `entry`=56699;

DELETE FROM `spell_script_names` WHERE `spell_id` IN
(106666, 106669, 106671, 106673, 106675, 106677);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(106666, 'spell_madness_of_deathwing_carrying_winds_script_1'),
(106669, 'spell_madness_of_deathwing_carrying_winds_script_2'),
(106671, 'spell_madness_of_deathwing_carrying_winds_script_3'),
(106673, 'spell_madness_of_deathwing_carrying_winds_script_4'),
(106675, 'spell_madness_of_deathwing_carrying_winds_script_5'),
(106677, 'spell_madness_of_deathwing_carrying_winds_script_6');

DELETE FROM `conditions` WHERE `SourceEntry` IN 
(105366, 105384);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '2', '105366', '0', '31', '0', '3', '53879', 'Spine of Deathwing - Plate Fly Off Left'),
('13', '2', '105384', '0', '31', '0', '3', '53879', 'Spine of Deathwing - Plate Fly Off Right');

DELETE FROM `creature_text` WHERE `entry`=56173;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(56173, 0, 0, 'You have done NOTHING. I will tear your world APART.', 14, 0, 100, 0, 0, 26527, 'Madness of Deathwing - Aggro'),
(56173, 1, 0, 'There is no hope, no future, all life is doomed!', 14, 0, 100, 0, 0, 26528, 'Madness of Deathwing - Slay 01'),
(56173, 1, 1, 'Know that your world will soon follow!.', 14, 0, 100, 0, 0, 26529, 'Madness of Deathwing - Slay 02'),
(56173, 1, 2, 'Like you, all Azeroth shall burn!', 14, 0, 100, 0, 0, 26530, 'Madness of Deathwing - Slay 03');

DELETE FROM `locales_creature_text` WHERE `entry`=56173;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(56173, 0, 0, 'У вас НИЧЕГО не вышло. Я РАЗОРВУ ваш мир на куски.'),
(56173, 1, 0, 'Нет ни будущего, ни надежды, этот мир обречен!'),
(56173, 1, 1, 'Этот мир ждет такая же участь!'),
(56173, 1, 2, 'Весь Азерот сгорит так же как вы!');

DELETE FROM `creature_text` WHERE `entry`=56103;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(56103, 0, 0, 'It is done; at last the Destroyer has met his end. Now, we can begin to heal our world.', 14, 0, 100, 0, 0, 25901, 'Thrall - Maelstorm 01'),
(56103, 1, 0, 'Only you can give us our opening!', 14, 0, 100, 0, 0, 25902, 'Thrall - Maelstorm 02'),
(56103, 2, 0, 'Avenge the fallen! Continue the fight!', 14, 0, 100, 0, 0, 25903, 'Thrall - Maelstorm Slay 01');

DELETE FROM `locales_creature_text` WHERE `entry`=56103;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(56103, 0, 0, 'Свершилось. Смертокрыл повержен. Теперь можно заняться воccтановлением этого мира.'),
(56103, 1, 0, 'Только вы можете дать нам шанс нанести ему удар!'),
(56103, 2, 0, 'Сражайтесь! Отомстите за павших!');
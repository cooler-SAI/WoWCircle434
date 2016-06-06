
DELETE FROM `creature` WHERE `map`=568 AND `id` IN (52850, 24187, 52839, 24136);
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN (24187, 52839, 24136);

UPDATE `creature_template` SET `Scriptname`='boss_daakara' WHERE `entry`=23863;
UPDATE `creature_template` SET `flags_extra`=128, `Scriptname`='npc_daakara_vortex' WHERE `entry`=24136;
UPDATE `creature_template` SET `Scriptname`='npc_daakara_amani_lynx' WHERE `entry`=52839;
UPDATE `creature_template` SET `flags_extra`=128, `Scriptname`='npc_daakara_pillar_of_flame' WHERE `entry`=24187;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (42583, 97647);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(42583, 'spell_daakara_claw_rage_charge'),
(97647, 'spell_daakara_sweeping_winds');

DELETE FROM `creature_text` WHERE `entry`=23863;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(23863, 0, 0, 'Got me some new tricks... like me bruddah bear!', 14, 0, 0, 0, 0, 24220, 'Daakara bear'),
(23863, 1, 0, 'Mebbe me fall... but da Amani empire... never going to die.', 14, 0, 0, 0, 0, 24222, 'Daakara death'),
(23863, 2, 0, 'You don\' have to look to da sky to see da dragonhawk!', 14, 0, 0, 0, 0, 24223, 'Daakara dragonhawk'),
(23863, 3, 0, 'Dere be no hidin\' from da eagle!', 14, 0, 0, 0, 0, 24224, 'Daakara eagle'),
(23863, 4, 0, 'Fire kill you just as quick!', 14, 0, 0, 0, 0, 24225, 'Daakara fire breath'),
(23863, 5, 0, 'Da Amani de chuka!', 14, 0, 0, 0, 0, 24226, 'Daakara kill 1'),
(23863, 5, 1, 'Lot more gonna fall like you!', 14, 0, 0, 0, 0, 24227, 'Daakara kill2'),
(23863, 5, 2, 'Oh, look you... dead.', 14, 0, 0, 0, 0, 24228, 'Daakara kill 3'),
(23863, 6, 0, 'Let me introduce you to me new bruddas: fang and claw!', 14, 0, 0, 0, 0, 24230, 'Daakara lynx'),
(23863, 7, 0, 'De Zandalari give us strength. Nobody push around de Amani no more!', 14, 0, 0, 0, 0, 24353, 'Daakara aggro');

DELETE FROM `locales_creature_text` WHERE `entry`=23863;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(23863, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Сейчас-сейчас. Выучил вот пару новых фокусов... вместе с братишкой-медведем.'),
(23863, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Я могу умереть... но империя Амани... никогда... не падет...'),
(23863, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Не нужно смотреть в небо, чтобы увидеть дракондора!'),
(23863, 3, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Никому не спрятаться от орла!'),
(23863, 4, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Огонь вас быстро убьет!'),
(23863, 5, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Да Амани ди чука!'),
(23863, 5, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'За тобой последуют тысячи.'),
(23863, 5, 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ух ты... мертвец!'),
(23863, 6, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Знакомьтесь, мои новые братишки: клык и коготь!!'),
(23863, 7, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Зандалары делают нас сильнее. Больше никто не посмеет обижать Амани!');
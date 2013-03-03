UPDATE `creature_template` SET `unit_class` = 4, `ScriptName` = 'boss_majordomo_staghelm' WHERE `entry` = 52571;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (98451,98450);
INSERT INTO `spell_script_names` VALUES
(98451, 'spell_staghelm_burning_orbs'),
(98450, 'spell_staghelm_searing_seeds_aura');

DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (98374,98379);
INSERT INTO `spell_linked_spell` VALUES
(98374, -98379, 0, 'Cat Form - removes Scorpion Form'),
(98379, -98374, 0, 'Scorpion Form - removes Cat Form');

DELETE FROM `creature_text` WHERE `entry` = 52571;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(52571, 0, 0, "The master's power takes on many forms ...", 14, 0, 100, 0, 0, 24483, 'Majordomo Staghelm - SAY_TRANSFORM_1'),
(52571, 1, 0, "Behold the rage of the Firelands!", 14, 0, 100, 0, 0, 24485, 'Majordomo Staghelm - SAY_TRANSFORM_2'),
(52571, 2, 0, "Blaze of Glory!", 14, 0, 100, 0, 0, 24472, 'Majordomo Staghelm - SAY_HUMANOID_1'),
(52571, 3, 0, "Nothing but ash!", 14, 0, 100, 0, 0, 24478, 'Majordomo Staghelm - SAY_HUMANOID_2'),
(52571, 4, 0, "My studies... had only just begun...", 14, 0, 100, 0, 0, 24471, 'Majordomo Staghelm - SAY_DEATH'),
(52571, 5, 0, "So much power!", 14, 0, 100, 0, 0, 24480, 'Majordomo Staghelm - SAY_BERSERK'),
(52571, 6, 0, "Burn.", 14, 0, 100, 0, 0, 24477, 'Majordomo Staghelm - SAY_KILL_1'),
(52571, 6, 1, "Soon ALL of Azeroth will burn!", 14, 0, 100, 0, 0, 24479, 'Majordomo Staghelm - SAY_KILL_2'),
(52571, 6, 2, "You stood in the fire!", 14, 0, 100, 0, 0, 24481, 'Majordomo Staghelm - SAY_KILL_3');

DELETE FROM `locales_creature_text` WHERE `entry` = 52571;
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(52571, 0, 0, 'Сила моего хозяина может принимать разные формы ...'),
(52571, 1, 0, 'Узрите ярость Огненных Просторов!'),
(52571, 2, 0, 'Слава огню!'),
(52571, 3, 0, 'Ты станешь пеплом!'),
(52571, 4, 0, 'Я только началь постигать суть ...'),
(52571, 5, 0, 'Сколько силы!'),
(52571, 6, 0, 'Гори!'),
(52571, 6, 1, 'Скоро весь Азерот запылает!'),
(52571, 6, 2, 'Не надо было стоять в огне!');

UPDATE `creature_template` SET `mechanic_immune_mask` = 650854271 WHERE `entry` IN (52571,53856,53857,53858);

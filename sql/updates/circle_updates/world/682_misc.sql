

DELETE FROM `creature` WHERE `map`=859 AND `id` IN (52288, 52525, 52197, 52288, 52302);
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN (52288, 52525, 52197, 52288, 52302);

UPDATE `creature` SET `position_x`=-12020.12, `position_y`=-1699.96, `position_z`=39.54, `orientation`=0.60 WHERE `id`=52155;
UPDATE `gameobject_template` SET `flags`=4 WHERE `entry`=208844;

UPDATE `creature_template` SET `flags_extra`=2 WHERE `entry` IN (52331, 52332);

UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, flags_extra=128 WHERE `entry`=52302;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, flags_extra=128 WHERE `entry`=52288;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, flags_extra=128 WHERE `entry`=52525;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, flags_extra=128 WHERE `entry`=52197;

UPDATE `access_requirement` SET `level_min`=85 WHERE  `mapId`=859 AND `difficulty`=1;
UPDATE `access_requirement` SET `level_min`=86 WHERE  `mapId`=859 AND `difficulty`=0;

UPDATE `instance_template` SET `script`='instance_zulgurub' WHERE  `map`=859;

UPDATE `creature_template` SET `ScriptName`='boss_venoxis' WHERE `entry`=52155;
UPDATE `creature_template` SET `ScriptName`='npc_venoxis_venomous_effusion_stalker' WHERE `entry`=52302;
UPDATE `creature_template` SET `ScriptName`='npc_venoxis_venomous_effusion' WHERE `entry`=52288;
UPDATE `creature_template` SET `ScriptName`='npc_venoxis_bloodvenom' WHERE `entry`=52525;
UPDATE `creature_template` SET `ScriptName`='npc_venoxis_venoxis_pool_of_acid_tears' WHERE `entry`=52197;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (96475, 96477, 96521, 97089);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(96475, 'spell_venoxis_toxic_link'),
(96477, 'spell_venoxis_toxic_link_aura'),
(96521, 'spell_venoxis_pool_of_acid_tears_dmg'),
(97089, 'spell_venoxis_pool_of_acid_tears_dmg');

DELETE FROM `creature_text` WHERE `entry`=52155;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(52155, 0, 0, 'My death means... nothing...', 14, 0, 0, 0, 0, 24318, 'Venoxis death'),
(52155, 1, 0, 'Let the coils of death unfurl!', 14, 0, 0, 0, 0, 24319, 'Venoxis event 01'),
(52155, 2, 0, 'Yes... ssssuccumb to the venom...', 14, 0, 0, 0, 0, 24320, 'Venoxis event 02'),
(52155, 3, 0, 'Hisss word, FILLS me, MY BLOOD IS VENOM, AND YOU WILL BATHE IN THE GLORY OF THE SNAKE GOD!!!', 14, 0, 0, 0, 0, 24321, 'Venoxis event 03'),
(52155, 4, 0, 'The mortal coil unwindsss...', 14, 0, 0, 0, 0, 24322, 'Venoxis kill 01'),
(52155, 4, 1, 'Your sssacrifice pleases him.', 14, 0, 0, 0, 0, 24323, 'Venoxis kill 02'),
(52155, 5, 0, 'You dissssturb the plans of Gurubashi, little one. It\'sss to late for you. Too late for all of you!', 14, 0, 0, 0, 0, 24326, 'Venoxis quest event 01');

DELETE FROM `locales_creature_text` WHERE `entry`=52155;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(52155, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Моя сссссмерть... ничего... не изменит...'),
(52155, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Кольца сссмерти сожмутся вокруг вассс!'),
(52155, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Пропитайтесссь ядом...'),
(52155, 3, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Он превращает мою кровь в яд. Почувствуйте мощь змеиного бога!'),
(52155, 4, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Посссследний виток твоей жизни!'),
(52155, 4, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ему нравитссся твоя жертва...'),
(52155, 5, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы нарушаете планы Гурубаши, коротышки. Но уже поздно. Сссслишком поздно!');


-- /run PlaySoundFile("Sound\\Creature\\Venoxis\\vo_zg2_venoxis_quest_event_01.wav");
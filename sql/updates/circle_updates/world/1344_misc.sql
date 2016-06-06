

DELETE FROM `creature` WHERE `id`=53495;

UPDATE `creature` SET `spawnmask`=15 WHERE `id`=53494;

REPLACE INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(53495, 0, 0, 0, 0, 0, '99258'),
(54145, 0, 0, 0, 0, 0, '97699 98250 100746');

UPDATE `creature_model_info` SET `bounding_radius`=10, `combat_reach`=10 WHERE `modelid`=38621;

UPDATE `creature_template` SET `minlevel`=88, `maxlevel`=88 WHERE `entry`=53495;

UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=97 WHERE `entry` IN (53494, 53587);
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=125 WHERE `entry` IN (53588, 53589);

UPDATE `creature_template` SET `ScriptName`='boss_baleroc' WHERE `entry`=53494;
UPDATE `creature_template` SET `ScriptName`='npc_baleroc_shard_of_torment' WHERE `entry`=53495;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_molten_flamefather' WHERE `entry`=54143;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_magma_conduit' WHERE `entry`=54145;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_magmakin' WHERE `entry`=54144;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (99259, 99489, 99515, 99517);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(99259, 'spell_baleroc_shards_of_torment_aoe'),
(99489, 'spell_baleroc_tormented_aoe'),
(99515, 'spell_baleroc_final_countdown'),
(99517, 'spell_baleroc_final_countdown_script');

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (17577, 18100);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(17577, 11, 0, 0, 'achievement_share_the_pain');

DELETE FROM `creature_text` WHERE `entry`=53494;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(53494, 0, 0, 'You are forbidden from my master\'s domain, mortals.', 14, 0, 0, 0, 0, 24441, 'Baleroc aggro'),
(53494, 1, 0, 'Mortal filth... the master\'s keep is forbidden...', 14, 0, 0, 0, 0, 24444, 'Baleroc death'),
(53494, 2, 0, 'Fool mortals. Hurl yourselves into your own demise!', 14, 0, 0, 0, 0, 24446, 'Baleroc event 02'),
(53494, 3, 0, 'By the Firelord\'s command, you, too, shall perish!', 14, 0, 0, 0, 0, 24447, 'Baleroc event03'),
(53494, 4, 0, 'You have been judged.', 14, 0, 0, 0, 0, 24449, 'Baleroc kill 02'),
(53494, 5, 0, 'Your flesh is forfeit to the fires of this realm.', 14, 0, 0, 0, 0, 24450, 'Baleroc kill 03'),
(53494, 4, 1, 'Behold your weakness.', 14, 0, 0, 0, 0, 24451, 'Baleroc kill 04'),
(53494, 4, 2, 'None shall pass!', 14, 0, 0, 0, 0, 24452, 'Baleroc kill 05'),
(53494, 6, 0, 'Burn beneath my molten fury!', 14, 0, 0, 0, 0, 24459, 'Baleroc quest 03');

DELETE FROM `locales_creature_text` WHERE `entry`=53494;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(53494, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У вас нет права находиться во владениях моего хозяина, смертные.'),
(53494, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ничтожные твари... Владения моего хозяина закрыты для вас...'),
(53494, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Глупцы. Бегите же навстречу смерти.'),
(53494, 3, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'По приказу Повелителя Огня вы все умрете!'),
(53494, 4, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ты ни на что не годишься.'),
(53494, 5, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Твоя плоть питает наше пламя.'),
(53494, 4, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Осознай свою слабость.'),
(53494, 4, 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Никто не пройдет.'),
(53494, 6, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Горите в огне моей ярости.');
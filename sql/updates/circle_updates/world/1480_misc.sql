UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=45455;
DELETE FROM `conditions` WHERE `SourceEntry` IN (86911, 87517);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '86911', '0', '31', '0', '3', '46492', 'Asaad - Grounding field visual beams'),
('13', '1', '87517', '0', '31', '0', '3', '46492', 'Asaad - Grounding field visual beams');

UPDATE `creature` SET `spawnmask`=0 WHERE `id`=42895 AND `map`=657;

UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=46492;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=45455 AND `spell_id`=87742;

UPDATE `creature_template` SET `npcflag`=0, `unit_flags`=0 WHERE `entry`=45455;
DELETE FROM `creature_addon` WHERE `guid`=332156;
DELETE FROM `creature_template_addon` WHERE `entry`=43873;
DELETE FROM `creature_template_addon` WHERE `entry`=43874;

DELETE FROM `creature_text` WHERE `entry`=43878;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43878, 0, 0, 'Filthy beasts! Your presence in Skywall will not be tolerated!', 14, 0, 0, 0, 0, 20876, 'VO_VP_Ertan_Event01');
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43878, 1, 0, 'Ravaged!', 14, 0, 0, 0, 0, 20877, 'VO_VP_Ertan_Event02');
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43878, 2, 0, 'AHHHHHHHHH!', 14, 0, 0, 0, 0, 20878, 'VO_VP_Ertan_Event03');

DELETE FROM `locales_creature_text` WHERE `entry`=43878;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(43878, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Грязные животные! Мы не потерпим вашего присутствия в Небесной выси!'),
(43878, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Умри!');

DELETE FROM `creature_text` WHERE `entry`=43875;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43875, 0, 0, 'YOU tread upon the sacrosanct! Mortals have no place amidst the clouds.', 14, 0, 0, 0, 0, 20867, 'VO_VP_AsimAlAkir_Event01');
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43875, 1, 0, 'BACK to the earth with you!', 14, 0, 0, 0, 0, 20868, 'VO_VP_AsimAlAkir_Event02');
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43875, 2, 0, 'Al\'Akir, your servant calls for aid!', 14, 0, 0, 0, 0, 20869, 'VO_VP_AsimAlAkir_Event03');
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) 
VALUES (43875, 3, 0, 'The winds take me!', 14, 0, 0, 0, 0, 20870, 'VO_VP_AsimAlAkir_Event04');

DELETE FROM `locales_creature_text` WHERE `entry`=43875;
INSERT INTO `locales_creature_text` (`entry`, `textgroup`, `id`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`) VALUES
(43875, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы попрали святая святых! Смертным не место в небесных чертогах!'),
(43875, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Назад, в землю, из которой ты вышел!'),
(43875, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ал\'акир, твой слуга взывает о помощи!'),
(43875, 3, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ветра приняли меня!');
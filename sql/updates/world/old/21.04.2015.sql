UPDATE `quest_template` SET `SourceItemId` = 0 WHERE `Id` = 14238;

DELETE FROM `gameobject_template` WHERE `entry` = 195704; 
INSERT INTO `gameobject_template` VALUES (195704, 10, 9132, 'Arcane Cage', '', '', '', 35, 32, 1.2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 15595);

UPDATE `gameobject_template` SET `ScriptName` = 'gob_red_but' WHERE `entry` = 196439;
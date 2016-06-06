UPDATE `gameobject_template` SET `data3` = 10800000 WHERE `entry` = 201741;
DELETE FROM `gameobject` WHERE `id`=201741;
INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
('201741','631','15','1','4573.8','2854.84','384.684','1.93731','0','0','0','1','6000','100','1'),
('201741','631','15','1','4522.76','2769.22','351.101','-3.10665','0','0','0','1','6000','100','1'),
('201741','631','15','1','4574.18','2683.47','384.684','-0.593412','0','0','0','1','6000','100','1');

DELETE FROM `spell_script_names` WHERE `spell_id` IN (72385,72441,72442,72443);
INSERT INTO `spell_script_names` VALUES
(72385, 'spell_deathbringer_boiling_blood'),
(72441, 'spell_deathbringer_boiling_blood'),
(72442, 'spell_deathbringer_boiling_blood'),
(72443, 'spell_deathbringer_boiling_blood');

UPDATE `creature_template` SET `mindmg` = 509, `maxdmg` = 683, `attackpower` = 805, `baseattacktime` = 1000, `rangeattacktime` = 1000, `speed_walk` = 1.6, `speed_run` = 1.42857 WHERE `entry` IN (37813,38402,38582,38583);
UPDATE `creature_template` SET `dmg_multiplier` = 60 WHERE `entry` = 37813;
UPDATE `creature_template` SET `dmg_multiplier` = 90 WHERE `entry` = 38402;
UPDATE `creature_template` SET `dmg_multiplier` = 85 WHERE `entry` = 38582;
UPDATE `creature_template` SET `dmg_multiplier` = 120 WHERE `entry` = 38583;



SET @MEMORIAL := 202443;
SET @Script := 20244300;
UPDATE `gameobject_template` SET `data2`=0, `data7`=0, `data19`=11431, `AIName` = 'SmartGameObjectAI' WHERE `entry` = @MEMORIAL;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (@MEMORIAL);
DELETE FROM `smart_scripts` WHERE `source_type`=9 AND `entryorguid`=@Script;
INSERT INTO `smart_scripts` VALUES
(@MEMORIAL,1,0,0,62,0,100,0,11431,0,0,0,80,@Script,0,0,0,0,0,1,0,0,0,0,0,0,0,'Memorial - On gossip select - Run Script'),
(@Script,9,0,0,0,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Actionlist - On action 0 - Close gossip'),
(@Script,9,1,0,0,0,100,0,0,0,0,0,68,16,0,0,0,0,0,7,0,0,0,0,0,0,0,'Actionlist - On action 1 - Startmovie');
DELETE FROM `conditions` WHERE `SourceEntry` =202443;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(22,1,202443,1,0,17,0,4530,0,0,0,0, '', 'Memorial - Need The Frozen Throne achievement to play movie');


UPDATE `gameobject_template` SET `data2`=10, `data5`=8 WHERE `entry`=193963;
DELETE FROM `spell_script_names` WHERE `spell_id`=61551;
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(61551, 'spell_gen_toy_train_set');

UPDATE `creature_template` SET `ScriptName` = 'npc_train_wrecker' WHERE `entry` = 33404; 

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry` = 62943; 
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceEntry`,`ConditionTypeOrReference`,`ConditionValue2`,`Comment`) VALUES 
(13, 62943, 18, 193963, 'Wind-Up Train Wrecker - target Toy Train Set');

UPDATE `creature_template` SET `ScriptName` = 'npc_mini_tyrael' WHERE `entry` = 29089;
UPDATE `creature_template` SET `ScriptName` = 'npc_pengu' WHERE `entry` = 32595;


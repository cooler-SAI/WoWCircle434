DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_ultraxion_last_defender_of_azeroth';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES  
(106182, 'spell_ultraxion_last_defender_of_azeroth');

DELETE FROM `creature_text` WHERE `entry`=56665;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(56665, 0, 0, 'I sense a great disturbance in the balance approaching. The chaos of it burns my mind!', 12, 0, 100, 0, 0, 26148, 'Ysera pre ultraxion'),
(56665, 1, 0, 'In dreams, we may overcome any obstacle.', 14, 0, 100, 0, 0, 26149, 'Ysera ultraxion');

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (18463, 18462, 18484, 18449, 18391);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(18391, 11, 0, 0, 'achievement_minutes_to_midnight');

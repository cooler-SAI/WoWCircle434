UPDATE `creature_template` SET `spell1`=96212 WHERE `entry`=27894;
UPDATE `creature_template` SET `spell1`=96212 WHERE `entry`=32795;

DELETE FROM `disables` WHERE `sourceType`=0 AND `entry`=86425;
DELETE FROM `conditions` WHERE `SourceEntry`=86425;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 86425, 0, 0, 31, 1, 3, 46393, 0, 0, 0, '', 'Billy Goat Blaster - Billy Goat Blast');

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(18222, 0, 0, 0, 0, 'Poached Sunscale Salmon - Health Regeneration');

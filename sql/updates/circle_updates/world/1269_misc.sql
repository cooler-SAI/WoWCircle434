UPDATE `creature_template` SET `unit_flags`=512 WHERE `entry`=41251;
DELETE FROM `conditions` WHERE `SourceEntry`=93773;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 93773, 0, 0, 31, 1, 3, 41251, 0, 0, 0, '', 'Viewpoint Equalizer - Equalize Viewpoint');
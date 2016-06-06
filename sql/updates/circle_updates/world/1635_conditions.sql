DELETE FROM `conditions` WHERE `SourceEntry`=77682;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 77682, 0, 0, 31, 1, 3, 41520, 0, 0, 0, '', 'Duarn\'s Net - Hurl Net');

DELETE FROM `conditions` WHERE `SourceEntry`=71775;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 71775, 0, 0, 31, 1, 3, 9162, 0, 0, 0, '', 'Scarps of Rotting Meat - Throw Meat'),
(17, 0, 71775, 0, 1, 31, 1, 3, 9163, 0, 0, 0, '', 'Scarps of Rotting Meat - Throw Meat'),
(17, 0, 71775, 0, 2, 31, 1, 3, 9164, 0, 0, 0, '', 'Scarps of Rotting Meat - Throw Meat');
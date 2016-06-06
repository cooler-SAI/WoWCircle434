DELETE FROM `conditions` WHERE `SourceEntry`=101883;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 101883, 0, 0, 31, 1, 3, 54518, 0, 0, 0, '', 'Darkmoon Bandage - Heal Injured Carnie');

DELETE FROM `spell_ranks` WHERE `first_spell_id` IN (12880, 57518);

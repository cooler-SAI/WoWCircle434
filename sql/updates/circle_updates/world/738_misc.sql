DELETE FROM `conditions` WHERE `SourceEntry`=89821;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 89821, 0, 0, 31, 1, 3, 48269, 0, 0, 0, '', 'Hercular\'s Rod');

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(56641, 'spell_quests_spell_hit');
UPDATE `creature_template` SET `unit_flags`=33554436, `dynamicflags`=8, `flags_extra`=2, `ScriptName`='npc_power_word_barrier' WHERE `entry`=33248;

DELETE FROM `conditions` WHERE `SourceEntry`=74409;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '74409', '0', '31', '0',  '3', '39834', 'Ysondre\'s Tear - Seal Dream Portal');

DELETE FROM `spell_proc_event` WHERE `entry` IN (70579, 70578, 70577);
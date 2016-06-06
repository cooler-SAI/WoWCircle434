DELETE FROM `disables` WHERE `sourceType`=0 AND `entry`=92734;

UPDATE `creature_template` SET `dynamicflags`=`dynamicflags`|8 WHERE `entry` IN (49680, 49848);

REPLACE INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
(13, 3, 92734, 0, 31, 0, 3, 49683, 'Exploding Stuff');

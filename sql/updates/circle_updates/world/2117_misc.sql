DELETE FROM `conditions` WHERE `SourceEntry` IN 
(83603, 83611, 84092, 83601, 83609);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
(13, 1, 83603, 0, 31, 0, 3, 44600, 'Halfus Wyrmbreaker - Stone Grip'),
(13, 7, 83611, 0, 31, 0, 3, 44600, 'Halfus Wyrmbreaker - Nether Blindness'),
(13, 1, 84092, 0, 31, 0, 3, 44600, 'Halfus Wyrmbreaker - Cyclone Winds'),
(13, 1, 83601, 0, 31, 0, 3, 44687, 'Halfus Wyrmbreaker - Time Dilation'),
(13, 1, 83609, 0, 31, 0, 3, 44687, 'Halfus Wyrmbreaker - Atrothic Poison');

UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry` IN (44687, 46227, 46228, 46229);

UPDATE `creature_template` SET `npcflag`=0, `unit_flags`=0 WHERE `entry` IN
(44652, 46224, 46225, 46226,
44645, 46212, 46213, 46214,
44650, 46218, 46219, 46220,
44797, 46215, 46216, 46217,
44641, 46221, 46222, 46223);

DELETE FROM `creature_template_addon` WHERE `entry` IN
(44652, 46224, 46225, 46226,
44645, 46212, 46213, 46214,
44650, 46218, 46219, 46220,
44797, 46215, 46216, 46217,
44641, 46221, 46222, 46223);

UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN (44600, 46209, 46210, 46211);

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry` IN
(44652, 46224, 46225, 46226,
44645, 46212, 46213, 46214,
44650, 46218, 46219, 46220,
44797, 46215, 46216, 46217,
44641, 46221, 46222, 46223,
44600, 46209, 46210, 46211);

UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822 WHERE `entry` IN 
(44600, 46209, 46210, 46211);
UPDATE `creature_template` SET `dmg_multiplier`=97 WHERE `entry` IN
(44600, 46209);
UPDATE `creature_template` SET `dmg_multiplier`=110 WHERE `entry` IN
(46210, 46211);

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (15472, 16025);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `ScriptName`) VALUES 
(15472, 11, 'achievement_i_cant_hear_you_over_the_sound_of_how_awesome_i_am');

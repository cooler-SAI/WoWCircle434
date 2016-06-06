DELETE FROM `conditions` WHERE `SourceEntry` IN (75657, 94970);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
(13, 3, 75657, 0, 31, 0, 3, 39731, 'Ammunae - Energize'),
(13, 3, 94970, 0, 31, 0, 3, 39731, 'Ammunae - Energize');
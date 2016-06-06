DELETE FROM `conditions` WHERE `SourceEntry`=57912;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
(13, 1, 57912, 0, 31, 0, 3, 30661, 'Violet Hold - Defense System'),
(13, 1, 57912, 1, 31, 0, 3, 30961, 'Violet Hold - Defense System'),
(13, 1, 57912, 2, 31, 0, 3, 30662, 'Violet Hold - Defense System'),
(13, 1, 57912, 3, 31, 0, 3, 30962, 'Violet Hold - Defense System'),
(13, 1, 57912, 4, 31, 0, 3, 30663, 'Violet Hold - Defense System'),
(13, 1, 57912, 5, 31, 0, 3, 30918, 'Violet Hold - Defense System'),
(13, 1, 57912, 6, 31, 0, 3, 30664, 'Violet Hold - Defense System'),
(13, 1, 57912, 7, 31, 0, 3, 30963, 'Violet Hold - Defense System'),
(13, 1, 57912, 8, 31, 0, 3, 30666, 'Violet Hold - Defense System'),
(13, 1, 57912, 9, 31, 0, 3, 30667, 'Violet Hold - Defense System'),
(13, 1, 57912, 10, 31, 0, 3, 30668, 'Violet Hold - Defense System'),
(13, 1, 57912, 11, 31, 0, 3, 32191, 'Violet Hold - Defense System');

UPDATE `creature_template` SET `Inhabittype`=7 WHERE `entry`=30837;
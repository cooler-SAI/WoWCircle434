UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=135 WHERE `entry` IN
(55265, 57409, 57773, 57774, 55312, 55313, 55308, 55309, 55689, 57462, 55294, 56576, 56427, 57699);
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=155 WHERE `entry` IN
(57771, 57772, 56577, 56578, 55310, 55311, 55314, 55315, 57955, 57956, 57847, 57848, 57995, 57996);

DELETE FROM `creature_loot_template` WHERE `entry`=56427;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(56427, 71998, 35, 1, 0, 1, 3),
(56427, 77952, -100, 1, 0, 1, 1),
(56427, 77202, 100, 1, 0, -77202, 1),
(56427, 78172, 100, 1, 0, -78172, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=57699;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57699, 71998, 55, 1, 0, 1, 3),
(57699, 77952, -100, 1, 0, 1, 3),
(57699, 77202, 100, 1, 0, -77202, 4),
(57699, 78172, 100, 1, 0, -78172, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=57847;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57847, 71998, 35, 1, 0, 1, 3),
(57847, 77952, -100, 1, 0, 1, 1),
(57847, 77993, 100, 1, 0, -77993, 1),
(57847, 78852, 100, 1, 0, -78852, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=57848;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57848, 71998, 35, 1, 0, 1, 3),
(57848, 77952, -100, 1, 0, 1, 1),
(57848, 77993, 100, 1, 0, -77993, 4),
(57848, 78852, 100, 1, 0, -78852, 2);

DELETE FROM `reference_loot_template` WHERE `entry`=77202;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77202, 77202, 12, 1, 1, 1, 1), 
(77202, 77224, 11, 1, 1, 1, 1), 
(77202, 77225, 11, 1, 1, 1, 1), 
(77202, 77241, 10, 1, 1, 1, 1), 
(77202, 77240, 10, 1, 1, 1, 1), 
(77202, 77226, 9, 1, 1, 1, 1), 
(77202, 77239, 9, 1, 1, 1, 1), 
(77202, 77234, 9, 1, 1, 1, 1),
(77202, 77227, 9, 1, 1, 1, 1),
(77202, 77207, 1, 1, 1, 1, 1), 
(77202, 77209, 1, 1, 1, 1, 1), 
(77202, 77211, 1, 1, 1, 1, 1),
(77202, 77228, 1, 1, 1, 1, 1),
(77202, 77230, 1, 1, 1, 1, 1), 
(77202, 77232, 1, 1, 1, 1, 1), 
(77202, 77208, 1, 1, 1, 1, 1), 
(77202, 77210, 1, 1, 1, 1, 1), 
(77202, 77229, 1, 1, 1, 1, 1), 
(77202, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=77993;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77993, 77993, 12, 1, 1, 1, 1), 
(77993, 78445, 11, 1, 1, 1, 1), 
(77993, 78447, 11, 1, 1, 1, 1), 
(77993, 78451, 10, 1, 1, 1, 1), 
(77993, 78448, 10, 1, 1, 1, 1), 
(77993, 78449, 9, 1, 1, 1, 1), 
(77993, 78446, 9, 1, 1, 1, 1), 
(77993, 78450, 9, 1, 1, 1, 1),
(77993, 78452, 9, 1, 1, 1, 1),
(77993, 78001, 1, 1, 1, 1, 1),
(77993, 78003, 1, 1, 1, 1, 1),
(77993, 78489, 1, 1, 1, 1, 1),
(77993, 78490, 1, 1, 1, 1, 1),
(77993, 78492, 1, 1, 1, 1, 1),
(77993, 77999, 1, 1, 1, 1, 1),
(77993, 78000, 1, 1, 1, 1, 1),
(77993, 78002, 1, 1, 1, 1, 1),
(77993, 78491, 1, 1, 1, 1, 1),
(77993, 78493, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78172;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78172, 78172, 40, 1, 1, 1, 1),
(78172, 78182, 30, 1, 1, 1, 1),
(78172, 78177, 30, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78852;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78852, 78852, 40, 1, 1, 1, 1),
(78852, 78850, 30, 1, 1, 1, 1),
(78852, 78851, 30, 1, 1, 1, 1);

UPDATE `creature_template` SET `lootid`=56427 WHERE `entry`=56427;
UPDATE `creature_template` SET `lootid`=57699 WHERE `entry`=57699;
UPDATE `creature_template` SET `lootid`=57847 WHERE `entry`=57847;
UPDATE `creature_template` SET `lootid`=57848 WHERE `entry`=57848;
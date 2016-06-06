DELETE FROM `creature_loot_template` WHERE `entry`=55308;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55308, 71998, 35, 1, 0, 1, 3),
(55308, 77952, -100, 1, 0, 1, 1),
(55308, 77204, 100, 1, 0, -77204, 1),
(55308, 78173, 100, 1, 0, -78173, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=55309;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55309, 71998, 55, 1, 0, 1, 3),
(55309, 77952, -100, 1, 0, 1, 3),
(55309, 77204, 100, 1, 0, -77204, 4),
(55309, 78173, 100, 1, 0, -78173, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=55310;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55310, 71998, 35, 1, 0, 1, 3),
(55310, 77952, -100, 1, 0, 1, 1),
(55310, 77989, 100, 1, 0, -77989, 1),
(55310, 78855, 100, 1, 0, -78855, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=55311;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55311, 71998, 35, 1, 0, 1, 3),
(55311, 77952, -100, 1, 0, 1, 1),
(55311, 77989, 100, 1, 0, -77989, 4),
(55311, 78855, 100, 1, 0, -78855, 2);

DELETE FROM `reference_loot_template` WHERE `entry`=77204;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77204, 77204, 14, 1, 1, 1, 1), 
(77204, 77255, 12, 1, 1, 1, 1), 
(77204, 77260, 11, 1, 1, 1, 1), 
(77204, 77216, 11, 1, 1, 1, 1), 
(77204, 77258, 11, 1, 1, 1, 1), 
(77204, 77215, 11, 1, 1, 1, 1), 
(77204, 77259, 11, 1, 1, 1, 1), 
(77204, 77257, 9, 1, 1, 1, 1), 
(77204, 77207, 1, 1, 1, 1, 1), 
(77204, 77209, 1, 1, 1, 1, 1), 
(77204, 77211, 1, 1, 1, 1, 1),
(77204, 77228, 1, 1, 1, 1, 1),
(77204, 77230, 1, 1, 1, 1, 1), 
(77204, 77232, 1, 1, 1, 1, 1), 
(77204, 77208, 1, 1, 1, 1, 1), 
(77204, 77210, 1, 1, 1, 1, 1), 
(77204, 77229, 1, 1, 1, 1, 1), 
(77204, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=77989;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77989, 77989, 14, 1, 1, 1, 1),
(77989, 78388, 12, 1, 1, 1, 1),
(77989, 78391, 11, 1, 1, 1, 1),
(77989, 78392, 11, 1, 1, 1, 1),
(77989, 78390, 11, 1, 1, 1, 1),
(77989, 78387, 11, 1, 1, 1, 1),
(77989, 78389, 11, 1, 1, 1, 1),
(77989, 78393, 9, 1, 1, 1, 1),
(77989, 78001, 1, 1, 1, 1, 1),
(77989, 78003, 1, 1, 1, 1, 1),
(77989, 78489, 1, 1, 1, 1, 1),
(77989, 78490, 1, 1, 1, 1, 1),
(77989, 78492, 1, 1, 1, 1, 1),
(77989, 77999, 1, 1, 1, 1, 1),
(77989, 78000, 1, 1, 1, 1, 1),
(77989, 78002, 1, 1, 1, 1, 1),
(77989, 78491, 1, 1, 1, 1, 1),
(77989, 78493, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78173;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78173, 78173, 40, 1, 1, 1, 1),
(78173, 78178, 30, 1, 1, 1, 1),
(78173, 78183, 30, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78855;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78855, 78855, 40, 1, 1, 1, 1),
(78855, 78854, 30, 1, 1, 1, 1),
(78855, 78853, 30, 1, 1, 1, 1);

UPDATE `creature_template` SET `lootid`=55308 WHERE `entry`=55308;
UPDATE `creature_template` SET `lootid`=55309 WHERE `entry`=55309;
UPDATE `creature_template` SET `lootid`=55310 WHERE `entry`=55310;
UPDATE `creature_template` SET `lootid`=55311 WHERE `entry`=55311;
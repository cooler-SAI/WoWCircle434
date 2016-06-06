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
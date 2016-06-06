DELETE FROM `creature_loot_template` WHERE `entry`=55265;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55265, 71998, 35, 1, 0, 1, 3),
(55265, 77952, -100, 1, 0, 1, 1),
(55265, 77212, 100, 1, 0, -77212, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=57409;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57409, 71998, 35, 1, 0, 1, 3),
(57409, 77952, -100, 1, 0, 1, 3),
(57409, 77212, 100, 1, 0, -77212, 6);

DELETE FROM `creature_loot_template` WHERE `entry`=57771;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57771, 71998, 35, 1, 0, 1, 3),
(57771, 77952, -100, 1, 0, 1, 1),
(57771, 78371, 100, 1, 0, -78371, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=57772;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57772, 71998, 35, 1, 0, 1, 3),
(57772, 77952, -100, 1, 0, 1, 3),
(57772, 78371, 100, 1, 0, -78371, 6);

DELETE FROM `reference_loot_template` WHERE `entry`=77212;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77212, 77212, 7, 1, 1, 1, 1),
(77212, 77213, 7, 1, 1, 1, 1),
(77212, 77214, 7, 1, 1, 1, 1),
(77212, 77261, 7, 1, 1, 1, 1),
(77212, 77262, 7, 1, 1, 1, 1),
(77212, 77263, 7, 1, 1, 1, 1),
(77212, 77265, 7, 1, 1, 1, 1),
(77212, 77266, 7, 1, 1, 1, 1),
(77212, 77267, 7, 1, 1, 1, 1),
(77212, 77268, 7, 1, 1, 1, 1),
(77212, 77269, 7, 1, 1, 1, 1),
(77212, 77270, 7, 1, 1, 1, 1),
(77212, 77271, 6, 1, 1, 1, 1),
(77212, 77207, 1, 1, 1, 1, 1), 
(77212, 77209, 1, 1, 1, 1, 1), 
(77212, 77211, 1, 1, 1, 1, 1),
(77212, 77228, 1, 1, 1, 1, 1),
(77212, 77230, 1, 1, 1, 1, 1), 
(77212, 77232, 1, 1, 1, 1, 1), 
(77212, 77208, 1, 1, 1, 1, 1), 
(77212, 77210, 1, 1, 1, 1, 1), 
(77212, 77229, 1, 1, 1, 1, 1), 
(77212, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78371;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78371, 78361, 7, 1, 1, 1, 1),
(78371, 78362, 7, 1, 1, 1, 1),
(78371, 78363, 7, 1, 1, 1, 1),
(78371, 78364, 7, 1, 1, 1, 1),
(78371, 78365, 7, 1, 1, 1, 1),
(78371, 78366, 7, 1, 1, 1, 1),
(78371, 78367, 7, 1, 1, 1, 1),
(78371, 78369, 7, 1, 1, 1, 1),
(78371, 78370, 7, 1, 1, 1, 1),
(78371, 78371, 7, 1, 1, 1, 1),
(78371, 78372, 7, 1, 1, 1, 1),
(78371, 78373, 7, 1, 1, 1, 1),
(78371, 78368, 6, 1, 1, 1, 1),
(78371, 78001, 1, 1, 1, 1, 1),
(78371, 78003, 1, 1, 1, 1, 1),
(78371, 78489, 1, 1, 1, 1, 1),
(78371, 78490, 1, 1, 1, 1, 1),
(78371, 78492, 1, 1, 1, 1, 1),
(78371, 77999, 1, 1, 1, 1, 1),
(78371, 78000, 1, 1, 1, 1, 1),
(78371, 78002, 1, 1, 1, 1, 1),
(78371, 78491, 1, 1, 1, 1, 1),
(78371, 78493, 1, 1, 1, 1, 1);

UPDATE `creature_template` SET `lootid`=55265 WHERE `entry`=55265;
UPDATE `creature_template` SET `lootid`=57409 WHERE `entry`=57409;
UPDATE `creature_template` SET `lootid`=57771 WHERE `entry`=57771;
UPDATE `creature_template` SET `lootid`=57772 WHERE `entry`=57772;
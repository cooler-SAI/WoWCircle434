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
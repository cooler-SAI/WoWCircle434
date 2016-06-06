DELETE FROM `creature_loot_template` WHERE `entry`=55312;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55312, 71998, 35, 1, 0, 1, 3),
(55312, 77952, -100, 1, 0, 1, 1),
(55312, 77203, 100, 1, 0, -77203, 1),
(55312, 78171, 100, 1, 0, -78171, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=55313;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55313, 71998, 55, 1, 0, 1, 3),
(55313, 77952, -100, 1, 0, 1, 3),
(55313, 77203, 100, 1, 0, -77203, 4),
(55313, 78171, 100, 1, 0, -78171, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=55314;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55314, 71998, 35, 1, 0, 1, 3),
(55314, 77952, -100, 1, 0, 1, 1),
(55314, 77991, 100, 1, 0, -77991, 1),
(55314, 78858, 100, 1, 0, -78858, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=55315;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55315, 71998, 35, 1, 0, 1, 3),
(55315, 77952, -100, 1, 0, 1, 1),
(55315, 77991, 100, 1, 0, -77991, 4),
(55315, 78858, 100, 1, 0, -78858, 2);

DELETE FROM `reference_loot_template` WHERE `entry`=77203;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77203, 77217, 12, 1, 1, 1, 1),
(77203, 77219, 12, 1, 1, 1, 1),
(77203, 77218, 11, 1, 1, 1, 1),
(77203, 77253, 11, 1, 1, 1, 1),
(77203, 77252, 11, 1, 1, 1, 1),
(77203, 77254, 11, 1, 1, 1, 1),
(77203, 77203, 11, 1, 1, 1, 1),
(77203, 77206, 11, 1, 1, 1, 1), 
(77203, 77207, 1, 1, 1, 1, 1), 
(77203, 77209, 1, 1, 1, 1, 1), 
(77203, 77211, 1, 1, 1, 1, 1),
(77203, 77228, 1, 1, 1, 1, 1),
(77203, 77230, 1, 1, 1, 1, 1), 
(77203, 77232, 1, 1, 1, 1, 1), 
(77203, 77208, 1, 1, 1, 1, 1), 
(77203, 77210, 1, 1, 1, 1, 1), 
(77203, 77229, 1, 1, 1, 1, 1), 
(77203, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=77991;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77991, 78403, 12, 1, 1, 1, 1),
(77991, 78404, 12, 1, 1, 1, 1),
(77991, 77990, 11, 1, 1, 1, 1),
(77991, 77991, 11, 1, 1, 1, 1),
(77991, 78401, 11, 1, 1, 1, 1),
(77991, 78406, 11, 1, 1, 1, 1),
(77991, 78402, 11, 1, 1, 1, 1),
(77991, 78405, 11, 1, 1, 1, 1),
(77991, 78001, 1, 1, 1, 1, 1),
(77991, 78003, 1, 1, 1, 1, 1),
(77991, 78489, 1, 1, 1, 1, 1),
(77991, 78490, 1, 1, 1, 1, 1),
(77991, 78492, 1, 1, 1, 1, 1),
(77991, 77999, 1, 1, 1, 1, 1),
(77991, 78000, 1, 1, 1, 1, 1),
(77991, 78002, 1, 1, 1, 1, 1),
(77991, 78491, 1, 1, 1, 1, 1),
(77991, 78493, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78171;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78171, 78171, 40, 1, 1, 1, 1),
(78171, 78181, 30, 1, 1, 1, 1),
(78171, 78176, 30, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78858;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78858, 78858, 40, 1, 1, 1, 1),
(78858, 78857, 30, 1, 1, 1, 1),
(78858, 78856, 30, 1, 1, 1, 1);

REPLACE INTO `creature_currency` (`creature_id`, `CurrencyId1`, `CurrencyId2`, `CurrencyId3`, `CurrencyCount1`, `CurrencyCount2`, `CurrencyCount3`) VALUES 
(55312, 396, 614, 0, 12000, 1, 0),
(55313, 396, 614, 0, 14000, 1, 0),
(55314, 396, 614, 0, 12000, 1, 0),
(55315, 396, 614, 0, 14000, 1, 0);

UPDATE `creature_template` SET `lootid`=55312 WHERE `entry`=55312;
UPDATE `creature_template` SET `lootid`=55313 WHERE `entry`=55313;
UPDATE `creature_template` SET `lootid`=55314 WHERE `entry`=55314;
UPDATE `creature_template` SET `lootid`=55315 WHERE `entry`=55315;
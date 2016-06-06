DELETE FROM `creature_loot_template` WHERE `entry`=55689;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(55689, 71998, 35, 1, 0, 1, 3),
(55689, 77952, -100, 1, 0, 1, 1),
(55689, 78012, 100, 1, 0, -78012, 1),
(55689, 78170, 100, 1, 0, -78170, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=57462;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57462, 71998, 55, 1, 0, 1, 3),
(57462, 77952, -100, 1, 0, 1, 3),
(57462, 78012, 100, 1, 0, -78012, 4),
(57462, 78170, 100, 1, 0, -78170, 2);

DELETE FROM `creature_loot_template` WHERE `entry`=57955;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57955, 71998, 35, 1, 0, 1, 3),
(57955, 77952, -100, 1, 0, 1, 1),
(57955, 78419, 100, 1, 0, -78419, 1),
(57955, 78861, 100, 1, 0, -78861, 1);

DELETE FROM `creature_loot_template` WHERE `entry`=57956;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(57956, 71998, 35, 1, 0, 1, 3),
(57956, 77952, -100, 1, 0, 1, 1),
(57956, 78419, 100, 1, 0, -78419, 4),
(57956, 78861, 100, 1, 0, -78861, 2);

DELETE FROM `reference_loot_template` WHERE `entry`=78012;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78012, 78012, 13, 1, 1, 1, 1), 
(78012, 78011, 13, 1, 1, 1, 1), 
(78012, 77221, 12, 1, 1, 1, 1), 
(78012, 77249, 11, 1, 1, 1, 1), 
(78012, 77250, 11, 1, 1, 1, 1), 
(78012, 77251, 11, 1, 1, 1, 1), 
(78012, 77220, 10, 1, 1, 1, 1), 
(78012, 77248, 9, 1, 1, 1, 1), 
(78012, 77207, 1, 1, 1, 1, 1), 
(78012, 77209, 1, 1, 1, 1, 1), 
(78012, 77211, 1, 1, 1, 1, 1),
(78012, 77228, 1, 1, 1, 1, 1),
(78012, 77230, 1, 1, 1, 1, 1), 
(78012, 77232, 1, 1, 1, 1, 1), 
(78012, 77208, 1, 1, 1, 1, 1), 
(78012, 77210, 1, 1, 1, 1, 1), 
(78012, 77229, 1, 1, 1, 1, 1), 
(78012, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78419;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78419, 78419, 13, 1, 1, 1, 1),
(78419, 78413, 13, 1, 1, 1, 1),
(78419, 78418, 12, 1, 1, 1, 1),
(78419, 78417, 11, 1, 1, 1, 1),
(78419, 78416, 11, 1, 1, 1, 1),
(78419, 78415, 11, 1, 1, 1, 1),
(78419, 78414, 10, 1, 1, 1, 1),
(78419, 78420, 9, 1, 1, 1, 1),
(78419, 78001, 1, 1, 1, 1, 1),
(78419, 78003, 1, 1, 1, 1, 1),
(78419, 78489, 1, 1, 1, 1, 1),
(78419, 78490, 1, 1, 1, 1, 1),
(78419, 78492, 1, 1, 1, 1, 1),
(78419, 77999, 1, 1, 1, 1, 1),
(78419, 78000, 1, 1, 1, 1, 1),
(78419, 78002, 1, 1, 1, 1, 1),
(78419, 78491, 1, 1, 1, 1, 1),
(78419, 78493, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78170;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78170, 78170, 40, 1, 1, 1, 1),
(78170, 78180, 30, 1, 1, 1, 1),
(78170, 78175, 30, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78861;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78861, 78861, 40, 1, 1, 1, 1),
(78861, 78859, 30, 1, 1, 1, 1),
(78861, 78860, 30, 1, 1, 1, 1);

UPDATE `creature_template` SET `lootid`=55689 WHERE `entry`=55689;
UPDATE `creature_template` SET `lootid`=57462 WHERE `entry`=57462;
UPDATE `creature_template` SET `lootid`=57955 WHERE `entry`=57955;
UPDATE `creature_template` SET `lootid`=57956 WHERE `entry`=57956;
UPDATE `gameobject` SET `spawnmask`=0 WHERE `map`=967 AND `id`=210221;

UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=210160;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=210161;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=210162;
UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=210163;

DELETE FROM `gameobject_loot_template` WHERE `entry`=210160;
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(210160, 71998, 35, 1, 0, 1, 3),
(210160, 77952, -100, 1, 0, 1, 1),
(210160, 77245, 100, 1, 0, -77245, 1),
(210160, 78174, 100, 1, 0, -78174, 1);

DELETE FROM `gameobject_loot_template` WHERE `entry`=210161;
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(210161, 71998, 55, 1, 0, 1, 3),
(210161, 77952, -100, 1, 0, 1, 3),
(210161, 77245, 100, 1, 0, -77245, 4),
(210161, 78174, 100, 1, 0, -78174, 2);

DELETE FROM `gameobject_loot_template` WHERE `entry`=210162;
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(210162, 71998, 35, 1, 0, 1, 3),
(210162, 77952, -100, 1, 0, 1, 1),
(210162, 78433, 100, 1, 0, -78433, 1),
(210162, 78849, 100, 1, 0, -78849, 1);

DELETE FROM `gameobject_loot_template` WHERE `entry`=210163;
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(210163, 71998, 35, 1, 0, 1, 3),
(210163, 77952, -100, 1, 0, 1, 1),
(210163, 78433, 100, 1, 0, -78433, 4),
(210163, 78849, 100, 1, 0, -78849, 2);

DELETE FROM `reference_loot_template` WHERE `entry`=77245;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(77245, 77245, 13, 1, 1, 1, 1), 
(77245, 77243, 13, 1, 1, 1, 1), 
(77245, 77247, 10, 1, 1, 1, 1), 
(77245, 78013, 10, 1, 1, 1, 1), 
(77245, 77242, 10, 1, 1, 1, 1), 
(77245, 77223, 10, 1, 1, 1, 1), 
(77245, 77205, 8, 1, 1, 1, 1), 
(77245, 77244, 8, 1, 1, 1, 1), 
(77245, 77246, 8, 1, 1, 1, 1), 
(77245, 77207, 1, 1, 1, 1, 1), 
(77245, 77209, 1, 1, 1, 1, 1), 
(77245, 77211, 1, 1, 1, 1, 1),
(77245, 77228, 1, 1, 1, 1, 1),
(77245, 77230, 1, 1, 1, 1, 1), 
(77245, 77232, 1, 1, 1, 1, 1), 
(77245, 77208, 1, 1, 1, 1, 1), 
(77245, 77210, 1, 1, 1, 1, 1), 
(77245, 77229, 1, 1, 1, 1, 1), 
(77245, 77231, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78433;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78433, 78433, 13, 1, 1, 1, 1), 
(78433, 78434, 13, 1, 1, 1, 1), 
(78433, 78430, 10, 1, 1, 1, 1), 
(78433, 78432, 10, 1, 1, 1, 1), 
(78433, 78435, 10, 1, 1, 1, 1), 
(78433, 78429, 10, 1, 1, 1, 1), 
(78433, 77992, 8, 1, 1, 1, 1), 
(78433, 78436, 8, 1, 1, 1, 1), 
(78433, 78431, 8, 1, 1, 1, 1), 
(78433, 78001, 1, 1, 1, 1, 1),
(78433, 78003, 1, 1, 1, 1, 1),
(78433, 78489, 1, 1, 1, 1, 1),
(78433, 78490, 1, 1, 1, 1, 1),
(78433, 78492, 1, 1, 1, 1, 1),
(78433, 77999, 1, 1, 1, 1, 1),
(78433, 78000, 1, 1, 1, 1, 1),
(78433, 78002, 1, 1, 1, 1, 1),
(78433, 78491, 1, 1, 1, 1, 1),
(78433, 78493, 1, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78174;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78174, 78174, 40, 1, 1, 1, 1),
(78174, 78184, 30, 1, 1, 1, 1),
(78174, 78179, 30, 1, 1, 1, 1);

DELETE FROM `reference_loot_template` WHERE `entry`=78849;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(78849, 78849, 40, 1, 1, 1, 1),
(78849, 78848, 30, 1, 1, 1, 1),
(78849, 78847, 30, 1, 1, 1, 1);
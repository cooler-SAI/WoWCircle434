DELETE FROM `item_loot_template` WHERE (`entry`=45724);
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(45724, -241, 100, 0, 1, 1);
DELETE FROM `item_loot_template` WHERE (`entry`=44113) AND (`item`=43016);
DELETE FROM `item_loot_template` WHERE (`entry`=44113) AND (`item`=-81);
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(44113, -81, 75, 0, 1, 1);
DELETE FROM `item_loot_template` WHERE (`entry`=54537) AND (`item`=-395);
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(54537, -395, 100, 0, 25, 25);
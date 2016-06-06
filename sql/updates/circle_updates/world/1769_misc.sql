ALTER TABLE `research_site`  DROP COLUMN `map`;

UPDATE `gameobject_template` SET `data1`=202655 WHERE `entry`=202655;
UPDATE `gameobject_template` SET `data1`=203071 WHERE `entry`=203071;
UPDATE `gameobject_template` SET `data1`=203078 WHERE `entry`=203078;
UPDATE `gameobject_template` SET `data1`=204282 WHERE `entry`=204282;
UPDATE `gameobject_template` SET `data1`=206836 WHERE `entry`=206836;
UPDATE `gameobject_template` SET `data1`=207187 WHERE `entry`=207187;
UPDATE `gameobject_template` SET `data1`=207188 WHERE `entry`=207188;
UPDATE `gameobject_template` SET `data1`=207189 WHERE `entry`=207189;
UPDATE `gameobject_template` SET `data1`=207190 WHERE `entry`=207190;

DELETE FROM `gameobject_loot_template` WHERE `entry` IN (202655, 203071, 203078, 204282, 206836, 207187, 207188, 207189, 207190);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(202655, -385, 100, 1, 0, 3, 14),
(202655, 63128, 7, 1, 0, 1, 1),

(203071, -394, 100, 1, 0, 3, 14),
(203071, 63127, 7, 1, 0, 1, 1),

(203078, -400, 100, 1, 0, 3, 14),
(203078, 64396, 7, 1, 0, 1, 1),

(204282, -384, 100, 1, 0, 3, 14),
(204282, 52843, 7, 1, 0, 1, 1),

(206836, -393, 100, 1, 0, 3, 14),

(207187, -397, 100, 1, 0, 3, 14),
(207187, 64392, 7, 1, 0, 1, 1),

(207188, -398, 100, 1, 0, 3, 14),
(207188, 64394, 7, 1, 0, 1, 1),

(207189, -399, 100, 1, 0, 3, 14),
(207189, 64395, 7, 1, 0, 1, 1),

(207190, -401, 100, 1, 0, 3, 14),
(207190, 64397, 7, 1, 0, 1, 1);

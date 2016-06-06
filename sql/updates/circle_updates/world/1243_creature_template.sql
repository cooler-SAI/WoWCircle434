UPDATE `creature_loot_template` SET `item`=68175 WHERE `entry`=36296 AND `item`=51804;
UPDATE `creature_loot_template` SET `item`=68176 WHERE `entry`=36296 AND `item`=51805;
UPDATE `creature_loot_template` SET `item`=68172 WHERE `entry`=36296 AND `item`=51806;
UPDATE `creature_loot_template` SET `item`=68174 WHERE `entry`=36296 AND `item`=51807;
UPDATE `creature_loot_template` SET `item`=68173 WHERE `entry`=36296 AND `item`=51808;

UPDATE `creature_template` SET `health_mod`=40, `dmg_multiplier`=40 WHERE `entry` IN (36565, 36296, 36272);
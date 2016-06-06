UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822 WHERE `entry` IN 
(55294, 56576, 56577, 56578);
UPDATE `creature_template` SET `dmg_multiplier`=97 WHERE `entry` IN
(55294, 56576);
UPDATE `creature_template` SET `dmg_multiplier`=110 WHERE `entry` IN
(56577, 56578);

REPLACE INTO `creature_currency` (`creature_id`, `CurrencyId1`, `CurrencyId2`, `CurrencyId3`, `CurrencyCount1`, `CurrencyCount2`, `CurrencyCount3`) VALUES 
(55294, 396, 614, 0, 12000, 1, 0),
(56576, 396, 614, 0, 14000, 1, 0),
(56577, 396, 614, 0, 12000, 1, 0),
(56578, 396, 614, 0, 14000, 1, 0);
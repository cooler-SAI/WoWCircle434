DELETE FROM `spell_linked_spell` WHERE `spell_trigger`=26573;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (26573, 36946, 'Paladin - Consecration');

DELETE FROM `spell_bonus_data` WHERE `entry`=36946;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (36946, -1, 0.04, -1, 0.04, 'Paladin - Consecration');

UPDATE `creature_template` SET `flags_extra`=128 WHERE `entry`=43499;
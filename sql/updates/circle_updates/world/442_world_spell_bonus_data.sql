DELETE FROM `spell_bonus_data` WHERE `entry`=15237;
DELETE FROM `spell_bonus_data` WHERE `entry`=23455;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (15237, 0.143, -1, -1, -1, 'Priest - Holy Nova Damage');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (23455, 0.1963, -1, -1, -1, 'Priest - Holy Nova Heal Rank 1');
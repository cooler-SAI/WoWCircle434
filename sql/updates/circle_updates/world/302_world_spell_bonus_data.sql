DELETE FROM `spell_bonus_data` WHERE `entry`=19750;
DELETE FROM `spell_bonus_data` WHERE `entry`=635;
DELETE FROM `spell_bonus_data` WHERE `entry`=25914;
DELETE FROM `spell_bonus_data` WHERE `entry`=25912;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (635, 0.432, -1, -1, -1, 'Paladin - Holy Light');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (19750, 0.863, -1, -1, -1, 'Paladin - Flash of Light');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (25912, 0.429, -1, -1, -1, 'Paladin - Holy Shock Triggered Hurt Rank 1');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (25914, 0.269, -1, -1, -1, 'Paladin - Holy Shock Triggered Heal Rank 1');
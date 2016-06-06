DELETE FROM `spell_bonus_data` WHERE `entry`=1120;
DELETE FROM `spell_bonus_data` WHERE `entry`=17877;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (1120, 0, 0.36, 0, 0, 'Warlock - Drain Soul');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (17877, 1.056, 0, 0, 0, 'Warlock - Shadowburn');

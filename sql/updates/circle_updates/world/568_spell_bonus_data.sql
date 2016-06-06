DELETE FROM `spell_bonus_data` WHERE `entry`=33763;
DELETE FROM `spell_bonus_data` WHERE `entry`=33778;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (33763, 0, 0.03, 0, 0, 'Druid - Lifebloom HoT');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (33778, 0.518, 0, 0, 0, 'Druid - Lifebloom final heal');
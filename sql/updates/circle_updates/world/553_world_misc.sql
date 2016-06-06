DELETE FROM `spell_bonus_data` WHERE `entry`=1822;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (1822, 0, 0, 0.147, 0.147, 'Druid - Rake');

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_dru_savage_defense';
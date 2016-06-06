DELETE FROM `spell_script_names` WHERE `spell_id`=73680;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (73680, 'spell_sha_unleash_elements');

DELETE FROM `spell_script_names` WHERE `spell_id`=77829;
DELETE FROM `spell_script_names` WHERE `spell_id`=77830;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (77829, 'spell_shaman_ancestral_resolve');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (77830, 'spell_shaman_ancestral_resolve');

DELETE FROM `spell_bonus_data` WHERE `entry`=331;
DELETE FROM `spell_bonus_data` WHERE `entry`=8004;
DELETE FROM `spell_bonus_data` WHERE `entry`=61295;
DELETE FROM `spell_bonus_data` WHERE `entry`=77472;
DELETE FROM `spell_bonus_data` WHERE `entry`=379;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (331, 0.302, -1, -1, -1, 'Shaman - Healing Wave');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (8004, 0.483, -1, -1, -1, 'Shaman - Healing Surge');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (61295, 0.22, 0.068, -1, -1, 'Shaman - Riptide');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (77472, 0.967, -1, -1, -1, 'Shaman - Greater Healing Wave');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (379, 0, 0, 0, 0, 'Shaman - Earth Shield Triggered');

DELETE FROM `spell_bonus_data` WHERE `entry`=73682;
DELETE FROM `spell_bonus_data` WHERE `entry`=73683;
DELETE FROM `spell_bonus_data` WHERE `entry`=73685;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (73682, 0.386, -1, -1, -1, 'Shaman - Unleash Frost');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (73683, 0.429, -1, -1, -1, 'Shaman - Unleash Flame');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (73685, 0.201, -1, -1, -1, 'Shaman - Unleash Life');
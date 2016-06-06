REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(82327, 0.2, 0, 0, 0, 'Paladin - Holy Radiance'),
(86452, 0.08, 0, 0, 0, 'Paladin - Holy Radiance');

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(26364, 0.267, 0, 0, 0, 'Shaman - Lightning Shield Proc Rank 1');

DELETE FROM `spell_group` WHERE `id` = 1125;
DELETE FROM `spell_group_stack_rules` WHERE `group_id` = 1125;
INSERT INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES 
(1125, 11366, 'Pyroblast'),
(1125, 92315, 'Pyroblast!');
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`, `comments`) VALUES 
(1125, 2, 'Pyroblast + Pyroblast! (1 caster)');

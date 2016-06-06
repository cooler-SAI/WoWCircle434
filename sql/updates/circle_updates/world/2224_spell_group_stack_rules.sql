DELETE FROM `spell_group` WHERE `spell_id` IN (8921, 93402);
DELETE FROM `spell_group_stack_rules` WHERE `group_id` = 2123;

INSERT INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES ('2124', '8921', 'Moonfire');
INSERT INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES ('2124', '93402', 'Sunfire');
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`, `comments`) VALUES ('2124', '2', 'Sunfire + Moonfire');
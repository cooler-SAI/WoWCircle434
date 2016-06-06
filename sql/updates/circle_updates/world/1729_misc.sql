-- Hunter's mark & Marked for Death stacking

REPLACE INTO `spell_group` (`id`, `spell_id`) 
VALUES 
('1122', '1130'),
('1122', '88691');

REPLACE INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES ('1122', '1');
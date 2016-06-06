REPLACE INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES ('2123', '91565', 'Faerie Fire (armor reduce)');
REPLACE INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES ('1012', '-2123', '- Faerie Fire normal');
UPDATE `spell_group` SET `comments`='- Faerie Fire feral' WHERE (`id`='1012') AND (`spell_id`='-1007');
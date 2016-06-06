REPLACE INTO `pet_scaling_data` (`spell_id`, `class`, `comment`) VALUES 
(89953, 9, 'All Crit Chance, Attack Speed, Magic Resistance'),
(110474, 6, 'Critical chance, Magic Resistance ');

REPLACE INTO `creature_template_addon` (`entry`, `auras`) VALUES 
(11859, ''),
(89, '39007');

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(89953, 'spell_pet_stat_calculate'),
(110474, 'spell_pet_stat_calculate');

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(51963, 0.453, 0, 0, 0, 'Death Knight - Gargoyle Strike');
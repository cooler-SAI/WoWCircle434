INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(73899, 'spell_quests_spell_hit'),
(122, 'spell_quests_spell_hit'),
(2098, 'spell_quests_spell_hit'),
(172, 'spell_quests_spell_hit'),
(100, 'spell_quests_spell_hit'),
(8921, 'spell_quests_spell_hit'),
(589, 'spell_quests_spell_hit');

UPDATE `creature` SET `movementtype`=0 WHERE `id`=44848;
UPDATE `creature_template` SET `ScriptName`='npc_training_dummy' WHERE `entry`=44848;
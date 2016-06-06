UPDATE `gameobject_template` SET `flags`=0 WHERE `entry`=194739;
UPDATE `gameobject_template` SET `faction`=0, `flags`=48 WHERE `entry`=194749;
UPDATE `gameobject_template` SET `faction`=0, `flags`=48 WHERE `entry`=194774;
UPDATE `gameobject_template` SET `faction`=0, `flags`=48 WHERE `entry`=194775;
UPDATE `gameobject_template` SET `faction`=0, `flags`=48 WHERE `entry`=194776;

UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=34050;
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=34211;

UPDATE `creature_template` SET `ScriptName`='mob_lightning_elemental' WHERE `entry`=32958;
UPDATE `creature_template` SET `ScriptName`='mob_rune_of_summoning' WHERE `entry`=33051;
UPDATE `creature_template` SET `ScriptName`='mob_rune_of_power' WHERE `entry`=33705;

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62713, 'spell_elder_ironbranch_essence_targeting'),
(62968, 'spell_elder_brightleaf_essence_targeting'),
(65761, 'spell_elder_brightleaf_essence_targeting'),
(63006, 'spell_aggregation_pheromones_targeting');

REPLACE INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(10459, 11, 0, 0, 'achievement_knock_on_wood_25'),
(10459, 12, 1, 0, ''),
(10460, 11, 0, 0, 'achievement_knock_knock_on_wood_25'),
(10460, 12, 1, 0, ''),
(10461, 11, 0, 0, 'achievement_knock_knock_knock_on_wood_25'),
(10461, 12, 1, 0, ''),
(10758, 0, 0, 0, ''),
(10758, 11, 0, 0, 'achievement_getting_back_to_nature_25'),
(10758, 12, 1, 0, '');

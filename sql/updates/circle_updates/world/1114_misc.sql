DELETE FROM `creature` WHERE `guid`=335405 AND `id`=44418;

DELETE FROM `conditions` WHERE `SourceEntry`=91304;

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_chimaeron_double_attack';
DELETE FROM `spell_script_names` WHERE `spell_id` IN (82871, 82705);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(82871, 'spell_chimaeron_caustic_slime_target'),
(82705, 'spell_chimaeron_finkles_mixture');

DELETE FROM `disables` WHERE `sourceType`=4 AND `entry` IN (14005, 16002, 14406, 15660);
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (14005, 16002, 14406, 15660);
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (14005, 16002, 14406, 15660);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(16002, 11, 0, 0, 'achievement_full_of_sound_and_fury');

UPDATE `creature_template` SET `baseattacktime`=4000 WHERE `entry` IN (43296, 47774, 47775, 47776);
UPDATE `creature_template` SET `mindmg`=1096, `maxdmg`=1465, `dmg_multiplier`=100 WHERE `entry` IN (43296, 47774, 47775, 47776);
UPDATE `creature_template` SET `dmg_multiplier` = 20, `flags_extra` = `flags_extra`|1 WHERE `entry` IN (31612,31611,31610);

UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask`|1|2|4|8|16|32|64|128|256|512|1024|2048|4096|8192|65536|131072|524288|4194304|8388608|33554432|67108864|536870912 WHERE `entry` IN (28684,28921,29120,31612,31611,31610);
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask`|1|2|4|8|16|32|64|128|512|1024|4096|8192|65536|131072|524288|4194304|8388608|67108864|536870912 WHERE `entry` IN (28729,28730,28731,31615,31616,31617);

UPDATE `creature` SET `position_x` = 506.178, `position_y` = 669.927, `position_z` = 776.306, `orientation` = 0.855211 WHERE `id` = 28729;
UPDATE `creature` SET `position_x` = 500.963, `position_y` = 667.828, `position_z` = 776.306, `orientation` = 0.83428 WHERE `id` = 28733;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` IN (52438, 52449, 53030, 59417);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceEntry`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`) VALUES
(13, 52438, 18, 1, 28684),
(13, 52449, 18, 1, 28684);

DELETE FROM `creature` WHERE `id` IN (29062,29063,29064);
DELETE FROM `creature` WHERE `id` IN (29096,29097,29098);

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` = 4244;
INSERT INTO `achievement_criteria_data` VALUES
(4244,11,0,0,'achievement_hadronox_denied');

UPDATE `creature_template` SET `ScriptName`='npc_anubar_crusher' WHERE `entry`=28922;
UPDATE `creature_template` SET `ScriptName`='npc_hadronox_nerubian' WHERE `entry` IN (29064, 290643, 29062);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (53030, 59417);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(53030, 'spell_hadronox_leech_poison'),
(59417, 'spell_hadronox_leech_poison');


DELETE FROM `creature` WHERE `id` IN (55566, 55573) AND `map`=601;
DELETE FROM `creature_addon` WHERE `guid` IN (351029, 351026, 351027, 351028, 350966, 350967);
DELETE FROM `waypoint_data` WHERE `id` IN (351029, 351026, 351027, 351028, 350966, 350967);
UPDATE `creature_template_addon` SET `bytes1`=0 WHERE `entry`=55573;

UPDATE `creature_template` SET `unit_flags`=33024 WHERE `entry`=55564;
UPDATE `creature` SET `position_x`=438.629333, `position_y`=786.311035, `position_z`=829.11, `orientation`=3.648181 WHERE `guid`=351029 AND `id`=55564 AND `map`=601;
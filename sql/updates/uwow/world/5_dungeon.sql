UPDATE `creature_template` SET `ScriptName` = 'boss_deathwing' WHERE `entry` in (57962, 56173, 58000, 58001);
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_trall' WHERE `entry` = 56103;
UPDATE `creature_template` SET `ScriptName` = 'npc_arm_tentacle_one' WHERE `entry` in (56846, 57974, 58133, 58134);
UPDATE `creature_template` SET `ScriptName` = 'npc_arm_tentacle_two' WHERE `entry` in (56167, 57973, 58131, 58132);
UPDATE `creature_template` SET `ScriptName` = 'npc_wing_tentacle_one' WHERE `entry` = 56168;
UPDATE `creature_template` SET `ScriptName` = 'npc_wing_tentacle_two' WHERE `entry` = 57972;
UPDATE `creature_template` SET `ScriptName` = 'npc_mutated_corruption' WHERE `entry` in (56471, 57976, 58137, 58138);
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_alexstrasza' WHERE `entry` = 56099;
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_ysera' WHERE `entry` = 56100;
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_kalecgos' WHERE `entry` = 56101;
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_nozdormy' WHERE `entry` = 56102;
UPDATE `creature_template` SET `ScriptName` = 'npc_regenerative_blood' WHERE `entry` = 56263;
UPDATE `creature_template` SET `ScriptName` = 'npc_elementium_bolt' WHERE `entry` = 56262;
UPDATE `creature_template` SET `ScriptName` = 'npc_congealing_blood' WHERE `entry` = 57798;
UPDATE `creature_template` SET `ScriptName` = 'npc_corruption_parasite' WHERE `entry` = 57479;
UPDATE `creature_template` SET `ScriptName` = 'npc_time_zone' WHERE `entry` = 56311;
UPDATE `creature_template` SET `ScriptName` = 'npc_elementium_terror' WHERE `entry` = 56710;
UPDATE `creature_template` SET `ScriptName` = 'npc_blistering_tentacle' WHERE `entry` = 56188;
UPDATE `creature_template` SET `ScriptName` = 'npc_platfrom' WHERE `entry` = 56307;


UPDATE `creature_template` SET `ScriptName` = 'npc_ds_warden_time' WHERE `entry` = 56142;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_warden_dream' WHERE `entry` = 56140;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_warden_life' WHERE `entry` = 56139;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_mount_dream' WHERE `entry` = 57475;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_mount_time' WHERE `entry` = 57474;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_mount_life' WHERE `entry` = 57473;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_parachute_1' WHERE `entry` = 57629;
UPDATE `creature_template` SET `ScriptName` = 'npc_ds_parachute_2' WHERE `entry` = 57630;

DELETE FROM spell_script_names WHERE spell_id in (105408, 106548, 106835, 108597, 108649, 105565, 105569, 105444, 105445);
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES
(105408, 'spell_burning_blood_trigger'),
(106835, 'spell_corrupted_blood'),
(108597, 'spell_corrupting_parasite'),
(108649, 'spell_corrupting_parasite_trigger'),
(105444, 'spell_blistering_heat'),
(105445, 'spell_blistering_heat_aura'),
(106548, 'spell_agonizing_pain_dmg');


DELETE FROM spell_target_position WHERE id in (108537, 110063, 106999);
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES
('108537', '0', '967', '-12128.3', '12253.8', '0.0451', '5.5326'),
('110063', '0', '967', '-12074.3', '12159.5', '-2.650', '2.8448'),
('106999', '0', '967', '-12100.5', '12173.6', '2.73425', '5.09636');

DELETE FROM `conditions` WHERE `SourceEntry` in (106600, 105565, 105569, 106588, 106613, 106624, 105969, 110208, 109102);

INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13, 3, 105569, 0, 0, 31, 0, 3, 56188, 0, 0, 0, '', 'Madnes of Deathwing - Alexstraza cauterize target'),
(13, 3, 105565, 0, 0, 31, 0, 3, 56188, 0, 0, 0, '', 'Madnes of Deathwing - Alexstraza cauterize target'),
(17, 6, 106600, 0, 0, 31, 0, 3, 56846, 0, 0, 0, '', 'Madnes of Deathwing - nozdormu'),
(17, 6, 106588, 0, 0, 31, 0, 3, 56168, 0, 0, 0, '', 'Madnes of Deathwing - alexstraza'),
(17, 6, 106613, 0, 0, 31, 0, 3, 56167, 0, 0, 0, '', 'Madnes of Deathwing - ysera'),
(17, 6, 106624, 0, 0, 31, 0, 3, 57972, 0, 0, 0, '', 'Madnes of Deathwing - kalecgos'),
(17, 136, 109102, 0, 0, 31, 0, 3, 56173, 0, 0, 0, '', 'Madnes of Deathwing - SPELL_CONGEALING_BLOOD');


UPDATE `creature_template` SET `InhabitType` = 4, `HoverHeight` = 1 WHERE `entry` in
(57962, 56173, 58000, 58001, 56168, 57972, 58129, 58130, 56167, 57973, 58131, 58132, 56846, 57974, 58133, 58134, 57695, 57696, 57686, 57694, 56844, 56099, 56102, 56100, 56101, 56471, 57976, 58137, 58138);

UPDATE `creature_template` SET `unit_flags` = 0 WHERE `entry` in (56846, 57974, 58133, 58134, 56167, 57973, 58131, 58132, 56168, 57972, 58129, 58130);
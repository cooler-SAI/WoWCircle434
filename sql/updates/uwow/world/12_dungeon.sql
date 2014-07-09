

REPLACE INTO spell_script_names VALUES
('88427', 'spell_electrocute'),
('93261', 'spell_wind_burst'),
('93262', 'spell_wind_burst'),
('93263', 'spell_wind_burst'),
('93297', 'spell_lightning_clouds_damage'),
('93298', 'spell_lightning_clouds_damage'),
('93299', 'spell_lightning_clouds_damage'),
('89588', 'spell_lightning_clouds_damage'),
('93255', 'spell_lightning_strike'),
('93256', 'spell_lightning_strike'),
('93257', 'spell_lightning_strike'),
('88214', 'spell_lightning_strike'),
('88238', 'spell_lightning_strike_effect'),
('93854', 'spell_lightning_strike_effect'),
('93855', 'spell_lightning_strike_effect'),
('93856', 'spell_lightning_strike_effect'),
('85422', 'spell_nurture_aura'),
('85429', 'spell_nurture_aura'),
('86367', 'spell_nezir_sleet_storm'),
('93135', 'spell_nezir_sleet_storm'),
('93136', 'spell_nezir_sleet_storm'),
('93137', 'spell_nezir_sleet_storm'),
('85483', 'spell_wind_blast'),
('93138', 'spell_wind_blast'),
('93139', 'spell_wind_blast'),
('89104', 'at_reletness_storm'),
('93140', 'spell_wind_blast');


DELETE FROM `gameobject` where map = '754';

INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
('206699', '754', '15', '1', '-48.5569', '1054.37', '200.528', '4.70171', '0', '0', '0.999898', '-0.0280381', '300', '0', '1'),
('206700', '754', '15', '1', '188.114', '813.822', '202.528', '3.12542', '0', '0', '0.999898', '-0.0280381', '300', '0', '1'),
('206701', '754', '15', '1', '-52.8208', '577.173', '200.528', '1.56596', '0', '0', '0.999898', '-0.0280381', '300', '0', '1'),
('207922', '754', '15', '1', '-50.4909', '815.868', '189.984', '3.11662', '0', '0', '0.999898', '-0.0280381', '300', '0', '1'),
('207930', '754', '15', '1', '-247.607', '776.485', '198.458', '2.35754', '0', '0', '0.924137', '0.38206', '300', '0', '1'),
('207929', '754', '15', '1', '-94.2481', '619.219', '198.458', '5.50306', '0', '0', '0.380245', '-0.924886', '300', '0', '1'),
('207928', '754', '15', '1', '-11.2602', '618.869', '198.458', '3.93226', '0', '0', '0.922867', '-0.385118', '300', '0', '1'),
('207927', '754', '15', '1', '146.138', '772.356', '198.458', '0.826014', '0', '0', '0.401365', '0.915918', '300', '0', '1'),
('207926', '754', '15', '1', '146.203', '855.315', '198.458', '5.49834', '0', '0', '0.382429', '-0.923985', '300', '0', '1'),
('207925', '754', '15', '1', '-7.08924', '1012.53', '198.458', '2.3489', '0', '0', '0.922478', '0.386049', '300', '0', '1'),
('207924', '754', '15', '1', '-90.0826', '1012.85', '198.458', '0.772608', '0', '0', '0.376767', '0.926308', '300', '0', '1'),
('207923', '754', '15', '1', '-247.379', '859.524', '198.458', '3.92598', '0', '0', '0.924073', '-0.382215', '300', '0', '1'),
('207737', '754', '3', '1', '-271', '814.87', '203.034', '3.14159', '1.31722e-43', '0', '0', '0', '7200', '0', '1'),
('207891', '754', '1', '1', '-49.6458', '815.082', '260.101', '3.12414', '0', '0', '0.70773', '-0.706483', '-604800', '100', '1'),
('207892', '754', '2', '1', '-49.6458', '815.082', '260.101', '3.12414', '0', '0', '0.70773', '-0.706483', '-604800', '100', '1');

REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `unkInt32`, `AIName`, `ScriptName`, `WDBVerified`) VALUES
('206699', '14', '10135', 'Doodad_Skywall_Djinn_Healing01', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('206700', '14', '10136', 'Doodad_Skywall_Djinn_Frost01', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('206701', '14', '10137', 'Doodad_Skywall_Djinn_Tornado01', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207737', '33', '10369', 'Skywall Raid Center Platform', '', '', '', '0', '32', '1', '0', '0', '0', '0', '0', '0', '0', '0', '36', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '94', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207891', '3', '10401', 'Heart of Wind', '', '', '', '0', '0', '5', '0', '0', '0', '0', '0', '0', '57', '207891', '0', '1', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '1', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '1'),
('207892', '3', '10401', 'Heart of Wind', '', '', '', '0', '0', '5', '0', '0', '0', '0', '0', '0', '57', '0', '0', '1', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '1', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '15595'),
('207922', '1', '10393', 'Doodad_Skywall_Wind_DraftEffect_Center01', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207923', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_01', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207924', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_02', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207925', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_03', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207926', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_04', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207927', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_07', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207928', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_08', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207929', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_05', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1'),
('207930', '1', '10394', 'Doodad_Skywall_Wind_DraftEffect_06', '', '', '', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '1');


REPLACE INTO `creature_template` VALUES ('8777000', '0', '0', '0', '0', '0', '11686', '11686', '0', '0', 'Wind Burst Trigger', '', '', '0', '60', '60', '0', '0', '35', '35', '0', '1', '1.14286', '1', '0', '2', '2', '0', '24', '1', '0', '0', '1', '33554436', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '10', '16778240', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '0', '3', '1', '1.35', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', 'npc_wind_burst_trigger', '1');


UPDATE `creature_template` SET  `ScriptName` =  'boss_anshal' WHERE `entry` = 45870;
UPDATE `creature_template` SET  `ScriptName` =  'boss_nezir' WHERE `entry` = 45871;
UPDATE `creature_template` SET  `ScriptName` =  'boss_rohash' WHERE `entry` = 45872;
UPDATE `creature_template` SET  `ScriptName` =  'npc_ravenous_creeper_trigger' WHERE `entry` = 45813;
UPDATE `creature_template` SET  `ScriptName` =  'npc_ravenous_creeper' WHERE `entry` = 45812;
UPDATE `creature_template` SET  `ScriptName` =  'npc_soothing_breeze' WHERE `entry` = 46246;
UPDATE `creature_template` SET  `ScriptName` =  'npc_ice_patch' WHERE `entry` = 46186;
UPDATE `creature_template` SET  `ScriptName` =  'npc_tornado_rohash' WHERE `entry` = 46207;


UPDATE `creature_template` SET  `ScriptName` =  'npc_ice_storm' WHERE `entry` = 46973;
UPDATE `creature_template` SET  `ScriptName` =  'npc_ice_storm_rain' WHERE `entry` = 46734;
UPDATE `creature_template` SET  `ScriptName` =  'npc_stormling_pre_effect' WHERE `entry` = 47177;
UPDATE `creature_template` SET  `ScriptName` =  'npc_stormling' WHERE `entry` = 47175;
UPDATE `creature_template` SET  `ScriptName` =  'npc_stormling' WHERE `entry` = 47177;
UPDATE `creature_template` SET  `ScriptName` =  'npc_wind_burst_trigger' WHERE `entry` = 8777000;
UPDATE `creature_template` SET  `ScriptName` =  'npc_relentless_storm_initial_vehicle' WHERE `entry` = 47806;
UPDATE `creature_template` SET  `ScriptName` =  'npc_lightning_coulds' WHERE `entry` = 48190;
UPDATE `creature_template` SET  `ScriptName` =  'npc_lightning_coulds' WHERE `entry` = 48196;
UPDATE `creature_template` SET  `ScriptName` =  'npc_relentless_storm' WHERE `entry` = 47807;
UPDATE `creature_template` SET  `ScriptName` =  'npc_relentless_storm' WHERE `entry` = 47806;
UPDATE `creature_template` SET  `ScriptName` =  'npc_wind_funnel' WHERE `entry` = 48855;
UPDATE `creature_template` SET  `ScriptName` =  'npc_wind_funnel' WHERE `entry` = 48854;
UPDATE `creature_template` SET  `ScriptName` =  'npc_squall_line' WHERE `entry` = 48852;
UPDATE `creature_template` SET  `ScriptName` =  'npc_squall_line' WHERE `entry` = 47034;




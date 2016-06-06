DELETE FROM `creature` WHERE `map`=720 AND `guid`=337570 AND `id`=53167;
DELETE FROM `creature_addon` WHERE `guid`=337570;
DELETE FROM `creature` WHERE `map`=720 AND `guid`=341062 AND `id`=53141;
DELETE FROM `creature_addon` WHERE `guid`=341062;
DELETE FROM `creature` WHERE `map`=720 AND `guid`=337537 AND `id`=53141;
DELETE FROM `creature_addon` WHERE `guid`=337537;
DELETE FROM `creature` WHERE `map`=720 AND `guid`=337565 AND `id`=53141;
DELETE FROM `creature_addon` WHERE `guid`=337565;
DELETE FROM `creature` WHERE `map`=720 AND `guid`=341471 AND `id`=53167;
DELETE FROM `creature_addon` WHERE `guid`=341471;
DELETE FROM `creature` WHERE `map`=720 AND `guid`=341104 AND `id`=54161;
DELETE FROM `creature_addon` WHERE `guid`=341104;

UPDATE creature_template SET unit_flags=unit_flags & ~33554688 where entry in (
35592,
42098,
45979,
47242,
52409,
52447,
52498,
52524,
52530,
52558,
52571,
52577,
52581,
52593,
52659,
52672,
53086,
53087,
53094,
53095,
53096,
53102,
53115,
53116,
53119,
53120,
53121,
53127,
53128,
53129,
53130,
53134,
53141,
53167,
53178,
53185,
53186,
53187,
53188,
53206,
53209,
53216,
53222,
53223,
53224,
53231,
53237,
53244,
53266,
53361,
53369,
53393,
53395,
53420,
53433,
53435,
53467,
53474,
53487,
53488,
53490,
53492,
53494,
53495,
53529,
53545,
53575,
53585,
53617,
53619,
53621,
53630,
53639,
53640,
53648,
53680,
53691,
53723,
53732,
53784,
53787,
53788,
53789,
53792,
53794,
53795,
53900,
53901,
53914,
53952,
54015,
54019,
54020,
54073,
54143,
54161,
54274,
54275,
54276,
54277,
54299,
54348,
54367,
54401,
54402,
54563) and unit_flags & 33554688;

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry` IN 
(52053, 52054, 53134, 53691, 52142, 53979, 54079, 54080, 52393, 52155, 52151, 52059, 52053, 52148,
52409, 53797, 53798, 53799, 52498, 53576, 53577, 53578, 52530, 54044, 54045, 54046, 52558, 52559,
52560, 52561, 52571, 53856, 53857, 53858, 53494, 53587, 53588, 53589, 53096, 54151, 54161, 54162
);

UPDATE `creature_template` SET `ScriptName`='npc_firelands_ancient_core_hound' WHERE `entry`=53134;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_ancient_lava_dweller' WHERE `entry`=53130;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_fire_scorpion' WHERE `entry`=53127;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_fire_turtle_hatchling' WHERE `entry`=53096;
UPDATE `creature_template` SET `ScriptName`='npc_firelands_flame_archon' WHERE `entry`=54161;

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(99693, 'spell_firelands_ancient_core_hound_dinner_time'),
(99736, 'spell_firelands_ancient_core_hound_flame_breath'),
(97549, 'spell_firelands_ancient_lava_dweller_lava_shower'),
(100263, 'spell_firelands_fire_turtle_hatchling_shell_spin'),
(100797, 'spell_firelands_flame_archon_fiery_torment');
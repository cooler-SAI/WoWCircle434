
DELETE FROM `creature` WHERE `id` IN 
(53450, 53474, 53642, 53635,
53631, 53745, 53082, 53433,
53450,  53474, 52524,
52581, 52447, 53745, 53082, 
53433, 53178, 53492, 53490);

UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN 
(53450, 53474, 53642, 53635,
53631, 53745, 53082, 53433,
53450,  53474, 52524,
52581, 52447, 53745, 53082, 
53433, 53178, 53492, 53490);

UPDATE `creature` SET `spawnmask`=15 WHERE `id`=52498;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=53082 AND `spell_id`=98997;

UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=53082;
UPDATE `creature_template` SET `scale`=1 WHERE `entry` IN (52524, 53599, 53600, 53601);

UPDATE `creature_template` SET `unit_class`=2 WHERE `entry` IN 
(52498, 53576, 53577, 53578,
52581, 53582, 53584, 53584);

UPDATE `gameobject_template` SET `flags`=36 WHERE `entry`=208877;

UPDATE `creature` SET `spawnmask`=15 WHERE `id`=52498;

UPDATE `creature_template` SET `lootid`=0 WHERE `entry` IN (52498, 53576, 53577, 53578);

UPDATE `creature_template` SET `ScriptName`='boss_bethtilac' WHERE `entry`=52498;
UPDATE `creature_template` SET `ScriptName`='npc_bethtilac_spiderweb_filament' WHERE `entry`=53082;
UPDATE `creature_template` SET `ScriptName`='npc_bethtilac_cinderweb_spinner' WHERE `entry`=52524;
UPDATE `creature_template` SET `ScriptName`='npc_bethtilac_cinderweb_drone' WHERE `entry`=52581;
UPDATE `creature_template` SET `ScriptName`='npc_bethtilac_cinderweb_spiderling' WHERE `entry`=52447;
UPDATE `creature_template` SET `ScriptName`='npc_bethtilac_engorged_broodling' WHERE `entry`=53745;

DELETE FROM `spell_script_names` WHERE `spell_id` IN 
(99052, 98934, 100648, 100834, 100835,
98471, 100826, 100827, 100828);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(99052, 'spell_bethtilac_smoldering_devastation'),
(98934, 'spell_bethtilac_ember_flare'),
(100648, 'spell_bethtilac_ember_flare'),
(100834, 'spell_bethtilac_ember_flare'),
(100835, 'spell_bethtilac_ember_flare'),
(98471, 'spell_bethtilac_burning_acid'),
(100826, 'spell_bethtilac_burning_acid'),
(100827, 'spell_bethtilac_burning_acid'),
(100828, 'spell_bethtilac_burning_acid');

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (17542, 18096);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(17542, 11, 0, 0, 'achievement_death_from_above');
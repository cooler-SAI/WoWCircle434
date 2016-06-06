DELETE FROM `spell_script_names` WHERE `spell_id` IN (103434, 104599, 104600, 104601, 105367);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(103434, 'spell_warlord_zonozz_disrupting_shadows'),
(104599, 'spell_warlord_zonozz_disrupting_shadows'),
(104600, 'spell_warlord_zonozz_disrupting_shadows'),
(104601, 'spell_warlord_zonozz_disrupting_shadows'),
(105367, 'spell_hagara_the_stormbinder_lightning_conduit');

DELETE FROM `conditions` WHERE `SourceEntry`=104031;
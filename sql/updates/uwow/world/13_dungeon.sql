UPDATE `creature_template` SET  `ScriptName` =  'npc_vapor' WHERE `entry` in ('49390', '47714');
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_watcher' WHERE `entry` in ('47404', '47403');
UPDATE `creature_template` SET  `ScriptName` =  'mob_cookie_cauldron' WHERE `entry` = '47754';
UPDATE `creature_template` SET  `ScriptName` =  'npc_lumbering_oaf' WHERE `entry` = '47297';
UPDATE `creature_template` SET  `ScriptName` =  'npc_rope_ship' WHERE `entry` = '49552';
UPDATE `creature_template` SET  `ScriptName` =  'npc_vanessa_intro' WHERE `entry` = '49429';
UPDATE `creature_template` SET  `ScriptName` =  'npc_note' WHERE `entry` = '49564';
UPDATE `creature_template` SET  `ScriptName` =  'npc_magma_pull' WHERE `entry` = '49454';
UPDATE `creature_template` SET  `ScriptName` =  'npc_steam_valve' WHERE `entry` = '49457';
UPDATE `creature_template` SET  `ScriptName` =  'npc_vanessa_nightmare' WHERE `entry` = '49671';
UPDATE `creature_template` SET  `ScriptName` =  'npc_icycle_dm' WHERE `entry` = '49481';
UPDATE `creature_template` SET  `ScriptName` =  'npc_glubtok_dm' WHERE `entry` = '49670';
UPDATE `creature_template` SET  `ScriptName` =  'npc_helix_dm' WHERE `entry` = '49674';
UPDATE `creature_template` SET  `ScriptName` =  'npc_mechanical_dm' WHERE `entry` = '49681';
UPDATE `creature_template` SET  `ScriptName` =  'npc_enraged_worgen_dm' WHERE `entry` = '49532';
UPDATE `creature_template` SET  `ScriptName` =  'npc_james_dm' WHERE `entry` = '49539';
UPDATE `creature_template` SET  `ScriptName` =  'npc_rope_away' WHERE `entry` = '49550';
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_shadowguard' WHERE `entry` = '49505';
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_enforcer' WHERE `entry` = '49502';
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_bloodwizard' WHERE `entry` = '48417';
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_envoker' WHERE `entry` = '48418';
UPDATE `creature_template` SET  `ScriptName` =  'npc_oaf_lackey' WHERE `entry` = '48445';
UPDATE `creature_template` SET  `ScriptName` =  'npc_goblin_overseer' WHERE `entry` = '48279';
UPDATE `creature_template` SET  `ScriptName` =  'npc_ogre_henchman' WHERE `entry` = '48230';
UPDATE `creature_template` SET  `ScriptName` =  'npc_lightning_orbs' WHERE `entry` = '49520';
UPDATE `creature_template` SET  `ScriptName` =  'npc_vanessa_anchor_bunny' WHERE `entry` = '51624';
UPDATE `creature_template` SET  `ScriptName` =  'npc_defias_cannon' WHERE `entry` = '48266';
UPDATE `creature_template` SET  `ScriptName` =  'npc_mining_powder' WHERE `entry` = '48284';
UPDATE `creature_template` SET  `ScriptName` =  'npc_mining_monkey' WHERE `entry` in ('48278', '48440');
UPDATE `creature_template` SET  `ScriptName` =  'npc_deadmines_bird' WHERE `entry` in ('48447', '48450');
UPDATE `creature_template` SET  `ScriptName` =  'npc_goblin_engineer' WHERE `entry` in ('622', '1731');
UPDATE `creature_template` SET  `ScriptName` =  'npc_glubtok_secondary_platter' WHERE `entry` in ('48975', '49039', '49040');
UPDATE `creature_template` SET  `ScriptName` =  'npc_glubtok_main_platter' WHERE `entry` = '48974';


UPDATE `gameobject_template` SET `ScriptName` = 'go_defias_cannon' WHERE `entry` = '16398';
UPDATE `gameobject_template` SET `ScriptName` = 'go_heavy_door' WHERE `entry` = '17154';
UPDATE `gameobject_template` SET `ScriptName` = 'go_deadmines_tp' WHERE `entry` = '19072';


REPLACE INTO `spell_script_names` VALUES
('92042', 'spell_coalesce_achievement'),
('87897', 'spell_glubtok_generic_proc'),
('87900', 'spell_glubtok_generic_proc'),
('91397', 'spell_glubtok_firewall_targetting');

REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
('48092', '46598', '1', '0');

REPLACE INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
('16210', '11', '0', '0', 'achievement_prototype_reaper');

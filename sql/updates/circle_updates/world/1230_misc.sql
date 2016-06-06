
DELETE FROM `creature` WHERE `id` IN (50613, 50615, 50503, 50085, 50934, 51047, 51080, 52065, 50439, 50522) AND `map`=33;

UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN ( 46962, 3887, 4278, 46963, 46964, 50613, 50615, 50503, 50085, 50934, 51047, 51080, 52065, 50439, 50522);

UPDATE `creature_addon` SET `auras`='' WHERE `guid` IN 
(325854, 325870, 325936, 327218,
325939, 325940, 371486, 327200,
327193, 327195, 327199, 327196,
327181, 327197, 327187, 327309,
327191, 327207, 327210);

UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry` IN (36296, 36272, 36565);

UPDATE `creature_template` SET `ScriptName`='boss_baron_ashbury' WHERE `entry`=46962;
UPDATE `creature_template` SET `ScriptName`='boss_baron_silverlaine' WHERE `entry`=3887;
UPDATE `creature_template` SET `ScriptName`='boss_commander_springvale' WHERE `entry`=4278;
UPDATE `creature_template` SET `ScriptName`='boss_lord_valden' WHERE `entry`=46963;
UPDATE `creature_template` SET `ScriptName`='boss_lord_godfrey' WHERE `entry`=46964;
UPDATE `creature_template` SET `ScriptName`='npc_apothecary_hummel' WHERE `entry`=36296;

UPDATE `creature_template` SET `ScriptName`='npc_wailing_guardsman' WHERE `entry`=50613;
UPDATE `creature_template` SET `ScriptName`='npc_tormented_officer' WHERE `entry`=50615;
UPDATE `creature_template` SET `ScriptName`='npc_springvale_desecration_bunny' WHERE `entry`=50503;
UPDATE `creature_template` SET `ScriptName`='npc_silverlaine_worgen_spirit' WHERE `entry`=50085;
UPDATE `creature_template` SET `ScriptName`='npc_silverlaine_worgen_spirit' WHERE `entry`=50934;
UPDATE `creature_template` SET `ScriptName`='npc_silverlaine_worgen_spirit' WHERE `entry`=51047;
UPDATE `creature_template` SET `ScriptName`='npc_silverlaine_worgen_spirit' WHERE `entry`=51080;
UPDATE `creature_template` SET `ScriptName`='npc_godfrey_pistol_barrage' WHERE `entry`=52065;
UPDATE `creature_template` SET `ScriptName`='npc_valden_mystery_toxin' WHERE `entry`=50439;
UPDATE `creature_template` SET `ScriptName`='npc_valden_mystery_toxin' WHERE `entry`=50522;

REPLACE INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(16081, 11, 0, 0, 'achievement_pardon_denied');

DELETE FROM `spell_script_names` WHERE `spell_id` IN (93857, 93707, 93617);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(93857, 'spell_silverlaine_summon_worgen_spirit'),
(93707, 'spell_godfrey_summon_bloodthirsty_ghouls'),
(93617, 'spell_valden_toxic_coagulant');

UPDATE `instance_template` SET `script`='instance_shadowfang_keep' WHERE `map`=33;
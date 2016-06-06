UPDATE `creature_template` SET `faction_a`=16, `faction_h`=16 WHERE `entry`=23574;
UPDATE `creature_template` SET `faction_a`=16, `faction_h`=16 WHERE `entry`=23578;
UPDATE `creature_template` SET `faction_a`=16, `faction_h`=16 WHERE `entry`=24239;
UPDATE `creature_template` SET `ScriptName`='boss_hex_lord_malacrass' WHERE `entry`=24239;
UPDATE `creature_template` SET `ScriptName`='npc_gazakroth' WHERE `entry`=24244;
UPDATE `creature_template` SET `ScriptName`='npc_darkheart' WHERE `entry`=24246;
UPDATE `creature_template` SET `ScriptName`='npc_slither' WHERE `entry`=24242;
UPDATE `creature_template` SET `ScriptName`='npc_alyson_antille' WHERE `entry`=24240;

DELETE FROM `spell_script_names` WHERE `spell_id`=43522;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (43522, 'spell_hexlord_unstable_affliction');

DELETE FROM `conditions` WHERE `SourceEntry`=82699;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '44132', '31', '1', '24239', 'Drain Power');

UPDATE `creature_template` SET `ScriptName`='boss_halfus_wyrmbreaker' WHERE `entry`=44600;
UPDATE `creature_template` SET `ScriptName`='boss_theralion' WHERE `entry`=45993;
UPDATE `creature_template` SET `ScriptName`='boss_valiona' WHERE `entry`=45992;

UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, `faction_A`=16, `faction_H`=16, `flags_extra`=128 WHERE `entry`=47501;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, `faction_A`=16, `faction_H`=16, `flags_extra`=128 WHERE `entry`=47040;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, `faction_A`=16, `faction_H`=16, `flags_extra`=128 WHERE `entry`=41264;
UPDATE `creature_template` SET `minlevel`=87, `maxlevel`=87, `faction_A`=16, `faction_H`=16, `flags_extra`=128 WHERE `entry`=49432;
UPDATE `creature_template` SET `ScriptName`='npc_ignacious_inferno_rush' WHERE `entry`=47501;
UPDATE `creature_template` SET `ScriptName`='npc_ignacious_inferno_leap' WHERE `entry`=47040;
UPDATE `creature_template` SET `ScriptName`='npc_feludius_water_bomb' WHERE `entry`=41264;
UPDATE `creature_template` SET `ScriptName`='npc_ignacious_flame_strike' WHERE `entry`=41264;

DELETE FROM `spell_script_names` WHERE `spell_id`=82860;
DELETE FROM `spell_script_names` WHERE `spell_id`=92523;
DELETE FROM `spell_script_names` WHERE `spell_id`=92524;
DELETE FROM `spell_script_names` WHERE `spell_id`=92525;
INSERT INTO `spell_script_names` VALUES(82860, 'spell_ignacious_inferno_rush');
INSERT INTO `spell_script_names` VALUES(92523, 'spell_ignacious_inferno_rush');
INSERT INTO `spell_script_names` VALUES(92524, 'spell_ignacious_inferno_rush');
INSERT INTO `spell_script_names` VALUES(92525, 'spell_ignacious_inferno_rush');

DELETE FROM `conditions` WHERE `SourceEntry`=82699;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '82699', '31', '1', '41264', 'Water Bomb');

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry`=43687;
UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry`=49612;
UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry`=49613;
UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry`=49614;

REPLACE INTO `spell_linked_spell` VALUES 
(82665, 82666, 2, 'Feludius - Heart of Ice + Frost Imbued'),
(82660, 82663, 2, 'Ignacious - Burning Blood + Flame Imbued');

DELETE FROM `spell_script_names` WHERE `spell_id` IN (83718, 92541, 92542, 92543);
INSERT INTO `spell_script_names` VALUES
(83718, 'spell_terrastra_harden_skin'),
(92541, 'spell_terrastra_harden_skin'),
(92542, 'spell_terrastra_harden_skin'),
(92543, 'spell_terrastra_harden_skin');
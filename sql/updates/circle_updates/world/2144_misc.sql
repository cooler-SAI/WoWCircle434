DELETE FROM `spell_bonus_data` WHERE `entry` IN (21991,23781,60483);
INSERT INTO `spell_bonus_data` VALUES
(21991, 0, 0, 0, 0, 'Talisman of Binding (item - Talisman of Binding Shard)'),
(23781, 0, 0, 0, 0, 'Aegis Heal (item - Aegis of Preservation)'),
(60483, 0, 0, 0, 0, 'Pendulum of Telluric Currents');

UPDATE `creature_template` SET `ScriptName` = 'npc_shahram' WHERE `entry` = 10718;

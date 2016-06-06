DELETE FROM creature_template_addon WHERE entry IN (28525, 28543, 28542, 28544);
INSERT INTO creature_template_addon VALUES (28525, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28543, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28544, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28542, 0, 0, 0, 1, 0, 64328);

UPDATE creature_template SET flags_extra = 0, scale = 4 WHERE entry IN (28525, 28543, 28542, 28544);


UPDATE `creature_template` SET `ScriptName` = 'npc_shahram' WHERE `entry` = 10718;

DELETE FROM `spell_bonus_data` WHERE `entry` IN (21991,23781,60483);
INSERT INTO `spell_bonus_data` VALUES
(21991, 0, 0, 0, 0, 'Talisman of Binding (item - Talisman of Binding Shard)'),
(23781, 0, 0, 0, 0, 'Aegis Heal (item - Aegis of Preservation)'),
(60483, 0, 0, 0, 0, 'Pendulum of Telluric Currents');

UPDATE `item_template` SET `flagsCustom` = 16 WHERE `entry` IN (45784,45817,45786,45815,45787,45816,45788,45814,45897);

UPDATE `achievement_criteria_data` SET `ScriptName` = 'achievement_orbital_devastation' WHERE `criteria_id` = 10059 AND `type` = 11;
UPDATE `achievement_criteria_data` SET `ScriptName` = 'achievement_orbital_bombardment' WHERE `criteria_id` = 10061 AND `type` = 11;
UPDATE `achievement_criteria_data` SET `ScriptName` = 'achievement_nuked_from_orbit' WHERE `criteria_id` = 10060 AND `type` = 11;

DELETE FROM `creature` WHERE `id` = 32871;

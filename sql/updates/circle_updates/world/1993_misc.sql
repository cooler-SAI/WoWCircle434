

UPDATE `creature_template` SET `mechanic_immune_mask`=617299967 WHERE `entry` IN 
(57160, 57294, 57159, 57293, 57158, 57292, 57280, 57296, 57231, 57295);

UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_ancient_water_lord' WHERE `entry`=57160;
UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_earthen_destroyer' WHERE `entry`=57158;
UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_earthen_soldier' WHERE `entry`=57159;
UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_twilight_siege_captain' WHERE `entry`=57280;
UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_twilight_portal' WHERE `entry`= 57231;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (107796, 107596);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(107796, 'spell_dragon_soul_ancient_water_lord_flood'),
(107596, 'spell_dragon_soul_earthen_destroyer_boulder_smash');

UPDATE `gameobject_template` SET `faction`=0 WHERE `entry`=206576;
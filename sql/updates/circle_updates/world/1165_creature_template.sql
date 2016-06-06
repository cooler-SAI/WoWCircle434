UPDATE `creature_template` SET `unit_class`=8, `spell1`=0 WHERE `entry`=53432;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_burning_treant' WHERE `entry`=53432;

DELETE FROM `creature_onkill_reputation` WHERE `creature_id`=14881;
DELETE FROM `creature` WHERE `id`=47128;
UPDATE `creature_template` SET `equipment_id`=130842, `ScriptName`='npc_guardian_of_ancient_kings' WHERE `entry`=46490;
UPDATE `creature_template` SET `equipment_id`=130841, `ScriptName`='npc_guardian_of_ancient_kings' WHERE `entry`=46400;
UPDATE `creature_template` SET `equipment_id`=130843, `ScriptName`='npc_guardian_of_ancient_kings' WHERE `entry`=46506;

REPLACE INTO `creature_equip_template` (`entry`, `itementry1`, `itementry2`, `itementry3`) 
VALUES (130841, 46017, 47085, 0);
REPLACE INTO `creature_equip_template` (`entry`, `itementry1`, `itementry2`, `itementry3`) 
VALUES (130842, 47526, 46964, 0);
REPLACE INTO `creature_equip_template` (`entry`, `itementry1`, `itementry2`, `itementry3`) 
VALUES (130843, 47515, 0, 0);
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=50056;
DELETE FROM `smart_scripts` WHERE `entryorguid`=50056;
DELETE FROM `smart_scripts` WHERE `entryorguid`=50055;
UPDATE `creature_template` SET `AIName`='', ScriptName='npc_garr' WHERE `entry`=50056;
UPDATE `creature_template` SET `AIName`='', ScriptName='npc_garr_firesworn' WHERE `entry`=50055;
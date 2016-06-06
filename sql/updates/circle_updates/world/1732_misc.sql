UPDATE `creature` SET `npcflag`=1 WHERE `guid`=51788;
UPDATE `creature_template` SET `npcflag`=0 WHERE `entry`=10162;
REPLACE INTO `instance_template` (`map`, `parent`, `script`, `allowMount`) VALUES 
(469, 229, 'instance_blackwing_lair', 0);


INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 32307, 0, 0, 4, 0, 3518, 0, 0, 0, 0, '', 'Plant Warmaul Ogre Banner');

DELETE FROM `conditions` WHERE `SourceEntry`=64306;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 64306, 0, 0, 31, 1, 3, 33206, 0, 0, 0, '', 'Call Withered Ent');


DELETE FROM `conditions` WHERE `SourceEntry`=75342;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '3', '75342', '0', '31', '0',  '3', '40174', 'Toshe\'s Hunting Spears');

DELETE FROM `conditions` WHERE `SourceEntry`=78499;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '3', '78499', '0', '31', '0',  '3', '42003', 'Iron Hammer Bomb'),
('13', '3', '78499', '1', '31', '0',  '3', '41924', 'Iron Hammer Bomb'),
('13', '3', '78499', '2', '31', '0',  '3', '42012', 'Iron Hammer Bomb'),
('13', '3', '78499', '3', '31', '0',  '3', '41902', 'Iron Hammer Bomb');


DELETE FROM `conditions` WHERE `SourceEntry`=88914;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 88914, 0, 0, 31, 1, 3, 46141, 0, 0, 0, '', 'Wyrmhunter Hooks');

DELETE FROM `conditions` WHERE `SourceEntry`=75342;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '3', '85555', '0', '31', '0',  '3', '45851', 'Burn Corpse');

DELETE FROM `creature_loot_template` WHERE `item` IN (63128, 63127, 52843, 64392, 64394, 64396, 64395, 64397);

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(79627, 0, 0, 0, 0, 'Deathblood Venom - Poison');


DELETE FROM `conditions` WHERE `SourceEntry`=81372;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 81372, 0, 0, 31, 1, 3, 7013, 0, 0, 0, '', 'Chloroform'),
(17, 0, 81372, 0, 1, 31, 1, 3, 43535, 0, 0, 0, '', 'Chloroform');

DELETE FROM `conditions` WHERE `SourceEntry`=82579;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 82579, 0, 0, 31, 1, 3, 7013, 0, 0, 0, '', 'Chloroform'),
(17, 0, 82579, 0, 1, 31, 1, 3, 43535, 0, 0, 0, '', 'Chloroform');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=47219 AND `spell_id`=87987;
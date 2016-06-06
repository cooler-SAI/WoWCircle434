DELETE FROM `conditions` WHERE `SourceEntry`=91647;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 91647, 0, 0, 31, 1, 3, 49177, 0, 0, 0, '', 'Tanrir\'s Overcharged Totem - Totem Overcharge');

UPDATE `npc_spellclick_spells` SET `spell_id`=88068 WHERE `npc_entry`=47216 AND `spell_id`=87986;
UPDATE `npc_spellclick_spells` SET `spell_id`=88068 WHERE `npc_entry`=47213 AND `spell_id`=87984;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=36756 AND `spell_id`=45801;
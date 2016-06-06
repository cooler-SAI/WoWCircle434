DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_pri_vampiric_touch';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (34914, 'spell_pri_vampiric_touch');

DELETE FROM `spell_proc_event` WHERE `entry` IN (78202, 78203, 78204);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(78202, 0, 6, 32768, 0, 0, 0, 1027, 0, 100, 0),
(78203, 0, 6, 32768, 0, 0, 0, 1027, 0, 100, 0),
(78204, 0, 6, 32768, 0, 0, 0, 1027, 0, 100, 0);

DELETE FROM `spell_bonus_data` WHERE `entry` IN (589, 2944, 34914);
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(589, -1, 0.161, -1, -1, 'Priest - Shadow Word: Pain'),
(2944, -1, 0.211, -1, -1, 'Priest - Devouring Plague'),
(34914, -1, 0.352, -1, -1, 'Priest - Vampiric Touch');

UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=19668;
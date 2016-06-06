UPDATE `creature_template` SET `ScriptName`='npc_frostfire_orb' WHERE `entry`=45322;
UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=44214;
UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=45322;

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(83619, 0.193, 0, 0, 0, 'Mage - FIre Power'),
(82739, 0.134, 0, 0, 0, 'Mage - Flame Orb'),
(95969, 0.134, 0, 0, 0, 'Mage - Frostfire Orb rank 1'),
(84721, 0.134, 0, 0, 0, 'Mage - Frostfire Orb rank 2');
UPDATE `spell_proc_event` SET `SpellFamilyMask2`=16512 WHERE `entry`=44545;
UPDATE `spell_proc_event` SET `SpellFamilyMask2`=16512 WHERE `entry`=44543;
UPDATE `spell_proc_event` SET `SpellFamilyMask2`=16512 WHERE `entry`=83074;
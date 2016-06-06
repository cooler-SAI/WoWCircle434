-- Wrecking Crew proc fix

DELETE FROM `spell_proc_event` WHERE (`entry`='56612'); -- rank 3 don't exsist anymore
REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) 
VALUES 
('46867', '0', '4', '33554432', '0', '0', '0', '2', '0', '50', '0'), -- rank 1
('56611', '0', '4', '33554432', '0', '0', '0', '2', '0', '100', '0'); -- rank 2
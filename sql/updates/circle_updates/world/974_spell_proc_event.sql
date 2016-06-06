DELETE FROM `spell_proc_event` WHERE `entry`=28176;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(28176, 0, 5, 0, 0, 0, 0, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry` IN (47245, 47246, 47247);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(47245, 0, 5, 4, 0, 0, 0, 1027, 0, 2, 0),
(47246, 0, 5, 4, 0, 0, 0, 1027, 0, 4, 0),
(47247, 0, 5, 4, 0, 0, 0, 1027, 0, 6, 0);
-- Item  Warrior T12 Protection 2P Bonus
DELETE FROM `spell_proc_event` WHERE `entry`=99239;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(99239, 0, 4, 0, 512, 0, 16, 1027, 0, 100, 0);

-- Item  Warrior T12 DPS 4P Bonus
DELETE FROM `spell_proc_event` WHERE `entry`=99295;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(99295, 0, 4, 33554432, 67117056, 0, 16, 1027, 0, 0, 0);
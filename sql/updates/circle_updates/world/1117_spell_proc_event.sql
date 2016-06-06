DELETE FROM `spell_proc_event` WHERE `entry` IN (75171, 75177, 94746, 94747, 95653);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(75171, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 50),
(75177, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 50),
(94746, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 47),
(94747, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 47),
(95653, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 50);
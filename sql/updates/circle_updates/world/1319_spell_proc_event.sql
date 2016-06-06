DELETE FROM `spell_proc_event` WHERE `entry` IN (96947, 97130);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(96947, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 60),
(97130, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 60);
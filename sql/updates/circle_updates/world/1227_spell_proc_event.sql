DELETE FROM `spell_proc_event` WHERE `entry` IN (16180, 16196);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(16180, 0, 11, 448, 0, 327696, 0, 1026, 0, 100, 0),
(16196, 0, 11, 448, 0, 327696, 0, 1026, 0, 100, 0);
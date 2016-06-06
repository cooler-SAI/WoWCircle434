REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(105552, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45),
(105908, 0, 4, 1024, 0, 0, 0, 0, 0, 0, 0),
(75174, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45);

DELETE FROM `disables` WHERE `sourceType`=0 AND `entry`=1706;
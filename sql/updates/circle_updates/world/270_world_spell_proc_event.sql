DELETE FROM `spell_proc_event` WHERE `entry`=50685;
DELETE FROM `spell_proc_event` WHERE `entry`=50686;
DELETE FROM `spell_proc_event` WHERE `entry`=50687;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (50685, 0, 4, 64, 0, 0, 16, 2, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (50686, 0, 4, 64, 0, 0, 16, 2, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (50687, 0, 4, 64, 0, 0, 16, 2, 0, 0, 0);

UPDATE `spell_bonus_data` SET `ap_bonus`=0.265 WHERE `entry`=6343;
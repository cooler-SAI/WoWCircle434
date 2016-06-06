DELETE FROM `spell_proc_event` WHERE `entry`=14910;
DELETE FROM `spell_proc_event` WHERE `entry`=33371;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (14910, 0, 6, 0, 0, 65536, 65536, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (33371, 0, 6, 0, 0, 65536, 65536, 1027, 0, 0, 0);

DELETE FROM `spell_bonus_data` WHERE `entry`=8092;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (8092, 0.9858, -1, -1, -1, 'Priest - Mind Blast');
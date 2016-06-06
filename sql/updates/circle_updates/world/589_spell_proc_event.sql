DELETE FROM `spell_bonus_data` WHERE `entry`=79136;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (79136, 0, 0, 0.176, 0, 'Rogue - Venomous Wounds');

DELETE FROM `spell_proc_event` WHERE `entry`=79133;
DELETE FROM `spell_proc_event` WHERE `entry`=79134;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (79133, 0, 8, 1048832, 0, 0, 262144, 1027, 0, 30, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (79134, 0, 8, 1048832, 0, 0, 262144, 1027, 0, 60, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=51625;
DELETE FROM `spell_proc_event` WHERE `entry`=51626;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51625, 0, 8, 268476416, 0, 0, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51626, 0, 8, 268476416, 0, 0, 0, 1027, 0, 0, 0);
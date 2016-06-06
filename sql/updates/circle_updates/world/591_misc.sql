DELETE FROM `spell_proc_event` WHERE `entry`=51698;
DELETE FROM `spell_proc_event` WHERE `entry`=51700;
DELETE FROM `spell_proc_event` WHERE `entry`=51701;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51698, 0, 0, 0, 0, 0, 349456, 1026, 0, 33, 4);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51700, 0, 0, 0, 0, 0, 349456, 1026, 0, 66, 4);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51701, 0, 0, 0, 0, 0, 349456, 1026, 0, 100, 4);

DELETE FROM `spell_bonus_data` WHERE `entry`=774;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (774, -1, 0.154, -1, -1, 'Druid - Rejuvenation');
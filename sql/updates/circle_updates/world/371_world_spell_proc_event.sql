DELETE FROM `spell_proc_event` WHERE `entry`=85126;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85126, 1, 10, 0, 0, 0, 20, 1027, 0, 100, 0);

DELETE FROM `spell_bonus_data` WHERE `entry`=25742;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (25742, 0, 0, 0, 0, 'Paladin - Seal of Righteousness Dummy Proc');

DELETE FROM `spell_proc_event` WHERE `entry`=20154;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (20154, 1, 0, 0, 0, 0, 0, 1027, 0, 0, 0);

DELETE FROM `spell_bonus_data` WHERE `entry`=20187;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (20187, 0.32, -1, 0.2, 0, 'Paladin - Judgement of Righteousness');

DELETE FROM `spell_bonus_data` WHERE `entry`=54158;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (54158, 0.25, -1, 0.16, 0, 'Paladin - Judgement (Seal of Insight, Seal of Justice)');

DELETE FROM `spell_bonus_data` WHERE `entry`=31804;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (31804, 0.223, -1, 0.142, 0, 'Paladin - Judgement of Truth');

DELETE FROM `spell_bonus_data` WHERE `entry`=20170;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (20170, 0, 0, 0, 0, 'Paladin - Seal of Righteousness Dummy Proc');

DELETE FROM `spell_proc_event` WHERE `entry`=20164;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (20164, 1, 0, 0, 0, 0, 0, 1027, 0, 100, 0);

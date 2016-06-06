DELETE FROM `spell_proc_event` WHERE `entry`=87168;
DELETE FROM `spell_proc_event` WHERE `entry`=87172;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (87168, 0, 10, 8388608, 4194312, 16778240, 272, 0, 0, 50, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (87172, 0, 10, 8388608, 4194312, 16778240, 272, 0, 0, 100, 0);
DELETE FROM `spell_proc_event` WHERE `entry` IN (89901, 31878, 31876);

DELETE FROM `spell_bonus_data` WHERE `entry`=20187;
DELETE FROM `spell_bonus_data` WHERE `entry`=31804;
DELETE FROM `spell_bonus_data` WHERE `entry`=54158;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (20187, 0.32, -1, 0.2, 0, 'Paladin - Judgement of Righteousness');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (31804, 0.223, -1, 0.14, 0, 'Paladin - Judgement of Vengeance');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (54158, 0.25, -1, 0.16, 0, 'Paladin - Judgement (Seal of Light, Seal of Wisdom, Seal of Justice)');

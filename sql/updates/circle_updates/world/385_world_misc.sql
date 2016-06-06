DELETE FROM `spell_bonus_data` WHERE `entry`=87188;
DELETE FROM `spell_bonus_data` WHERE `entry`=87189;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (87188, 0.11, 0, 0, 0, 'Paladin - Enlightened Judgements Rank 1');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (87189, 0.22, 0, 0, 0, 'Paladin - Enlightened Judgements Rank 2');

DELETE FROM `spell_bonus_data` WHERE `entry`=94286;
DELETE FROM `spell_bonus_data` WHERE `entry`=94288;
DELETE FROM `spell_bonus_data` WHERE `entry`=94289;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (94286, 0.02, 0, 0, 0, 'Paladin - Protector of the Innocent Rank 1');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (94288, 0.04, 0, 0, 0, 'Paladin - Protector of the Innocent Rank 2');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (94289, 0.06, 0, 0, 0, 'Paladin - Protector of the Innocent Rank 3');

DELETE FROM `spell_proc_event` WHERE `entry`=53557;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (53557, 0, 0, 8388608, 0, 0, 0, 1027, 0, 0, 0);
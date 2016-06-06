DELETE FROM `spell_bonus_data` WHERE `entry`=85673;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (85673, 0, 0, 0, 0, 'Paladin - Word of Glory');

DELETE FROM `spell_proc_event` WHERE `entry`=85117;
DELETE FROM `spell_proc_event` WHERE `entry`=86172;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85117, 0, 10, 8388608, 2228354, 40960, 87376, 1027, 0, 7, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (86172, 0, 10, 8388608, 2228354, 40960, 87376, 1027, 0, 15, 0);

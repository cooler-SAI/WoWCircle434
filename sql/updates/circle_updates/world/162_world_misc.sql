DELETE FROM `spell_proc_event` WHERE `entry`=53569;
DELETE FROM `spell_proc_event` WHERE `entry`=53576;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (53569, 0, 10, 2097152, 65536, 0, 0, 2, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (53576, 0, 10, 2097152, 65536, 0, 0, 2, 0, 0, 0);

DELETE FROM `spell_bonus_data`	WHERE `entry`=31935;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (31935, 0.21, -1, 0.41, -1, 'Paladin - Avenger Shield');
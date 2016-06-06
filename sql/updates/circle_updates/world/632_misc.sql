REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (1535, 'spell_sha_fire_nova');

DELETE FROM `spell_bonus_data` WHERE `entry`=8349;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (8349, 0.164, 0, 0, 0, 'Shaman - Fire Nova damage');

DELETE FROM `spell_proc_event` WHERE `entry`=77794;
DELETE FROM `spell_proc_event` WHERE `entry`=77795;
DELETE FROM `spell_proc_event` WHERE `entry`=77796;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77794, 0, 11, 2416967680, 0, 0, 65536, 1027, 0, 101, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77795, 0, 11, 2416967680, 0, 0, 65536, 1027, 0, 101, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77796, 0, 11, 2416967680, 0, 0, 65536, 1027, 0, 101, 0);
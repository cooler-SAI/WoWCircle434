REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (8042, 'spell_sha_fulmination');

DELETE FROM `spell_proc_event` WHERE `entry`=88756;
DELETE FROM `spell_proc_event` WHERE `entry`=88764;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (88756, 0, 11, 3, 2147483648, 32768, 65536, 1027, 0, 30, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (88764, 0, 11, 3, 2147483648, 32768, 65536, 1027, 0, 60, 0);

DELETE FROM `spell_bonus_data` WHERE `entry`=86961;
DELETE FROM `spell_bonus_data` WHERE `entry`=86958;
DELETE FROM `spell_bonus_data` WHERE `entry`=88767;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (86961, 0, 0, 0, 0, 'Shaman - Cleansing Waters rank 1');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (86958, 0, 0, 0, 0, 'Shaman - Cleansing Waters rank 2');
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (88767, 0, 0, 0, 0, 'Shaman - Fulmination damage');
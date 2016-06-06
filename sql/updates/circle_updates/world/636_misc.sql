DELETE FROM `spell_proc_event` WHERE `entry`=77655;
DELETE FROM `spell_proc_event` WHERE `entry`=77656;
DELETE FROM `spell_proc_event` WHERE `entry`=77657;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77655, 0, 11, 1073741824, 0, 0, 65536, 1027, 0, 33, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77656, 0, 11, 1073741824, 0, 0, 65536, 1027, 0, 66, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (77657, 0, 11, 1073741824, 0, 0, 65536, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=51525;
DELETE FROM `spell_proc_event` WHERE `entry`=51526;
DELETE FROM `spell_proc_event` WHERE `entry`=51527;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51525, 0, 11, 0, 0, 135172, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51526, 0, 11, 0, 0, 135172, 0, 1027, 0, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51527, 0, 11, 0, 0, 135172, 0, 1027, 0, 0, 0);

UPDATE `creature_template` SET `spell1`=3606 WHERE `entry`=2523;

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_sha_lava_lash';

DELETE FROM `spell_bonus_data` WHERE `entry`=77661;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (77661, 0, 0, 0, 0, 'Shaman - Searing Flame');
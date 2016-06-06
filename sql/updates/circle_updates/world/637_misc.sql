DELETE FROM `spell_proc_event` WHERE `entry`=82984;
DELETE FROM `spell_proc_event` WHERE `entry`=82988;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (82984, 0, 11, 1, 0, 0, 65536, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (82988, 0, 11, 1, 0, 0, 65536, 1027, 0, 100, 0);

DELETE FROM `spell_proc_event` WHERE `entry`=51528;
DELETE FROM `spell_proc_event` WHERE `entry`=51529;
DELETE FROM `spell_proc_event` WHERE `entry`=51530;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51528, 0, 0, 0, 0, 0, 0, 1027, 2.5, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51529, 0, 0, 0, 0, 0, 0, 1027, 5, 0, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51530, 0, 0, 0, 0, 0, 0, 1027, 7.5, 0, 0);

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_sha_flame_shock';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (8050, 'spell_sha_flame_shock');

DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_sha_totemic_wrath';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (77746, 'spell_sha_totemic_wrath');

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (-53817, -60349, 'Shaman - Maelstorm Weapon Visual');

UPDATE `creature_template` SET `spell2`=77746 WHERE `entry`=5950;
UPDATE `creature_template` SET `spell2`=77746 WHERE `entry`=15439;
UPDATE `creature_template` SET `spell2`=77746 WHERE `entry`=5929;
UPDATE `creature_template` SET `spell2`=77746 WHERE `entry`=2523;
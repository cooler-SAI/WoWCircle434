UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=29264;

DELETE FROM `spell_script_names` WHERE `spell_id`=51490;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (51490, 'spell_sha_thunderstorm');

DELETE FROM `spell_proc_event` WHERE `entry` IN (51522, 10400, 33757);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(33757, 0, 0, 0, 0, 0, 0, 1027, 0, 100, 3),
(10400, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 0),
(51522, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 0);

DELETE FROM `spell_bonus_data` WHERE `entry`=51490;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES (51490, 0.571, -1, -1, -1, 'Shaman - Thunderstorm');
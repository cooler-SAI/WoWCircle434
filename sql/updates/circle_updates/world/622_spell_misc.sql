UPDATE `creature_template` SET `spell1`=98007, `spell2`=98017 WHERE `entry`=53006;

DELETE FROM `spell_script_names` WHERE `spell_id`=98020;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (98020, 'spell_sha_spirit_link');
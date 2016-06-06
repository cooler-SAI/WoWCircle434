DELETE FROM `creature` WHERE `map`=632 AND `id` IN (36595, 37596, 36536);
DELETE FROM `creature` WHERE `map`=632 AND `id` IN (38161, 38160, 37583, 37774, 37779, 37582, 37584, 37587, 37588, 37496, 37497, 37094);

UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soulguard_watchman' WHERE `entry`=36478;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soulguard_reaper' WHERE `entry`=36499;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soulguard_animator' WHERE `entry`=36516;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soul_horror' WHERE `entry`=36522;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_spiteful_apparition' WHERE `entry`=36551;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soulguard_bonecaster' WHERE `entry`=36564;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_soulguard_adept' WHERE `entry`=36620;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_spectral_warden' WHERE `entry`=36666;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_jaina_fos' WHERE `entry`=37597;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (12976, 12752);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(12976, 11, 0, 0, 'achievement_three_faced'),
(12752, 11, 0, 0, 'achievement_soul_power');
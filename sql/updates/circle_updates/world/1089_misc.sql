UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry` IN 
(41440, 49971, 49977, 49983,
41841, 49975, 49981, 49987,
41576, 49973, 49979, 49985,
41961, 49972, 49978, 49984, 
50030, 41901, 49811, 49982,
49976, 49988, 41505);
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry` IN
(41440, 49971, 49977, 49983,
41841, 49975, 49981, 49987);

DELETE FROM `creature` WHERE `id`=41505;

UPDATE `creature_template_addon` SET `auras`='77712' WHERE `entry`=41576;
UPDATE `creature_template` SET `modelid1`=39808, `modelid2`=0 WHERE `entry` IN (41576, 49973, 49979, 49985);

DELETE FROM `spell_script_names` WHERE `spell_id` IN 
(77615, 77715, 95655, 95656, 95657,
 77925, 77932, 77937, 92831,
 88699, 88700, 92828, 89798,
 77786, 92971, 92972, 92973);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(77615, 'spell_maloriak_debilitating_slime'),
(77715, 'spell_maloriak_shatter'),
(95655, 'spell_maloriak_shatter'),
(95656, 'spell_maloriak_shatter'),
(95657, 'spell_maloriak_shatter'),
(77925, 'spell_maloriak_throw_bottle'),
(77932, 'spell_maloriak_throw_bottle'),
(77937, 'spell_maloriak_throw_bottle'),
(92831, 'spell_maloriak_throw_bottle'),
(88699, 'spell_maloriak_drink_bottle'),
(88700, 'spell_maloriak_drink_bottle'),
(92828, 'spell_maloriak_drink_bottle'),
(89798, 'spell_lord_victor_nefarius_master_adventurer_award');

UPDATE `creature_template` SET `mindmg`=350, `maxdmg`=450, `dmg_multiplier`=105 WHERE `entry`=41378;
UPDATE `creature_template` SET `mindmg`=550, `maxdmg`=950, `dmg_multiplier`=11 WHERE `entry`=41440;
UPDATE `creature_template` SET `mindmg`=550, `maxdmg`=950, `dmg_multiplier`=35 WHERE `entry`=41841;
UPDATE `creature_template` SET `mindmg`=550, `maxdmg`=950, `dmg_multiplier`=19 WHERE `entry`=49971;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=105 WHERE `entry`=49974;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=35 WHERE `entry`=49975;
UPDATE `creature_template` SET `mindmg`=550, `maxdmg`=950, `dmg_multiplier`=9 WHERE `entry`=49977;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=135 WHERE `entry`=49980;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=35 WHERE `entry`=49981;
UPDATE `creature_template` SET `mindmg`=550, `maxdmg`=950, `dmg_multiplier`=19 WHERE `entry`=49983;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=135 WHERE `entry`=49986;
UPDATE `creature_template` SET `mindmg`=548, `maxdmg`=822, `dmg_multiplier`=35 WHERE `entry`=49987;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (14399, 15659, 16013, 16014);
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry` IN (14399, 15659, 16013, 16014);
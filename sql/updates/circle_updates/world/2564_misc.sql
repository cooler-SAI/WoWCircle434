UPDATE `creature_template` SET `unit_flags`=0x300, `flags_extra`=2 WHERE `entry` IN (48115, 48633, 48634, 48636, 48637, 48638);
UPDATE `creature_template` SET `ScriptName`='npc_battle_guild_standart' WHERE `entry` IN (48115, 48633, 48634, 48636, 48637, 48638);

REPLACE INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES 
(48115, 0, 0, 0, 0, 0, '89481'),
(48633, 0, 0, 0, 0, 0, '89481'),
(48634, 0, 0, 0, 0, 0, '89481'),
(48636, 0, 0, 0, 0, 0, '89481'),
(48637, 0, 0, 0, 0, 0, '89481'),
(48638, 0, 0, 0, 0, 0, '89481');

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(90216, 'spell_gen_battle_guild_standart'),
(90708, 'spell_gen_battle_guild_standart');
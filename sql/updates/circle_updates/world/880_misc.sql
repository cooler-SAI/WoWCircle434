DELETE FROM `creature` WHERE `id` IN (40080, 40065);
REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (75206, 'spell_item_flameseers_staff_flamebreaker');
UPDATE `creature` SET `spawntimesecs`=60 WHERE `id`=38896;
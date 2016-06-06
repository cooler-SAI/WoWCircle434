UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN (20885, 20886);
REPLACE INTO `smart_scripts` (`entryorguid`, `id`, `event_type`, `event_chance`, `event_flags`, `action_type`, `action_param1`, `action_param2`, `comment`) VALUES 
(20885, 0, 4, 100, 6, 34, 2, 1, 'Dalliah the Doomsayer - Set Instance Data on Aggro'),
(20885, 1, 6, 100, 6, 34, 2, 3, 'Dalliah the Doomsayer - Set Instance Data on Death'),
(20885, 2, 7, 100, 6, 34, 2, 2, 'Dalliah the Doomsayer - Set Instance Data on Evade'),
(20886, 0, 4, 100, 6, 34, 3, 1, 'Wrath-Scryer Soccothrates - Set Instance Data on Aggro'),
(20886, 1, 6, 100, 6, 34, 3, 3, 'Wrath-Scryer Soccothrates - Set Instance Data on Death'),
(20886, 2, 7, 100, 6, 34, 3, 2, 'Wrath-Scryer Soccothrates - Set Instance Data on Evade');
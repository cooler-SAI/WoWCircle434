DELETE FROM `creature` WHERE `id` IN 
(20905, 20906, 20908,
20909, 20910, 20911, 20977);
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN 
(20905, 20906, 20908,
20909, 20910, 20911);
REPLACE INTO `smart_scripts` (`entryorguid`, `id`, `event_type`, `event_chance`, `event_flags`, `action_type`, `action_param1`, `action_param2`, `comment`) VALUES 
(20905, 0, 6, 100, 6, 34, 5, 3, 'Blazing Trickster - Set Instance Data on Death'),
(20905, 1, 7, 100, 6, 34, 4, 2, 'Blazing Trickster - Set Instance Data on Evade'),
(20906, 0, 6, 100, 6, 34, 5, 3, 'Phase-Hunter - Set Instance Data on Death'),
(20906, 1, 7, 100, 6, 34, 4, 2, 'Phase-Hunter - Set Instance Data on Evade'),
(20908, 0, 6, 100, 6, 34, 7, 3, 'Akkiris Lightning-Waker - Set Instance Data on Death'),
(20908, 1, 7, 100, 6, 34, 4, 2, 'Akkiris Lightning-Waker - Set Instance Data on Evade'),
(20909, 0, 6, 100, 6, 34, 7, 3, 'Sulfuron Magma-Thrower - Set Instance Data on Death'),
(20909, 1, 7, 100, 6, 34, 4, 2, 'Sulfuron Magma-Thrower - Set Instance Data on Evade'),
(20910, 0, 6, 100, 6, 34, 8, 3, 'Twilight Drakonaar - Set Instance Data on Death'),
(20910, 1, 7, 100, 6, 34, 4, 2, 'Twilight Drakonaar - Set Instance Data on Evade'),
(20911, 0, 6, 100, 6, 34, 8, 3, 'Blackwing Drakonaar - Set Instance Data on Death'),
(20911, 1, 7, 100, 6, 34, 4, 2, 'Blackwing Drakonaar - Set Instance Data on Evade');
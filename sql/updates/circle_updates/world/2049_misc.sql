REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(99220, 0, 5, 0, 0, 0, 0, 0, 0, 0, 45),
(62600, 0, 0, 0, 0, 0, 0, 2, 0, 100, 0);

DELETE FROM `spell_group` WHERE `spell_id` IN (79631, 79632);
INSERT INTO `spell_group` (`id`, `spell_id`, `comments`) VALUES 
(1, 79632, NULL),
(2, 79631, NULL);

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(107994, 0, 0, 0, 0, 'Vial of Shadows (Normal)'),
(109721, 0, 0, 0, 0, 'Vial of Shadows (LFR)'),
(109724, 0, 0, 0, 0, 'Vial of Shadows (Heroic)');
UPDATE `spell_proc_event` SET `procEx`=1027, `ppmRate`=0 WHERE `entry`=16864;

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(107998, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 27),
(109753, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 27),
(109755, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 27),
(109826, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25),
(108002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25),
(109823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25);

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(109754, 0, 0, 0.726, 0, 'Bone-Link Fetish - Whirling Maw (heroic)'),
(107997, 0, 0, 0.675, 0, 'Bone-Link Fetish - Whirling Maw (normal)'),
(109752, 0, 0, 0.598, 0, 'Bone-Link Fetish - Whirling Maw (lfr)'),
(109825, 1.411, 0, 0, 0, 'Windward Heart - Nick of Time (heroic)'),
(109825, 1.25, 0, 0, 0, 'Windward Heart - Nick of Time (normal)'),
(109825, 1.107, 0, 0, 0, 'Windward Heart - Nick of Time (lfr)');
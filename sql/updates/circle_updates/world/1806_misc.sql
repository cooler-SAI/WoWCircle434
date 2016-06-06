REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(52212, 0, 0, 0.064, 0, 'Death Knight - Death and Decay'),
(42208, 0.162, 0, 0, 0, 'Mage - Blizzard Triggered Spell'),
(42231, 0.095, 0, 0, 0, 'Druid - Hurricane Triggered'),
(81297, 0.027, 0, 0.027, 0, 'Paladin - Consecration');

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES 
(59578, 96287, 2, 'Paladin - Art of War (base mod + dot mod)');

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(51123, 0, 0, 0, 0, 0, 0, 1027, 1, 0, 0),
(51127, 0, 0, 0, 0, 0, 0, 1027, 2, 0, 0),
(51128, 0, 0, 0, 0, 0, 0, 1027, 4, 0, 0);
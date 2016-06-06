REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(96887, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3),
(97119, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3);

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES
(96891, 0, 0, 0, 0, 'Variable Pulse Lightning Capacitor - Lightning Bolt');

UPDATE `item_template` SET `spellid_1`=21079, `spelltrigger_1`=1 WHERE `entry`=17142;
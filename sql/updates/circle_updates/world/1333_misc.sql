DELETE FROM `spell_proc_event` WHERE `entry` IN (105765);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(105765, 0, 10, 0x00800000, 0, 0, 0, 1027, 0, 0, 0);

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(-53817, -105869, 0, 'Item - Shaman T13 Enhancement 2P Bonus');
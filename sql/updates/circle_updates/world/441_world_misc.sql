DELETE FROM `spell_proc_event` WHERE `entry`=14751;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (14751, 0, 6, 7808, 36, 65536, 87040, 1027, 0, 100, 0);

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (81206, 81207, 2, 'Priest - Renew bonus heal from Chakra: Sanctuary');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (14751, -81206, 0, 'Priest - Chakra');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (14751, -81207, 0, 'Priest - Chakra');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (14751, -81208, 0, 'Priest - Chakra');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (14751, -81209, 0, 'Priest - Chakra');
DELETE FROM `spell_proc_event` WHERE `entry`=81659;
DELETE FROM `spell_proc_event` WHERE `entry`=81662;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81659, 0, 6, 1048704, 0, 64, 0, 1027, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (81662, 0, 6, 1048704, 0, 64, 0, 1027, 0, 100, 0);

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (81660, 87154, 2, 'Evangelism');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (81661, -87118, 0, 'Evangelism stacking');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (81661, 87154, 2, 'Evangelism');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (87117, 87154, 2, 'Evangelism');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (87118, -81661, 0, 'Evangelism stacking');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (87118, 87154, 2, 'Evangelism');
UPDATE `spell_proc_event` SET `procFlags`=16 WHERE `entry`=20784;

DELETE FROM `spell_proc_event` WHERE `entry` IN (53256, 53259, 53260);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(53256, 0, 9, 2048, 0, 0, 0, 1027, 0, 0, 0),
(53259, 0, 9, 2048, 0, 0, 0, 1027, 0, 0, 0),
(53260, 0, 9, 2048, 0, 0, 0, 1027, 0, 0, 0);

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(82692, -88843, 0, 'Hunter - Focus Fire');
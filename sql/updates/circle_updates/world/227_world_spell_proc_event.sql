DELETE FROM `spell_proc_event` WHERE `entry`=88820;
DELETE FROM `spell_proc_event` WHERE `entry`=88821;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (88820, 0, 0, 3221225472, 0, 1024, 16384, 3, 0, 10, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (88821, 0, 0, 3221225472, 0, 1024, 16384, 3, 0, 20, 0);
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (20473, -88819, 0, 'Holy Shock - removes Daybreak');

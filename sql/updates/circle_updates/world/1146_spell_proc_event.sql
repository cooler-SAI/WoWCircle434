DELETE FROM `spell_proc_event` WHERE `entry` IN (30293, 30295, 91702);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(30293, 0, 5, 128, 131200, 0, 0, 0, 0, 0, 0),
(30295, 0, 5, 128, 131200, 0, 0, 0, 0, 0, 0),
(91702, 0, 0, 0, 0, 0, 0, 1026, 0, 0, 0);

INSERT IGNORE INTO `spell_pet_auras` (`spell`, `effectId`, `pet`, `aura`) VALUES
(85112, 1, 416, 85112),
(91986, 1, 416, 91986);
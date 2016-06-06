DELETE FROM `spell_pet_auras` WHERE `spell` IN (85112, 91986);
INSERT INTO `spell_pet_auras` (`spell`, `pet`, `aura`) VALUES
(85112, 416, 85112),
(91986, 416, 91986);
DELETE FROM `spell_linked_spell` WHERE spell_trigger = 63560;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(63560, -93426, 0, 'Dark Transformation remove aura'),
(63560, -91342, 1, 'Shadow Infusion remove aura');
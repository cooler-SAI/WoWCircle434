-- Mage Fingers of Frost & Frost Armor proc correction

UPDATE `spell_proc_event` SET `SpellFamilyMask0`='544' WHERE `entry` in ('83074', '44545', '44543');
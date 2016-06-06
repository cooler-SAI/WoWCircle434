DELETE FROM `spell_proc_event` WHERE `entry`=85767;
DELETE FROM `spell_proc_event` WHERE `entry`=85768;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85767, 0, 0, 0, 0, 0, 262144, 1026, 0, 100, 0);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (85768, 0, 0, 0, 0, 0, 262144, 1026, 0, 100, 0);

DELETE FROM `spell_script_names` WHERE `spell_id`=80398;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (80398, 'spell_warlock_dark_intent');

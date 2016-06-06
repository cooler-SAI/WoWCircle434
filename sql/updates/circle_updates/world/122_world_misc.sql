REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) 
VALUES (49027, 0, 0, 0, 0, 0, 0, 1027, 0, 5, 0);
REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) 
VALUES (49542, 0, 0, 0, 0, 0, 0, 1027, 0, 10, 0);
UPDATE `creature_template` SET `ScriptName`='npc_bloodworm' WHERE `entry`=28017;
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry`=28017;
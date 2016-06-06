UPDATE `creature_template` SET `ScriptName`='npc_gurthalak_tentacle_of_the_old_ones' WHERE `entry` IN (58078, 57220, 58077);

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(109725, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9),
(107995, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9),
(109722, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9);

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(109724, 0, 0, 0.339, 0, 'Vial of Shadows (Heroic)'),
(107994, 0, 0, 0.3, 0, 'Vial of Shadows (Normal)'),
(109721, 0, 0, 0.266, 0, 'Vial of Shadows (LFR)');

UPDATE `creature_template` SET `ScriptName`='npc_custom_caster_guard' WHERE `entry` IN (11859, 53432, 53491);

REPLACE INTO `playercreateinfo_spell` (`race`, `class`, `Spell`, `Note`) VALUES (2, 9, 20575, 'Command');

REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(12328, 0, 0, 0, 0, 0, 0, 1027, 0, 0, 0);

UPDATE `spell_proc_event` SET `SpellFamilyMask1`=192 WHERE `entry` IN (85106, 85107, 85108);
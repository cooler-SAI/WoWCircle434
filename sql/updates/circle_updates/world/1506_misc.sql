REPLACE INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(95871, 0, 0, 0, 0, 0, 0, 0, 0, 0, 120),
(99070, 0, 10, 3221225472, 0, 1024, 0, 1027, 0, 0, 0);

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(92649, 'spell_item_cauldron_of_battle'),
(92712, 'spell_item_big_cauldron_of_battle');

UPDATE `creature_template_addon` SET `auras`='34429 28305 99155' WHERE `entry`=19668;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger`=57934 AND `spell_effect`=57933;

REPLACE INTO `spell_group` (`id`, `spell_id`) VALUES 
(1, 79638),
(1, 79639),
(1, 79640),
(2, 79638),
(2, 79639),
(2, 79640);

REPLACE INTO `spell_group` (`id`, `spell_id`) VALUES 
(1, 79474),
(1, 79477),
(1, 79468),
(1, 79481),
(2, 134873),
(2, 79480),
(1, 79635),
(1, 79469),
(2, 79469),
(1, 94160),
(2, 94160),
(1, 79470),
(2, 79470),
(1, 79472),
(2, 79472),
(1, 79471),
(2, 79471),
(1, 92730),
(1, 92725),
(1, 92729),
(1, 92731),
(2, 92730),
(2, 92725),
(2, 92729),
(2, 92731);

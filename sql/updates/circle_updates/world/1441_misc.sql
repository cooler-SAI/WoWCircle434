DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_hun_invigoration';
REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(53209, 'spell_hun_chimera_shot');

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (82726, 82728, 'Hunter - Fervor');
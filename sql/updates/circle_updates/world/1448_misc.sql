UPDATE `spell_proc_event` SET `procEx`=1027 WHERE `entry`=54278;
DELETE FROM `spell_proc_event` WHERE `entry`=47220;
DELETE FROM `spell_proc_event` WHERE `entry`=47221;

REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(80240, 85466, 0, 'Bane of Havoc');

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES
(3110, 0, 0, 0, 0, 'Warlock - Firebolt');
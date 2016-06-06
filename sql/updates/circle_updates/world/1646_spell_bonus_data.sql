REPLACE INTO `spell_bonus_data` (`entry`, `comments`) VALUES 
(879, 'Paladin - Exorcism');

UPDATE `creature_template` SET `unit_flags`=33554434, `type_flags`=0 WHERE `entry` IN (46490, 46499, 46506);
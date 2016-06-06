UPDATE `creature_template` SET `dmgschool`=2 WHERE `entry`=15438;

REPLACE INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
(31707, 0, 0, 0, 0, 'Mage - Water Elemental Waterbolt'),
(59637, 0, 0, 0, 0, 'Mage - Mirror Image Fire Blast'),
(59638, 0, 0, 0, 0, 'Mage - Mirror Image Frostbolt'),
(33395, 0, 0, 0, 0, 'Mage - Water Elemental Freeze');

REPLACE INTO `pet_scaling_data` (`spell_id`, `class`, `comment`) VALUES 
(89764, 8, 'All Critical Chance, Attack Speed, Magic Resistance');
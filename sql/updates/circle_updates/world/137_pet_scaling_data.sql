SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `pet_scaling_data`
-- ----------------------------
DROP TABLE IF EXISTS `pet_scaling_data`;
CREATE TABLE `pet_scaling_data` (
  `spell_id` mediumint(8) unsigned NOT NULL default '0',
  `class` tinyint(1) unsigned NOT NULL default '0',
  `comment` text,
  PRIMARY KEY  (`spell_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pet_scaling_data
-- ----------------------------
INSERT INTO `pet_scaling_data` VALUES ('51996', '6', 'all damage done, attack speed, immunity to charm effects');
INSERT INTO `pet_scaling_data` VALUES ('89446', '3', 'All Crit Chance, Attack Speed, Magic Resistance');
INSERT INTO `pet_scaling_data` VALUES ('61017', '3', 'All Hit chance, Spell Hit Chance, Expertise');
INSERT INTO `pet_scaling_data` VALUES ('61697', '6', 'All Hit chance, Spell Hit Chance, Sleep Immunity');
INSERT INTO `pet_scaling_data` VALUES ('61013', '9', 'All Hit Chance, Spell Hit Chance, Expertise from hit');
INSERT INTO `pet_scaling_data` VALUES ('61783', '7', 'All Hit Chance, Spell Hit Chance, All Damage Done');

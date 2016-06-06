-- ----------------------------
-- Table structure for `currency_reset_save`
-- ----------------------------
DROP TABLE IF EXISTS `currency_reset_save`;
CREATE TABLE `currency_reset_save` (
  `id` tinyint(3) NOT NULL,
  `lastguid` int(10) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
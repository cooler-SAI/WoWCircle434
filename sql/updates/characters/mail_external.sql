-- ----------------------------
-- Table structure for `mail_external`
-- ----------------------------
DROP TABLE IF EXISTS `mail_external`;
CREATE TABLE `mail_external` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `sender` bigint(20) DEFAULT NULL,
  `receiver` bigint(20) DEFAULT NULL,
  `subject` varchar(200) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT 'Support',
  `message` varchar(500) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT 'Support',
  `money` bigint(20) NOT NULL DEFAULT '0',
  `stationery` int(10) DEFAULT '41',
  `sent` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mail_external
-- ----------------------------
-- ----------------------------
-- Table structure for `mail_external_items`
-- ----------------------------
DROP TABLE IF EXISTS `mail_external_items`;
CREATE TABLE `mail_external_items` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `mail_id` bigint(20) DEFAULT NULL,
  `item` bigint(20) DEFAULT NULL,
  `count` bigint(20) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mail_external_items
-- ----------------------------
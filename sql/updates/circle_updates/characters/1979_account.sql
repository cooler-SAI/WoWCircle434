ALTER TABLE `account`  
   CHANGE COLUMN `mutetime` `mutetime` BIGINT(40) NOT NULL DEFAULT '0' AFTER `expansion`;

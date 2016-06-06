ALTER TABLE `guild_rank`  CHANGE COLUMN `rname` `rname` VARCHAR(100) NOT NULL DEFAULT '' AFTER `rid`;
ALTER TABLE `guild`  CHANGE COLUMN `experience` `experience` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0' AFTER `level`;
UPDATE `guild_rank` SET `BankMoneyPerDay`=`BankMoneyPerDay`/10000;
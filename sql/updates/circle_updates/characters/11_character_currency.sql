ALTER TABLE `character_currency`
  ADD COLUMN `flags`  TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `week_count`;
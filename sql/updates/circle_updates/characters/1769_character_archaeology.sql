ALTER TABLE `character_archaeology`
ADD COLUMN `sites0` TEXT NULL AFTER `guid`,
ADD COLUMN `sites1` TEXT NULL AFTER `sites0`,
ADD COLUMN `sites2` TEXT NULL AFTER `sites1`,
ADD COLUMN `sites3` TEXT NULL AFTER `sites2`,
DROP COLUMN `sites`;
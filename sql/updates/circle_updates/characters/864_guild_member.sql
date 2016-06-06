ALTER TABLE `guild_member`
    ADD `week_activity` BIGINT(20) UNSIGNED DEFAULT 0 NOT NULL,
    ADD `total_activity` BIGINT(20) UNSIGNED DEFAULT 0 NOT NULL;

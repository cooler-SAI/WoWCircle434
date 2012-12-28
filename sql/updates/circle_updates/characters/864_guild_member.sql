ALTER TABLE `guild_member`
    CHANGE `week_activity` `week_activity` BIGINT(20) UNSIGNED DEFAULT 0 NOT NULL,
    CHANGE `total_activity` `total_activity` BIGINT(20) UNSIGNED DEFAULT 0 NOT NULL;
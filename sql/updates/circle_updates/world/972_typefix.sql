ALTER TABLE `guild_xp_for_level` 
   CHANGE `lvl` `lvl` TINYINT(3) UNSIGNED NOT NULL;
ALTER TABLE `phase_definitions` 
   CHANGE `phaseId` `phaseId` SMALLINT(5) UNSIGNED DEFAULT '0' NOT NULL;
ALTER TABLE `research_loot` 
   CHANGE `site_id` `site_id` mediumint(8) UNSIGNED NOT NULL, 
   CHANGE `race` `race` tinyint(3) UNSIGNED default '0' NOT NULL;
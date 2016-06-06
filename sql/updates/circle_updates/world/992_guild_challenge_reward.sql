DROP TABLE IF EXISTS `guild_challenge_reward`;
CREATE TABLE `guild_challenge_reward`
( 
   `Type` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Challenge type',
   `Expirience` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Guild expirience', 
   `Gold` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Gold reward', 
   `Gold2` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Gold reward 2', 
   `Count` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'ChallengeCount', 
   PRIMARY KEY (`Type`)
 );

INSERT INTO `guild_challenge_reward` (`Type`, `Expirience`, `Gold`, `Gold2`, `Count`) VALUES
(0,       0,    0,   0, 0),
(1,  300000,  250, 125, 7),
(2, 3000000, 1000, 500, 1),
(3, 1500000,  500, 250, 3);
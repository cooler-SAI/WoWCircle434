CREATE  TABLE `character_rated_bg` (
  `id` INT NOT NULL ,
  `rating` MEDIUMINT NOT NULL ,
  `mmv` MEDIUMINT NOT NULL ,
  `games` INT NOT NULL ,
  `wins` INT NOT NULL ,
  `weekGames` MEDIUMINT NOT NULL ,
  `weekWins` MEDIUMINT NOT NULL ,
  PRIMARY KEY (`id`) 
);
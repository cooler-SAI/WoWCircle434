-- Clear all valor and conquest points
UPDATE `character_currency` SET `character_currency`.`total_count`=0 WHERE `character_currency`.`currency`=396;
UPDATE `character_currency` SET `character_currency`.`total_count`=0 WHERE `character_currency`.`currency`=390;

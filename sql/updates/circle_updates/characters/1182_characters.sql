ALTER TABLE `characters`  
    ADD COLUMN `currentPetSlot` INT(10) NULL DEFAULT NULL AFTER `grantableLevels`,
    ADD COLUMN `petSlotUsed` INT(10) UNSIGNED NULL DEFAULT NULL AFTER `currentPetSlot`;

DROP PROCEDURE IF EXISTS `update_pets`;
-- Now we have hunters with only one pet
-- Check for all hunters and set `petSlotUsed`=1 for each who has pet
-- Check for all dk and warlocks and set `currentPetSlot`=100, `petSlotUsed`=3452816845 for each who has pet
delimiter //
CREATE PROCEDURE `update_pets`()
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
DECLARE owner_guid INT;
DECLARE done INT DEFAULT 0;
DECLARE cur CURSOR FOR SELECT `character_pet`.`owner` FROM  `character_pet`;
DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1; 
OPEN cur; 
WHILE done = 0 DO
FETCH cur INTO owner_guid;
UPDATE `characters` SET `currentPetSlot`=0, `petSlotUsed`=1 WHERE `class`=3 AND `guid`=owner_guid;
UPDATE `characters` SET `currentPetSlot`=100, `petSlotUsed`=3452816845 WHERE `class` IN (6, 9) AND `guid`=owner_guid;
END WHILE;
CLOSE cur;
END;
//
delimiter ;

CALL `update_pets`;
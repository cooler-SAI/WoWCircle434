DROP PROCEDURE IF EXISTS `temp_character_pet`;
delimiter //
CREATE PROCEDURE `temp_character_pet`()
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT 'Correct pet slots.'
BEGIN
DECLARE owner INT DEFAULT 0;
DECLARE curowner INT DEFAULT 0;
DECLARE curid INT DEFAULT 0;
DECLARE oldslot INT DEFAULT 0;
DECLARE newslot INT DEFAULT -1;
DECLARE id INT DEFAULT 0;
DECLARE slotused INT DEFAULT 0;
DECLARE currentslot INT DEFAULT 0;
DECLARE done INT DEFAULT 0;
DECLARE cur CURSOR FOR SELECT `cp`.`id`, `cp`.`owner`, `cp`.`slot`, `cc`.`petSlotUsed`, `cc`.`currentPetSlot` FROM `character_pet` AS `cp` LEFT JOIN `characters` AS `cc` ON `cp`.`owner`=`cc`.`guid` WHERE `cc`.`class`=3 ORDER BY `cp`.`owner`, `cp`.`slot`;
DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1; 
OPEN cur; 
WHILE done = 0 DO
    FETCH cur INTO id, owner, oldslot, slotused, currentslot;
    IF curowner <> owner THEN
        SET newslot = -1;
        SET curowner = owner;
    END IF;
    IF curid <> id THEN
    SET curid = id;
    SET newslot = newslot + 1;
    UPDATE `character_pet` SET `character_pet`.`slot`=newslot WHERE `character_pet`.`id`=id; 
    END IF;
END WHILE;
CLOSE cur;
END;
//
delimiter ;

CALL `temp_character_pet`;

DROP PROCEDURE IF EXISTS `temp_character_pet`;
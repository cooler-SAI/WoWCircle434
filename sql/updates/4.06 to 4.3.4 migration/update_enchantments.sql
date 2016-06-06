DROP PROCEDURE IF EXISTS `temp_update_enchantments`;
delimiter //
CREATE PROCEDURE `temp_update_enchantments`()
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT 'Insert transmogrify data into enchantments.'
BEGIN
DECLARE enchant TEXT;
DECLARE guid INT;
DECLARE lstr TEXT DEFAULT '';
DECLARE rstr TEXT DEFAULT '';
DECLARE newstr TEXT DEFAULT '';
DECLARE done INT DEFAULT 0;
DECLARE cur CURSOR FOR SELECT `item_instance`.`guid`, `item_instance`.`enchantments` FROM  `item_instance`;
DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1; 
OPEN cur; 
WHILE done = 0 DO
FETCH cur INTO guid, enchant;
SET lstr = SUBSTRING_INDEX(enchant, ' ', 27);
SET rstr = SUBSTRING_INDEX(enchant, ' ', -16);
SET newstr = CONCAT_WS(' ', lstr, '0', '0', '0', rstr);
UPDATE `item_instance` SET `item_instance`.`enchantments`=newstr WHERE `item_instance`.`guid`=guid;  
END WHILE;
CLOSE cur;
END;
//
delimiter ;

CALL `temp_update_enchantments`;

DROP PROCEDURE IF EXISTS `temp_update_enchantments`;
DROP PROCEDURE IF EXISTS `temp_update_titles`;
delimiter //
CREATE PROCEDURE `temp_update_titles`()
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT 'Update titles.'
BEGIN
DECLARE title TEXT DEFAULT '';
DECLARE guid INT;
DECLARE lstr TEXT DEFAULT '';
DECLARE newstr TEXT DEFAULT '';
DECLARE done INT DEFAULT 0;
DECLARE cur CURSOR FOR SELECT `characters`.`guid`, `characters`.`knownTitles` FROM `characters`;
DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1; 
OPEN cur; 
WHILE done = 0 DO
FETCH cur INTO guid, title;
SET lstr = SUBSTRING_INDEX(title, ' ', 6);
SET newstr = CONCAT(lstr, ' ', '0 ' , '0 ');
UPDATE `characters` SET `characters`.`knownTitles`=newstr WHERE `characters`.`guid`=guid;  
END WHILE;
CLOSE cur;
END;
//
delimiter ;

CALL `temp_update_titles`;

DROP PROCEDURE IF EXISTS `temp_update_titles`;
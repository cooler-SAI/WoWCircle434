UPDATE `character_currency` SET `total_count`=10000000 WHERE `currency` IN (390, 392) AND `total_count` > 10000000;

DROP PROCEDURE IF EXISTS `temp_update_currency_2`;
delimiter //
CREATE PROCEDURE `temp_update_currency_2`()
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT 'Reset honor and conquest points.'
BEGIN

DECLARE rate INT DEFAULT 1;

DECLARE guid INT DEFAULT 0;
DECLARE currency INT DEFAULT 0;
DECLARE honor INT DEFAULT 0;
DECLARE conquest INT DEFAULT 0;
DECLARE counts INT DEFAULT 0;
DECLARE done INT DEFAULT 0;
DECLARE money BIGINT DEFAULT 0;
DECLARE oldmoney BIGINT DEFAULT 0;
DECLARE cur CURSOR FOR SELECT `character_currency`.`guid`, `character_currency`.`currency`, `character_currency`.`total_count`, `characters`.`money` FROM `character_currency` RIGHT JOIN `characters` ON `character_currency`.`guid` = `characters`.`guid` WHERE `character_currency`.`currency` IN (390, 392) ORDER BY `character_currency`.`guid` ASC, `character_currency`.`currency` DESC;
DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1; 
OPEN cur; 
WHILE done = 0 DO
    FETCH cur INTO guid, currency, counts, oldmoney;
    IF currency = 392 THEN
        SET honor = counts;
    END IF;
    IF currency = 390 THEN
        SET conquest = counts;
        SET honor = honor + conquest;
        IF honor > 400000 THEN
            SET money = (honor - 400000) * 35 * rate;
            SET honor = 400000;
            IF oldmoney + money > 9000000000 THEN
                SET money = 9000000000 - oldmoney;
            END IF;
        END IF;
        UPDATE `character_currency` SET `character_currency`.`total_count`=honor WHERE `character_currency`.`guid`=guid AND `character_currency`.`currency`=392;
        IF money > 0 THEN
            UPDATE `characters` SET `characters`.`money`=`characters`.`money` + money WHERE `characters`.`guid`=guid;
        END IF;
        SET honor = 0;
        SET currency = 0;
        SET conquest = 0;
        SET money = 0;
        SET oldmoney = 0;
    END IF;
END WHILE;
CLOSE cur;

END;
//
delimiter ;

CALL `temp_update_currency_2`;

DROP PROCEDURE IF EXISTS `temp_update_currency_2`;
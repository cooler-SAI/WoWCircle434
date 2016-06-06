-- Halfus Wyrmbreaker
DELETE FROM `conditions` WHERE `SourceEntry` IN (83609, 87683, 84092, 83601, 83603, 83611);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '83609', '0', '31', '0', '3', '44687', 'Atrophic poison'),
('13', '1', '87683', '0', '31', '0', '3', '44600', 'Dragon vengeance'),
('13', '1', '84092', '0', '31', '0', '3', '44600', 'Cyclone winds'),
('13', '1', '83601', '0', '31', '0', '3', '44687', 'Time dilaton'),
('13', '1', '83603', '0', '31', '0', '3', '44600', 'Stone grip'),
('13', '1', '83611', '0', '31', '0', '3', '44600', 'Nether blindness');

UPDATE `creature_template` SET `InhabitType`=5 WHERE `entry` IN (43735, 49619, 49620, 49621);

-- Theralion and Valiona
DELETE FROM `conditions` WHERE `SourceEntry`=86840;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '86840', '0', '31', '0', '3', '48798', 'Valiona - Devouring flames');


-- Cho'gall
DELETE FROM `conditions` WHERE `SourceEntry` IN 
(91331, 93206, 93207, 93208, 81194, 93264, 93265,
 93266, 81572, 93218, 93219, 93220, 82630, 82356);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '91331', '0', '31', '0', '3', '43324', 'Chogall - twisted devotion'),
('13', '1', '93206', '0', '31', '0', '3', '43324', 'Chogall - twisted devotion'),
('13', '1', '93207', '0', '31', '0', '3', '43324', 'Chogall - twisted devotion'),
('13', '1', '93208', '0', '31', '0', '3', '43324', 'Chogall - twisted devotion'),
('13', '1', '81194', '0', '31', '0', '3', '43324', 'Chogall - fire destruction'),
('13', '1', '93264', '0', '31', '0', '3', '43324', 'Chogall - fire destruction'),
('13', '1', '93265', '0', '31', '0', '3', '43324', 'Chogall - fire destruction'),
('13', '1', '93266', '0', '31', '0', '3', '43324', 'Chogall - fire destruction'),
('13', '1', '81572', '0', '31', '0', '3', '43324', 'Chogall - empowered shadows'),
('13', '1', '93218', '0', '31', '0', '3', '43324', 'Chogall - empowered shadows'),
('13', '1', '93219', '0', '31', '0', '3', '43324', 'Chogall - empowered shadows'),
('13', '1', '93220', '0', '31', '0', '3', '43324', 'Chogall - empowered shadows'),
('13', '1', '82630', '0', '31', '0', '3', '43622', 'Chogall - consume blood of the old god'),
('13', '1', '82356', '0', '31', '0', '3', '43999', 'Chogall - corruption of the old god visual');

-- Sinestra
DELETE FROM `conditions` WHERE `SourceEntry`=95855;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
('13', '1', '95855', '0', '31', '0', '3', '46588', 'Sinestra - Call Flames');
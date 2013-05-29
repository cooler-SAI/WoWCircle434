DELETE FROM `command` WHERE `name` = 'modify qcurrency';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('modify qcurrency', 8, 'Syntax: .modify qcurrency #id #value\nAdd $amount of $currency to the selected player.');
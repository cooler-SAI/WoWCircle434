DELETE FROM `trinity_string` WHERE `entry`=556;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(556, 'Found near creatures (distance %f): %u ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
DELETE FROM `command` WHERE `name` LIKE 'npc near';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('npc near', 5, 'Syntax: .npc near  [#distance]\r\n\r\nOutput npc at distance #distance from player. Output npc guids and coordinates sorted by distance from character. If #distance not provided use 10 as default value.');

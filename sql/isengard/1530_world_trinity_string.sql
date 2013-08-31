-- announce strings
DELETE FROM trinity_string WHERE entry=11102;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(11102, 'You must enter a reason of mute', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы должны указать причину молчанки');

-- diff strings
DELETE FROM trinity_string WHERE entry in (11103, 11104, 11105);
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(11103, 'Update time diff:|cff40c040 %u - good', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Update time diff:|cff40c040 %u - отлично'),
-- (11104, 'Update time diff:|0xffe6cc80 %u - normal', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Update time diff:|0xffe6cc80 %u. - хорошо'),
(11104, 'Update time diff: %u - normal', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Update time diff: %u. - хорошо'),
(11105, 'Update time diff:|cffff0000 %u - poor', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Update time diff:|cffff0000 %u - плохо');

DELETE FROM `trinity_string` WHERE `entry` = 11106;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(11106, '|cFFFFBF00[%s]:|cFFFFFFFF [%s] |cFF00FFFFdetected as possible cheater.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, '|cFFFFBF00[%s]:|cFFFFFFFF [%s] |cFF00FFFFдетект возможного читера.');


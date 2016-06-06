DELETE FROM creature_template_addon WHERE entry IN (28525, 28543, 28542, 28544);
INSERT INTO creature_template_addon VALUES (28525, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28543, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28544, 0, 0, 0, 1, 0, 64328);
INSERT INTO creature_template_addon VALUES (28542, 0, 0, 0, 1, 0, 64328);

UPDATE creature_template SET flags_extra = 0, scale = 4 WHERE entry IN (28525, 28543, 28542, 28544);

DELETE FROM `creature_addon` WHERE `guid` NOT IN (SELECT `guid` FROM `creature`);
UPDATE `quest_template` SET `RequiredRaces`=2098253 WHERE `Id`=11821;
DELETE FROM `creature_involvedrelation` WHERE `id`=25937 AND `quest`=11821;
DELETE FROM `creature_questrelation` WHERE `id`=25937 AND `quest`=11821;

DELETE FROM `gameobject_questrelation` WHERE `id`=187559 AND `quest`=11732;
DELETE FROM `gameobject_questrelation` WHERE `id`=187559 AND `quest`=11755;
DELETE FROM `gameobject_questrelation` WHERE `id`=187559 AND `quest`=11766;
DELETE FROM `gameobject_questrelation` WHERE `id`=187559 AND `quest`=11786;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187559 AND `quest`=11732;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187559 AND `quest`=11755;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187559 AND `quest`=11766;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187559 AND `quest`=11786;
DELETE FROM `gameobject_questrelation` WHERE `id`=187564 AND `quest`=11745;
DELETE FROM `gameobject_questrelation` WHERE `id`=187564 AND `quest`=11749;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187564 AND `quest`=11745;
DELETE FROM `gameobject_involvedrelation` WHERE `id`=187564 AND `quest`=11749;
UPDATE `quest_template` SET `RequiredRaces`=946 WHERE `Id`=11755;
UPDATE `quest_template` SET `RequiredRaces`=946 WHERE `Id`=11732;
-- Правка радиуса моделек ловушек
UPDATE `creature_model_info` SET `bounding_radius`=1, `combat_reach`=1 WHERE `modelid`=38466;
UPDATE `creature_model_info` SET `bounding_radius`=1, `combat_reach`=1 WHERE `modelid`=38468;

-- Разблокируем достижения
DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=18099;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17576;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17775;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17776;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17777;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17778;
DELETE FROM `disables` WHERE `sourceType`=4 AND `entry`=17779;

-- Достижение "Пока не сыграл в ящик"
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (17576, 17775, 17776, 17777, 17778, 17779);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(17576, 11, 0, 0, 'achievement_bucket_list'),
(17775, 11, 0, 0, 'achievement_bucket_list'),
(17776, 11, 0, 0, 'achievement_bucket_list'),
(17777, 11, 0, 0, 'achievement_bucket_list'),
(17778, 11, 0, 0, 'achievement_bucket_list'),
(17779, 11, 0, 0, 'achievement_bucket_list');

-- Лут
UPDATE `creature_template` SET `lootid`=53691 WHERE `entry`=53691;
UPDATE `creature_template` SET `lootid`=53979 WHERE `entry`=53979;
UPDATE `creature_template` SET `lootid`=54079 WHERE `entry`=54079;
UPDATE `creature_template` SET `lootid`=54080 WHERE `entry`=54080;

-- Открываем доступ в героический режим без ачивки за Рагнароса
UPDATE `access_requirement` SET `leader_achievement`=0 WHERE `mapId`=720 AND `difficulty`=2;
UPDATE `access_requirement` SET `leader_achievement`=0 WHERE `mapId`=720 AND `difficulty`=3;
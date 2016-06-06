UPDATE `creature_template` SET `flags_extra` = `flags_extra`|128 WHERE `entry`=53495;
DELETE FROM `creature` WHERE `id` = 53237;
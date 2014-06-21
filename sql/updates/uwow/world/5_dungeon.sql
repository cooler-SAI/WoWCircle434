UPDATE `creature_template` SET `ScriptName` = 'boss_deathwing' WHERE `entry` in (57962, 56173, 58000, 58001);
UPDATE `creature_template` SET `ScriptName` = 'npc_maelstrom_trall' WHERE `entry` = 56103;
UPDATE `creature_template` SET `ScriptName` = 'npc_arm_tentacle_one' WHERE `entry` in (56846, 57974, 58133, 58134);
UPDATE `creature_template` SET `ScriptName` = 'npc_arm_tentacle_two' WHERE `entry` in (56167, 57973, 58131, 58132);
UPDATE `creature_template` SET `ScriptName` = 'npc_wing_tentacle_one' WHERE `entry` in (56168);
UPDATE `creature_template` SET `ScriptName` = 'npc_wing_tentacle_two' WHERE `entry` in (57972);
UPDATE `creature_template` SET `ScriptName` = 'npc_mutated_corruption' WHERE `entry` in (56471, 57976, 58137, 58138);

UPDATE `creature_template` SET `InhabitType` = 4, `HoverHeight` = 1 WHERE `entry` in
(57962, 56173, 58000, 58001, 56168, 57972, 58129, 58130, 56167, 57973, 58131, 58132, 56846, 57974, 58133, 58134, 57695, 57696, 57686, 57694, 56844, 56099, 56102, 56100, 56101, 56471, 57976, 58137, 58138);

UPDATE `creature_template` SET `unit_flags` = 0 WHERE `entry` in (56846, 57974, 58133, 58134, 56167, 57973, 58131, 58132, 56168, 57972, 58129, 58130);
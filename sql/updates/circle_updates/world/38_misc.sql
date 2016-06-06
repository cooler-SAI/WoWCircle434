UPDATE `creature_template` SET `npcflag`=16777216, `vehicleId`=200 WHERE `entry`=50269;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=50269;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) 
VALUES (50269, 93970, 1, 0);


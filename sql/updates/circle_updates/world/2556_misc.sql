DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=45191 AND `spell_id`=98975;
UPDATE `creature_template` SET `npcflag`=`npcflag`&~0x01000000 WHERE `entry`=45191;
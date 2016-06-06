DELETE FROM `script_texts` WHERE `entry` BETWEEN -1469001 AND -1469000;
DELETE FROM `creature_text` WHERE `entry`=12017;
INSERT INTO `creature_text`(`entry`,`groupid`,`id`,`type`,`sound`,`probability`,`comment`,`text`) VALUES
(12017,0,0,14,8286,100,"broodlord SAY_AGGRO","None of your kind should be here! You've doomed only yourselves!"),
(12017,1,0,14,8287,100,"broodlord SAY_LEASH","Clever Mortals but I am not so easily lured away from my sanctum!");

-- chromaggus
DELETE FROM `script_texts` WHERE `entry` BETWEEN -1469003 AND -1469002;
DELETE FROM `creature_text` WHERE `entry`=14020;
INSERT INTO `creature_text`(`entry`,`groupid`,`id`,`type`,`sound`,`probability`,`comment`,`text`) VALUES
(14020,0,0,16,0,100,"chromaggus EMOTE_FRENZY","goes into a killing frenzy!"),
(14020,1,0,16,0,100,"chromaggus EMOTE_SHIMMER","flinches as its skin shimmers.");

-- flamegor
DELETE FROM `script_texts` WHERE `entry`=-1469031;
DELETE FROM `creature_text` WHERE `entry`=11981;
INSERT INTO `creature_text`(`entry`,`groupid`,`id`,`type`,`sound`,`probability`,`comment`,`text`) VALUES
(11981,0,0,16,0,100,"flamegor EMOTE_FRENZY","%s goes into a frenzy!");

-- razorgore
DELETE FROM `script_texts` WHERE `entry` BETWEEN -1469025 AND -1469022;
DELETE FROM `creature_text` WHERE `entry`=12435;
INSERT INTO `creature_text`(`entry`,`groupid`,`id`,`type`,`sound`,`probability`,`comment`,`text`) VALUES
(12435,0,0,14,8275,100,"razorgore SAY_EGGS_BROKEN1","You'll pay for forcing me to do this."),
(12435,1,0,14,8276,100,"razorgore SAY_EGGS_BROKEN2","Fools! These eggs are more precious than you know."),
(12435,2,0,14,8277,100,"razorgore SAY_EGGS_BROKEN3","No! Not another one! I'll have your heads for this atrocity."),
(12435,3,0,14,8278,100,"razorgore SAY_DEATH","If I fall into the abyss I'll take all of you mortals with me...");

-- vaelastrasz
DELETE FROM `script_texts` WHERE `entry` BETWEEN -1469030 AND -1469026;
DELETE FROM `creature_text` WHERE `entry`=13020;
INSERT INTO `creature_text`(`entry`,`groupid`,`id`,`type`,`sound`,`probability`,`comment`,`text`) VALUES
(13020,0,0,14,8281,100,"vaelastrasz SAY_LINE1","Too late...friends. Nefarius' corruption has taken hold. I cannot...control myself."),
(13020,1,0,14,8282,100,"vaelastrasz SAY_LINE2","I beg you Mortals, flee! Flee before I lose all control. The Black Fire rages within my heart. I must release it!"),
(13020,2,0,14,8283,100,"vaelastrasz SAY_LINE3","FLAME! DEATH! DESTRUCTION! COWER MORTALS BEFORE THE WRATH OF LORD....NO! I MUST FIGHT THIS!"),
(13020,3,0,14,8285,100,"vaelastrasz SAY_HALFLIFE","Nefarius' hate has made me stronger than ever before. You should have fled, while you could, mortals! The fury of Blackrock courses through my veins!"),
(13020,4,0,14,8284,100,"vaelastrasz SAY_KILLTARGET","Forgive me $N, your death only adds to my failure.");

-- Add Missing Gossip Menu for Lord Victor Nefarius
UPDATE `creature_template` SET `gossip_menu_id`=21330 WHERE `entry`=10162;

-- Add Missing Gossip Menu items for Lord Victor Nefarius
DELETE FROM `gossip_menu` WHERE `entry`=21330 AND `text_id`=7134;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (21330,7134);
DELETE FROM `gossip_menu` WHERE `entry`=21331 AND `text_id`=7198;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (21331,7198);
DELETE FROM `gossip_menu` WHERE `entry`=21332 AND `text_id`=7199;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (21332,7199);

-- Add Missing Gossip Menu options for Lord Victor Nefarius
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (21330,21331,21332);
INSERT INTO `gossip_menu_option` (`menu_id`,`id`,`option_icon`,`option_text`,`option_id`,`npc_option_npcflag`,`action_menu_id`,`action_poi_id`,`box_coded`,`box_money`,`box_text`) VALUES
(21330,0,0, 'I''ve made no mistakes.',1,1,21331,0,0,0, ''),
(21331,0,0, 'You have lost your mind, Nefarius. You speak in riddles.',1,1,21332,0,0,0, ''),
(21332,0,0, 'Please do.',1,1,0,0,0,0, '');

-- Add Missing Gossip Menu for Vaelastrasz the Corrupt
UPDATE `creature_template` SET `gossip_menu_id`=21333 WHERE `entry`=13020;

-- Add Missing Gossip Menu items for Vaelastrasz the Corrupt
DELETE FROM `gossip_menu` WHERE `entry`=21333 AND `text_id`=7156;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (21333,7156);
DELETE FROM `gossip_menu` WHERE `entry`=21334 AND `text_id`=7256;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (21334,7256);

-- Add Missing Gossip Menu options for Vaelastrasz the Corrupt
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (21333,21334);
INSERT INTO `gossip_menu_option` (`menu_id`,`id`,`option_icon`,`option_text`,`option_id`,`npc_option_npcflag`,`action_menu_id`,`action_poi_id`,`box_coded`,`box_money`,`box_text`) VALUES
(21333,0,0, 'I cannot Vaelastraz! Surely something can be done to heal you!',1,1,21334,0,0,0, ''),
(21334,0,0, 'Vaelastraz, no!!',1,1,21332,0,0,0, '');

DELETE FROM `creature_text` WHERE `entry` IN (11583,10162);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
('11583','0','0','Well done, my minions. The mortals\' courage begins to wane! Now, let\'s see how they contend with the true Lord of Blackrock Spire!','14','0','100','0','0','8288','nefarian SAY_AGGRO'),
('11583','0','1','Enough! Now you vermin shall feel the force of my birthright, the fury of the earth itself.','14','0','100','0','0','8289','nefarian SAY_XHEALTH'),
('11583','0','2','Burn, you wretches! Burn!','14','0','100','0','0','8290','nefarian SAY_SHADOWFLAME'),
('11583','1','0','Impossible! Rise my minions! Serve your master once more!','14','0','100','0','0','8291','nefarian SAY_RAISE_SKELETONS'),
('11583','2','0','Worthless $N! Your friends will join you soon enough!','14','0','100','0','0','8293','nefarian SAY_SLAY'),
('11583','3','0','This cannot be! I am the Master here! You mortals are nothing to my kind! DO YOU HEAR? NOTHING!','14','0','100','0','0','8292','nefarian SAY_DEATH'),
('11583','4','0','Mages too? You should be more careful when you play with magic...','14','0','100','0','0','0','nefarian SAY_MAGE'),
('11583','5','0','Warriors, I know you can hit harder than that! Let\'s see it!','14','0','100','0','0','0','nefarian SAY_WARRIOR'),
('11583','6','0','Druids and your silly shapeshifting. Let\'s see it in action!','14','0','100','0','0','0','nefarian SAY_DRUID'),
('11583','7','0','Priests! If you\'re going to keep healing like that, we might as well make it a little more interesting!','14','0','100','0','0','0','nefarian SAY_PRIEST'),
('11583','8','0','Paladins, I\'ve heard you have many lives. Show me.','14','0','100','0','0','0','nefarian SAY_PALADIN'),
('11583','9','0','Shamans, show me what your totems can do!','14','0','100','0','0','0','nefarian SAY_SHAMAN'),
('11583','10','0','Warlocks, you shouldn\'t be playing with magic you don\'t understand. See what happens?','14','0','100','0','0','0','nefarian SAY_WARLOCK'),
('11583','11','0','Hunters and your annoying pea-shooters!','14','0','100','0','0','0','nefarian SAY_HUNTER'),
('11583','12','0','Rogues? Stop hiding and face me!','14','0','100','0','0','0','nefarian SAY_ROGUE'),
('11583','13','0','\'Death Knights, get over here!\'','14','0','100','0','0','0','nefarian SAY_DEATH_KNIGHT'),
('10162','0','0','Excellent... it would appear as if the meddlesome insects have arrived just in time to feed my legion. Welcome, mortals!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','1','0','Let not even a drop of their blood remain upon the arena floor, my children. Feast on their souls!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','2','0','Foolsss... Kill the one in the dress!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','3','0','Inconceivable!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','4','0','Your efforts will prove fruitless. None shall stand in our way!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','5','0','Do not force my hand, children! I shall use your hides to line my boots.','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','6','0','Use the freezing breath, imbecile!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','7','0','THIS CANNOT BE!!! Rend, deal with these insects.','14','0','100','0','0','0','Lord Victor Nefarius (UBRS)'),
('10162','8','0','The Warchief shall make quick work of you, mortals. Prepare yourselves!','14','0','100','25','0','0','Lord Victor Nefarius (UBRS)'),
('10162','9','0','Taste in my power!','14','0','100','0','0','0','Lord Victor Nefarius (UBRS)'),
('10162','10','0','Your victory shall be short lived. The days of both the Alliance and Horde are coming to an end! The next time we meet shall be the last.','14','0','100','5','0','0','Lord Victor Nefarius (UBRS)'),
('10162','11','0','The mortals have fallen! The new age of the Blackrock legion is at hand!','14','0','100','22','0','0','Lord Victor Nefarius (UBRS)'),
('10162','12','0','In this world where time is your enemy, it is my greatest ally. This grand game of life that you think you play in fact plays you. To that I say...','14','0','100','1','0','0','Lord Victor Nefarius SAY_GAMESBEGIN_1 (BWL)'),
('10162','13','0','Let the games begin!','14','0','100','22','0','8280','Lord Victor Nefarius SAY_GAMESBEGIN_2 (BWL)'),
('10162','14','0','Ah, the heroes. You are persistent, aren\'t you. Your allied attempted to match his power against mine, and had to pay the price. Now he shall serve me, by slaughtering you. Get up, little red wyrm...and destroy them!','14','0','100','23','0','8279','Lord Victor Nefarius SAY_VAEL_INTRO (BWL)');

UPDATE creature_template SET spell2=19873, spell3=19872, spell4=0 WHERE entry=12435;
UPDATE gameobject_template SET ScriptName='go_orb_of_domination' WHERE entry=177808;

DELETE FROM spell_script_names WHERE spell_id=19873;
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES
(19873, 'spell_egg_event');

DELETE FROM spell_linked_spell WHERE spell_trigger=42013 AND spell_effect=45537;
INSERT INTO spell_linked_spell (spell_trigger, spell_effect, comment) VALUES
(42013, 45537, 'Visual Channel');

-- nefarian
REPLACE INTO spell_target_position (id, target_map, target_position_x, target_position_y, target_position_z, target_orientation) VALUES
(22981, 469, -7536.73, -1280.18, 476.799, 2.11467),
(22982, 469, -7503.34, -1163.17, 476.797, 2.57019),
(22983, 469, -7482.73, -1196.49, 476.799, 2.31887),
(22984, 469, -7462.38, -1224.99, 476.786, 2.37385),
(22978, 469, -7534.08, -1198.57, 476.799, 2.23641),
(22979, 469, -7514.02, -1224.35, 476.8, 2.21677),
(22980, 469, -7496.05, -1248.45, 476.799, 2.09111),
(22972, 469, -7516.38, -1263.78, 476.773, 2.18536),
(22975, 469, -7582.36, -1213.9, 476.799, 2.17357),
(22976, 469, -7564.18, -1240.31, 476.799, 2.17357),
(22977, 469, -7550.29, -1260.5, 476.799, 2.17357);

UPDATE creature_template SET InhabitType=7 WHERE entry=11583;

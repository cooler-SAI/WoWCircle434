UPDATE `instance_encounters` SET `lastEncounterDungeon`=334 WHERE `entry`=1182 AND `difficulty`=-1;
UPDATE `creature_template` SET `unit_flags`=512, `type_flags`=0, `faction_a`=189, `faction_h`=189 WHERE `entry`=52331;
UPDATE `creature_template` SET `unit_flags`=`type_flags`|512, `flags_extra`=2 WHERE `entry`=52419;
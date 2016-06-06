DELETE FROM `transports` WHERE `guid`=35;
DELETE FROM `transports` WHERE `guid`=21;

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (45831, 'spell_gen_av_drakthar_presence');

UPDATE `creature_template` SET `faction_a`=35, `faction_h`=35 WHERE `entry` IN (34924, 34922, 34918, 34919);

UPDATE `creature_template` SET `unit_flags`=`unit_flags`|33554432, `dynamicflags`=8 WHERE `entry` IN (34935, 34929, 35410, 35427);

UPDATE `creature_classlevelstats` SET `basehp1`=`basehp0` WHERE (`level`>=1 AND `level` <=57) and `basehp1`=1;
UPDATE `creature_classlevelstats` SET `basehp2`=`basehp0` WHERE (`level`>=1 AND `level` <=57) AND `basehp2` = 1;
UPDATE `creature_classlevelstats` SET `basehp3`=`basehp0` WHERE (`level`>=1 AND `level` <=57) AND `basehp3` = 1;

UPDATE `creature_classlevelstats` SET `basehp2`=`basehp1` WHERE (`level`>=58 AND `level` <=67) AND `basehp2` = 1;
UPDATE `creature_classlevelstats` SET `basehp3`=`basehp1` WHERE (`level`>=58 AND `level` <=67) AND `basehp3` = 1;

UPDATE `creature_classlevelstats` SET `basehp3`=`basehp2` WHERE (`level`>=68 AND `level` <=79) AND `basehp3` = 1;
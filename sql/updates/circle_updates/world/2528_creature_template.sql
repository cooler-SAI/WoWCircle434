-- Remove fleeng flag from Treant druid summon

UPDATE creature_template SET unit_flags = 0 WHERE entry = 1964;

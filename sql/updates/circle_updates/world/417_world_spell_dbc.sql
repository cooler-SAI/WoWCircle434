DELETE FROM spell_dbc WHERE id IN (
37264, -- exists
56817, -- exists
54748, -- exists
64748, -- exists
64934, -- exists
65142  -- exists but strange
);

UPDATE spell_dbc SET id = 200000 WHERE id = 100000;
UPDATE spell_dbc SET id = 200001 WHERE id = 100001;
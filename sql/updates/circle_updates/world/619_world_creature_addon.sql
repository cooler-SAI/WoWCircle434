-- client: emote=45 + movementtype=random => memory exhausted => crash
-- possibly all ONESHOT anims should be replaced by STATE
UPDATE creature_template_addon SET emote = 333 WHERE emote = 45;
UPDATE creature_addon SET emote = 333 WHERE emote = 45;
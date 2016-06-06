-- taken from https://github.com/ProjectSkyfire/SkyFireEMU/issues/517

DELETE FROM game_graveyard_zone WHERE id=109;
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (109, 214, 0);
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (109, 5287, 0);
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (109, 5339, 0);

DELETE FROM game_graveyard_zone WHERE id=389;
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (389, 33, 0);
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (389, 214, 0);
INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (389, 5339, 0);
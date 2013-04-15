#ifndef DEF_ENDTIME_H
#define DEF_ENDTIME_H

#define ETScriptName "instance_end_time"

enum Data
{
    DATA_ECHO_OF_SYLVANAS   = 0,
	DATA_ECHO_OF_BAINE      = 1,
	DATA_ECHO_OF_TYRANDE    = 2,
	DATA_ECHO_OF_JAINA      = 3,
    DATA_MUROZOND           = 4,
    DATA_JAINA_PICKED_STATE = 5,
    DATA_ECHO_1             = 6,
    DATA_ECHO_2             = 7,
    DATA_FIRST_ENCOUNTER    = 8,
    DATA_SECOND_ENCOUNTER   = 9,
};

enum GameObjectIds
{
    MUROZOND_CACHE  = 209547,
    GO_ET_TELEPORT  = 209438,
};

enum CreatureIds
{
    NPC_ECHO_OF_JAINA       = 54445,
    NPC_ECHO_OF_BAINE       = 54431,
    NPC_ECHO_OF_SYLVANAS    = 54123,
    NPC_ECHO_OF_TYRANDE     = 54544,
    NPC_MUROZOND            = 54432,
};

enum WorldStatesET
{
    WORLDSTATE_SHOW_FRAGMENTS       = 6046,
    WORLDSTATE_FRAGMENTS_COLLECTED  = 6025,
};

#endif

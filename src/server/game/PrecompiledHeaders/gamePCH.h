//add here most rarely modified headers to speed up debug build compilation
#ifndef _GAMEPCH_H
#define _GAMEPCH_H
#include "WorldSocket.h"        // must be first to make ACE happy with ACE includes in it

#include "Common.h"

#include "MapManager.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "ObjectMgr.h"
#endif  /* _GAMEPCH_H */
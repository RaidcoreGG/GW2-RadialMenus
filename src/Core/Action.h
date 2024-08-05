#ifndef RADIALACTION_H
#define RADIALACTION_H

#include "EActionType.h"
#include "Nexus/Nexus.h"

///----------------------------------------------------------------------------------------------------
/// ActionBase Struct
///----------------------------------------------------------------------------------------------------
struct ActionBase
{
	EActionType Type;
};

///----------------------------------------------------------------------------------------------------
/// ActionGeneric Struct
///----------------------------------------------------------------------------------------------------
struct ActionGeneric : ActionBase
{
	const char* Identifier;
};

///----------------------------------------------------------------------------------------------------
/// ActionGameInputBind Struct
///----------------------------------------------------------------------------------------------------
struct ActionGameInputBind : ActionBase
{
	EGameBinds Identifier;
};

///----------------------------------------------------------------------------------------------------
/// ActionDelay Struct
///----------------------------------------------------------------------------------------------------
struct ActionDelay : ActionBase
{
	int Duration;
};

#endif

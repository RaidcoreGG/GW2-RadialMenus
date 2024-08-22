#ifndef RADIALACTION_H
#define RADIALACTION_H

#include <string>

#include "Conditions.h"
#include "EActionType.h"
#include "Nexus/Nexus.h"

///----------------------------------------------------------------------------------------------------
/// ActionBase Struct
///----------------------------------------------------------------------------------------------------
struct ActionBase
{
	EActionType Type;
	Conditions  Activation;
};

///----------------------------------------------------------------------------------------------------
/// ActionGeneric Struct
///----------------------------------------------------------------------------------------------------
struct ActionGeneric : ActionBase
{
	std::string Identifier;
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

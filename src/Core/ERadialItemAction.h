#ifndef ERADIALITEMACTION_H
#define ERADIALITEMACTION_H

#include "Nexus/Nexus.h"

enum class ERadialItemActionType
{
	None,
	InputBind,
	GameInputBind,
	GameInputBindPress,
	GameInputBindRelease,
	Event,
	Delay
};

struct Action
{
	ERadialItemActionType Type;
};

struct ActionGeneric : Action
{
	const char* Identifier;
};

struct ActionGameInputBind : Action
{
	EGameBinds Identifier;
};

struct ActionDelay : Action
{
	int Duration;
};

#endif

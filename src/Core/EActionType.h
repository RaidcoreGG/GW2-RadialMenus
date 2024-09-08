#ifndef ERADIALACTIONTYPE_H
#define ERADIALACTIONTYPE_H

///----------------------------------------------------------------------------------------------------
/// EActionType Enumeration
///----------------------------------------------------------------------------------------------------
enum class EActionType
{
	None,
	InputBind,
	GameInputBind,
	GameInputBindPress,
	GameInputBindRelease,
	Event,
	Delay,
	Return
};

#endif

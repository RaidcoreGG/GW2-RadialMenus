#ifndef CONDITIONS_H
#define CONDITIONS_H

#include "nlohmann/json.hpp"
using json = nlohmann::json;

///----------------------------------------------------------------------------------------------------
/// EObserveBoolean Enumeration
///----------------------------------------------------------------------------------------------------
enum class EObserveBoolean
{
	None,
	False,
	True
};

///----------------------------------------------------------------------------------------------------
/// Conditions Struct
///----------------------------------------------------------------------------------------------------
struct Conditions
{
	EObserveBoolean IsCombat;
	EObserveBoolean IsMounted;
	EObserveBoolean IsCommander;
	EObserveBoolean IsCompetitive;
	EObserveBoolean IsMapOpen;
	EObserveBoolean IsTextboxActive;
	EObserveBoolean IsInstance;

	/* derived game states */
	EObserveBoolean IsGameplay;
	
	/* derived positional states */
	EObserveBoolean IsUnderwater;
	EObserveBoolean IsOnWaterSurface;
	EObserveBoolean IsAirborne;

	void SetIndex(int aIndex, EObserveBoolean aState);
	EObserveBoolean GetIndex(int aIndex);
};

void to_json(json& j, const Conditions& c);
void from_json(const json& j, Conditions& c);

#endif

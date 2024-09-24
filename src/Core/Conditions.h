#ifndef CONDITIONS_H
#define CONDITIONS_H

#include "nlohmann/json.hpp"
using json = nlohmann::json;

///----------------------------------------------------------------------------------------------------
/// EObserveBoolean Enumeration
///----------------------------------------------------------------------------------------------------
enum class EObserveBoolean
{
	Either,
	False,
	True
};

///----------------------------------------------------------------------------------------------------
/// EObserveMount Enumeration
///----------------------------------------------------------------------------------------------------
enum class EObserveMount
{
	Any = -2,
	NotMounted = -1,

	Either = 0,
	
	Jackal,
	Griffon,
	Springer,
	Skimmer,
	Raptor,
	RollerBeetle,
	Warclaw,
	Skyscale,
	Skiff,
	SiegeTurtle
};

///----------------------------------------------------------------------------------------------------
/// Conditions Struct
///----------------------------------------------------------------------------------------------------
struct Conditions
{
	EObserveBoolean IsCombat;
	EObserveMount   IsMounted;
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

	void SetValue(int aIndex, int aState);
	int GetValue(int aIndex);
};

void to_json(json& j, const Conditions& c);
void from_json(const json& j, Conditions& c);

#endif

#ifndef CONDITIONS_H
#define CONDITIONS_H

#include "nlohmann/json.hpp"
using json = nlohmann::json;

///----------------------------------------------------------------------------------------------------
/// EObserveState Enumeration
///----------------------------------------------------------------------------------------------------
enum class EObserveState
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
	EObserveState IsCombat;
	EObserveState IsMounted;
	EObserveState IsCommander;
	EObserveState IsCompetitive;
	EObserveState IsMapOpen;
	EObserveState IsTextboxActive;
	EObserveState IsInstance;

	/* derived game states */
	EObserveState IsGameplay;
	
	/* derived positional states */
	EObserveState IsUnderwater;
	EObserveState IsOnWaterSurface;
	EObserveState IsAirborne;
};

void to_json(json& j, const Conditions& c);
void from_json(const json& j, Conditions& c);

#endif

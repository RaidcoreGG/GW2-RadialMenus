#ifndef CONDITIONS_H
#define CONDITIONS_H

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

#endif

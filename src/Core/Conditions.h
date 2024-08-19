#ifndef CONDITIONS_H
#define CONDITIONS_H

enum class EObserveState
{
	None,
	MustEqualFalse,
	MustEqualTrue
};

struct Conditions
{
	EObserveState IsCombat;
	EObserveState IsMounted;
	EObserveState IsCommander;
	EObserveState IsCompetitive;
	EObserveState IsMapOpen;

	/* derived positional states */
	EObserveState IsUnderwater;
	EObserveState IsOnWaterSurface;
	EObserveState IsAirborne;
};

#endif

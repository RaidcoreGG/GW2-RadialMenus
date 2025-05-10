#include "Conditions.h"

void Conditions::SetValue(int aIndex, int aState)
{
	switch (aIndex)
	{
		case 0: this->IsCombat = (EObserveBoolean)aState; break;
		case 1: this->IsMounted = (EObserveMount)aState; break;
		case 2: this->IsCommander = (EObserveBoolean)aState; break;
		case 3: this->IsCompetitive = (EObserveBoolean)aState; break;
		case 4: this->IsMapOpen = (EObserveBoolean)aState; break;
		case 5: this->IsTextboxActive = (EObserveBoolean)aState; break;
		case 6: this->IsInstance = (EObserveBoolean)aState; break;

		case 7: this->IsGameplay = (EObserveBoolean)aState; break;

		case 8: this->IsUnderwater = (EObserveBoolean)aState; break;
		case 9: this->IsOnWaterSurface = (EObserveBoolean)aState; break;
		case 10: this->IsAirborne = (EObserveBoolean)aState; break;
	}
}

int Conditions::GetValue(int aIndex)
{
	switch (aIndex)
	{
		case 0: return (int)this->IsCombat;
		case 1: return (int)this->IsMounted;
		case 2: return (int)this->IsCommander;
		case 3: return (int)this->IsCompetitive;
		case 4: return (int)this->IsMapOpen;
		case 5: return (int)this->IsTextboxActive;
		case 6: return (int)this->IsInstance;

		case 7: return (int)this->IsGameplay;

		case 8: return (int)this->IsUnderwater;
		case 9: return (int)this->IsOnWaterSurface;
		case 10: return (int)this->IsAirborne;
	}

	return 0;
}

void to_json(json& j, const Conditions& c)
{
	j = json{
		{"IsCombat", c.IsCombat},
		{"IsMounted", c.IsMounted},
		{"IsCommander", c.IsCommander},
		{"IsCompetitive", c.IsCompetitive},
		{"IsMapOpen", c.IsMapOpen},
		{"IsTextboxActive", c.IsTextboxActive},
		{"IsInstance", c.IsInstance},

		/* derived game states */
		{"IsGameplay", c.IsGameplay},

		/* derived positional states */
		{"IsUnderwater", c.IsUnderwater},
		{"IsOnWaterSurface", c.IsOnWaterSurface},
		{"IsAirborne", c.IsAirborne},
	};
}

void from_json(const json& j, Conditions& c)
{
	c.IsCombat = j.value("IsCombat", EObserveBoolean::Either);
	c.IsMounted = j.value("IsMounted", EObserveMount::Either);
	c.IsCommander = j.value("IsCommander", EObserveBoolean::Either);
	c.IsCompetitive = j.value("IsCompetitive", EObserveBoolean::Either);
	c.IsMapOpen = j.value("IsMapOpen", EObserveBoolean::Either);
	c.IsTextboxActive = j.value("IsTextboxActive", EObserveBoolean::Either);
	c.IsInstance = j.value("IsInstance", EObserveBoolean::Either);

	/* derived game states */
	c.IsGameplay = j.value("IsGameplay", EObserveBoolean::Either);

	/* derived positional states */
	c.IsUnderwater = j.value("IsUnderwater", EObserveBoolean::Either);
	c.IsOnWaterSurface = j.value("IsOnWaterSurface", EObserveBoolean::Either);
	c.IsAirborne = j.value("IsAirborne", EObserveBoolean::Either);
}

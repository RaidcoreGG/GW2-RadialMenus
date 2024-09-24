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
	if (!j["IsCombat"].is_null()) { j["IsCombat"].get_to(c.IsCombat); }
	if (!j["IsMounted"].is_null()) { j["IsMounted"].get_to(c.IsMounted); }
	if (!j["IsCommander"].is_null()) { j["IsCommander"].get_to(c.IsCommander); }
	if (!j["IsCompetitive"].is_null()) { j["IsCompetitive"].get_to(c.IsCompetitive); }
	if (!j["IsMapOpen"].is_null()) { j["IsMapOpen"].get_to(c.IsMapOpen); }
	if (!j["IsTextboxActive"].is_null()) { j["IsTextboxActive"].get_to(c.IsTextboxActive); }
	if (!j["IsInstance"].is_null()) { j["IsInstance"].get_to(c.IsInstance); }

	/* derived game states */
	if (!j["IsGameplay"].is_null()) { j["IsGameplay"].get_to(c.IsGameplay); }

	/* derived positional states */
	if (!j["IsUnderwater"].is_null()) { j["IsUnderwater"].get_to(c.IsUnderwater); }
	if (!j["IsOnWaterSurface"].is_null()) { j["IsOnWaterSurface"].get_to(c.IsOnWaterSurface); }
	if (!j["IsAirborne"].is_null()) { j["IsAirborne"].get_to(c.IsAirborne); }
}

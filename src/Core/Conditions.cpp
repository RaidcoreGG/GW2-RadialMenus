#include "Conditions.h"


void Conditions::SetIndex(int aIndex, EObserveState aState)
{
	switch (aIndex)
	{
		case 0: this->IsCombat = aState; break;
		case 1: this->IsMounted = aState; break;
		case 2: this->IsCommander = aState; break;
		case 3: this->IsCompetitive = aState; break;
		case 4: this->IsMapOpen = aState; break;
		case 5: this->IsTextboxActive = aState; break;
		case 6: this->IsInstance = aState; break;

		case 7: this->IsGameplay = aState; break;

		case 8: this->IsUnderwater = aState; break;
		case 9: this->IsOnWaterSurface = aState; break;
		case 10: this->IsAirborne = aState; break;
	}
}

EObserveState Conditions::GetIndex(int aIndex)
{
	switch (aIndex)
	{
		case 0: return this->IsCombat;
		case 1: return this->IsMounted;
		case 2: return this->IsCommander;
		case 3: return this->IsCompetitive;
		case 4: return this->IsMapOpen;
		case 5: return this->IsTextboxActive;
		case 6: return this->IsInstance;

		case 7: return this->IsGameplay;

		case 8: return this->IsUnderwater;
		case 9: return this->IsOnWaterSurface;
		case 10: return this->IsAirborne;
	}

	return EObserveState::None;
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

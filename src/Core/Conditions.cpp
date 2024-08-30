#include "Conditions.h"

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

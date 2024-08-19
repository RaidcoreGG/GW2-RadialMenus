#ifndef ICON_H
#define ICON_H

#include <string>

#include "EIconType.h"
#include "Nexus/Nexus.h"

///----------------------------------------------------------------------------------------------------
/// Icon Struct
///----------------------------------------------------------------------------------------------------
struct Icon
{
	EIconType                   Type;
	std::string                 Value;

	/* runtime */
	Texture* Texture;
};

#endif

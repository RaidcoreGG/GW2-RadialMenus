#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "Nexus/Nexus.h"

namespace Lang
{
	void Init(LOCALIZATION_SET aSetterFunc)
	{
		if (aSetterFunc == nullptr) { return; }

		aSetterFunc("KB_RADIALS", "en", "All Radials");
	}
}

#endif

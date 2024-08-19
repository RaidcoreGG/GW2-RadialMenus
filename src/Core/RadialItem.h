#ifndef RADIALITEM_H
#define RADIALITEM_H

#include <string>
#include <vector>

#include "Icon.h"
#include "Action.h"

///----------------------------------------------------------------------------------------------------
/// RadialItem Struct
///----------------------------------------------------------------------------------------------------
typedef struct RadialItem
{
	std::string                 Identifier;
	unsigned int                Color;
	unsigned int                ColorHover;
	Icon                        Icon;
	std::vector<ActionBase*>    Actions;
} RadialMenuItem;

#endif

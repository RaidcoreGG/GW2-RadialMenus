#ifndef RADIALITEM_H
#define RADIALITEM_H

#include <string>
#include <vector>

#include "Action.h"
#include "Conditions.h"
#include "Icon.h"

///----------------------------------------------------------------------------------------------------
/// RadialItem Struct
///----------------------------------------------------------------------------------------------------
typedef struct RadialItem
{
	std::string              Identifier;
	int                      Priority;
	unsigned int             Color;
	unsigned int             ColorHover;
	Icon                     Icon;
	Conditions               Visibility;
	Conditions               Activation;
	int                      ActivationTimeout;
	std::vector<ActionBase*> Actions;

	/* runtime values */
	float                    DisplayItemSize;
} RadialMenuItem;

#endif

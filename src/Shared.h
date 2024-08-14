#ifndef SHARED_H
#define SHARED_H

#include <Windows.h>

#include "Nexus/Nexus.h"
#include "Mumble/Mumble.h"
#include "Core/RadialContext.h"

extern HMODULE SelfModule;
extern AddonAPI* APIDefs;
extern AddonDefinition AddonDef;

extern Mumble::Data* MumbleLink;
extern Mumble::Identity* MumbleIdentity;
extern NexusLinkData* NexusLink;

extern CRadialContext* RadialCtx;

#endif

#ifndef SHARED_H
#define SHARED_H

#include <Windows.h>
#include <filesystem>

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
extern std::filesystem::path GW2Root;
extern std::filesystem::path AddonDirectory;
extern std::filesystem::path PacksDirectory;

#endif

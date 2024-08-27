#include "Shared.h"

HMODULE SelfModule                   = nullptr;
AddonAPI* APIDefs                    = nullptr;
AddonDefinition AddonDef             = {};

Mumble::Data* MumbleLink             = nullptr;
Mumble::Identity* MumbleIdentity     = nullptr;
NexusLinkData* NexusLink             = nullptr;

CRadialContext* RadialCtx            = nullptr;
std::filesystem::path GW2Root        = {};
std::filesystem::path AddonDirectory = {};
std::filesystem::path PacksDirectory = {};

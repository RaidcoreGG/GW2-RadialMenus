#include "Shared.h"

HMODULE SelfModule                   = nullptr;
AddonAPI* APIDefs                    = nullptr;
AddonDefinition AddonDef             = {};
HWND WindowHandle                    = nullptr;

Mumble::Data* MumbleLink             = nullptr;
Mumble::Identity* MumbleIdentity     = nullptr;
NexusLinkData* NexusLink             = nullptr;

CRadialContext* RadialCtx            = nullptr;
std::filesystem::path GW2Root        = {};
std::filesystem::path AddonDirectory = {};
std::filesystem::path PacksDirectory = {};
std::filesystem::path IconsDirectory = {};

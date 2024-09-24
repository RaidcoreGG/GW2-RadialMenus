///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  entry.cpp
/// Description  :  Entry point for the addon.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include <Windows.h>

#include "Core/Addon.h"
#include "Remote.h"
#include "Shared.h"
#include "Version.h"

///----------------------------------------------------------------------------------------------------
/// DllMain:
/// 	DLL entry point.
///----------------------------------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: SelfModule = hModule; DisableThreadLibraryCalls(hModule); break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

///----------------------------------------------------------------------------------------------------
/// GetAddonDef:
/// 	Returns information about the addon.
///----------------------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef()
{
	AddonDef.Signature = 67000002;
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "Radial Menus";
	AddonDef.Version.Major = V_MAJOR;
	AddonDef.Version.Minor = V_MINOR;
	AddonDef.Version.Build = V_BUILD;
	AddonDef.Version.Revision = V_REVISION;
	AddonDef.Author = "Raidcore";
	AddonDef.Description = "Highly customisable radial menus.";
	AddonDef.Load = Addon::Load;
	AddonDef.Unload = Addon::Unload;

	AddonDef.Provider = EUpdateProvider_GitHub;
	AddonDef.UpdateLink = REMOTE_URL;

	return &AddonDef;
}

///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  Addon.cpp
/// Description  :  Core addon logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "Addon.h"

#include "imgui/imgui.h"

#include "RTAPI/RTAPI.hpp"

#include "Language.h"
#include "Shared.h"
#include "StateObserver.h"

namespace Addon
{
	// FIXME: TEMPORARY UNTIL NEXUS ADDS DATALINK
	void OnMumbleIdentityUpdated(void* aEventArgs)
	{
		MumbleIdentity = (Mumble::Identity*)aEventArgs;
		std::thread([]() {
			APIDefs->Events.Unsubscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);
		}).detach();
	}

	void Load(AddonAPI* aApi)
	{
		APIDefs = aApi;

		/* If you are using ImGui you will need to set these. */
		ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext);
		ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree);

		MumbleLink = (Mumble::Data*)APIDefs->DataLink.Get("DL_MUMBLE_LINK");
		NexusLink = (NexusLinkData*)APIDefs->DataLink.Get("DL_NEXUS_LINK");
		MumbleIdentity = (Mumble::Identity*)APIDefs->DataLink.Get("DL_MUMBLE_LINK_IDENTITY");
		RTAPIData = (RTAPI::RealTimeData*)APIDefs->DataLink.Get(DL_RTAPI);

		if (RTAPIData && RTAPIData->GameBuild == 0)
		{
			RTAPIData = nullptr;
		}

		GW2Root = APIDefs->Paths.GetGameDirectory();
		AddonDirectory = APIDefs->Paths.GetAddonDirectory("RadialMenus");
		PacksDirectory = APIDefs->Paths.GetAddonDirectory("RadialMenus\\packs");
		IconsDirectory = APIDefs->Paths.GetAddonDirectory("RadialMenus\\icons");

		bool firstInstall = false;
		if (!std::filesystem::exists(AddonDirectory))
		{
			firstInstall = true;
		}
		std::filesystem::create_directory(AddonDirectory);
		std::filesystem::create_directory(PacksDirectory);
		std::filesystem::create_directory(IconsDirectory);

		APIDefs->Events.Subscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);
		APIDefs->Events.Subscribe("EV_ADDON_LOADED", (EVENT_CONSUME)OnAddonLoaded);
		APIDefs->Events.Subscribe("EV_ADDON_UNLOADED", (EVENT_CONSUME)OnAddonUnloaded);

		Lang::Init(APIDefs->Localization.Set);
		RadialCtx = new CRadialContext();
		RadialCtx->Load();

		if (firstInstall == true)
		{
			RadialCtx->CreateDefaultMountRadial();
		}

		APIDefs->Renderer.Register(ERenderType_Render, Addon::Render);
		APIDefs->Renderer.Register(ERenderType_OptionsRender, Addon::RenderOptions);
		APIDefs->WndProc.Register(Addon::WndProc);
	}

	void Unload()
	{
		APIDefs->Events.Unsubscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);
		APIDefs->Events.Unsubscribe("EV_ADDON_LOADED", (EVENT_CONSUME)OnAddonLoaded);
		APIDefs->Events.Unsubscribe("EV_ADDON_UNLOADED", (EVENT_CONSUME)OnAddonUnloaded);

		APIDefs->Renderer.Deregister(Addon::Render);
		APIDefs->Renderer.Deregister(Addon::RenderOptions);
		APIDefs->WndProc.Deregister(Addon::WndProc);

		delete RadialCtx;
	}

	void Render()
	{
		StateObserver::Advance();
		assert(RadialCtx);
		RadialCtx->Render();
	}

	void RenderOptions()
	{
		assert(RadialCtx);
		RadialCtx->RenderOptions();
	}

	void OnAddonLoaded(int* aSignature)
	{
		if (!aSignature) { return; }

		if (*aSignature == RTAPI_SIG)
		{
			RTAPIData = (RTAPI::RealTimeData*)APIDefs->DataLink.Get(DL_RTAPI);
		}
	}
	void OnAddonUnloaded(int* aSignature)
	{
		if (!aSignature) { return; }

		if (*aSignature == RTAPI_SIG)
		{
			RTAPIData = nullptr;
		}
	}

	void OnInputBind(const char* aIdentifier, bool aIsRelease)
	{
		assert(RadialCtx);
		RadialCtx->OnInputBind(aIdentifier, aIsRelease);
	}

	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowHandle = hWnd;

		StateObserver::WndProc(hWnd, uMsg, wParam, lParam);
		assert(RadialCtx);
		return RadialCtx->WndProc(hWnd, uMsg, wParam, lParam);
	}
}

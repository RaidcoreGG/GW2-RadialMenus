///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  Addon.cpp
/// Description  :  Core addon logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "Addon.h"

#include "imgui/imgui.h"

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

		APIDefs->Renderer.Register(ERenderType_Render, Addon::Render);
		APIDefs->Renderer.Register(ERenderType_OptionsRender, Addon::RenderOptions);
		APIDefs->WndProc.Register(Addon::WndProc);

		MumbleLink = (Mumble::Data*)APIDefs->DataLink.Get("DL_MUMBLE_LINK");
		NexusLink = (NexusLinkData*)APIDefs->DataLink.Get("DL_NEXUS_LINK");
		MumbleIdentity = (Mumble::Identity*)APIDefs->DataLink.Get("DL_MUMBLE_LINK_IDENTITY");

		APIDefs->Events.Subscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);

		Lang::Init(APIDefs->Localization.Set);
		RadialCtx = new CRadialContext();

		GW2Root = APIDefs->Paths.GetGameDirectory();
		AddonDirectory = APIDefs->Paths.GetAddonDirectory("RadialMenus");

		//if (!std::filesystem::exists(AddonDirectory))
		{
			std::filesystem::create_directory(AddonDirectory);
			unsigned int col = ImColor(255, 255, 255, 255);
			unsigned int colHov = ImColor(245, 192, 67, 255);

			RadialCtx->Add("Mounts", ERadialType::Normal);

			RadialCtx->AddItem("Mounts", "Raptor", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_RAPTOR.png");
			RadialCtx->AddItemAction("Mounts", "Raptor", EActionType::GameInputBind, EGameBinds_SpumoniMAM01);

			RadialCtx->AddItem("Mounts", "Springer", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_SPRINGER.png");
			RadialCtx->AddItemAction("Mounts", "Springer", EActionType::GameInputBind, EGameBinds_SpumoniMAM02);

			RadialCtx->AddItem("Mounts", "Skimmer", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_SKIMMER.png");
			RadialCtx->AddItemAction("Mounts", "Skimmer", EActionType::GameInputBind, EGameBinds_SpumoniMAM03);

			RadialCtx->AddItem("Mounts", "Jackal", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_JACKAL.png");
			RadialCtx->AddItemAction("Mounts", "Jackal", EActionType::GameInputBind, EGameBinds_SpumoniMAM04);

			RadialCtx->AddItem("Mounts", "Griffon", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_GRIFFON.png");
			RadialCtx->AddItemAction("Mounts", "Griffon", EActionType::GameInputBind, EGameBinds_SpumoniMAM05);

			RadialCtx->AddItem("Mounts", "Roller Beetle", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_ROLLERBEETLE.png");
			RadialCtx->AddItemAction("Mounts", "Roller Beetle", EActionType::GameInputBind, EGameBinds_SpumoniMAM06);

			RadialCtx->AddItem("Mounts", "Warclaw", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_WARCLAW.png");
			RadialCtx->AddItemAction("Mounts", "Warclaw", EActionType::GameInputBind, EGameBinds_SpumoniMAM07);

			RadialCtx->AddItem("Mounts", "Skyscale", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_SKYSCALE.png");
			RadialCtx->AddItemAction("Mounts", "Skyscale", EActionType::GameInputBind, EGameBinds_SpumoniMAM08);

			RadialCtx->AddItem("Mounts", "Siege Turtle", col, colHov, EIconType::File, "addons/Nexus/Textures/ICON_SIEGETURTLE.png");
			RadialCtx->AddItemAction("Mounts", "Siege Turtle", EActionType::GameInputBind, EGameBinds_SpumoniMAM09);
		}
	}

	void Unload()
	{
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
		StateObserver::RenderDebug();
	}

	void OnInputBind(const char* aIdentifier, bool aIsRelease)
	{
		assert(RadialCtx);
		RadialCtx->OnInputBind(aIdentifier, aIsRelease);
	}

	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		StateObserver::WndProc(hWnd, uMsg, wParam, lParam);
		assert(RadialCtx);
		return RadialCtx->WndProc(hWnd, uMsg, wParam, lParam);
	}
}

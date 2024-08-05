#include <math.h>
#include <string>
#include <Windows.h>

#include "Language.h"
#include "Remote.h"
#include "resource.h"
#include "Shared.h"
#include "Version.h"

#include "imgui/imgui.h"
#include "Mumble/Mumble.h"
#include "Nexus/Nexus.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: SelfModule = hModule; break;
	case DLL_PROCESS_DETACH: break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

/* proto */
void AddonLoad(AddonAPI* aApi);
void AddonUnload();
void AddonRender();

void ProcessKeybind(const char* aIdentifier, bool aIsRelease)
{
	std::string identifier = aIdentifier;
	if (identifier == "KB_RADIALS" && !aIsRelease)
	{
		APIDefs->InputBinds.Invoke("KB_MUMBLEOVERLAY", false);
		APIDefs->InputBinds.Invoke("KB_MUMBLEOVERLAY", true);
	}
}

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
	AddonDef.Description = "Adds highly customisable radial menus.";
	AddonDef.Load = AddonLoad;
	AddonDef.Unload = AddonUnload;

	AddonDef.Provider = EUpdateProvider_GitHub;
	AddonDef.UpdateLink = REMOTE_URL;

	return &AddonDef;
}

#include <vector>
#include "Core\RadialMenu.h"
std::vector<CRadialMenu*> Radials;

void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi;

	/* If you are using ImGui you will need to set these. */
	ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree);

	APIDefs->Renderer.Register(ERenderType_Render, AddonRender);
	//APIDefs->RegisterWndProc(AddonWndProc);

	APIDefs->InputBinds.RegisterWithString("KB_RADIALS", ProcessKeybind, "CTRL+R");

	Lang::Init(APIDefs->Localization.Set);

	MumbleLink = (Mumble::Data*)APIDefs->DataLink.Get("DL_MUMBLE_LINK");
	NexusLink = (NexusLinkData*)APIDefs->DataLink.Get("DL_NEXUS_LINK");

	unsigned int col = ImColor(255, 255, 255, 255);
	unsigned int colHov = ImColor(255, 64, 0, 255);

	CRadialMenu* menu1 = new CRadialMenu("RM_TEST", ERadialType::Normal);
	RadialItem* item1 = new RadialItem();
	item1->Color = col;
	item1->ColorHover = colHov;
	item1->IconIdentifier = "ICON_RAPTOR";
	item1->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SquadMarkerPlaceWorld1
							 });
	RadialItem* item2 = new RadialItem();
	item2->Color = col;
	item2->ColorHover = colHov;
	item2->IconIdentifier = "ICON_SPRINGER";
	item2->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM02
							 });
	item2->Actions.push_back(new ActionDelay{
		EActionType::Delay,
		1500
							 });
	item2->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SkillUtility2
							 });
	RadialItem* item3 = new RadialItem();
	item3->Color = col;
	item3->ColorHover = colHov;
	item3->IconIdentifier = "ICON_SKIMMER";
	item3->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM03
							 });
	RadialItem* item4 = new RadialItem();
	item4->Color = col;
	item4->ColorHover = colHov;
	item4->IconIdentifier = "ICON_JACKAL";
	item4->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM04
							 });
	RadialItem* item5 = new RadialItem();
	item5->Color = col;
	item5->ColorHover = colHov;
	item5->IconIdentifier = "ICON_GRIFFON";
	item5->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM05
							 });
	RadialItem* item6 = new RadialItem();
	item6->Color = col;
	item6->ColorHover = colHov;
	item6->IconIdentifier = "ICON_ROLLERBEETLE";
	item6->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM06
							 });
	RadialItem* item7 = new RadialItem();
	item7->Color = col;
	item7->ColorHover = colHov;
	item7->IconIdentifier = "ICON_WARCLAW";
	item7->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM07
							 });
	RadialItem* item8 = new RadialItem();
	item8->Color = col;
	item8->ColorHover = colHov;
	item8->IconIdentifier = "ICON_SKYSCALE";
	item8->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM08
							 });
	item8->Actions.push_back(new ActionGeneric{
		EActionType::InputBind,
		"KB_MUMBLEOVERLAY"
	});
	item8->Actions.push_back(new ActionDelay{
		EActionType::Delay,
		1000
	 });
	item8->Actions.push_back(new ActionGeneric{
		EActionType::InputBind,
		"KB_MUMBLEOVERLAY"
	});
	RadialItem* item9 = new RadialItem();
	item9->Color = col;
	item9->ColorHover = colHov;
	item9->IconIdentifier = "ICON_SIEGETURTLE";
	item9->Actions.push_back(new ActionGameInputBind{
		EActionType::GameInputBind,
		EGameBinds::EGameBinds_SpumoniMAM09
	});
	menu1->AddItem(item1);
	menu1->AddItem(item2);
	menu1->AddItem(item3);
	menu1->AddItem(item4);
	menu1->AddItem(item5);
	menu1->AddItem(item6);
	menu1->AddItem(item7);
	menu1->AddItem(item8);
	menu1->AddItem(item9);
	Radials.push_back(menu1);
}

void AddonUnload()
{
	APIDefs->Renderer.Deregister(AddonRender);
	//APIDefs->DeregisterWndProc(AddonWndProc);
}

void AddonRender()
{
	for (CRadialMenu* radial : Radials)
	{
		radial->Render();
	}
}
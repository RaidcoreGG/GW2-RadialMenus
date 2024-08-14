///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialContext.h
/// Description  :  Primary interface for radial menus.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "RadialContext.h"

#include <map>

#include "imgui/imgui_internal.h"

#include "Addon.h"
#include "Shared.h"
#include "Util.h"

/* helper */
namespace ImGui
{
	bool ColorEdit4U32(const char* label, ImU32* color, ImGuiColorEditFlags flags = 0) {
		float col[4];
		col[0] = (float)((*color >> 0) & 0xFF) / 255.0f;
		col[1] = (float)((*color >> 8) & 0xFF) / 255.0f;
		col[2] = (float)((*color >> 16) & 0xFF) / 255.0f;
		col[3] = (float)((*color >> 24) & 0xFF) / 255.0f;

		bool result = ColorEdit4(label, col, flags);

		*color = ((ImU32)(col[0] * 255.0f)) |
			((ImU32)(col[1] * 255.0f) << 8) |
			((ImU32)(col[2] * 255.0f) << 16) |
			((ImU32)(col[3] * 255.0f) << 24);

		return result;
	}
}

void GameBindSelectable(ActionBase* aAction, const char* aLabel, EGameBinds aGameBind)
{
	if (ImGui::Selectable(APIDefs->Localization.Translate(aLabel)))
	{
		((ActionGameInputBind*)aAction)->Identifier = aGameBind;
	}
}

std::string GameBindToString(EGameBinds aGameBind)
{
	static std::map<EGameBinds, std::string> LookupTable =
	{
		// Movement
		{ EGameBinds_MoveForward, "((MoveForward))" },
		{ EGameBinds_MoveBackward, "((MoveBackward))" },
		{ EGameBinds_MoveLeft, "((MoveLeft))" },
		{ EGameBinds_MoveRight, "((MoveRight))" },
		{ EGameBinds_MoveTurnLeft, "((MoveTurnLeft))" },
		{ EGameBinds_MoveTurnRight, "((MoveTurnRight))" },
		{ EGameBinds_MoveDodge, "((MoveDodge))" },
		{ EGameBinds_MoveAutoRun, "((MoveAutoRun))" },
		{ EGameBinds_MoveWalk, "((MoveWalk))" },
		{ EGameBinds_MoveJump, "((MoveJump))"},
		{ EGameBinds_MoveSwimUp, "((MoveSwimUp))" },
		{ EGameBinds_MoveSwimDown, "((MoveSwimDown))" },
		{ EGameBinds_MoveAboutFace, "((MoveAboutFace))" },

		// Skills
		{ EGameBinds_SkillWeaponSwap, "((SkillWeaponSwap))" },
		{ EGameBinds_SkillWeapon1, "((SkillWeapon1))" },
		{ EGameBinds_SkillWeapon2, "((SkillWeapon2))" },
		{ EGameBinds_SkillWeapon3, "((SkillWeapon3))" },
		{ EGameBinds_SkillWeapon4, "((SkillWeapon4))" },
		{ EGameBinds_SkillWeapon5, "((SkillWeapon5))" },
		{ EGameBinds_SkillHeal, "((SkillHeal))" },
		{ EGameBinds_SkillUtility1, "((SkillUtility1))" },
		{ EGameBinds_SkillUtility2, "((SkillUtility2))" },
		{ EGameBinds_SkillUtility3, "((SkillUtility3))" },
		{ EGameBinds_SkillElite, "((SkillElite))" },
		{ EGameBinds_SkillProfession1, "((SkillProfession1))" },
		{ EGameBinds_SkillProfession2, "((SkillProfession2))" },
		{ EGameBinds_SkillProfession3, "((SkillProfession3))" },
		{ EGameBinds_SkillProfession4, "((SkillProfession4))" },
		{ EGameBinds_SkillProfession5, "((SkillProfession5))" },
		{ EGameBinds_SkillProfession6, "((SkillProfession6))" },
		{ EGameBinds_SkillProfession7, "((SkillProfession7))" },
		{ EGameBinds_SkillSpecialAction, "((SkillSpecialAction))" },

		// Targeting
		{ EGameBinds_TargetAlert, "((TargetAlert))" },
		{ EGameBinds_TargetCall, "((TargetCall))" },
		{ EGameBinds_TargetTake, "((TargetTake))" },
		{ EGameBinds_TargetCallLocal, "((TargetCallLocal))" },
		{ EGameBinds_TargetTakeLocal, "((TargetTakeLocal))" },
		{ EGameBinds_TargetEnemyNearest, "((TargetEnemyNearest))" },
		{ EGameBinds_TargetEnemyNext, "((TargetEnemyNext))" },
		{ EGameBinds_TargetEnemyPrev, "((TargetEnemyPrev))" },
		{ EGameBinds_TargetAllyNearest, "((TargetAllyNearest))" },
		{ EGameBinds_TargetAllyNext, "((TargetAllyNext))" },
		{ EGameBinds_TargetAllyPrev, "((TargetAllyPrev))" },
		{ EGameBinds_TargetLock, "((TargetLock))" },
		{ EGameBinds_TargetSnapGroundTarget, "((TargetSnapGroundTarget))" },
		{ EGameBinds_TargetSnapGroundTargetToggle, "((TargetSnapGroundTargetToggle))" },
		{ EGameBinds_TargetAutoTargetingDisable, "((TargetAutoTargetingDisable))" },
		{ EGameBinds_TargetAutoTargetingToggle, "((TargetAutoTargetingToggle))" },
		{ EGameBinds_TargetAllyTargetingMode, "((TargetAllyTargetingMode))" },
		{ EGameBinds_TargetAllyTargetingModeToggle, "((TargetAllyTargetingModeToggle))" },

		// UI Binds
		{ EGameBinds_UiCommerce, "((UiCommerce))" }, // TradingPost
		{ EGameBinds_UiContacts, "((UiContacts))" },
		{ EGameBinds_UiGuild, "((UiGuild))" },
		{ EGameBinds_UiHero, "((UiHero))" },
		{ EGameBinds_UiInventory, "((UiInventory))" },
		{ EGameBinds_UiKennel, "((UiKennel))" }, // Pets
		{ EGameBinds_UiLogout, "((UiLogout))" },
		{ EGameBinds_UiMail, "((UiMail))" },
		{ EGameBinds_UiOptions, "((UiOptions))" },
		{ EGameBinds_UiParty, "((UiParty))" },
		{ EGameBinds_UiPvp, "((UiPvp))" },
		{ EGameBinds_UiPvpBuild, "((UiPvpBuild))" },
		{ EGameBinds_UiScoreboard, "((UiScoreboard))" },
		{ EGameBinds_UiSeasonalObjectivesShop, "((UiSeasonalObjectivesShop))" }, // Wizard's Vault
		{ EGameBinds_UiInformation, "((UiInformation))" },
		{ EGameBinds_UiChatToggle, "((UiChatToggle))" },
		{ EGameBinds_UiChatCommand, "((UiChatCommand))" },
		{ EGameBinds_UiChatFocus, "((UiChatFocus))" },
		{ EGameBinds_UiChatReply, "((UiChatReply))" },
		{ EGameBinds_UiToggle, "((UiToggle))" },
		{ EGameBinds_UiSquadBroadcastChatToggle, "((UiSquadBroadcastChatToggle))" },
		{ EGameBinds_UiSquadBroadcastChatCommand, "((UiSquadBroadcastChatCommand))" },
		{ EGameBinds_UiSquadBroadcastChatFocus, "((UiSquadBroadcastChatFocus))" },

		// Camera
		{ EGameBinds_CameraFree, "((CameraFree))" },
		{ EGameBinds_CameraZoomIn, "((CameraZoomIn))" },
		{ EGameBinds_CameraZoomOut, "((CameraZoomOut))" },
		{ EGameBinds_CameraReverse, "((CameraReverse))" },
		{ EGameBinds_CameraActionMode, "((CameraActionMode))" },
		{ EGameBinds_CameraActionModeDisable, "((CameraActionModeDisable))" },

		// Screenshots
		{ EGameBinds_ScreenshotNormal, "((ScreenshotNormal))" },
		{ EGameBinds_ScreenshotStereoscopic, "((ScreenshotStereoscopic))" },

		// Map
		{ EGameBinds_MapToggle, "((MapToggle))" },
		{ EGameBinds_MapFocusPlayer, "((MapFocusPlayer))" },
		{ EGameBinds_MapFloorDown, "((MapFloorDown))" },
		{ EGameBinds_MapFloorUp, "((MapFloorUp))" },
		{ EGameBinds_MapZoomIn, "((MapZoomIn))" },
		{ EGameBinds_MapZoomOut, "((MapZoomOut))" },

		// Mounts
		{ EGameBinds_SpumoniToggle, "((SpumoniToggle))" },
		{ EGameBinds_SpumoniMovement, "((SpumoniMovement))" },
		{ EGameBinds_SpumoniSecondaryMovement, "((SpumoniSecondaryMovement))" },
		{ EGameBinds_SpumoniMAM01, "((SpumoniMAM01))" }, // Raptor
		{ EGameBinds_SpumoniMAM02, "((SpumoniMAM02))" }, // Springer
		{ EGameBinds_SpumoniMAM03, "((SpumoniMAM03))" }, // Skimmer
		{ EGameBinds_SpumoniMAM04, "((SpumoniMAM04))" }, // Jackal
		{ EGameBinds_SpumoniMAM05, "((SpumoniMAM05))" }, // Griffon
		{ EGameBinds_SpumoniMAM06, "((SpumoniMAM06))" }, // RollerBeetle
		{ EGameBinds_SpumoniMAM07, "((SpumoniMAM07))" }, // Warclaw
		{ EGameBinds_SpumoniMAM08, "((SpumoniMAM08))" }, // Skyscale
		{ EGameBinds_SpumoniMAM09, "((SpumoniMAM09))" }, // SiegeTurtle

		// Spectator Binds
		{ EGameBinds_SpectatorNearestFixed, "((SpectatorNearestFixed))" },
		{ EGameBinds_SpectatorNearestPlayer, "((SpectatorNearestPlayer))" },
		{ EGameBinds_SpectatorPlayerRed1, "((SpectatorPlayerRed1))" },
		{ EGameBinds_SpectatorPlayerRed2, "((SpectatorPlayerRed2))" },
		{ EGameBinds_SpectatorPlayerRed3, "((SpectatorPlayerRed3))" },
		{ EGameBinds_SpectatorPlayerRed4, "((SpectatorPlayerRed4))" },
		{ EGameBinds_SpectatorPlayerRed5, "((SpectatorPlayerRed5))" },
		{ EGameBinds_SpectatorPlayerBlue1, "((SpectatorPlayerBlue1))" },
		{ EGameBinds_SpectatorPlayerBlue2, "((SpectatorPlayerBlue2))" },
		{ EGameBinds_SpectatorPlayerBlue3, "((SpectatorPlayerBlue3))" },
		{ EGameBinds_SpectatorPlayerBlue4, "((SpectatorPlayerBlue4))" },
		{ EGameBinds_SpectatorPlayerBlue5, "((SpectatorPlayerBlue5))" },
		{ EGameBinds_SpectatorFreeCamera, "((SpectatorFreeCamera))" },
		{ EGameBinds_SpectatorFreeCameraMode, "((SpectatorFreeCameraMode))" },
		{ EGameBinds_SpectatorFreeMoveForward, "((SpectatorFreeMoveForward))" },
		{ EGameBinds_SpectatorFreeMoveBackward, "((SpectatorFreeMoveBackward))" },
		{ EGameBinds_SpectatorFreeMoveLeft, "((SpectatorFreeMoveLeft))" },
		{ EGameBinds_SpectatorFreeMoveRight, "((SpectatorFreeMoveRight))" },
		{ EGameBinds_SpectatorFreeMoveUp, "((SpectatorFreeMoveUp))" },
		{ EGameBinds_SpectatorFreeMoveDown, "((SpectatorFreeMoveDown))" },

		// Squad Markers
		{ EGameBinds_SquadMarkerPlaceWorld1, "((SquadMarkerPlaceWorld1))" }, // Arrow
		{ EGameBinds_SquadMarkerPlaceWorld2, "((SquadMarkerPlaceWorld2))" }, // Circle
		{ EGameBinds_SquadMarkerPlaceWorld3, "((SquadMarkerPlaceWorld3))" }, // Heart
		{ EGameBinds_SquadMarkerPlaceWorld4, "((SquadMarkerPlaceWorld4))" }, // Square
		{ EGameBinds_SquadMarkerPlaceWorld5, "((SquadMarkerPlaceWorld5))" }, // Star
		{ EGameBinds_SquadMarkerPlaceWorld6, "((SquadMarkerPlaceWorld6))" }, // Swirl
		{ EGameBinds_SquadMarkerPlaceWorld7, "((SquadMarkerPlaceWorld7))" }, // Triangle
		{ EGameBinds_SquadMarkerPlaceWorld8, "((SquadMarkerPlaceWorld8))" }, // Cross
		{ EGameBinds_SquadMarkerClearAllWorld, "((SquadMarkerClearAllWorld))" },
		{ EGameBinds_SquadMarkerSetAgent1, "((SquadMarkerSetAgent1))" }, // Arrow
		{ EGameBinds_SquadMarkerSetAgent2, "((SquadMarkerSetAgent2))" }, // Circle
		{ EGameBinds_SquadMarkerSetAgent3, "((SquadMarkerSetAgent3))" }, // Heart
		{ EGameBinds_SquadMarkerSetAgent4, "((SquadMarkerSetAgent4))" }, // Square
		{ EGameBinds_SquadMarkerSetAgent5, "((SquadMarkerSetAgent5))" }, // Star
		{ EGameBinds_SquadMarkerSetAgent6, "((SquadMarkerSetAgent6))" }, // Swirl
		{ EGameBinds_SquadMarkerSetAgent7, "((SquadMarkerSetAgent7))" }, // Triangle
		{ EGameBinds_SquadMarkerSetAgent8, "((SquadMarkerSetAgent8))" }, // Cross
		{ EGameBinds_SquadMarkerClearAllAgent, "((SquadMarkerClearAllAgent))" },

		// Mastery Skills
		{ EGameBinds_MasteryAccess, "((MasteryAccess))" },
		{ EGameBinds_MasteryAccess01, "((MasteryAccess01))" }, // Fishing
		{ EGameBinds_MasteryAccess02, "((MasteryAccess02))" }, // Skiff
		{ EGameBinds_MasteryAccess03, "((MasteryAccess03))" }, // Jade Bot Waypoint
		{ EGameBinds_MasteryAccess04, "((MasteryAccess04))" }, // Rift Scan
		{ EGameBinds_MasteryAccess05, "((MasteryAccess05))" }, // Skyscale

		// Miscellaneous Binds
		{ EGameBinds_MiscAoELoot, "((MiscAoELoot))" },
		{ EGameBinds_MiscInteract, "((MiscInteract))" },
		{ EGameBinds_MiscShowEnemies, "((MiscShowEnemies))" },
		{ EGameBinds_MiscShowAllies, "((MiscShowAllies))" },
		{ EGameBinds_MiscCombatStance, "((MiscCombatStance))" }, // Stow/Draw
		{ EGameBinds_MiscToggleLanguage, "((MiscToggleLanguage))" },
		{ EGameBinds_MiscTogglePetCombat, "((MiscTogglePetCombat))" },
		{ EGameBinds_MiscToggleFullScreen, "((MiscToggleFullScreen))" },

		// Toys/Novelties
		{ EGameBinds_ToyUseDefault, "((ToyUseDefault))" },
		{ EGameBinds_ToyUseSlot1, "((ToyUseSlot1))" }, // Chair
		{ EGameBinds_ToyUseSlot2, "((ToyUseSlot2))" }, // Instrument
		{ EGameBinds_ToyUseSlot3, "((ToyUseSlot3))" }, // Held Item
		{ EGameBinds_ToyUseSlot4, "((ToyUseSlot4))" }, // Toy
		{ EGameBinds_ToyUseSlot5, "((ToyUseSlot5))" }, // Tonic
		//ToyUseSlot6 unused

		// Build Templates
		{ EGameBinds_Loadout1, "((Loadout1))" },
		{ EGameBinds_Loadout2, "((Loadout2))" },
		{ EGameBinds_Loadout3, "((Loadout3))" },
		{ EGameBinds_Loadout4, "((Loadout4))" },
		{ EGameBinds_Loadout5, "((Loadout5))" },
		{ EGameBinds_Loadout6, "((Loadout6))" },
		{ EGameBinds_Loadout7, "((Loadout7))" },
		{ EGameBinds_Loadout8, "((Loadout8))" },

		// Equipment Templates
		{ EGameBinds_GearLoadout1, "((GearLoadout1))" },
		{ EGameBinds_GearLoadout2, "((GearLoadout2))" },
		{ EGameBinds_GearLoadout3, "((GearLoadout3))" },
		{ EGameBinds_GearLoadout4, "((GearLoadout4))" },
		{ EGameBinds_GearLoadout5, "((GearLoadout5))" },
		{ EGameBinds_GearLoadout6, "((GearLoadout6))" },
		{ EGameBinds_GearLoadout7, "((GearLoadout7))" },
		{ EGameBinds_GearLoadout8, "((GearLoadout8))" }
	};

	return LookupTable[aGameBind];
}

void CRadialContext::Activate(CRadialMenu* aRadial)
{
	if (!aRadial) { return; }

	if (this->ActiveRadial)
	{
		ESelectionMode mode = this->ActiveRadial->GetSelectionMode();
		/* if there's an active radial and the keybind is pressed again with click mode -> cancel */
		if (mode == ESelectionMode::Click || mode == ESelectionMode::Hover)
		{
			this->Release(ESelectionMode::Escape);
		}
		return;
	}
	else
	{
		this->ActiveRadial = aRadial;
		this->ActiveRadial->Activate();
	}
}

bool CRadialContext::Release(ESelectionMode aMode)
{
	if (!this->ActiveRadial) { return false; }

	ESelectionMode targetMode = this->ActiveRadial->GetSelectionMode();

	switch (aMode)
	{
		default:
		case ESelectionMode::None: { return false; }
		case ESelectionMode::Click:
		{
			if (!(targetMode == ESelectionMode::Click || targetMode == ESelectionMode::ReleaseOrClick)) { return false; }
			this->ActiveRadial->Release();
			break;
		}
		case ESelectionMode::Release:
		{
			if (!(targetMode == ESelectionMode::Release || targetMode == ESelectionMode::ReleaseOrClick)) { return false; }
			this->ActiveRadial->Release();
			break;
		}
		case ESelectionMode::Hover:
		{
			if (targetMode != ESelectionMode::Hover) { return false; }
			this->ActiveRadial->Release();
			break;
		}
		case ESelectionMode::Escape:
		{
			this->ActiveRadial->Release(true);
			break;
		}
	}

	this->ActiveRadial = nullptr;

	return true;
}

void CRadialContext::Add(std::string aIdentifier, ERadialType aType)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	CRadialMenu* radial = new CRadialMenu(APIDefs, SelfModule, 1, aIdentifier, aType);
	std::string bind = radial->GetInputBind();
	this->RadialIBMap[bind] = radial;
	APIDefs->InputBinds.RegisterWithString(bind.c_str(), Addon::OnInputBind, "(null)");
	APIDefs->Localization.Set(bind.c_str(), "en", aIdentifier.c_str());
	this->Radials.push_back(radial);
}

void CRadialContext::Remove(std::string aIdentifier)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [aIdentifier](CRadialMenu* menu) { return menu->GetName() == aIdentifier; });
	
	if (it != this->Radials.end())
	{
		delete (*it);
		this->Radials.erase(it);
	}
}

void CRadialContext::AddItem(std::string aRadialId, std::string aItemId, unsigned int aColor, unsigned int aColorHover, EIconType aIconType, std::string aIconValue)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [aRadialId](CRadialMenu* menu) { return menu->GetName() == aRadialId; });

	if (it != this->Radials.end())
	{
		RadialItem* item = new RadialItem();
		item->Identifier = aItemId;
		item->Color = aColor;
		item->ColorHover = aColorHover;
		item->Icon.Type = aIconType;
		item->Icon.Value = aIconValue;
		(*it)->AddItem(item);
	}
}

void CRadialContext::RemoveItem(std::string aRadialId, std::string aItemId)
{

}

void CRadialContext::AddItemAction(std::string aRadialId, std::string aItemId, EActionType aType, std::string aValue)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [aRadialId](CRadialMenu* menu) { return menu->GetName() == aRadialId; });

	if (it != this->Radials.end())
	{
		RadialItem* item = (*it)->GetItem(aItemId);

		if (item)
		{
			ActionGeneric* action = new ActionGeneric();
			action->Type = aType;
			action->Identifier = _strdup(aValue.c_str());

			item->Actions.push_back(action);
		}
	}
}

void CRadialContext::AddItemAction(std::string aRadialId, std::string aItemId, EActionType aType, EGameBinds aValue)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [aRadialId](CRadialMenu* menu) { return menu->GetName() == aRadialId; });

	if (it != this->Radials.end())
	{
		RadialItem* item = (*it)->GetItem(aItemId);

		if (item)
		{
			ActionGameInputBind* action = new ActionGameInputBind();
			action->Type = aType;
			action->Identifier = aValue;

			item->Actions.push_back(action);
		}
	}
}

void CRadialContext::AddItemAction(std::string aRadialId, std::string aItemId, int aValue)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [aRadialId](CRadialMenu* menu) { return menu->GetName() == aRadialId; });

	if (it != this->Radials.end())
	{
		RadialItem* item = (*it)->GetItem(aItemId);

		if (item)
		{
			ActionDelay* action = new ActionDelay();
			action->Type = EActionType::Delay;
			action->Duration = aValue;

			item->Actions.push_back(action);
		}
	}
}

void CRadialContext::RemoveItemAction(std::string aRadialId, std::string aItemId)
{

}

void CRadialContext::Render()
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	for (CRadialMenu* radial : this->Radials)
	{
		radial->Render();
		if (radial->GetHoveredIndex() > -1 && radial->GetSelectionMode() == ESelectionMode::Hover)
		{
			this->Release(ESelectionMode::Hover);
		}
	}
}

void CRadialContext::RenderOptions()
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	static CRadialMenu* EditingMenu = nullptr;
	static RadialItem* EditingItem = nullptr;

	float width = ImGui::GetWindowContentRegionWidth();

	ImGui::BeginChild("##radialitemselector", ImVec2(width / 4, 0));
	ImGui::TextDisabled("Select Radial Menu:");
	for (CRadialMenu* radial : this->Radials)
	{
		if (ImGui::TreeNode(radial->GetName().c_str()))
		{
			if (ImGui::Selectable(("Settings##" + radial->GetName()).c_str(), EditingMenu == radial))
			{
				EditingMenu = radial;
				EditingItem = nullptr;
			}
			ImGui::Separator();

			int capacity = radial->GetCapacity();
			std::vector<RadialItem*> items = radial->GetItems();

			int i = 0;

			for (RadialItem* item : items)
			{
				i++;

				ImGui::TextDisabled("Item");
				ImGui::SameLine();
				if (i > capacity)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 255, 0, 255));
				}
				if (ImGui::Selectable(item->Identifier.c_str(), EditingItem == item))
				{
					EditingMenu = nullptr;
					EditingItem = item;
				}
				if (i > capacity)
				{
					ImGui::PopStyleColor();
				}
			}

			if (capacity > items.size())
			{
				if (ImGui::Button(("Add##" + radial->GetName()).c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::GetCursorPosX(), 0)))
				{

				}
			}
			else if (capacity < items.size())
			{
				ImGui::TextColored(ImColor(255, 0, 0, 255), "Too many items!");
			}

			ImGui::TreePop();
		}
	}
	if (ImGui::Button("New##radial", ImVec2(ImGui::GetWindowContentRegionWidth(), 0)))
	{

	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##radialitemeditor", ImVec2(width / 4 * 3, 0));
	if (EditingMenu) /* editing general menu settings */
	{
		ImGui::TextDisabled("Input Bind");
		ImGui::Text(EditingMenu->GetInputBind().c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("shown as \"%s\"", EditingMenu->GetName().c_str());

		ImGui::TextDisabled("Name");

		static bool editingName;
		static char inputBuff[64];
		if (!editingName)
		{
			if (ImGui::Button(EditingMenu->GetName().c_str()))
			{
				editingName = true;
				strcpy_s(inputBuff, EditingMenu->GetName().c_str());
			}
		}
		else
		{
			if (ImGui::InputText("##radialname", inputBuff, 64, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editingName = false;
				EditingMenu->SetName(inputBuff);
				APIDefs->Localization.Set(EditingMenu->GetInputBind().c_str(), "en", inputBuff);
			}
		}

		ImGui::TextDisabled("Type");
		std::string type;
		switch (EditingMenu->GetType())
		{
			case ERadialType::Small:
				type = "Small";
				break;
			case ERadialType::Normal:
				type = "Normal";
				break;
		}
		if (ImGui::BeginCombo("##radialtype", type.c_str()))
		{
			if (ImGui::Selectable("Small"))
			{
				EditingMenu->SetType(ERadialType::Small);
			}
			if (ImGui::Selectable("Normal"))
			{
				EditingMenu->SetType(ERadialType::Normal);
			}
			ImGui::EndCombo();
		}

		ImGui::TextDisabled("Selection Mode");
		std::string selMode;
		switch (EditingMenu->GetSelectionMode())
		{
			case ESelectionMode::Click:
				selMode = "Click";
				break;
			case ESelectionMode::Release:
				selMode = "Release";
				break;
			case ESelectionMode::ReleaseOrClick:
				selMode = "Release / Click";
				break;
			case ESelectionMode::Hover:
				selMode = "Hover";
				break;
		}
		if (ImGui::BeginCombo("##radialselectionmode", selMode.c_str()))
		{
			if (ImGui::Selectable("Click"))
			{
				EditingMenu->SetSelectionMode(ESelectionMode::Click);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Left mouse button will select the element.");
				ImGui::EndTooltip();
			}

			if (ImGui::Selectable("Release"))
			{
				EditingMenu->SetSelectionMode(ESelectionMode::Release);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Releasing the input bind will select the element.");
				ImGui::EndTooltip();
			}

			if (ImGui::Selectable("Release / Click"))
			{
				EditingMenu->SetSelectionMode(ESelectionMode::ReleaseOrClick);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Releasing the input bind or left mouse button will select the element.");
				ImGui::EndTooltip();
			}

			if (ImGui::Selectable("Hover"))
			{
				EditingMenu->SetSelectionMode(ESelectionMode::Hover);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Hovering over an element will immediately select it.");
				ImGui::EndTooltip();
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Draw in Center", &EditingMenu->DrawInCenter);
		ImGui::Checkbox("Restore Cursor Position", &EditingMenu->RestoreCursor);
	}
	else if (EditingItem) /* editing sub item */
	{
		ImGui::TextDisabled("Name");

		static bool editingName;
		static char inputBuff[64];
		if (!editingName)
		{
			if (ImGui::Button(EditingItem->Identifier.c_str()))
			{
				editingName = true;
				strcpy_s(inputBuff, EditingItem->Identifier.c_str());
			}
		}
		else
		{
			if (ImGui::InputText("##radialitemname", inputBuff, 64, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editingName = false;
				EditingItem->Identifier = inputBuff;
			}
		}

		ImGui::TextDisabled("Color");
		ImGui::ColorEdit4U32("Color", &EditingItem->Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		if (ImGui::SmallButton("Apply to all##color"))
		{
			unsigned int colorCopy = EditingItem->Color;
			std::string name = EditingItem->Identifier;

			auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [name](CRadialMenu* menu)
			{
				bool found = false;
				for (RadialItem* item : menu->GetItems())
				{
					if (item->Identifier == name)
					{
						found = true;
					}
				}
				return found;
			});

			if (it != this->Radials.end())
			{
				for (RadialItem* item : (*it)->GetItems())
				{
					item->Color = colorCopy;
				}
			}
		}

		ImGui::TextDisabled("Color Hover");
		ImGui::ColorEdit4U32("Color Hover", &EditingItem->ColorHover, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		if (ImGui::SmallButton("Apply to all##colorhover"))
		{
			unsigned int colorCopy = EditingItem->ColorHover;
			std::string name = EditingItem->Identifier;

			auto it = std::find_if(this->Radials.begin(), this->Radials.end(), [name](CRadialMenu* menu)
			{
				bool found = false;
				for (RadialItem* item : menu->GetItems())
				{
					if (item->Identifier == name)
					{
						found = true;
					}
				}
				return found;
			});

			if (it != this->Radials.end())
			{
				for (RadialItem* item : (*it)->GetItems())
				{
					item->ColorHover = colorCopy;
				}
			}
		}

		ImGui::TextDisabled("Icon");

		std::string iconType;
		switch (EditingItem->Icon.Type)
		{
			case EIconType::File:
				iconType = "File";
				break;
			case EIconType::URL:
				iconType = "URL";
				break;
		}
		if (ImGui::BeginCombo("##radialitemicontype", iconType.c_str()))
		{
			if (ImGui::Selectable("File"))
			{
				EditingItem->Icon.Type = EIconType::File;
			}
			if (ImGui::Selectable("URL"))
			{
				EditingItem->Icon.Type = EIconType::URL;
			}
			ImGui::EndCombo();
		}

		if (EditingItem->Icon.Type == EIconType::File)
		{
			if (ImGui::Button(EditingItem->Icon.Value.c_str(), ImVec2(ImGui::CalcItemWidth(), 0)))
			{
				OPENFILENAME ofn{};
				char buff[MAX_PATH]{};

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = 0;
				ofn.lpstrFile = buff;
				ofn.nMaxFile = MAX_PATH;
				//ofn.lpstrFilter = "Guild Wars 2 Executable\0*.exe";
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn))
				{
					EditingItem->Icon.Value = ofn.lpstrFile != 0 ? ofn.lpstrFile : "";
					if (!EditingItem->Icon.Value.empty())
					{
						EditingItem->Icon.Texture = nullptr;
						APIDefs->Textures.LoadFromFile(EditingItem->Icon.Value.c_str(), EditingItem->Icon.Value.c_str(), nullptr);
					}
				}
			}
		}
		else if (EditingItem->Icon.Type == EIconType::URL)
		{
			static bool editingUrl;
			static char inputBuff[MAX_PATH];
			if (!editingUrl)
			{
				if (ImGui::Button(EditingItem->Icon.Value.c_str(), ImVec2(ImGui::CalcItemWidth(), 0)))
				{
					editingUrl = true;
					strcpy_s(inputBuff, EditingItem->Icon.Value.c_str());
				}
			}
			else
			{
				if (ImGui::InputText("##radialitemiconurl", inputBuff, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					editingUrl = false;
					EditingItem->Icon.Value = inputBuff;
					if (!EditingItem->Icon.Value.empty())
					{
						EditingItem->Icon.Texture = nullptr;
						APIDefs->Textures.LoadFromURL(EditingItem->Icon.Value.c_str(), URL::GetBase(EditingItem->Icon.Value).c_str(), URL::GetEndpoint(EditingItem->Icon.Value).c_str(), nullptr);
					}
				}
			}
		}

		ImGui::TextDisabled("Actions");

		ImGui::BeginTable("##radialitemactions", 3);
		int i = 0;
		int idxDel = -1;
		for (ActionBase* action : EditingItem->Actions)
		{
			std::string actionType;
			switch (action->Type)
			{
				case EActionType::InputBind: actionType = "InputBind (Nexus)"; break;
				case EActionType::GameInputBind: actionType = "InputBind (Game)"; break;
				case EActionType::GameInputBindPress: actionType = "InputBind Press (Game)"; break;
				case EActionType::GameInputBindRelease: actionType = "InputBind Release (Game)"; break;
				case EActionType::Event: actionType = "Event"; break;
				case EActionType::Delay: actionType = "Delay"; break;
			}
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::BeginCombo(("##radialitemactiontype" + std::to_string(i)).c_str(), actionType.c_str()))
			{
				if (ImGui::Selectable("InputBind (Nexus)"))
				{
					if (action->Type != EActionType::InputBind)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionGeneric();
						EditingItem->Actions[i]->Type = EActionType::InputBind;
					}
				}
				if (ImGui::Selectable("InputBind (Game)"))
				{
					if (action->Type != EActionType::GameInputBind)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionGameInputBind();
						EditingItem->Actions[i]->Type = EActionType::GameInputBind;
					}
				}
				if (ImGui::Selectable("InputBind Press (Game)"))
				{
					if (action->Type != EActionType::GameInputBindPress)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionGameInputBind();
						EditingItem->Actions[i]->Type = EActionType::GameInputBindPress;
					}
				}
				if (ImGui::Selectable("InputBind Release (Game)"))
				{
					if (action->Type != EActionType::GameInputBindRelease)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionGameInputBind();
						EditingItem->Actions[i]->Type = EActionType::GameInputBindRelease;
					}
				}
				if (ImGui::Selectable("Event"))
				{
					if (action->Type != EActionType::Event)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionGeneric();
						EditingItem->Actions[i]->Type = EActionType::Event;
					}
				}
				if (ImGui::Selectable("Delay"))
				{
					if (action->Type != EActionType::Delay)
					{
						delete EditingItem->Actions[i];
						EditingItem->Actions[i] = new ActionDelay();
						EditingItem->Actions[i]->Type = EActionType::Delay;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			switch (action->Type)
			{
				case EActionType::InputBind:
				case EActionType::Event:
				{
					static char inputBuff[MAX_PATH];
					strcpy_s(inputBuff, ((ActionGeneric*)action)->Identifier.c_str());

					if (ImGui::InputText(("##actionidentifier" + std::to_string(i)).c_str(), inputBuff, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
					{
						((ActionGeneric*)action)->Identifier = _strdup(inputBuff);
					}
					break;
				}
				case EActionType::GameInputBind:
				case EActionType::GameInputBindPress:
				case EActionType::GameInputBindRelease:
				{
					if (ImGui::BeginCombo(("##actiongamebind" + std::to_string(i)).c_str(), APIDefs->Localization.Translate(GameBindToString(((ActionGameInputBind*)action)->Identifier).c_str())))
					{
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Movement))")))
						{
							GameBindSelectable(action, "((MoveForward))", EGameBinds_MoveForward);
							GameBindSelectable(action, "((MoveBackward))", EGameBinds_MoveBackward);
							GameBindSelectable(action, "((MoveLeft))", EGameBinds_MoveLeft);
							GameBindSelectable(action, "((MoveRight))", EGameBinds_MoveRight);
							GameBindSelectable(action, "((MoveTurnLeft))", EGameBinds_MoveTurnLeft);
							GameBindSelectable(action, "((MoveTurnRight))", EGameBinds_MoveTurnRight);
							GameBindSelectable(action, "((MoveDodge))", EGameBinds_MoveDodge);
							GameBindSelectable(action, "((MoveAutoRun))", EGameBinds_MoveAutoRun);
							GameBindSelectable(action, "((MoveWalk))", EGameBinds_MoveWalk);
							GameBindSelectable(action, "((MoveJump))", EGameBinds_MoveJump);
							GameBindSelectable(action, "((MoveSwimUp))", EGameBinds_MoveSwimUp);
							GameBindSelectable(action, "((MoveSwimDown))", EGameBinds_MoveSwimDown);
							GameBindSelectable(action, "((MoveAboutFace))", EGameBinds_MoveAboutFace);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Skills))")))
						{
							GameBindSelectable(action, "((SkillWeaponSwap))", EGameBinds_SkillWeaponSwap);
							GameBindSelectable(action, "((SkillWeapon1))", EGameBinds_SkillWeapon1);
							GameBindSelectable(action, "((SkillWeapon2))", EGameBinds_SkillWeapon2);
							GameBindSelectable(action, "((SkillWeapon3))", EGameBinds_SkillWeapon3);
							GameBindSelectable(action, "((SkillWeapon4))", EGameBinds_SkillWeapon4);
							GameBindSelectable(action, "((SkillWeapon5))", EGameBinds_SkillWeapon5);
							GameBindSelectable(action, "((SkillHeal))", EGameBinds_SkillHeal);
							GameBindSelectable(action, "((SkillUtility1))", EGameBinds_SkillUtility1);
							GameBindSelectable(action, "((SkillUtility2))", EGameBinds_SkillUtility2);
							GameBindSelectable(action, "((SkillUtility3))", EGameBinds_SkillUtility3);
							GameBindSelectable(action, "((SkillElite))", EGameBinds_SkillElite);
							GameBindSelectable(action, "((SkillProfession1))", EGameBinds_SkillProfession1);
							GameBindSelectable(action, "((SkillProfession2))", EGameBinds_SkillProfession2);
							GameBindSelectable(action, "((SkillProfession3))", EGameBinds_SkillProfession3);
							GameBindSelectable(action, "((SkillProfession4))", EGameBinds_SkillProfession4);
							GameBindSelectable(action, "((SkillProfession5))", EGameBinds_SkillProfession5);
							GameBindSelectable(action, "((SkillProfession6))", EGameBinds_SkillProfession6);
							GameBindSelectable(action, "((SkillProfession7))", EGameBinds_SkillProfession7);
							GameBindSelectable(action, "((SkillSpecialAction))", EGameBinds_SkillSpecialAction);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Targeting))")))
						{
							GameBindSelectable(action, "((TargetAlert))", EGameBinds_TargetAlert);
							GameBindSelectable(action, "((TargetCall))", EGameBinds_TargetCall);
							GameBindSelectable(action, "((TargetTake))", EGameBinds_TargetTake);
							GameBindSelectable(action, "((TargetCallLocal))", EGameBinds_TargetCallLocal);
							GameBindSelectable(action, "((TargetTakeLocal))", EGameBinds_TargetTakeLocal);
							GameBindSelectable(action, "((TargetEnemyNearest))", EGameBinds_TargetEnemyNearest);
							GameBindSelectable(action, "((TargetEnemyNext))", EGameBinds_TargetEnemyNext);
							GameBindSelectable(action, "((TargetEnemyPrev))", EGameBinds_TargetEnemyPrev);
							GameBindSelectable(action, "((TargetAllyNearest))", EGameBinds_TargetAllyNearest);
							GameBindSelectable(action, "((TargetAllyNext))", EGameBinds_TargetAllyNext);
							GameBindSelectable(action, "((TargetAllyPrev))", EGameBinds_TargetAllyPrev);
							GameBindSelectable(action, "((TargetLock))", EGameBinds_TargetLock);
							GameBindSelectable(action, "((TargetSnapGroundTarget))", EGameBinds_TargetSnapGroundTarget);
							GameBindSelectable(action, "((TargetSnapGroundTargetToggle))", EGameBinds_TargetSnapGroundTargetToggle);
							GameBindSelectable(action, "((TargetAutoTargetingDisable))", EGameBinds_TargetAutoTargetingDisable);
							GameBindSelectable(action, "((TargetAutoTargetingToggle))", EGameBinds_TargetAutoTargetingToggle);
							GameBindSelectable(action, "((TargetAllyTargetingMode))", EGameBinds_TargetAllyTargetingMode);
							GameBindSelectable(action, "((TargetAllyTargetingModeToggle))", EGameBinds_TargetAllyTargetingModeToggle);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((User Interface))")))
						{
							GameBindSelectable(action, "((UiCommerce))", EGameBinds_UiCommerce);
							GameBindSelectable(action, "((UiContacts))", EGameBinds_UiContacts);
							GameBindSelectable(action, "((UiGuild))", EGameBinds_UiGuild);
							GameBindSelectable(action, "((UiHero))", EGameBinds_UiHero);
							GameBindSelectable(action, "((UiInventory))", EGameBinds_UiInventory);
							GameBindSelectable(action, "((UiKennel))", EGameBinds_UiKennel);
							GameBindSelectable(action, "((UiLogout))", EGameBinds_UiLogout);
							GameBindSelectable(action, "((UiMail))", EGameBinds_UiMail);
							GameBindSelectable(action, "((UiOptions))", EGameBinds_UiOptions);
							GameBindSelectable(action, "((UiParty))", EGameBinds_UiParty);
							GameBindSelectable(action, "((UiPvp))", EGameBinds_UiPvp);
							GameBindSelectable(action, "((UiPvpBuild))", EGameBinds_UiPvpBuild);
							GameBindSelectable(action, "((UiScoreboard))", EGameBinds_UiScoreboard);
							GameBindSelectable(action, "((UiSeasonalObjectivesShop))", EGameBinds_UiSeasonalObjectivesShop);
							GameBindSelectable(action, "((UiInformation))", EGameBinds_UiInformation);
							GameBindSelectable(action, "((UiChatToggle))", EGameBinds_UiChatToggle);
							GameBindSelectable(action, "((UiChatCommand))", EGameBinds_UiChatCommand);
							GameBindSelectable(action, "((UiChatFocus))", EGameBinds_UiChatFocus);
							GameBindSelectable(action, "((UiChatReply))", EGameBinds_UiChatReply);
							GameBindSelectable(action, "((UiToggle))", EGameBinds_UiToggle);
							GameBindSelectable(action, "((UiSquadBroadcastChatToggle))", EGameBinds_UiSquadBroadcastChatToggle);
							GameBindSelectable(action, "((UiSquadBroadcastChatCommand))", EGameBinds_UiSquadBroadcastChatCommand);
							GameBindSelectable(action, "((UiSquadBroadcastChatFocus))", EGameBinds_UiSquadBroadcastChatFocus);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Camera))")))
						{
							GameBindSelectable(action, "((CameraFree))", EGameBinds_CameraFree);
							GameBindSelectable(action, "((CameraZoomIn))", EGameBinds_CameraZoomIn);
							GameBindSelectable(action, "((CameraZoomOut))", EGameBinds_CameraZoomOut);
							GameBindSelectable(action, "((CameraReverse))", EGameBinds_CameraReverse);
							GameBindSelectable(action, "((CameraActionMode))", EGameBinds_CameraActionMode);
							GameBindSelectable(action, "((CameraActionModeDisable))", EGameBinds_CameraActionModeDisable);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Screenshot))")))
						{
							GameBindSelectable(action, "((ScreenshotNormal))", EGameBinds_ScreenshotNormal);
							GameBindSelectable(action, "((ScreenshotStereoscopic))", EGameBinds_ScreenshotStereoscopic);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Map))")))
						{
							GameBindSelectable(action, "((MapToggle))", EGameBinds_MapToggle);
							GameBindSelectable(action, "((MapFocusPlayer))", EGameBinds_MapFocusPlayer);
							GameBindSelectable(action, "((MapFloorDown))", EGameBinds_MapFloorDown);
							GameBindSelectable(action, "((MapFloorUp))", EGameBinds_MapFloorUp);
							GameBindSelectable(action, "((MapZoomIn))", EGameBinds_MapZoomIn);
							GameBindSelectable(action, "((MapZoomOut))", EGameBinds_MapZoomOut);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Mounts))")))
						{
							GameBindSelectable(action, "((SpumoniToggle))", EGameBinds_SpumoniToggle);
							GameBindSelectable(action, "((SpumoniMovement))", EGameBinds_SpumoniMovement);
							GameBindSelectable(action, "((SpumoniSecondaryMovement))", EGameBinds_SpumoniSecondaryMovement);
							GameBindSelectable(action, "((SpumoniMAM01))", EGameBinds_SpumoniMAM01);
							GameBindSelectable(action, "((SpumoniMAM02))", EGameBinds_SpumoniMAM02);
							GameBindSelectable(action, "((SpumoniMAM03))", EGameBinds_SpumoniMAM03);
							GameBindSelectable(action, "((SpumoniMAM04))", EGameBinds_SpumoniMAM04);
							GameBindSelectable(action, "((SpumoniMAM05))", EGameBinds_SpumoniMAM05);
							GameBindSelectable(action, "((SpumoniMAM06))", EGameBinds_SpumoniMAM06);
							GameBindSelectable(action, "((SpumoniMAM07))", EGameBinds_SpumoniMAM07);
							GameBindSelectable(action, "((SpumoniMAM08))", EGameBinds_SpumoniMAM08);
							GameBindSelectable(action, "((SpumoniMAM09))", EGameBinds_SpumoniMAM09);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Spectators))")))
						{
							GameBindSelectable(action, "((SpectatorNearestFixed))", EGameBinds_SpectatorNearestFixed);
							GameBindSelectable(action, "((SpectatorNearestPlayer))", EGameBinds_SpectatorNearestPlayer);
							GameBindSelectable(action, "((SpectatorPlayerRed1))", EGameBinds_SpectatorPlayerRed1);
							GameBindSelectable(action, "((SpectatorPlayerRed2))", EGameBinds_SpectatorPlayerRed2);
							GameBindSelectable(action, "((SpectatorPlayerRed3))", EGameBinds_SpectatorPlayerRed3);
							GameBindSelectable(action, "((SpectatorPlayerRed4))", EGameBinds_SpectatorPlayerRed4);
							GameBindSelectable(action, "((SpectatorPlayerRed5))", EGameBinds_SpectatorPlayerRed5);
							GameBindSelectable(action, "((SpectatorPlayerBlue1))", EGameBinds_SpectatorPlayerBlue1);
							GameBindSelectable(action, "((SpectatorPlayerBlue2))", EGameBinds_SpectatorPlayerBlue2);
							GameBindSelectable(action, "((SpectatorPlayerBlue3))", EGameBinds_SpectatorPlayerBlue3);
							GameBindSelectable(action, "((SpectatorPlayerBlue4))", EGameBinds_SpectatorPlayerBlue4);
							GameBindSelectable(action, "((SpectatorPlayerBlue5))", EGameBinds_SpectatorPlayerBlue5);
							GameBindSelectable(action, "((SpectatorFreeCamera))", EGameBinds_SpectatorFreeCamera);
							GameBindSelectable(action, "((SpectatorFreeCameraMode))", EGameBinds_SpectatorFreeCameraMode);
							GameBindSelectable(action, "((SpectatorFreeMoveForward))", EGameBinds_SpectatorFreeMoveForward);
							GameBindSelectable(action, "((SpectatorFreeMoveBackward))", EGameBinds_SpectatorFreeMoveBackward);
							GameBindSelectable(action, "((SpectatorFreeMoveLeft))", EGameBinds_SpectatorFreeMoveLeft);
							GameBindSelectable(action, "((SpectatorFreeMoveRight))", EGameBinds_SpectatorFreeMoveRight);
							GameBindSelectable(action, "((SpectatorFreeMoveUp))", EGameBinds_SpectatorFreeMoveUp);
							GameBindSelectable(action, "((SpectatorFreeMoveDown))", EGameBinds_SpectatorFreeMoveDown);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Squad))")))
						{
							GameBindSelectable(action, "((SquadMarkerPlaceWorld1))", EGameBinds_SquadMarkerPlaceWorld1);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld2))", EGameBinds_SquadMarkerPlaceWorld2);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld3))", EGameBinds_SquadMarkerPlaceWorld3);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld4))", EGameBinds_SquadMarkerPlaceWorld4);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld5))", EGameBinds_SquadMarkerPlaceWorld5);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld6))", EGameBinds_SquadMarkerPlaceWorld6);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld7))", EGameBinds_SquadMarkerPlaceWorld7);
							GameBindSelectable(action, "((SquadMarkerPlaceWorld8))", EGameBinds_SquadMarkerPlaceWorld8);
							GameBindSelectable(action, "((SquadMarkerClearAllWorld))", EGameBinds_SquadMarkerClearAllWorld);
							GameBindSelectable(action, "((SquadMarkerSetAgent1))", EGameBinds_SquadMarkerSetAgent1);
							GameBindSelectable(action, "((SquadMarkerSetAgent2))", EGameBinds_SquadMarkerSetAgent2);
							GameBindSelectable(action, "((SquadMarkerSetAgent3))", EGameBinds_SquadMarkerSetAgent3);
							GameBindSelectable(action, "((SquadMarkerSetAgent4))", EGameBinds_SquadMarkerSetAgent4);
							GameBindSelectable(action, "((SquadMarkerSetAgent5))", EGameBinds_SquadMarkerSetAgent5);
							GameBindSelectable(action, "((SquadMarkerSetAgent6))", EGameBinds_SquadMarkerSetAgent6);
							GameBindSelectable(action, "((SquadMarkerSetAgent7))", EGameBinds_SquadMarkerSetAgent7);
							GameBindSelectable(action, "((SquadMarkerSetAgent8))", EGameBinds_SquadMarkerSetAgent8);
							GameBindSelectable(action, "((SquadMarkerClearAllAgent))", EGameBinds_SquadMarkerClearAllAgent);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Mastery Skills))")))
						{
							GameBindSelectable(action, "((MasteryAccess))", EGameBinds_MasteryAccess);
							GameBindSelectable(action, "((MasteryAccess01))", EGameBinds_MasteryAccess01);
							GameBindSelectable(action, "((MasteryAccess02))", EGameBinds_MasteryAccess02);
							GameBindSelectable(action, "((MasteryAccess03))", EGameBinds_MasteryAccess03);
							GameBindSelectable(action, "((MasteryAccess04))", EGameBinds_MasteryAccess04);
							GameBindSelectable(action, "((MasteryAccess05))", EGameBinds_MasteryAccess05);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Miscellaneous))")))
						{
							GameBindSelectable(action, "((MiscAoELoot))", EGameBinds_MiscAoELoot);
							GameBindSelectable(action, "((MiscInteract))", EGameBinds_MiscInteract);
							GameBindSelectable(action, "((MiscShowEnemies))", EGameBinds_MiscShowEnemies);
							GameBindSelectable(action, "((MiscShowAllies))", EGameBinds_MiscShowAllies);
							GameBindSelectable(action, "((MiscCombatStance))", EGameBinds_MiscCombatStance);
							GameBindSelectable(action, "((MiscToggleLanguage))", EGameBinds_MiscToggleLanguage);
							GameBindSelectable(action, "((MiscTogglePetCombat))", EGameBinds_MiscTogglePetCombat);
							GameBindSelectable(action, "((MiscToggleFullScreen))", EGameBinds_MiscToggleFullScreen);
							GameBindSelectable(action, "((ToyUseDefault))", EGameBinds_ToyUseDefault);
							GameBindSelectable(action, "((ToyUseSlot1))", EGameBinds_ToyUseSlot1);
							GameBindSelectable(action, "((ToyUseSlot2))", EGameBinds_ToyUseSlot2);
							GameBindSelectable(action, "((ToyUseSlot3))", EGameBinds_ToyUseSlot3);
							GameBindSelectable(action, "((ToyUseSlot4))", EGameBinds_ToyUseSlot4);
							GameBindSelectable(action, "((ToyUseSlot5))", EGameBinds_ToyUseSlot5);
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu(APIDefs->Localization.Translate("((Templates))")))
						{
							GameBindSelectable(action, "((Loadout1))", EGameBinds_Loadout1);
							GameBindSelectable(action, "((Loadout2))", EGameBinds_Loadout2);
							GameBindSelectable(action, "((Loadout3))", EGameBinds_Loadout3);
							GameBindSelectable(action, "((Loadout4))", EGameBinds_Loadout4);
							GameBindSelectable(action, "((Loadout5))", EGameBinds_Loadout5);
							GameBindSelectable(action, "((Loadout6))", EGameBinds_Loadout6);
							GameBindSelectable(action, "((Loadout7))", EGameBinds_Loadout7);
							GameBindSelectable(action, "((Loadout8))", EGameBinds_Loadout8);
							GameBindSelectable(action, "((GearLoadout1))", EGameBinds_GearLoadout1);
							GameBindSelectable(action, "((GearLoadout2))", EGameBinds_GearLoadout2);
							GameBindSelectable(action, "((GearLoadout3))", EGameBinds_GearLoadout3);
							GameBindSelectable(action, "((GearLoadout4))", EGameBinds_GearLoadout4);
							GameBindSelectable(action, "((GearLoadout5))", EGameBinds_GearLoadout5);
							GameBindSelectable(action, "((GearLoadout6))", EGameBinds_GearLoadout6);
							GameBindSelectable(action, "((GearLoadout7))", EGameBinds_GearLoadout7);
							GameBindSelectable(action, "((GearLoadout8))", EGameBinds_GearLoadout8);
							ImGui::EndMenu();
						}
						ImGui::EndCombo();
					}

					break;
				}
				case EActionType::Delay:
				{
					ImGui::InputInt(("##delay" + std::to_string(i)).c_str(), &((ActionDelay*)action)->Duration, 1, 100);
					break;
				}
			}

			ImGui::TableSetColumnIndex(2);
			if (i != 0)
			{
				if (ImGui::ArrowButton(("up##" + std::to_string(i)).c_str(), ImGuiDir_Up))
				{
					ActionBase* tmp = EditingItem->Actions[i - 1];
					EditingItem->Actions[i - 1] = EditingItem->Actions[i];
					EditingItem->Actions[i] = tmp;
				}
			}
			else
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::ArrowButton(("up##" + std::to_string(i)).c_str(), ImGuiDir_Up);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			ImGui::SameLine();
			if (i != EditingItem->Actions.size() - 1)
			{
				if (ImGui::ArrowButton(("dn##" + std::to_string(i)).c_str(), ImGuiDir_Down))
				{
					ActionBase* tmp = EditingItem->Actions[i + 1];
					EditingItem->Actions[i + 1] = EditingItem->Actions[i];
					EditingItem->Actions[i] = tmp;
				}
			}
			else
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::ArrowButton(("dn##" + std::to_string(i)).c_str(), ImGuiDir_Down);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			ImGui::SameLine();
			float sz = ImGui::GetFrameHeight();
			if (ImGui::Button(("X##" + std::to_string(i)).c_str(), ImVec2(sz, sz)))
			{
				idxDel = i;
			}

			i++;
		}
		ImGui::EndTable();

		if (idxDel != -1)
		{
			delete EditingItem->Actions[idxDel];
			EditingItem->Actions.erase(EditingItem->Actions.begin() + idxDel);
		}
	}
	ImGui::EndChild();
}

void CRadialContext::OnInputBind(std::string aIdentifier, bool aIsRelease)
{
	auto it = this->RadialIBMap.find(aIdentifier);

	if (it != this->RadialIBMap.end())
	{
		if (!aIsRelease)
		{
			this->Activate(it->second);
		}
		else
		{
			this->Release(ESelectionMode::Release);
		}
	}
}

UINT CRadialContext::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				return this->Release(ESelectionMode::Escape) ? 0 : 1;
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			return this->Release(ESelectionMode::Click) ? 0 : 1;
			break;
		}
	}

	return uMsg;
}
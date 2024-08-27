///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialMenu.cpp
/// Description  :  Handles the logic for a radial menu element.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "RadialMenu.h"

#include <filesystem>

#include "imgui_extensions.h"

#include "resource.h"
#include "Util.h"
#include "Shared.h"
#include "StateObserver.h"

CRadialMenu::CRadialMenu(AddonAPI* aAPI, HMODULE aModule, int aID, std::string aIdentifier, ERadialType aRadialMenuType, ESelectionMode aSelectionMode)
{
	this->API = aAPI;
	this->Module = aModule;
	this->NexusLink = (NexusLinkData*)this->API->DataLink.Get("DL_NEXUS_LINK");

	this->ID = aID;
	this->Identifier = aIdentifier;
	this->Type = aRadialMenuType;
	this->SelectionMode = aSelectionMode;

	this->Invalidate();
}

CRadialMenu::~CRadialMenu()
{
	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	while (this->Items.size() > 0)
	{
		RadialItem* item = this->Items.front();
		
		while (item->Actions.size() > 0)
		{
			ActionBase* action = item->Actions.front();

			delete action;
			item->Actions.erase(item->Actions.begin());
		}
		delete item;
		this->Items.erase(this->Items.begin());
	}
}

void CRadialMenu::Save()
{

}

bool CRadialMenu::Render()
{
	if (this->Type == ERadialType::None) { return false; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	if (this->DrawnItems.size() > this->ItemsCapacity) { return false; }
	if (this->DrawnItems.size() < 2) { return false; }
	if (!this->IsActive) { return false; }

	ImVec2 size = ImVec2(this->Size.x * NexusLink->Scaling, this->Size.y * NexusLink->Scaling);
	ImVec2 sizeHalf = ImVec2((size.x / 2.0f), (size.y / 2.0f));

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 safePad = style.DisplaySafeAreaPadding;
	style.DisplaySafeAreaPadding = ImVec2(0, 0);

	int hoverIndex = -1;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(NexusLink->Width, NexusLink->Height), ImGuiCond_Appearing);
	if (ImGui::Begin(("RADIAL##" + this->Identifier).c_str(), (bool*)0, (ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings)))
	{
		ImGui::SetCursorPos(ImVec2(this->Origin.x - sizeHalf.x, this->Origin.y - sizeHalf.y));
		ImGui::BeginChild(("RADIALINNER##" + this->Identifier).c_str());
		ImVec2 initialPos = ImVec2(1, 1);

		hoverIndex = this->GetHoveredIndex();

		if (this->BaseTexture)
		{
			ImGui::SetCursorPos(initialPos);
			ImGui::Image(this->BaseTexture->Resource, size);
		}
		else
		{
			if (this->Type == ERadialType::Normal)
			{
				this->BaseTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_BASE", TEX_MW_BASE, this->Module);
			}
			else if (this->Type == ERadialType::Small)
			{
				this->BaseTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_BASE", TEX_MWS_BASE, this->Module);
			}
		}

		if (this->Type == ERadialType::Small)
		{
			if (this->DividerTexture)
			{
				for (size_t i = 0; i < this->DrawnItems.size(); i++)
				{
					ImGui::ImageRotated(this->DividerTexture->Resource, Origin, size, SegmentRadius * i);
				}
			}
			else
			{
				this->DividerTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_DIVIDER", TEX_MWS_DIVIDER, this->Module);
			}
		}

		ImVec2 center = initialPos;
		center.x += sizeHalf.x;
		center.y += sizeHalf.y;

		float contentDistance = this->SegmentContentDistance * NexusLink->Scaling;
		ImVec2 contentSize = ImVec2(this->SegmentContentSize.x * NexusLink->Scaling, this->SegmentContentSize.y * NexusLink->Scaling);
		ImVec2 contentSizeHalf = ImVec2(contentSize.x / 2.0f, contentSize.y / 2.0f);

		if (this->SegmentTexture)
		{
			for (size_t i = 0; i < this->DrawnItems.size(); i++)
			{
				RadialItem* item = this->DrawnItems[i];

				float segmentStart = (SegmentRadius * i);
				ImGui::ImageRotated(this->SegmentTexture->Resource, Origin, size, segmentStart, hoverIndex == i ? ImColor(item->ColorHover) : ImColor(item->Color));
				
				if (item->Icon.Texture)
				{
					float deg = (segmentStart + (SegmentRadius / 2)) - 90.0f; // center of segment and correct for 0 up
					float degRad = deg * 3.14159f / 180.0f;

					float x = contentDistance * cos(degRad) + center.x;
					float y = contentDistance * sin(degRad) + center.y;

					ImGui::SetCursorPos(ImVec2(x - contentSizeHalf.x, y - contentSizeHalf.y));
					ImGui::Image(item->Icon.Texture->Resource, contentSize);
				}
				else
				{
					item->Icon.Texture = this->API->Textures.Get(item->Icon.Value.c_str());
				}
			}
		}
		else
		{
			this->LoadSegmentTexture();
		}
		ImGui::EndChild();
	}
	ImGui::End();

	style.DisplaySafeAreaPadding = safePad;

	return hoverIndex != -1;
}

void CRadialMenu::Activate()
{
	if (this->IsActive) { return; }

	this->DrawnItems.clear();
	for (RadialItem* item : this->Items)
	{
		if (StateObserver::IsMatch(&item->Visibility))
		{
			this->DrawnItems.push_back(item);
		}
	}

	if (this->DrawnItems.size() == 0)
	{
		this->SegmentRadius = 0;
	}
	else
	{
		this->SegmentRadius = 360.0f / this->DrawnItems.size();
	}

	this->SegmentTexture = nullptr;

	this->Origin = this->MousePos = ImGui::GetMousePos();
	CURSORINFO curInfo{};
	curInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&curInfo);
	this->WasActionCamActive = !(curInfo.flags & CURSOR_SHOWING);

	/* override origin if draw in center */
	if (this->DrawInCenter)
	{
		this->Origin.x = NexusLink->Width / 2;
		this->Origin.y = NexusLink->Height / 2;

		/* winapi set cursor */
		SetCursorPos(this->Origin.x, this->Origin.y);
		ImGui::GetIO().MousePos = this->Origin;
	}

	if (this->WasActionCamActive && !RadialCtx->IsRightClickHeld)
	{
		std::thread([this]() {

			this->API->GameBinds.Press(EGameBinds_CameraActionMode);
			Sleep(10);
			this->API->GameBinds.Release(EGameBinds_CameraActionMode);
			Sleep(10); /* this delay is needed in order to set the cursor after action cam has been toggled */
			SetCursorPos(this->Origin.x, this->Origin.y);
			this->API->WndProc.SendToGameOnly(0, WM_MOUSEMOVE, 0, MAKELPARAM(this->MousePos.x, this->MousePos.y));
		}).detach();
	}

	// fix for weird rendering/cutoff
	this->Origin.x += 1.0f;
	this->Origin.y += 1.0f;

	this->IsActive = true;
}

void CRadialMenu::Release(bool aIsCancel)
{
	/* if is cancel, directly set it to -1 to not trigger any release */
	int idx = aIsCancel ? -1 : this->GetHoveredIndex();

	/* winapi set cursor */
	if (this->RestoreCursor)
	{
		SetCursorPos(this->MousePos.x, this->MousePos.y);
		ImGui::GetIO().MousePos = this->MousePos;
		this->API->WndProc.SendToGameOnly(0, WM_MOUSEMOVE, 0, MAKELPARAM(this->MousePos.x, this->MousePos.y));
	}

	if (this->WasActionCamActive && !RadialCtx->IsRightClickHeld)
	{
		this->API->GameBinds.InvokeAsync(EGameBinds_CameraActionMode, 10);
	}

	this->WasActionCamActive = false;

	if (idx > -1)
	{
		RadialItem* item = this->DrawnItems[idx];
		
		std::thread([this, item]() {
			/* FIXME: this needs to be able to abort if another item is selected halfway through execution */
			int msWaited = 0;
			while (!StateObserver::IsMatch(&item->Activation))
			{
				msWaited += 100;
				Sleep(100);

				if (msWaited > item->ActivationTimeout * 1000) { break; }
			}

			if (msWaited > item->ActivationTimeout * 1000)
			{
				this->API->UI.SendAlert("Cancelled after waiting for timeout.");
				return;
			}

			/* initially set to true, and only in the skip set to false, this way the first action ignores the setting */
			bool previousExecuted = true;
			for (ActionBase* act : item->Actions)
			{
				if (!StateObserver::IsMatch(&act->Activation))
				{
					previousExecuted = false;
					continue;
				}

				if (act->OnlyExecuteIfPrevious && !previousExecuted)
				{
					previousExecuted = false;
					continue;
				}

				switch (act->Type)
				{
					case EActionType::InputBind:
					{
						ActionGeneric* action = (ActionGeneric*)act;
						this->API->InputBinds.Invoke(action->Identifier.c_str(), false);
						this->API->InputBinds.Invoke(action->Identifier.c_str(), true);
						break;
					}
					case EActionType::GameInputBind:
					{
						ActionGameInputBind* action = (ActionGameInputBind*)act;
						/* FIXME: all of the following code is acopy of the Nexus Invoke function. Nexus does not provide Invoke only InvokeAsync.*/
						/* get modifier state */
						bool wasAltPressed = GetAsyncKeyState(VK_MENU);
						bool wasCtrlPressed = GetAsyncKeyState(VK_CONTROL);
						bool wasShiftPressed = GetAsyncKeyState(VK_SHIFT);

						/* unset modifier state */
						if (wasAltPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_SYSKEYUP, VK_MENU, Input::GetKeyMessageLPARAM(VK_MENU, false, true));
						}
						if (wasCtrlPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_KEYUP, VK_CONTROL, Input::GetKeyMessageLPARAM(VK_CONTROL, false, false));
						}
						if (wasShiftPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_KEYUP, VK_SHIFT, Input::GetKeyMessageLPARAM(VK_SHIFT, false, false));
						}

						/* execute action action */
						this->API->GameBinds.Press(action->Identifier);
						Sleep(100);
						this->API->GameBinds.Release(action->Identifier);

						/* restore modifier state */
						if (wasAltPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_SYSKEYDOWN, VK_MENU, Input::GetKeyMessageLPARAM(VK_MENU, true, true));
						}
						if (wasCtrlPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_KEYDOWN, VK_CONTROL, Input::GetKeyMessageLPARAM(VK_CONTROL, true, false));
						}
						if (wasShiftPressed)
						{
							this->API->WndProc.SendToGameOnly(0, WM_KEYDOWN, VK_SHIFT, Input::GetKeyMessageLPARAM(VK_SHIFT, true, false));
						}
						break;
					}
					case EActionType::GameInputBindPress:
					{
						ActionGameInputBind* action = (ActionGameInputBind*)act;
						this->API->GameBinds.Press(action->Identifier);
						break;
					}
					case EActionType::GameInputBindRelease:
					{
						ActionGameInputBind* action = (ActionGameInputBind*)act;
						this->API->GameBinds.Release(action->Identifier);
						break;
					}
					case EActionType::Event:
					{
						ActionGeneric* action = (ActionGeneric*)act;
						this->API->Events.Raise(action->Identifier.c_str(), nullptr);
						break;
					}
					case EActionType::Delay:
					{
						ActionDelay* action = (ActionDelay*)act;
						Sleep(action->Duration);
						break;
					}
				}

				previousExecuted = true;
			}
		}).detach();
	}

	this->Origin = this->MousePos = ImVec2(-1, -1);

	this->IsActive = false;
}

void CRadialMenu::AddItem(std::string aName, unsigned int aColor, unsigned int aColorHover, EIconType aIconType, std::string aIconValue, Conditions aVisibility, Conditions aActivation, int aActivationTimeout)
{
	RadialItem* item = new RadialItem();
	item->Color = aColor;
	item->ColorHover = aColorHover;
	item->Icon.Type = aIconType;
	item->Icon.Value = aIconValue;
	item->Visibility = aVisibility;
	item->Activation = aActivation;
	item->ActivationTimeout = aActivationTimeout;
	
	const std::lock_guard<std::mutex> lock(this->Mutex);

	std::string itemName = aName;
	int i;
	if (aName.empty())
	{
		i = this->Items.size() + 1;
	}
	else
	{
		i = 1;
	}
	bool exists = false;
	do
	{
		if (aName.empty())
		{
			itemName = "New Item " + std::to_string(i);
		}
		else if (i > 1)
		{
			/* if it's the first iteration and a name was provided, it's already set. */
			itemName = aName + " (" + std::to_string(i) + ")";
		}

		exists = false;
		for (RadialItem* existingItem : this->Items)
		{
			if (existingItem->Identifier == itemName)
			{
				i++;
				exists = true;
				break;
			}
		}
	} while (exists);

	item->Identifier = itemName;

	switch (this->Type)
	{
		case ERadialType::None: return;
		case ERadialType::Normal:
			if (this->Items.size() >= ItemsCapacity) { return; }
			break;
		case ERadialType::Small:
			if (this->Items.size() >= ItemsCapacity) { return; }
			break;
	}

	switch (item->Icon.Type)
	{
		case EIconType::File:
		{
			std::filesystem::path iconPath = item->Icon.Value;
			if (iconPath.is_relative())
			{
				APIDefs->Textures.LoadFromFile(item->Icon.Value.c_str(), (GW2Root / item->Icon.Value).string().c_str(), nullptr);
			}
			else
			{
				APIDefs->Textures.LoadFromFile(item->Icon.Value.c_str(), item->Icon.Value.c_str(), nullptr);
			}
			break;
		}
		case EIconType::URL:
		{
			this->API->Textures.LoadFromURL(item->Icon.Value.c_str(), URL::GetBase(item->Icon.Value).c_str(), URL::GetEndpoint(item->Icon.Value).c_str(), nullptr);
			break;
		}
	}

	this->Items.push_back(item);

	/* recalculate positions/offsets */
	this->Invalidate();
}

void CRadialMenu::RemoveItem(std::string aIdentifier)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Items.begin(), this->Items.end(), [aIdentifier](RadialItem* item) { return item->Identifier == aIdentifier; });

	if (it != this->Items.end())
	{
		for (ActionBase* action : (*it)->Actions)
		{
			delete action;
		}
		delete (*it);
		this->Items.erase(it);
	}

	this->Invalidate();
}

RadialItem* CRadialMenu::GetItem(std::string aIdentifier)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = std::find_if(this->Items.begin(), this->Items.end(), [aIdentifier](RadialItem* item) { return item->Identifier == aIdentifier; });

	if (it != this->Items.end())
	{
		return (*it);
	}

	return nullptr;
}

const std::vector<RadialItem*>& CRadialMenu::GetItems()
{
	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	return this->Items;
}

void CRadialMenu::MoveItemUp(std::string aIdentifier)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	for (size_t i = 0; i < this->Items.size(); i++)
	{
		if (this->Items[i]->Identifier == aIdentifier)
		{
			if (i == 0)
			{
				return;
			}

			RadialItem* tmp = this->Items[i - 1];
			this->Items[i - 1] = this->Items[i];
			this->Items[i] = tmp;

			break;
		}
	}
}

void CRadialMenu::MoveItemDown(std::string aIdentifier)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	for (size_t i = 0; i < this->Items.size(); i++)
	{
		if (this->Items[i]->Identifier == aIdentifier)
		{
			if (i == this->Items.size() - 1)
			{
				return;
			}

			RadialItem* tmp = this->Items[i + 1];
			this->Items[i + 1] = this->Items[i];
			this->Items[i] = tmp;

			break;
		}
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, EActionType aType, std::string aValue)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionGeneric* action = new ActionGeneric();
		action->Type = aType;
		action->Identifier = _strdup(aValue.c_str());

		item->Actions.push_back(action);
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, EActionType aType, EGameBinds aValue)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionGameInputBind* action = new ActionGameInputBind();
		action->Type = aType;
		action->Identifier = aValue;

		item->Actions.push_back(action);
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, int aValue)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionDelay* action = new ActionDelay();
		action->Type = EActionType::Delay;
		action->Duration = aValue;

		item->Actions.push_back(action);
	}
}

void CRadialMenu::RemoveItemAction(std::string aItemId, int aIndex)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		delete item->Actions[aIndex];
		item->Actions.erase(item->Actions.begin() + aIndex);
	}
}

int CRadialMenu::GetID()
{
	return this->ID;
}

void CRadialMenu::SetID(int aID)
{
	this->ID = aID;
}

const std::string& CRadialMenu::GetName()
{
	return this->Identifier;
}

void CRadialMenu::SetName(std::string aName)
{
	this->Identifier = aName;
}

ERadialType CRadialMenu::GetType()
{
	return this->Type;
}

void CRadialMenu::SetType(ERadialType aType)
{
	this->Type = aType;
	this->Invalidate();
}

ESelectionMode CRadialMenu::GetSelectionMode()
{
	return this->SelectionMode;
}

void CRadialMenu::SetSelectionMode(ESelectionMode aSelectionMode)
{
	this->SelectionMode = aSelectionMode;
}

int CRadialMenu::GetCapacity()
{
	return this->ItemsCapacity;
}

std::string CRadialMenu::GetInputBind()
{
	return "KB_RADIAL" + std::to_string(this->ID);
}

int CRadialMenu::GetHoveredIndex()
{
	int hoverIndex = -1;

	if (this->DrawnItems.size() < 2) { return hoverIndex; }

	ImVec2 currentMousePos = ImGui::GetMousePos();

	float angle = ImGui::Angle(Origin, currentMousePos);
	float distance = ImGui::Distance(Origin, currentMousePos);

	if (distance >= this->MinimalMouseMoveDistance * NexusLink->Scaling)
	{
		hoverIndex = ceilf(angle / SegmentRadius) - 1;
	}

	return hoverIndex;
}

void CRadialMenu::Invalidate()
{
	this->BaseTexture		= nullptr;
	this->DividerTexture	= nullptr;
	this->SegmentTexture	= nullptr;

	switch (this->Type)
	{
		default:
		case ERadialType::None:
			this->ItemsCapacity = 0;
			this->Size = ImVec2(0, 0);
			this->MinimalMouseMoveDistance = 0;
			this->SegmentContentDistance = 0;
			this->SegmentContentSize = ImVec2(0, 0);
			break;
		case ERadialType::Normal:
			this->ItemsCapacity = 12;
			this->Size = ImVec2(520.0f, 520.0f);
			this->MinimalMouseMoveDistance = 150.0f;
			this->SegmentContentDistance = 200.0f;
			this->SegmentContentSize = ImVec2(128.0f, 128.0f);
			break;
		case ERadialType::Small:
			this->ItemsCapacity = 8;
			this->Size = ImVec2(112.0f, 112.0f);
			this->MinimalMouseMoveDistance = 18.0f;
			this->SegmentContentDistance = 36.0f;
			this->SegmentContentSize = ImVec2(32.0f, 32.0f);
			break;
	}

	/*if (this->Items.size() == 0)
	{
		this->SegmentRadius = 0;
	}
	else
	{
		this->SegmentRadius = 360.0f / this->Items.size();
	}*/

	this->IsActive = false;
	this->Origin = ImVec2(-1, -1);
}

void CRadialMenu::LoadSegmentTexture()
{
	if (this->Type == ERadialType::Normal)
	{
		switch (this->DrawnItems.size())
		{
		case 2:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR2", TEX_MW_SELECTOR2, this->Module);
			break;
		case 3:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR3", TEX_MW_SELECTOR3, this->Module);
			break;
		case 4:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR4", TEX_MW_SELECTOR4, this->Module);
			break;
		case 5:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR5", TEX_MW_SELECTOR5, this->Module);
			break;
		case 6:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR6", TEX_MW_SELECTOR6, this->Module);
			break;
		case 7:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR7", TEX_MW_SELECTOR7, this->Module);
			break;
		case 8:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR8", TEX_MW_SELECTOR8, this->Module);
			break;
		case 9:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR9", TEX_MW_SELECTOR9, this->Module);
			break;
		case 10:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR10", TEX_MW_SELECTOR10, this->Module);
			break;
		case 11:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR11", TEX_MW_SELECTOR11, this->Module);
			break;
		case 12:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR12", TEX_MW_SELECTOR12, this->Module);
			break;
		}
	}
	else if (this->Type == ERadialType::Small)
	{
		switch (this->DrawnItems.size())
		{
		case 2:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR2", TEX_MWS_SELECTOR2, this->Module);
			break;
		case 3:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR3", TEX_MWS_SELECTOR3, this->Module);
			break;
		case 4:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR4", TEX_MWS_SELECTOR4, this->Module);
			break;
		case 5:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR5", TEX_MWS_SELECTOR5, this->Module);
			break;
		case 6:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR6", TEX_MWS_SELECTOR6, this->Module);
			break;
		case 7:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR7", TEX_MWS_SELECTOR7, this->Module);
			break;
		case 8:
			this->SegmentTexture = this->API->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR8", TEX_MWS_SELECTOR8, this->Module);
			break;
		}
	}
}

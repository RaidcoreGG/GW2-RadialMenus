///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialMenu.cpp
/// Description  :  Handles the logic for a radial menu element.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "RadialMenu.h"

#include <fstream>
#include <filesystem>

#include "imgui_extensions.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "resource.h"
#include "Util.h"
#include "Shared.h"
#include "StateObserver.h"

/* helper for window relative cursor pos */
ImVec2 GetCursorPosWR(int x, int y)
{
	LONG style = GetWindowLong(WindowHandle, GWL_STYLE);

	int xOffset = 0;
	int yOffset = 0;

	if (style & WS_CAPTION)
	{
		RECT windowRect{};
		GetWindowRect(WindowHandle, &windowRect);
		xOffset += windowRect.left + GetSystemMetrics(SM_CXSIZEFRAME);
		yOffset += windowRect.top + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYSIZE);
	}

	return ImVec2(x + xOffset, y + yOffset);
}

CRadialMenu::CRadialMenu(AddonAPI* aAPI, HMODULE aModule, std::filesystem::path aPath, int aID, std::string aIdentifier, float aScale, int aHoverTimeout, ERadialType aRadialMenuType, ESelectionMode aSelectionMode)
{
	this->API = aAPI;
	this->Module = aModule;
	this->NexusLink = (NexusLinkData*)this->API->DataLink.Get("DL_NEXUS_LINK");

	this->Path = aPath;

	this->ID = aID;
	this->Identifier = aIdentifier;
	this->Type = aRadialMenuType;
	this->SelectionMode = aSelectionMode;
	this->Scale = aScale;
	this->HoverTimeout = aHoverTimeout;

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
	const std::lock_guard<std::mutex> lock(this->Mutex);

	json radialJSON = json{
		{"ID", this->ID},
		{"Name", this->Identifier},
		{"Type", this->Type},
		{"SelectionMode", this->SelectionMode},

		{"DrawInCenter", this->DrawInCenter},
		{"RestoreCursor", this->RestoreCursor},
		{"Scale", this->Scale},
		{"HoverTimeout", this->HoverTimeout},
		{"ItemRotation", this->ItemRotationDegrees},
		{"ShowItemNameTooltip", this->ShowItemNameTooltip},

		{"Items", json::array()}
	};

	for (RadialItem* item : this->Items)
	{
		json itemJSON = json{
			{"Name", item->Identifier},
			{"Color", item->Color},
			{"ColorHover", item->ColorHover},
			{"IconType", item->Icon.Type},
			{"IconValue", item->Icon.Value},
			{"Visibility", item->Visibility},
			{"Activation", item->Activation},
			{"ActivationTimeout", item->ActivationTimeout},

			{"Actions", json::array()}
		};

		for (ActionBase* action : item->Actions)
		{
			json actionJSON = json{
				{"Type", action->Type},
				{"Activation", action->Activation},
				{"OnlyExecuteIfPrevious", action->OnlyExecuteIfPrevious}
			};

			switch (action->Type)
			{
				case EActionType::InputBind:
				case EActionType::Event:
				{
					actionJSON["Identifier"] = ((ActionGeneric*)action)->Identifier;
					break;
				}
				case EActionType::GameInputBind:
				case EActionType::GameInputBindPress:
				case EActionType::GameInputBindRelease:
				{
					actionJSON["Identifier"] = ((ActionGameInputBind*)action)->Identifier;
					break;
				}
				case EActionType::Delay:
				{
					actionJSON["Duration"] = ((ActionDelay*)action)->Duration;
					break;
				}
			}

			itemJSON["Actions"].push_back(actionJSON);
		}

		radialJSON["Items"].push_back(itemJSON);
	}

	std::ofstream file(this->Path);
	file << radialJSON.dump(1, '\t') << std::endl;
	file.close();
}

bool CRadialMenu::Render()
{
	if (this->SetCursor)
	{
		this->SetCursor = false;

		::SetCursorPos(this->SetCursorPosition.x, this->SetCursorPosition.y);
		ImGui::GetIO().MousePos = this->SetCursorPosition;
		this->API->WndProc.SendToGameOnly(0, WM_MOUSEMOVE, 0, MAKELPARAM(this->SetCursorPosition.x, this->SetCursorPosition.y));
	}

	if (this->Type == ERadialType::None) { return false; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	if (!this->IsActive) { return false; }

	this->DrawnItems.clear();
	for (RadialItem* item : this->Items)
	{
		if (StateObserver::IsMatch(&item->Visibility) && this->DrawnItems.size() < this->ItemsCapacity)
		{
			this->DrawnItems.push_back(item);
		}
	}

	this->LoadSegmentTexture();
	this->SegmentRadius = 360.0f / this->DrawnItems.size();

	if (this->DrawnItems.size() <= 0) { return false; }
	if (this->DrawnItems.size() == 1) { RadialCtx->Release(ESelectionMode::SingleItem); return false; }

	ImVec2 size = ImVec2(this->Size.x * NexusLink->Scaling, this->Size.y * NexusLink->Scaling);
	ImVec2 sizeHalf = ImVec2((size.x / 2.0f), (size.y / 2.0f));

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 safePad = style.DisplaySafeAreaPadding;
	style.DisplaySafeAreaPadding = ImVec2(0, 0);

	ImVec2 tooltipPad = style.WindowPadding;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(NexusLink->Width, NexusLink->Height), ImGuiCond_Appearing);
	if (ImGui::Begin(("RADIAL##" + this->Identifier).c_str(), (bool*)0, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings)))
	{
		ImGui::SetCursorPos(ImVec2(this->Origin.x - sizeHalf.x, this->Origin.y - sizeHalf.y));
		ImGui::BeginChild(("RADIALINNER##" + this->Identifier).c_str(), ImVec2(0,0), false, (ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings));
		ImVec2 initialPos = ImVec2(1, 1);

		int hoverIndex = this->GetCurrentlyHoveredIndex();

		if (this->HoverIndex != hoverIndex)
		{
			this->HoverStartTime = Time::GetTimestampMillis();
			this->HoverIndex = hoverIndex;
		}

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
					ImGui::ImageRotated(this->DividerTexture->Resource, this->Origin, size, this->SegmentRadius * i);
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
		ImVec2 contentSizeHover = ImVec2(contentSize.x * 1.2f, contentSize.y * 1.2f);

		if (this->SegmentTexture)
		{
			/* draw segment backgrounds */
			for (size_t i = 0; i < this->DrawnItems.size(); i++)
			{
				RadialItem* item = this->DrawnItems[i];

				float segmentStart = (SegmentRadius * i) + this->ItemRotationDegrees;
				ImGui::ImageRotated(this->SegmentTexture->Resource, this->Origin, size, segmentStart, hoverIndex == i ? ImColor(item->ColorHover) : ImColor(item->Color));
			}

			/* draw icons */
			for (size_t i = 0; i < this->DrawnItems.size(); i++)
			{
				RadialItem* item = this->DrawnItems[i];
				float segmentStart = (this->SegmentRadius * i) + this->ItemRotationDegrees;

				float szDiff = hoverIndex == i ? (contentSizeHover.x - contentSize.x) / 2.0f : 0;

				if (item->Icon.Texture)
				{
					float deg = (segmentStart + (this->SegmentRadius / 2)) - 90.0f; // center of segment and correct for 0 up
					float degRad = deg * 3.14159f / 180.0f;

					float x = contentDistance * cos(degRad) + center.x;
					float y = contentDistance * sin(degRad) + center.y;
					if (hoverIndex == i)
					{
						ImGui::SetCursorPos(ImVec2(x - contentSizeHalf.x - szDiff, y - contentSizeHalf.y - szDiff));
					}
					else
					{
						ImGui::SetCursorPos(ImVec2(x - contentSizeHalf.x, y - contentSizeHalf.y));
					}
					ImGui::Image(item->Icon.Texture->Resource, hoverIndex == i ? contentSizeHover : contentSize);
				}
				else
				{
					item->Icon.Texture = this->API->Textures.Get(item->Icon.Value.c_str());
				}
			}
		}

		if (this->ShowItemNameTooltip && this->HoverIndex > -1)
		{
			RadialItem* hovItem = this->DrawnItems[this->HoverIndex];

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, tooltipPad);
			ImGui::BeginTooltip();
			ImGui::Text(hovItem->Identifier.c_str());
			ImGui::EndTooltip();
			ImGui::PopStyleVar();
		}

		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopStyleVar();

	style.DisplaySafeAreaPadding = safePad;

	unsigned int hoverTime = Time::GetTimestampMillis() - this->HoverStartTime;

	return this->SelectionMode == ESelectionMode::Hover && this->HoverIndex != -1 && hoverTime >= this->HoverTimeout;
}

bool CRadialMenu::Activate()
{
	if (this->IsActive) { return true; }

	this->DrawnItems.clear();
	for (RadialItem* item : this->Items)
	{
		if (StateObserver::IsMatch(&item->Visibility) && this->DrawnItems.size() < this->ItemsCapacity)
		{
			this->DrawnItems.push_back(item);
		}
	}

	if (this->DrawnItems.size() == 1)
	{
		this->IsActive = true;
		return true;
	}
	
	if (this->DrawnItems.size() <= 0)
	{
		this->SegmentRadius = 0;
		return false;
	}

	this->Origin = this->MousePos = ImGui::GetMousePos();
	CURSORINFO curInfo{};
	curInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&curInfo);
	this->WasActionCamActive = !(curInfo.flags & CURSOR_SHOWING) && !(RadialCtx->IsLeftClickHeld || RadialCtx->IsRightClickHeld);
	
	/* override origin if draw in center */
	if (this->DrawInCenter)
	{
		this->Origin.x = NexusLink->Width / 2;
		this->Origin.y = NexusLink->Height / 2;

		/* winapi set cursor */
		this->SetCursorPosition = GetCursorPosWR(this->Origin.x, this->Origin.y);
		this->SetCursor = true;
	}

	if (this->WasActionCamActive)
	{
		std::thread([this]() {

			this->API->GameBinds.Press(EGameBinds_CameraActionMode);
			Sleep(10);
			this->API->GameBinds.Release(EGameBinds_CameraActionMode);
			Sleep(10); /* this delay is needed in order to set the cursor after action cam has been toggled */
			this->SetCursorPosition = GetCursorPosWR(this->Origin.x, this->Origin.y);
			this->SetCursor = true;
		}).detach();
	}
	
	if (RadialCtx->IsLeftClickHeld)
	{
		std::thread([this]() {
			this->API->WndProc.SendToGameOnly(0, WM_LBUTTONUP, 0, MAKELPARAM(this->Origin.x, this->Origin.y));
			Sleep(10); /* this delay is needed in order to set the cursor after action cam has been toggled */
			this->SetCursorPosition = GetCursorPosWR(this->Origin.x, this->Origin.y);
			this->SetCursor = true;
		}).detach();
	}
	if (RadialCtx->IsRightClickHeld)
	{
		std::thread([this]() {
			this->API->WndProc.SendToGameOnly(0, WM_RBUTTONUP, 0, MAKELPARAM(this->Origin.x, this->Origin.y));
			Sleep(10); /* this delay is needed in order to set the cursor after action cam has been toggled */
			this->SetCursorPosition = GetCursorPosWR(this->Origin.x, this->Origin.y);
			this->SetCursor = true;
		}).detach();
	}

	// fix for weird rendering/cutoff
	this->Origin.x += 1.0f;
	this->Origin.y += 1.0f;

	this->HoverIndex = -1;
	this->HoverStartTime = -1;

	this->IsActive = true;
	return true;
}

void CRadialMenu::Release(ESelectionMode aReason)
{
	if (!this->IsActive) { return; }
	this->IsActive = false;

	/* if is cancel, directly set it to -1 to not trigger any release */
	int idx = aReason == ESelectionMode::Escape ? -1 : this->HoverIndex;

	/* winapi set cursor */
	if (this->RestoreCursor && aReason != ESelectionMode::SingleItem)
	{
		this->SetCursorPosition = GetCursorPosWR(this->MousePos.x, this->MousePos.y);
		this->SetCursor = true;
	}

	if (this->WasActionCamActive && aReason != ESelectionMode::SingleItem)
	{
		this->API->GameBinds.InvokeAsync(EGameBinds_CameraActionMode, 10);
	}

	this->WasActionCamActive = false;

	if (aReason == ESelectionMode::SingleItem)
	{
		idx = 0;
	}

	if (idx > -1)
	{
		RadialItem* item = this->DrawnItems[idx];
		
		std::thread([item]() {
			RadialCtx->QueueItem(item);
		}).detach();
	}
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

void CRadialMenu::AddItemAction(std::string aItemId, EActionType aType, std::string aValue, Conditions aActivation, bool aOnlyExecuteIfPrevious)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionGeneric* action = new ActionGeneric();
		action->Type = aType;
		action->Activation = aActivation;
		action->OnlyExecuteIfPrevious = aOnlyExecuteIfPrevious;
		action->Identifier = aValue;

		item->Actions.push_back(action);
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, EActionType aType, EGameBinds aValue, Conditions aActivation, bool aOnlyExecuteIfPrevious)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionGameInputBind* action = new ActionGameInputBind();
		action->Type = aType;
		action->Activation = aActivation;
		action->OnlyExecuteIfPrevious = aOnlyExecuteIfPrevious;
		action->Identifier = aValue;

		item->Actions.push_back(action);
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, int aValue, Conditions aActivation, bool aOnlyExecuteIfPrevious)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionDelay* action = new ActionDelay();
		action->Type = EActionType::Delay;
		action->Activation = aActivation;
		action->OnlyExecuteIfPrevious = aOnlyExecuteIfPrevious;
		action->Duration = aValue;

		item->Actions.push_back(action);
	}
}

void CRadialMenu::AddItemAction(std::string aItemId, EActionType aType, Conditions aActivation, bool aOnlyExecuteIfPrevious)
{
	RadialItem* item = this->GetItem(aItemId);

	if (item)
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		ActionBase* action = new ActionBase();
		action->Type = aType;
		action->Activation = aActivation;
		action->OnlyExecuteIfPrevious = aOnlyExecuteIfPrevious;

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

std::filesystem::path CRadialMenu::GetPath()
{
	return this->Path;
}

void CRadialMenu::Invalidate()
{
	this->BaseTexture = nullptr;
	this->DividerTexture = nullptr;
	this->SegmentTexture = nullptr;

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
			this->Size = ImVec2(520.0f * this->Scale, 520.0f * this->Scale);
			this->MinimalMouseMoveDistance = 150.0f * this->Scale;
			this->SegmentContentDistance = 200.0f * this->Scale;
			this->SegmentContentSize = ImVec2(128.0f * this->Scale, 128.0f * this->Scale);
			break;
		case ERadialType::Small:
			this->ItemsCapacity = 8;
			this->Size = ImVec2(112.0f * this->Scale, 112.0f * this->Scale);
			this->MinimalMouseMoveDistance = 18.0f * this->Scale;
			this->SegmentContentDistance = 36.0f * this->Scale;
			this->SegmentContentSize = ImVec2(32.0f * this->Scale, 32.0f * this->Scale);
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

void CRadialMenu::ApplyColorToAll(unsigned int aColor, int aColorIndex)
{
	if (aColorIndex == 0) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	for (RadialItem* item : this->Items)
	{
		switch (aColorIndex)
		{
			case 1: item->Color = aColor; break;
			case 2: item->ColorHover = aColor; break;
		}
	}
}

void CRadialMenu::ApplyActivationTimeoutToAll(int aTimeout)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	for (RadialItem* item : this->Items)
	{
		item->ActivationTimeout = aTimeout;
	}
}

void CRadialMenu::ApplyConditionToAll(Conditions* aOrigin, int aConditionIndex, int aStateIndex)
{
	if (aConditionIndex == 0) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	EObserveBoolean targetState = aOrigin->GetIndex(aStateIndex);
	Conditions fullCopy = *aOrigin;

	for (RadialItem* item : this->Items)
	{
		Conditions* conditions = nullptr;

		switch (aConditionIndex)
		{
			case 1: conditions = &item->Visibility; break;
			case 2: conditions = &item->Activation; break;
		}

		if (conditions == nullptr) { continue; }

		if (aStateIndex == -99)
		{
			*conditions = fullCopy;
		}
		else
		{
			conditions->SetIndex(aStateIndex, targetState);
		}
	}
}

void CRadialMenu::ApplyConditionToAll(RadialItem* aItem, Conditions* aOrigin, int aStateIndex)
{
	if (aItem == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	EObserveBoolean targetState = aOrigin->GetIndex(aStateIndex);
	Conditions fullCopy = *aOrigin;

	for (ActionBase* action : aItem->Actions)
	{
		Conditions* conditions = &action->Activation;

		if (aStateIndex == -99)
		{
			*conditions = fullCopy;
		}
		else
		{
			conditions->SetIndex(aStateIndex, targetState);
		}
	}
}

int CRadialMenu::GetCurrentlyHoveredIndex()
{
	int hoverIndex = -1;

	if (this->DrawnItems.size() < 2) { return hoverIndex; }

	ImVec2 currentMousePos = ImGui::GetMousePos();

	float angle = ImGui::Angle(Origin, currentMousePos);
	float distance = ImGui::Distance(Origin, currentMousePos);

	if (distance >= this->MinimalMouseMoveDistance * NexusLink->Scaling)
	{
		float hoverOffset = abs((float)this->ItemRotationDegrees / this->SegmentRadius);
		hoverIndex = ceilf((angle - this->ItemRotationDegrees) / this->SegmentRadius) - 1;

		/* wrap around */
		if (hoverIndex < 0)
		{
			hoverIndex += this->DrawnItems.size();
		}
		else if (hoverIndex >= this->DrawnItems.size())
		{
			hoverIndex -= this->DrawnItems.size();
		}
	}

	return hoverIndex;
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

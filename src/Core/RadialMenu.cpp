///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialMenu.cpp
/// Description  :  Handles the logic for a radial menu element.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "RadialMenu.h"

#include "Shared.h"
#include "resource.h"
#include "imgui_extensions.h"

CRadialMenu::CRadialMenu(std::string aIdentifier, ERadialType aRadialMenuType)
{
	this->Identifier = aIdentifier;
	this->Type = aRadialMenuType;

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

	this->SegmentRadius = 0;

	this->IsActive = false;
	this->Origin = ImVec2(-1, -1);

	this->Invalidate();
}

void CRadialMenu::Render()
{
	if (this->Type == ERadialType::None) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	int amtItems = this->Items.size();

	if (amtItems < 2) { return; }
	if (!this->ShouldRender()) { return; }

	ImVec2 size = ImVec2(this->Size.x * NexusLink->Scaling, this->Size.y * NexusLink->Scaling);
	ImVec2 sizeHalf = ImVec2((size.x / 2.0f), (size.y / 2.0f));

	ImGui::SetNextWindowPos(ImVec2(this->Origin.x - sizeHalf.x, this->Origin.y - sizeHalf.y), ImGuiCond_Appearing);
	if (ImGui::Begin(("RADIAL##" + this->Identifier).c_str(), (bool*)0, (ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar)))
	{
		ImVec2 initialPos = ImVec2(1, 1);

		int hoverIndex = this->GetHoveredIndex();

		if (this->BaseTexture)
		{
			ImGui::SetCursorPos(initialPos);
			ImGui::Image(this->BaseTexture->Resource, size);
		}
		else
		{
			if (this->Type == ERadialType::Normal)
			{
				this->BaseTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_BASE", TEX_MW_BASE, SelfModule);
			}
			else if (this->Type == ERadialType::Small)
			{
				this->BaseTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_BASE", TEX_MWS_BASE, SelfModule);
			}
		}

		if (this->Type == ERadialType::Small)
		{
			if (this->DividerTexture)
			{
				for (size_t i = 0; i < amtItems; i++)
				{
					ImGui::ImageRotated(this->DividerTexture->Resource, Origin, size, SegmentRadius * i);
				}
			}
			else
			{
				this->DividerTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_DIVIDER", TEX_MWS_DIVIDER, SelfModule);
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
			for (size_t i = 0; i < amtItems; i++)
			{
				RadialItem* item = this->Items[i];
				float segmentStart = (SegmentRadius * i);
				ImGui::ImageRotated(this->SegmentTexture->Resource, Origin, size, segmentStart, hoverIndex == i ? ImColor(item->ColorHover) : ImColor(item->Color));
				
				if (item->Icon)
				{
					float deg = (segmentStart + (SegmentRadius / 2)) - 90.0f; // center of segment and correct for 0 up
					float degRad = deg * 3.14159f / 180.0f;

					float x = contentDistance * cos(degRad) + center.x;
					float y = contentDistance * sin(degRad) + center.y;

					ImGui::SetCursorPos(ImVec2(x - contentSizeHalf.x, y - contentSizeHalf.y));
					ImGui::Image(item->Icon->Resource, contentSize);
				}
				else
				{
					item->Icon = APIDefs->Textures.GetOrCreateFromFile(item->IconIdentifier.c_str(), "0.png");
				}
			}
		}
		else
		{
			this->LoadSegmentTexture();
		}
	}
	ImGui::End();
}

void CRadialMenu::AddItem(RadialItem* aMenuItem)
{
	if (aMenuItem == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

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

	this->Items.push_back(aMenuItem);

	this->SegmentRadius = 360.0f / this->Items.size();

	/* recalculate positions/offsets */

	this->Invalidate();
}

bool CRadialMenu::ShouldRender()
{
	/* check custom conditions here or use keybinds */
	if (!this->IsActive && GetAsyncKeyState(VK_CONTROL))
	{
		this->IsActive = true;
		Origin = ImGui::GetMousePos();

		// fix for weird rendering/cutoff
		Origin.x += 1.0f;
		Origin.y += 1.0f;
		return true;
	}
	else if (this->IsActive && GetAsyncKeyState(VK_CONTROL))
	{
		return true;
	}
	else if (this->IsActive && !GetAsyncKeyState(VK_CONTROL))
	{
		/* BIND RELEASE */
		int idx = this->GetHoveredIndex();
		if (idx > -1)
		{
			RadialItem* item = this->Items[idx];
			std::thread([item]() {
				/* FIXME: this needs to be able to abort if another item is selected halfway through execution */

				for (Action* act : item->Actions)
				{
					switch (act->Type)
					{
						case ERadialItemActionType::InputBind:
						{
							ActionGeneric* action = (ActionGeneric*)act;
							APIDefs->InputBinds.Invoke(action->Identifier, false);
							APIDefs->InputBinds.Invoke(action->Identifier, true);
							break;
						}
						case ERadialItemActionType::GameInputBind:
						{
							ActionGameInputBind* action = (ActionGameInputBind*)act;
							APIDefs->GameBinds.InvokeAsync(action->Identifier, 100);
							break;
						}
						case ERadialItemActionType::Event:
						{
							ActionGeneric* action = (ActionGeneric*)act;
							APIDefs->Events.Raise(action->Identifier, nullptr);
							break;
						}
						case ERadialItemActionType::Delay:
						{
							ActionDelay* action = (ActionDelay*)act;
							Sleep(action->Duration);
							break;
						}
					}
				}
			}).detach();
		}
	}

	this->IsActive = false;
	Origin = ImVec2(-1, -1);
	return false;
}

void CRadialMenu::Invalidate()
{
	this->BaseTexture		= nullptr;
	this->DividerTexture	= nullptr;
	this->SegmentTexture	= nullptr;

	this->IsActive = false;
	this->Origin = ImVec2(-1, -1);
}

int CRadialMenu::GetHoveredIndex()
{
	int hoverIndex = -1;

	ImVec2 currentMousePos = ImGui::GetMousePos();

	float angle = ImGui::Angle(Origin, currentMousePos);
	float distance = ImGui::Distance(Origin, currentMousePos);

	if (distance >= this->MinimalMouseMoveDistance * NexusLink->Scaling)
	{
		hoverIndex = ceilf(angle / SegmentRadius) - 1;
	}

	return hoverIndex;
}

RadialItem* CRadialMenu::GetHovered()
{
	return this->GetHoveredIndex() > -1 
		? this->Items[this->GetHoveredIndex()] 
		: nullptr;
}

void CRadialMenu::LoadSegmentTexture()
{
	int amtItems = this->Items.size();

	if (this->Type == ERadialType::Normal)
	{
		switch (amtItems)
		{
		case 2:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR2", TEX_MW_SELECTOR2, SelfModule);
			break;
		case 3:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR3", TEX_MW_SELECTOR3, SelfModule);
			break;
		case 4:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR4", TEX_MW_SELECTOR4, SelfModule);
			break;
		case 5:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR5", TEX_MW_SELECTOR5, SelfModule);
			break;
		case 6:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR6", TEX_MW_SELECTOR6, SelfModule);
			break;
		case 7:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR7", TEX_MW_SELECTOR7, SelfModule);
			break;
		case 8:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR8", TEX_MW_SELECTOR8, SelfModule);
			break;
		case 9:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR9", TEX_MW_SELECTOR9, SelfModule);
			break;
		case 10:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR10", TEX_MW_SELECTOR10, SelfModule);
			break;
		case 11:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR11", TEX_MW_SELECTOR11, SelfModule);
			break;
		case 12:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MW_SELECTOR12", TEX_MW_SELECTOR12, SelfModule);
			break;
		}
	}
	else if (this->Type == ERadialType::Small)
	{
		switch (amtItems)
		{
		case 2:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR2", TEX_MWS_SELECTOR2, SelfModule);
			break;
		case 3:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR3", TEX_MWS_SELECTOR3, SelfModule);
			break;
		case 4:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR4", TEX_MWS_SELECTOR4, SelfModule);
			break;
		case 5:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR5", TEX_MWS_SELECTOR5, SelfModule);
			break;
		case 6:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR6", TEX_MWS_SELECTOR6, SelfModule);
			break;
		case 7:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR7", TEX_MWS_SELECTOR7, SelfModule);
			break;
		case 8:
			this->SegmentTexture = APIDefs->Textures.GetOrCreateFromResource("TEX_MWS_SELECTOR8", TEX_MWS_SELECTOR8, SelfModule);
			break;
		}
	}
}

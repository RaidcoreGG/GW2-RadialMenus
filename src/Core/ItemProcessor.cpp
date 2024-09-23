///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  ItemProcessor.cpp
/// Description  :  Handles execution of items and renders an interactable widget.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "ItemProcessor.h"

#include "imgui/imgui.h"

#include "resource.h"
#include "Shared.h"
#include "StateObserver.h"
#include "Util.h"

CItemProcessor::CItemProcessor()
{
	this->ProcessorThread = std::thread(&CItemProcessor::Process, this);
}

CItemProcessor::~CItemProcessor()
{
	this->IsCancelled = true;
	this->KillThread = true;
	this->ConVar.notify_one();
	this->ProcessorThread.join();
}

void CItemProcessor::Render()
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	/* if not executing and not editing, don't render */
	if (!this->IsExecuting && !this->IsEditing) { return; }

	ImGuiStyle& style = ImGui::GetStyle();

	float sz = 52.0f * NexusLink->Scaling;

	static bool isHovered = false;
	bool isEditing = this->IsEditing;

	ImGui::SetNextWindowSize(ImVec2(sz + 32.0f, sz + 32.0f), ImGuiCond_Always);
	if (ImGui::Begin("RADIAL##ITEMWIDGET", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | (this->IsEditing ? 0 : ImGuiWindowFlags_NoMove)))
	{
		float halfSz = sz / 2.0f;

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorPos(ImVec2(16.0f, 16.0f));
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImU32 color = ImColor(255, 255, 255, 255);
		ImU32 colorUnder = ImColor(255, 255, 255, 128);
		float thickness = 3 * NexusLink->Scaling;

		static float spinProgress = 0.0f;

		spinProgress += 0.01f;
		if (spinProgress > 1.0f) { spinProgress = 0; }

		int amtTotal = 128;
		float amtDrawnPerc = isEditing ? spinProgress : ((float)this->ElapsedTime / (float)(this->ActiveItem.ActivationTimeout * 1000));
		int amtDrawn = amtTotal * amtDrawnPerc;

		std::vector<ImVec2> circle;
		for (size_t i = 0; i < amtDrawn; i++)
		{
			float degRad = i * 360 / amtTotal * 3.14159f / 180.0f;
			constexpr float degOff = 90 * 3.15159f / 180.0f;

			float x = (halfSz * cos(degRad - degOff)) + halfSz + p.x;
			float y = (halfSz * sin(degRad - degOff)) + halfSz + p.y;

			circle.push_back(ImVec2(x, y));

			if (i > 0)
			{
				drawList->AddLine(circle[i - 1], circle[i], colorUnder, thickness * 1.25);
				drawList->AddLine(circle[i - 1], circle[i], color, thickness);
			}
		}

		if (amtTotal == amtDrawn)
		{
			drawList->AddLine(circle[amtTotal - 1], circle[0], colorUnder, thickness * 1.25);
			drawList->AddLine(circle[amtTotal - 1], circle[0], color, thickness);
		}

		bool useFallback = this->ActiveItem.Icon.Type == EIconType::None || this->ActiveItem.Icon.Value.empty();
		Texture* icon = !useFallback ? this->ActiveItem.Icon.Texture : this->WidgetFallbackIcon;

		if (icon)
		{
			ImVec2 iconSize = isHovered ? ImVec2(sz * 1.2f, sz * 1.2f) : ImVec2(sz, sz);

			ImVec2 iconPos = ImGui::GetCursorPos();

			if (isHovered)
			{
				float diff = (iconSize.x - sz) / 2.0f;
				iconPos = ImVec2(iconPos.x - diff, iconPos.y - diff);
			}

			if (this->WidgetBase)
			{
				ImGui::SetCursorPos(iconPos);
				ImGui::Image(this->WidgetBase->Resource, iconSize);
			}
			else
			{
				this->WidgetBase = APIDefs->Textures.GetOrCreateFromResource("TEX_RADIALWIDGET_BASE", TEX_WIDGET_BASE, SelfModule);
			}

			ImGui::SetCursorPos(iconPos);
			ImGui::Image(icon->Resource, iconSize);

			if (isEditing)
			{
				drawList->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + iconSize.x, p.y), ImColor(255, 0, 0, 255), 2.0f);
				drawList->AddLine(ImVec2(p.x + iconSize.x, p.y), ImVec2(p.x + iconSize.x, p.y + iconSize.y), ImColor(255, 0, 0, 255), 2.0f);
				drawList->AddLine(ImVec2(p.x + iconSize.x, p.y + iconSize.y), ImVec2(p.x, p.y + iconSize.y), ImColor(255, 0, 0, 255), 2.0f);
				drawList->AddLine(ImVec2(p.x, p.y + iconSize.y), ImVec2(p.x, p.y), ImColor(255, 0, 0, 255), 2.0f);
			}
			else
			{
				isHovered = ImGui::IsItemHovered();

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					/* reset countdown */
					this->ElapsedTime = 0;
				}
				else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					/* cancel action */
					this->IsCancelled = true;
				}
			}
		}
		else
		{
			if (!icon && !useFallback)
			{
				this->ActiveItem.Icon.Type = EIconType::None;
				this->ActiveItem.Icon.Value = {};
			}

			this->WidgetFallbackIcon = APIDefs->Textures.GetOrCreateFromResource("ICON_RADIALWIDGET_FALLBACK", ICON_FALLBACK, SelfModule);
		}
	}
	ImGui::End();
}

void CItemProcessor::QueueItem(RadialItem* aItem)
{
	const std::lock_guard<std::mutex> lock(this->QueueMutex);

	/* if the same item is queued, abort execution */
	if (this->IsExecuting && aItem->Identifier == this->ActiveItem.Identifier)
	{
		this->QueuedItem = nullptr; // sanity
		this->IsCancelled = true;
		return;
	}
	else if (this->IsExecuting)
	{
		this->IsCancelled = true;
	}

	RadialItem* newItem = new RadialItem(*aItem);
	newItem->Actions.clear();

	for (ActionBase* act : aItem->Actions)
	{
		ActionBase* action = nullptr;

		switch (act->Type)
		{
			case EActionType::InputBind:
			case EActionType::Event:
			{
				action = new ActionGeneric(*(ActionGeneric*)act);
				((ActionGeneric*)action)->Identifier = ((ActionGeneric*)act)->Identifier;
				break;
			}
			case EActionType::GameInputBind:
			case EActionType::GameInputBindPress:
			case EActionType::GameInputBindRelease:
			{
				action = new ActionGameInputBind(*(ActionGameInputBind*)act);
				break;
			}
			case EActionType::Delay:
			{
				action = new ActionDelay(*(ActionDelay*)act);
				break;
			}
			case EActionType::Return:
			{
				action = new ActionBase(*(ActionBase*)act);
				break;
			}
		}

		newItem->Actions.push_back(action);
	}

	this->QueuedItem = newItem;
	this->ConVar.notify_one();
}

void CItemProcessor::Process()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lockThread(this->ThreadMutex);
		this->ConVar.wait(lockThread, [this] { return this->KillThread; });

		if (this->KillThread) { return; }

		while (this->IsExecuting)
		{
			Sleep(1);
		}

		{
			/* lock as we're accessing the queue item */
			const std::lock_guard<std::mutex> lockqueue(this->QueueMutex);
			/* set data */
			{
				const std::lock_guard<std::mutex> lock(this->Mutex);
				this->ActiveItem.Identifier        = this->QueuedItem->Identifier;
				this->ActiveItem.Icon              = this->QueuedItem->Icon;
				this->ActiveItem.Activation        = this->QueuedItem->Activation;
				this->ActiveItem.ActivationTimeout = this->QueuedItem->ActivationTimeout > 0 ? this->QueuedItem->ActivationTimeout : 1;
			}
			this->ActiveItem.Actions.swap(this->QueuedItem->Actions);

			/* unset queued */
			delete this->QueuedItem;
			this->QueuedItem = nullptr;
		}

		/* set states */
		this->IsCancelled = false;
		this->IsExecuting = true;
		this->ElapsedTime = 0;

		/* process */
		this->AwaitActivation();
		this->ExecuteActiveActions();
		this->DestroyActiveActions();
	}
}

void CItemProcessor::AwaitActivation()
{
	while (!StateObserver::IsMatch(&this->ActiveItem.Activation))
	{
		this->ElapsedTime += 50;
		Sleep(50);

		if (this->IsCancelled) { return; }
		if (this->ElapsedTime > this->ActiveItem.ActivationTimeout * 1000) { break; }
	}

	if (this->ElapsedTime > this->ActiveItem.ActivationTimeout * 1000)
	{
		std::string str = String::Format("Cancelled activation of %s after waiting for %d seconds.", this->ActiveItem.Identifier.c_str(), this->ActiveItem.ActivationTimeout);
		APIDefs->UI.SendAlert(str.c_str());
		this->IsCancelled = true;
		return;
	}
}

void CItemProcessor::ExecuteActiveActions()
{
	if (this->IsCancelled) { return; }

	/* initially set to true, and only in the skip set to false, this way the first action ignores the setting */
	bool previousExecuted = true;
	for (ActionBase* act : this->ActiveItem.Actions)
	{
		if (this->IsCancelled) { return; }

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
				APIDefs->InputBinds.Invoke(action->Identifier.c_str(), false);
				APIDefs->InputBinds.Invoke(action->Identifier.c_str(), true);
				break;
			}
			case EActionType::GameInputBind:
			{
				ActionGameInputBind* action = (ActionGameInputBind*)act;

				APIDefs->GameBinds.Press(action->Identifier);
				Sleep(100);
				APIDefs->GameBinds.Release(action->Identifier);

				break;
			}
			case EActionType::GameInputBindPress:
			{
				ActionGameInputBind* action = (ActionGameInputBind*)act;
				APIDefs->GameBinds.Press(action->Identifier);
				break;
			}
			case EActionType::GameInputBindRelease:
			{
				ActionGameInputBind* action = (ActionGameInputBind*)act;
				APIDefs->GameBinds.Release(action->Identifier);
				break;
			}
			case EActionType::Event:
			{
				ActionGeneric* action = (ActionGeneric*)act;
				APIDefs->Events.Raise(action->Identifier.c_str(), nullptr);
				break;
			}
			case EActionType::Delay:
			{
				ActionDelay* action = (ActionDelay*)act;
				Sleep(action->Duration);
				break;
			}
			case EActionType::Return:
			{
				/* EXPLICIT RETURN to prevent further actions */
				return;
			}
		}

		previousExecuted = true;
	}
}

void CItemProcessor::DestroyActiveActions()
{
	for (ActionBase* act : this->ActiveItem.Actions)
	{
		switch (act->Type)
		{
			case EActionType::InputBind:
			case EActionType::Event:
			{
				delete (ActionGeneric*)act;
				break;
			}
			case EActionType::GameInputBind:
			case EActionType::GameInputBindPress:
			case EActionType::GameInputBindRelease:
			{
				delete (ActionGameInputBind*)act;
				break;
			}
			case EActionType::Delay:
			{
				delete (ActionDelay*)act;
				break;
			}
			case EActionType::Return:
			{
				delete (ActionBase*)act;
				break;
			}
		}
	}

	this->ActiveItem.Actions.clear();

	this->IsExecuting = false;
}

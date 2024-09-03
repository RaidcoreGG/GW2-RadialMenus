///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialMenu.h
/// Description  :  Handles the logic for a radial menu element.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef RADIALMENU_H
#define RADIALMENU_H

#include <mutex>
#include <string>
#include <vector>
#include <filesystem>

#include "imgui/imgui.h"
#include "Mumble/Mumble.h"
#include "Nexus/Nexus.h"

#include "RadialItem.h"
#include "ESelectionMode.h"
#include "ERadialType.h"

///----------------------------------------------------------------------------------------------------
/// CRadialMenu Class
///----------------------------------------------------------------------------------------------------
class CRadialMenu
{
	public:
	bool                        DrawInCenter;
	bool                        RestoreCursor;
	float                       Scale;
	int                         HoverTimeout;
	int                         ItemRotationDegrees;

	///----------------------------------------------------------------------------------------------------
	/// ctor
	///----------------------------------------------------------------------------------------------------
	CRadialMenu(AddonAPI* aAPI, HMODULE aModule, std::filesystem::path aPath, int aID, std::string aIdentifier, float aScale = 1.0f, int aHoverTimeout = 0, ERadialType aRadialMenuType = ERadialType::Normal, ESelectionMode aSelectionMode = ESelectionMode::ReleaseOrClick);
	///----------------------------------------------------------------------------------------------------
	/// dtor
	///----------------------------------------------------------------------------------------------------
	~CRadialMenu();

	///----------------------------------------------------------------------------------------------------
	/// Save:
	/// 	Saves the changes to disk.
	///----------------------------------------------------------------------------------------------------
	void Save();

	///----------------------------------------------------------------------------------------------------
	/// Render:
	/// 	Renders the radial menu. Returns true if an item is hovered.
	///----------------------------------------------------------------------------------------------------
	bool Render();

	///----------------------------------------------------------------------------------------------------
	/// Activate:
	/// 	Activates the radial menu and starts rendering.
	/// 	Returns true if activated or false if conditions not met.
	///----------------------------------------------------------------------------------------------------
	bool Activate();

	///----------------------------------------------------------------------------------------------------
	/// Release:
	/// 	Releases the radial menu.
	///----------------------------------------------------------------------------------------------------
	void Release(bool aIsCancel = false);

	///----------------------------------------------------------------------------------------------------
	/// AddItem:
	/// 	Adds a selector to the radial menu.
	///----------------------------------------------------------------------------------------------------
	void AddItem(std::string aName, unsigned int aColor, unsigned int aColorHover, EIconType aIconType, std::string aIconValue,
				 Conditions aVisibility = {}, Conditions aActivation = {}, int aActivationTimeout = 30);

	///----------------------------------------------------------------------------------------------------
	/// RemoveItem:
	/// 	Adds a selector to the radial menu.
	///----------------------------------------------------------------------------------------------------
	void RemoveItem(std::string aIdentifier);

	///----------------------------------------------------------------------------------------------------
	/// GetItem:
	/// 	Returns the item matching the ID.
	///----------------------------------------------------------------------------------------------------
	RadialItem* GetItem(std::string aIdentifier);

	///----------------------------------------------------------------------------------------------------
	/// GetItems:
	/// 	Returns all radial items.
	///----------------------------------------------------------------------------------------------------
	const std::vector<RadialItem*>& GetItems();

	///----------------------------------------------------------------------------------------------------
	/// MoveItemUp:
	/// 	Moves an item up in order.
	///----------------------------------------------------------------------------------------------------
	void MoveItemUp(std::string aIdentifier);

	///----------------------------------------------------------------------------------------------------
	/// MoveItemDown:
	/// 	Moves an item down in order.
	///----------------------------------------------------------------------------------------------------
	void MoveItemDown(std::string aIdentifier);
	
	///----------------------------------------------------------------------------------------------------
	/// AddItemAction:
	/// 	Adds an action to an item.
	///----------------------------------------------------------------------------------------------------
	void AddItemAction(std::string aItemId, EActionType aType, std::string aValue, Conditions aActivation = {}, bool aOnlyExecuteIfPrevious = false);

	///----------------------------------------------------------------------------------------------------
	/// AddItemAction:
	/// 	Adds an action to an item.
	///----------------------------------------------------------------------------------------------------
	void AddItemAction(std::string aItemId, EActionType aType, EGameBinds aValue, Conditions aActivation = {}, bool aOnlyExecuteIfPrevious = false);

	///----------------------------------------------------------------------------------------------------
	/// AddItemAction:
	/// 	Adds an action to an item.
	///----------------------------------------------------------------------------------------------------
	void AddItemAction(std::string aItemId, int aValue, Conditions aActivation = {}, bool aOnlyExecuteIfPrevious = false);

	///----------------------------------------------------------------------------------------------------
	/// RemoveItemAction:
	/// 	Removes an action from an item.
	///----------------------------------------------------------------------------------------------------
	void RemoveItemAction(std::string aItemId, int aIndex);

	///----------------------------------------------------------------------------------------------------
	/// GetID:
	/// 	Returns the ID of the Radial.
	///----------------------------------------------------------------------------------------------------
	int GetID();

	///----------------------------------------------------------------------------------------------------
	/// SetID:
	/// 	Sets the ID of the Radial.
	///----------------------------------------------------------------------------------------------------
	void SetID(int aID);

	///----------------------------------------------------------------------------------------------------
	/// GetName:
	/// 	Returns the name.
	///----------------------------------------------------------------------------------------------------
	const std::string& GetName();

	///----------------------------------------------------------------------------------------------------
	/// SetName:
	/// 	Sets the name.
	///----------------------------------------------------------------------------------------------------
	void SetName(std::string aName);

	///----------------------------------------------------------------------------------------------------
	/// GetType:
	/// 	Returns the type.
	///----------------------------------------------------------------------------------------------------
	ERadialType GetType();

	///----------------------------------------------------------------------------------------------------
	/// SetType:
	/// 	Sets the type.
	///----------------------------------------------------------------------------------------------------
	void SetType(ERadialType aType);

	///----------------------------------------------------------------------------------------------------
	/// GetSelectionMode:
	/// 	Returns the SelectionMode.
	///----------------------------------------------------------------------------------------------------
	ESelectionMode GetSelectionMode();

	///----------------------------------------------------------------------------------------------------
	/// SetSelectionMode:
	/// 	Sets the SelectionMode.
	///----------------------------------------------------------------------------------------------------
	void SetSelectionMode(ESelectionMode aSelectionMode);

	///----------------------------------------------------------------------------------------------------
	/// GetCapacity:
	/// 	Returns the max capacity of items of this radial.
	///----------------------------------------------------------------------------------------------------
	int GetCapacity();

	///----------------------------------------------------------------------------------------------------
	/// GetInputBind:
	/// 	Constructs the input bind with the ID.
	///----------------------------------------------------------------------------------------------------
	std::string GetInputBind();

	///----------------------------------------------------------------------------------------------------
	/// GetPath:
	/// 	Returns the file path of this radial menu.
	///----------------------------------------------------------------------------------------------------
	std::filesystem::path GetPath();
	
	///----------------------------------------------------------------------------------------------------
	/// Invalidate:
	/// 	Invalidates all textures and other amount dependant resources, causing them to be refreshed.
	///----------------------------------------------------------------------------------------------------
	void Invalidate();

	///----------------------------------------------------------------------------------------------------
	/// ApplyColorToAll:
	/// 	Applies the given color to all items.
	/// 	ColorIndex: 0 = None, 1 = Color, 2 = ColorHover
	///----------------------------------------------------------------------------------------------------
	void ApplyColorToAll(unsigned int aColor, int aColorIndex);

	///----------------------------------------------------------------------------------------------------
	/// ApplyActivationTimeoutToAll:
	/// 	Applies the activation timeout to all items.
	///----------------------------------------------------------------------------------------------------
	void ApplyActivationTimeoutToAll(int aTimeout);

	///----------------------------------------------------------------------------------------------------
	/// ApplyConditionToAll:
	/// 	Applies the given condition to all items.
	/// 	aConditionIndex: 0 = None, 1 = Visibility, 2 = Activation
	/// 	aStateIndex: -99 copy all else matching the 0-based index of the observed state of the struct.
	///----------------------------------------------------------------------------------------------------
	void ApplyConditionToAll(Conditions* aOrigin, int aConditionIndex, int aStateIndex);

	///----------------------------------------------------------------------------------------------------
	/// ApplyConditionToAll:
	/// 	Applies the given condition to all actions in this item.
	/// 	aStateIndex: -99 copy all else matching the 0-based index of the observed state of the struct.
	///----------------------------------------------------------------------------------------------------
	void ApplyConditionToAll(RadialItem* aItem, Conditions* aOrigin, int aStateIndex);

	private:
	AddonAPI*                   API;
	HMODULE                     Module;
	NexusLinkData*              NexusLink;

	std::filesystem::path       Path;

	int                         ID;
	std::string                 Identifier;
	ERadialType                 Type;
	ESelectionMode              SelectionMode;

	std::mutex                  Mutex;
	std::vector<RadialItem*>    Items;
	int                         ItemsCapacity;
	std::vector<RadialItem*>    DrawnItems;

	Texture*                    BaseTexture;
	Texture*                    DividerTexture;
	Texture*                    SegmentTexture;

	ImVec2                      Size;
	float                       MinimalMouseMoveDistance;
	float                       SegmentContentDistance;
	ImVec2                      SegmentContentSize;
	float                       SegmentRadius;

	bool                        IsActive;
	ImVec2                      Origin;
	ImVec2                      MousePos;
	bool                        WasActionCamActive;

	int                         HoverIndex;
	unsigned long long          HoverStartTime;

	bool                        SetCursor;
	ImVec2                      SetCursorPosition;

	///----------------------------------------------------------------------------------------------------
	/// GetCurrentlyHoveredIndex:
	/// 	Returns the index of the currently hovered item.
	///----------------------------------------------------------------------------------------------------
	int GetCurrentlyHoveredIndex();

	///----------------------------------------------------------------------------------------------------
	/// LoadSegmentTexture:
	/// 	Loads the corresponding texture for the segment size.
	///----------------------------------------------------------------------------------------------------
	void LoadSegmentTexture();
};

#endif

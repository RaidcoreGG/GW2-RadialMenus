///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialMenu.h
/// Description  :  Handles the logic for a radial menu element.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef RADIALMENU_H
#define RADIALMENU_H

#include <string>
#include <mutex>
#include <vector>

#include "ERadialMenuType.h"
#include "ERadialItemAction.h"

#include "Nexus/Nexus.h"
#include "imgui/imgui.h"

///----------------------------------------------------------------------------------------------------
/// RadialItem Struct
///----------------------------------------------------------------------------------------------------
typedef struct RadialItem
{
	unsigned int			Color;
	unsigned int			ColorHover;
	std::string				IconIdentifier;
	Texture*				Icon;
	std::vector<Action*>	Actions;
} RadialMenuItem;

///----------------------------------------------------------------------------------------------------
/// CRadialMenu Class
///----------------------------------------------------------------------------------------------------
class CRadialMenu
{
public:
	///----------------------------------------------------------------------------------------------------
	/// ctor
	///----------------------------------------------------------------------------------------------------
	CRadialMenu(std::string aIdentifier, ERadialType aRadialMenuType);
	///----------------------------------------------------------------------------------------------------
	/// dtor
	///----------------------------------------------------------------------------------------------------
	~CRadialMenu() = default;

	///----------------------------------------------------------------------------------------------------
	/// Render:
	/// 	Renders the radial menu.
	///----------------------------------------------------------------------------------------------------
	void Render();

	///----------------------------------------------------------------------------------------------------
	/// AddItem:
	/// 	Adds a selector to the radial menu.
	///----------------------------------------------------------------------------------------------------
	void AddItem(RadialItem* aMenuItem);

private:
	std::string						Identifier;
	ERadialType						Type;
	ImVec2							Size;

	std::mutex						Mutex;
	std::vector<RadialItem*>		Items;
	int								ItemsCapacity;

	Texture*						BaseTexture;
	Texture*						DividerTexture;
	Texture*						SegmentTexture;

	float							MinimalMouseMoveDistance;
	float							SegmentContentDistance;
	ImVec2							SegmentContentSize;
	float							SegmentRadius;

	bool							IsActive;
	ImVec2							Origin;

	///----------------------------------------------------------------------------------------------------
	/// ShouldRender:
	/// 	Determines whether this radial menu should be visible.
	///----------------------------------------------------------------------------------------------------
	bool ShouldRender();

	///----------------------------------------------------------------------------------------------------
	/// Invalidate:
	/// 	Invalidates all textures and other amount dependant resources, causing them to be refreshed.
	///----------------------------------------------------------------------------------------------------
	void Invalidate();

	///----------------------------------------------------------------------------------------------------
	/// GetHoveredIndex:
	/// 	Returns the index of the currently hovered item.
	///----------------------------------------------------------------------------------------------------
	int GetHoveredIndex();

	///----------------------------------------------------------------------------------------------------
	/// GetHovered:
	/// 	Returns the currently hovered item.
	///----------------------------------------------------------------------------------------------------
	RadialItem* GetHovered();

	///----------------------------------------------------------------------------------------------------
	/// LoadSegmentTexture:
	/// 	Loads the corresponding texture for the segment size.
	///----------------------------------------------------------------------------------------------------
	void LoadSegmentTexture();
};

#endif

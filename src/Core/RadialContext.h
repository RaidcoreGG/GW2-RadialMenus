///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  RadialContext.h
/// Description  :  Primary interface for radial menus.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef RADIALCONTEXT_H
#define RADIALCONTEXT_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "ERadialType.h"
#include "RadialMenu.h"

///----------------------------------------------------------------------------------------------------
/// CRadialContext Class
///----------------------------------------------------------------------------------------------------
class CRadialContext
{
	public:
	///----------------------------------------------------------------------------------------------------
	/// ctor
	///----------------------------------------------------------------------------------------------------
	CRadialContext() = default;
	///----------------------------------------------------------------------------------------------------
	/// dtor
	///----------------------------------------------------------------------------------------------------
	~CRadialContext() = default;

	///----------------------------------------------------------------------------------------------------
	/// Activate:
	/// 	Activates the radial menu.
	///----------------------------------------------------------------------------------------------------
	void Activate(CRadialMenu* aRadial);

	///----------------------------------------------------------------------------------------------------
	/// Release:
	/// 	Releases the radial menu.
	///----------------------------------------------------------------------------------------------------
	bool Release(ESelectionMode aMode);

	void Add(std::string aIdentifier, ERadialType aType);
	void Remove(std::string aIdentifier);

	void AddItem(std::string aRadialId, std::string aItemId, unsigned int aColor, unsigned int aColorHover, EIconType aIconType, std::string aIconValue);
	void RemoveItem(std::string aRadialId, std::string aItemId);

	void AddItemAction(std::string aRadialId, std::string aItemId, EActionType aType, std::string aValue);
	void AddItemAction(std::string aRadialId, std::string aItemId, EActionType aType, EGameBinds aValue);
	void AddItemAction(std::string aRadialId, std::string aItemId, int aValue);
	void RemoveItemAction(std::string aRadialId, std::string aItemId);

	void Render();
	void RenderOptions();

	void OnInputBind(std::string aIdentifier, bool aIsRelease);
	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
	std::mutex										Mutex;
	std::vector<CRadialMenu*>						Radials;
	std::unordered_map<std::string, CRadialMenu*>	RadialIBMap;
	CRadialMenu*									ActiveRadial = nullptr;
};

#endif

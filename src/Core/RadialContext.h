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
#include <map>
#include <vector>

#include "ERadialType.h"
#include "RadialMenu.h"

///----------------------------------------------------------------------------------------------------
/// CRadialContext Class
///----------------------------------------------------------------------------------------------------
class CRadialContext
{
	public:
	bool IsLeftClickHeld = false;
	bool IsRightClickHeld = false;

	///----------------------------------------------------------------------------------------------------
	/// ctor
	///----------------------------------------------------------------------------------------------------
	CRadialContext() = default;
	///----------------------------------------------------------------------------------------------------
	/// dtor
	///----------------------------------------------------------------------------------------------------
	~CRadialContext() = default;

	void CreateDefaultMountRadial();

	void Load();
	void Save();

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

	void Render();
	void RenderOptions();

	void OnInputBind(std::string aIdentifier, bool aIsRelease);
	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void QueueItem(RadialItem* aItem);

	private:
	std::mutex                          Mutex;
	std::vector<CRadialMenu*>           Radials;
	std::map<std::string, CRadialMenu*> RadialIBMap;
	CRadialMenu*                        ActiveRadial = nullptr;

	/* Queued Item */
	std::mutex                          QueuedItemMutex;
	bool                                IsCanceled = false;
	RadialItem*                         QueuedItem = nullptr;
	int                                 QueuedElapsedTime = 0;
	Texture*                            WidgetBase = nullptr;

	/* Editor */
	CRadialMenu*                        EditingMenu = nullptr;
	RadialItem*                         EditingItem = nullptr;
	bool                                IsEditingWidget = false;

	void RenderEditorTab();
	void RenderSettingsTab();
	void RenderWidget();

	void LoadInternal();
	void SaveInternal();

	void ExecuteQueuedItem(RadialItem* aItem);
	void DestroyQueuedItem();

	CRadialMenu* Add(std::filesystem::path aPath, std::string aIdentifier, ERadialType aRadialMenuType = ERadialType::Normal, ESelectionMode aSelectionMode = ESelectionMode::ReleaseOrClick, int aID = -1);
	void Remove(std::string aIdentifier);
	void Remove(std::filesystem::path aPath);

	int GetLowestUnusedID();
	bool IsIDInUse(int aID, CRadialMenu* aRadialSkip = nullptr);

	std::string GetUnusedName(std::string aName, CRadialMenu* aRadialSkip = nullptr);
	bool IsNameInUse(std::string aName, CRadialMenu* aRadialSkip = nullptr);

	void GenerateIBMap();
};

#endif

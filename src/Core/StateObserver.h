///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  StateObserver.h
/// Description  :  Observes a set of states for specific conditions.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef STATEOBSERVER_H
#define STATEOBSERVER_H

#include <Windows.h>

#include "Conditions.h"

namespace StateObserver
{
	///----------------------------------------------------------------------------------------------------
	/// Advance:
	/// 	Updates the states.
	///----------------------------------------------------------------------------------------------------
	void Advance();

	///----------------------------------------------------------------------------------------------------
	/// IsMatch:
	/// 	Returns whether the conditions are met or not.
	///----------------------------------------------------------------------------------------------------
	bool IsMatch(Conditions* aConditions);

	///----------------------------------------------------------------------------------------------------
	/// WndProc:
	/// 	Handles raw input.
	///----------------------------------------------------------------------------------------------------
	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	std::string StateToString(EObserveBoolean aState);
	std::string StateToString(EObserveMount aState);

	///----------------------------------------------------------------------------------------------------
	/// WndProc:
	/// 	Handles raw input.
	///----------------------------------------------------------------------------------------------------
	void RenderDebug();
}

#endif

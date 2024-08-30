#include "RadialItem.h"
#include <Util.h>
#include "StateObserver.h"
#include <thread>

void RadialMenuItem::Activate(AddonAPI* API) {

	std::thread([this, API]() {
		/* FIXME: this needs to be able to abort if another item is selected halfway through execution */
		int msWaited = 0;
		while (!StateObserver::IsMatch(&this->Activation))
		{
			msWaited += 100;
			Sleep(100);

			if (msWaited > this->ActivationTimeout * 1000) { break; }
		}

		if (msWaited > this->ActivationTimeout * 1000)
		{
			API->UI.SendAlert("Cancelled after waiting for timeout.");
			return;
		}

		/* initially set to true, and only in the skip set to false, this way the first action ignores the setting */
		bool previousExecuted = true;
		for (ActionBase* act : this->Actions)
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
				API->InputBinds.Invoke(action->Identifier.c_str(), false);
				API->InputBinds.Invoke(action->Identifier.c_str(), true);
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
					API->WndProc.SendToGameOnly(0, WM_SYSKEYUP, VK_MENU, Input::GetKeyMessageLPARAM(VK_MENU, false, true));
				}
				if (wasCtrlPressed)
				{
					API->WndProc.SendToGameOnly(0, WM_KEYUP, VK_CONTROL, Input::GetKeyMessageLPARAM(VK_CONTROL, false, false));
				}
				if (wasShiftPressed)
				{
					API->WndProc.SendToGameOnly(0, WM_KEYUP, VK_SHIFT, Input::GetKeyMessageLPARAM(VK_SHIFT, false, false));
				}

				/* execute action action */
				API->GameBinds.Press(action->Identifier);
				Sleep(100);
				API->GameBinds.Release(action->Identifier);

				/* restore modifier state */
				if (GetAsyncKeyState(VK_MENU))
				{
					API->WndProc.SendToGameOnly(0, WM_SYSKEYDOWN, VK_MENU, Input::GetKeyMessageLPARAM(VK_MENU, true, true));
				}
				if (GetAsyncKeyState(VK_CONTROL))
				{
					API->WndProc.SendToGameOnly(0, WM_KEYDOWN, VK_CONTROL, Input::GetKeyMessageLPARAM(VK_CONTROL, true, false));
				}
				if (GetAsyncKeyState(VK_SHIFT))
				{
					API->WndProc.SendToGameOnly(0, WM_KEYDOWN, VK_SHIFT, Input::GetKeyMessageLPARAM(VK_SHIFT, true, false));
				}
				break;
			}
			case EActionType::GameInputBindPress:
			{
				ActionGameInputBind* action = (ActionGameInputBind*)act;
				API->GameBinds.Press(action->Identifier);
				break;
			}
			case EActionType::GameInputBindRelease:
			{
				ActionGameInputBind* action = (ActionGameInputBind*)act;
				API->GameBinds.Release(action->Identifier);
				break;
			}
			case EActionType::Event:
			{
				ActionGeneric* action = (ActionGeneric*)act;
				API->Events.Raise(action->Identifier.c_str(), nullptr);
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
///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  StateObserver.cpp
/// Description  :  Observes a set of states for specific conditions.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "StateObserver.h"

#include "Shared.h"
#include "Util.h"

namespace StateObserver
{
	static Conditions CurrentState;

	static long long LastJumpTimestamp;
	
	static float LastDeltaY;
	static Vector3 LastPosition;

	static bool IsFalling = false; // const deltaY : -1.0403
	static bool IsGliding = false; // const deltaY : -0.1145
	static bool IsAscending = false; // deltaY > 2.5
	static bool IsJumping = false;

	bool operator==(const Vector3& lhs, const Vector3& rhs)
	{
		if (trunc(1000. * lhs.X) == trunc(1000. * rhs.X) &&
			trunc(1000. * lhs.Y) == trunc(1000. * rhs.Y) &&
			trunc(1000. * lhs.Z) == trunc(1000. * rhs.Z))
		{
			return true;
		}
		return false;
	}

	bool operator!=(const Vector3& lhs, const Vector3& rhs)
	{
		return !(lhs == rhs);
	}

	void Advance()
	{
		if (!(NexusLink && MumbleLink && MumbleIdentity)) { return; }

		unsigned long long timestampNow = Time::GetTimestampMillis();

		/* only when the position is updated */
		if (MumbleLink->AvatarPosition != LastPosition)
		{
			
			float deltaY = MumbleLink->AvatarPosition.Y - LastPosition.Y;

			LastPosition = MumbleLink->AvatarPosition;

			IsFalling = false;
			IsGliding = false;
			IsAscending = false;

			if (MumbleLink->AvatarPosition.Y > 0)
			{
				double truncDeltaY = trunc(1000. * deltaY);
				if (truncDeltaY == trunc(1000. * -1.0403)) /* regular fall */
				{
					IsFalling = true;
				}
				if (truncDeltaY == trunc(1000. * -1.0431)) /* bond of faith fall */
				{
					IsFalling = true;
				}
				else if (truncDeltaY == trunc(1000. * -0.1145)) /* gliding */
				{
					IsGliding = true;
				}
				else if (deltaY < -0.35f && deltaY < LastDeltaY) /* falling acceleration */
				{
					IsFalling = true;
				}
				else if (deltaY > 0.25f)
				{
					IsAscending = true;
				}
			}

			LastDeltaY = deltaY;
		}
		
		if (timestampNow - LastJumpTimestamp <= 700)
		{
			IsJumping = true;
		}
		else
		{
			LastJumpTimestamp = -1;
			IsJumping = false;
		}

		CurrentState.IsCombat         = MumbleLink->Context.IsInCombat                                                              ? EObserveState::True : EObserveState::False;
		CurrentState.IsMounted        = MumbleLink->Context.MountIndex != Mumble::EMountIndex::None                                 ? EObserveState::True : EObserveState::False;
		CurrentState.IsCommander      = MumbleIdentity->IsCommander                                                                 ? EObserveState::True : EObserveState::False;
		CurrentState.IsCompetitive    = MumbleLink->Context.IsCompetitive                                                           ? EObserveState::True : EObserveState::False;
		CurrentState.IsMapOpen        = MumbleLink->Context.IsMapOpen                                                               ? EObserveState::True : EObserveState::False;
		CurrentState.IsTextboxActive  = MumbleLink->Context.IsTextboxFocused                                                        ? EObserveState::True : EObserveState::False;
		CurrentState.IsInstance       = MumbleLink->Context.InstanceID || MumbleLink->Context.MapType == Mumble::EMapType::Instance ? EObserveState::True : EObserveState::False;

		/* derived game states */
		CurrentState.IsGameplay       = NexusLink->IsGameplay                                                                       ? EObserveState::True : EObserveState::False;

		/* derived positional states */
		CurrentState.IsUnderwater     = MumbleLink->AvatarPosition.Y < -1.2f                                                        ? EObserveState::True : EObserveState::False;
		CurrentState.IsOnWaterSurface = (MumbleLink->AvatarPosition.Y >= -1.2f && MumbleLink->AvatarPosition.Y < 0.0f) ||
		                                (MumbleLink->Context.MountIndex == Mumble::EMountIndex::Skimmer &&
		                                 MumbleLink->AvatarPosition.Y >= 0.0f && MumbleLink->AvatarPosition.Y < 1.40f)              ? EObserveState::True : EObserveState::False;
		CurrentState.IsAirborne       = IsFalling || IsGliding || IsAscending || IsJumping                                          ? EObserveState::True : EObserveState::False;
	}

	bool IsMatch(Conditions* aConditions)
	{
		int isMatch = 0;

		isMatch += aConditions->IsCombat         == EObserveState::None ? true : aConditions->IsCombat         == CurrentState.IsCombat;
		isMatch += aConditions->IsMounted        == EObserveState::None ? true : aConditions->IsMounted        == CurrentState.IsMounted;
		isMatch += aConditions->IsCommander      == EObserveState::None ? true : aConditions->IsCommander      == CurrentState.IsCommander;
		isMatch += aConditions->IsCompetitive    == EObserveState::None ? true : aConditions->IsCompetitive    == CurrentState.IsCompetitive;
		isMatch += aConditions->IsMapOpen        == EObserveState::None ? true : aConditions->IsMapOpen        == CurrentState.IsMapOpen;
		isMatch += aConditions->IsTextboxActive  == EObserveState::None ? true : aConditions->IsTextboxActive  == CurrentState.IsTextboxActive;
		isMatch += aConditions->IsInstance       == EObserveState::None ? true : aConditions->IsInstance       == CurrentState.IsInstance;

		/* derived game states */
		isMatch += aConditions->IsGameplay       == EObserveState::None ? true : aConditions->IsGameplay       == CurrentState.IsGameplay;

		/* derived positional states */
		isMatch += aConditions->IsUnderwater     == EObserveState::None ? true : aConditions->IsUnderwater     == CurrentState.IsUnderwater;
		isMatch += aConditions->IsOnWaterSurface == EObserveState::None ? true : aConditions->IsOnWaterSurface == CurrentState.IsOnWaterSurface;
		isMatch += aConditions->IsAirborne       == EObserveState::None ? true : aConditions->IsAirborne       == CurrentState.IsAirborne;

		return isMatch == 11; /* 11 conditions */
	}

	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!(NexusLink && MumbleLink && MumbleIdentity)) { return uMsg; }

		if (uMsg == WM_KEYDOWN && wParam == VK_SPACE && NexusLink->IsGameplay && MumbleLink->AvatarPosition.Y > 0)
		{
			unsigned long long timestampNow = Time::GetTimestampMillis();

			if (timestampNow - LastJumpTimestamp > 700 || LastJumpTimestamp == -1)
			{
				LastJumpTimestamp = Time::GetTimestampMillis();
			}
		}

		return uMsg;
	}

	std::string StateToString(EObserveState aState)
	{
		std::string state;
		switch (aState)
		{
			case EObserveState::None:
				state = "WTF?";
				break;
			case EObserveState::False:
				state = "false";
				break;
			case EObserveState::True:
				state = "true";
				break;
		}

		return state;
	}

	void RenderDebug()
	{
		if (ImGui::CollapsingHeader("Debug##RadialMenus"))
		{
			ImGui::Text("DeltaY: %f", LastDeltaY);

			ImGui::Separator();

			ImGui::Text("Falling: %s", IsFalling ? "true" : "false");
			ImGui::Text("Gliding: %s", IsGliding ? "true" : "false");
			ImGui::Text("Ascending: %s", IsAscending ? "true" : "false");
			ImGui::Text("Jumping: %s", IsJumping ? "true" : "false");

			ImGui::Separator();

			ImGui::Text("IsCombat: %s", StateToString(CurrentState.IsCombat).c_str());
			ImGui::Text("IsMounted: %s", StateToString(CurrentState.IsMounted).c_str());
			ImGui::Text("IsCommander: %s", StateToString(CurrentState.IsCommander).c_str());
			ImGui::Text("IsCompetitive: %s", StateToString(CurrentState.IsCompetitive).c_str());
			ImGui::Text("IsMapOpen: %s", StateToString(CurrentState.IsMapOpen).c_str());
			ImGui::Text("IsTextboxActive: %s", StateToString(CurrentState.IsTextboxActive).c_str());
			ImGui::Text("IsInstance: %s", StateToString(CurrentState.IsInstance).c_str());

			/* derived game states */
			ImGui::Text("IsGameplay: %s", StateToString(CurrentState.IsGameplay).c_str());

			/* derived positional states */
			ImGui::Text("IsUnderwater: %s", StateToString(CurrentState.IsUnderwater).c_str());
			ImGui::Text("IsOnWaterSurface: %s", StateToString(CurrentState.IsOnWaterSurface).c_str());
			ImGui::Text("IsAirborne: %s", StateToString(CurrentState.IsAirborne).c_str());
		}
	}
}

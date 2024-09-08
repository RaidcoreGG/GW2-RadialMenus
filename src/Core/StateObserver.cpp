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

#define JUMP_DURATION_MS 800
#define DISMOUNT_DURATION_MS 700

namespace StateObserver
{
	static Conditions CurrentState;

	static unsigned long long LastJumpTimestamp;
	static unsigned long long LastDismountTimestamp;
	
	static float LastDeltaY;
	static Vector3 LastPosition;
	static int LastMap;

	static bool IsFalling = false; // const deltaY : -1.0403
	static bool IsGliding = false; // const deltaY : -0.1145
	static bool IsAscending = false; // deltaY > 2.5
	static bool IsJumping = false;
	static bool IsDismounting = false;
	static bool WasMounted = false;

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
		if (!(NexusLink && MumbleLink && MumbleIdentity)) { LastMap = 0; return; }

		unsigned long long timestampNow = Time::GetTimestampMillis();

		/* only when the position is updated */
		if (MumbleLink->AvatarPosition != LastPosition)
		{
			float deltaY = MumbleLink->AvatarPosition.Y - LastPosition.Y;
			if (MumbleLink->Context.MapID != LastMap)
			{
				deltaY = 0;
			}

			LastPosition = MumbleLink->AvatarPosition;
			LastMap = MumbleLink->Context.MapID;

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
				if (MumbleLink->Context.MountIndex == Mumble::EMountIndex::Griffon)
				{
					if (truncDeltaY <= trunc(100. * -0.152) && truncDeltaY >= trunc(1000. * -0.4332)) /* griffon gliding */
					{
						IsGliding = true;
					}
					else if (truncDeltaY == trunc(1000. * -0.8746)) /* griffon falling */
					{
						IsFalling = true;
					}
					else if (truncDeltaY <= trunc(100. * -1.254) && truncDeltaY >= trunc(1000. * -1.9131)) /* griffon dive-superdive */
					{
						IsGliding = true;
					}
				}
				else if (MumbleLink->Context.MountIndex == Mumble::EMountIndex::Skyscale && truncDeltaY == trunc(1000. * -0.6604)) /* skyscale descend */
				{
					IsFalling = true;
				}
			}

			LastDeltaY = deltaY;
		}
		
		if (timestampNow - LastJumpTimestamp <= JUMP_DURATION_MS)
		{
			IsJumping = true;
		}
		else
		{
			LastJumpTimestamp = -1;
			IsJumping = false;
		}

		/* (ignore if underwater) && if previous frame we were mounted, but not anymore */
		if (WasMounted && MumbleLink->Context.MountIndex == Mumble::EMountIndex::None)
		{
			LastDismountTimestamp = timestampNow;
		}

		if (timestampNow - LastDismountTimestamp <= DISMOUNT_DURATION_MS)
		{
			IsDismounting = true;
		}
		else
		{
			LastDismountTimestamp = -1;
			IsDismounting = false;
		}

		WasMounted = MumbleLink->Context.MountIndex != Mumble::EMountIndex::None;

		CurrentState.IsCombat         = MumbleLink->Context.IsInCombat                                                              ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsMounted        = MumbleLink->Context.MountIndex != Mumble::EMountIndex::None                                 ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsCommander      = MumbleIdentity->IsCommander                                                                 ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsCompetitive    = MumbleLink->Context.IsCompetitive                                                           ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsMapOpen        = MumbleLink->Context.IsMapOpen                                                               ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsTextboxActive  = MumbleLink->Context.IsTextboxFocused                                                        ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsInstance       = MumbleLink->Context.InstanceID || MumbleLink->Context.MapType == Mumble::EMapType::Instance ? EObserveBoolean::True : EObserveBoolean::False;

		/* derived game states */
		CurrentState.IsGameplay       = NexusLink->IsGameplay                                                                       ? EObserveBoolean::True : EObserveBoolean::False;

		/* derived positional states */
		CurrentState.IsUnderwater     = MumbleLink->AvatarPosition.Y < -1.2f                                                        ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsOnWaterSurface = (MumbleLink->AvatarPosition.Y >= -1.2f && MumbleLink->AvatarPosition.Y <= -1.0f)/* ||
		                                (MumbleLink->Context.MountIndex == Mumble::EMountIndex::Skimmer &&
		                                MumbleLink->AvatarPosition.Y >= 0.0f && MumbleLink->AvatarPosition.Y < 1.40f)*/             ? EObserveBoolean::True : EObserveBoolean::False;
		CurrentState.IsAirborne       = (IsFalling || IsGliding || IsAscending || IsJumping || IsDismounting) &&
		                                CurrentState.IsUnderwater == EObserveBoolean::False                                           ? EObserveBoolean::True : EObserveBoolean::False;
	}

	bool IsMatch(Conditions* aConditions)
	{
		int isMatch = 0;

		isMatch += aConditions->IsCombat         == EObserveBoolean::None ? true : aConditions->IsCombat         == CurrentState.IsCombat;
		isMatch += aConditions->IsMounted        == EObserveBoolean::None ? true : aConditions->IsMounted        == CurrentState.IsMounted;
		isMatch += aConditions->IsCommander      == EObserveBoolean::None ? true : aConditions->IsCommander      == CurrentState.IsCommander;
		isMatch += aConditions->IsCompetitive    == EObserveBoolean::None ? true : aConditions->IsCompetitive    == CurrentState.IsCompetitive;
		isMatch += aConditions->IsMapOpen        == EObserveBoolean::None ? true : aConditions->IsMapOpen        == CurrentState.IsMapOpen;
		isMatch += aConditions->IsTextboxActive  == EObserveBoolean::None ? true : aConditions->IsTextboxActive  == CurrentState.IsTextboxActive;
		isMatch += aConditions->IsInstance       == EObserveBoolean::None ? true : aConditions->IsInstance       == CurrentState.IsInstance;

		/* derived game states */
		isMatch += aConditions->IsGameplay       == EObserveBoolean::None ? true : aConditions->IsGameplay       == CurrentState.IsGameplay;

		/* derived positional states */
		isMatch += aConditions->IsUnderwater     == EObserveBoolean::None ? true : aConditions->IsUnderwater     == CurrentState.IsUnderwater;
		isMatch += aConditions->IsOnWaterSurface == EObserveBoolean::None ? true : aConditions->IsOnWaterSurface == CurrentState.IsOnWaterSurface;
		isMatch += aConditions->IsAirborne       == EObserveBoolean::None ? true : aConditions->IsAirborne       == CurrentState.IsAirborne;

		return isMatch == 11; /* 11 conditions */
	}

	UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!(NexusLink && MumbleLink && MumbleIdentity)) { return uMsg; }

		if (uMsg == WM_KEYDOWN && wParam == VK_SPACE && NexusLink->IsGameplay && MumbleLink->AvatarPosition.Y > 0)
		{
			unsigned long long timestampNow = Time::GetTimestampMillis();

			if (timestampNow - LastJumpTimestamp > JUMP_DURATION_MS || LastJumpTimestamp == -1)
			{
				LastJumpTimestamp = Time::GetTimestampMillis();
			}
		}

		return uMsg;
	}

	std::string StateToString(EObserveBoolean aState)
	{
		std::string state;
		switch (aState)
		{
			case EObserveBoolean::None:
				state = "WTF?";
				break;
			case EObserveBoolean::False:
				state = "false";
				break;
			case EObserveBoolean::True:
				state = "true";
				break;
		}

		return state;
	}

	void RenderDebug()
	{
		ImGui::Text("DeltaY: %f", LastDeltaY);

		ImGui::Separator();

		ImGui::Text("Falling: %s", IsFalling ? "true" : "false");
		ImGui::Text("Gliding: %s", IsGliding ? "true" : "false");
		ImGui::Text("Ascending: %s", IsAscending ? "true" : "false");
		ImGui::Text("Jumping: %s", IsJumping ? "true" : "false");
		ImGui::Text("Dismounting: %s", IsDismounting ? "true" : "false");

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

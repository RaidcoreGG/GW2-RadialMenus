#ifndef IMGUI_EXTENSIONS_H
#define IMGUI_EXTENSIONS_H

#include <cmath>

#include "imgui/imgui.h"

namespace ImGui
{
	///----------------------------------------------------------------------------------------------------
	/// Angle:
	/// 	Returns the angle between two ImVec2 points, between 0 and 360 degrees. 0 is up.
	///----------------------------------------------------------------------------------------------------
	float Angle(ImVec2& aPoint1, ImVec2& aPoint2);

	///----------------------------------------------------------------------------------------------------
	/// Distance:
	/// 	Returns the distance between two ImVec2 points.
	///----------------------------------------------------------------------------------------------------
	float Distance(ImVec2& aPoint1, ImVec2& aPoint2);

	///----------------------------------------------------------------------------------------------------
	/// Rotate:
	/// 	Rotates a point.
	///----------------------------------------------------------------------------------------------------
	ImVec2 Rotate(const ImVec2& aPoint, float aCosA, float aSinA);

	///----------------------------------------------------------------------------------------------------
	/// ImageRotated:
	/// 	Draws an image rotated by a given angle.
	///----------------------------------------------------------------------------------------------------
	void ImageRotated(ImTextureID aTextureIdentifier, ImVec2 aOrigin, ImVec2 aSize, float aAngle, ImColor aColor = ImColor(255, 255, 255, 255));
	
	///----------------------------------------------------------------------------------------------------
	/// ColorEdit4U32:
	/// 	ColorEdit widget that takes a 32bit integer as its input.
	///----------------------------------------------------------------------------------------------------
	bool ColorEdit4U32(const char* label, ImU32* color, ImGuiColorEditFlags flags = 0);

	///----------------------------------------------------------------------------------------------------
	/// ArrowButtonCondDisabled:
	/// 	Arrow button, that is conditionally disabled.
	///----------------------------------------------------------------------------------------------------
	bool ArrowButtonCondDisabled(const char* aName, ImGuiDir aDir, bool aDisabled);

	///----------------------------------------------------------------------------------------------------
	/// CrossButton:
	/// 	Cross button.
	///----------------------------------------------------------------------------------------------------
	bool CrossButton(const char* aName);
}

#endif

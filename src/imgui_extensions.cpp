#include "imgui_extensions.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui/imgui_internal.h"

namespace ImGui
{
	float Angle(ImVec2& aPoint1, ImVec2& aPoint2)
	{
		float deltaX = aPoint2.x - aPoint1.x;
		float deltaY = aPoint2.y - aPoint1.y;

		float angleRad = atan2(deltaY, deltaX);

		float angleDeg = angleRad * 180.0f / 3.14159f;

		// correct angle to have up = 0
		angleDeg += 90.0f;

		// correct angle to be between 0 and 360
		if (angleDeg < 0)
		{
			angleDeg += 360.0f;
		}

		return angleDeg;
	}

	float Distance(ImVec2& aPoint1, ImVec2& aPoint2)
	{
		return sqrt(powf(aPoint2.x - aPoint1.x, 2) + powf(aPoint2.y - aPoint1.y, 2));
	}

	ImVec2 Rotate(const ImVec2& aPoint, float aCosA, float aSinA)
	{
		return ImVec2(aPoint.x * aCosA - aPoint.y * aSinA, aPoint.x * aSinA + aPoint.y * aCosA);
	}

	void ImageRotated(ImTextureID aTextureIdentifier, ImVec2 aOrigin, ImVec2 aSize, float aAngle, ImColor aColor)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// convert degrees to radians
		aAngle *= 3.14159f / 180.0f;

		float cos_a = cosf(aAngle);
		float sin_a = sinf(aAngle);
		ImVec2 pos[4] =
		{
			aOrigin + Rotate(ImVec2(-aSize.x * 0.5f, -aSize.y * 0.5f), cos_a, sin_a),
			aOrigin + Rotate(ImVec2(+aSize.x * 0.5f, -aSize.y * 0.5f), cos_a, sin_a),
			aOrigin + Rotate(ImVec2(+aSize.x * 0.5f, +aSize.y * 0.5f), cos_a, sin_a),
			aOrigin + Rotate(ImVec2(-aSize.x * 0.5f, +aSize.y * 0.5f), cos_a, sin_a)
		};
		ImVec2 uvs[4] =
		{
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			ImVec2(0.0f, 1.0f)
		};

		draw_list->AddImageQuad(aTextureIdentifier, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], aColor);
	}

	bool ColorEdit4U32(const char* label, ImU32* color, ImGuiColorEditFlags flags) {
		float col[4];
		col[0] = (float)((*color >> 0) & 0xFF) / 255.0f;
		col[1] = (float)((*color >> 8) & 0xFF) / 255.0f;
		col[2] = (float)((*color >> 16) & 0xFF) / 255.0f;
		col[3] = (float)((*color >> 24) & 0xFF) / 255.0f;

		bool result = ColorEdit4(label, col, flags);

		*color = ((ImU32)(col[0] * 255.0f)) |
			((ImU32)(col[1] * 255.0f) << 8) |
			((ImU32)(col[2] * 255.0f) << 16) |
			((ImU32)(col[3] * 255.0f) << 24);

		return result;
	}
}

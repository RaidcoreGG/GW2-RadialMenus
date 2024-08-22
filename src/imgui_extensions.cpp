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

	bool ColorEdit4U32(const char* label, ImU32* color, ImGuiColorEditFlags flags)
	{
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

	bool ArrowButtonCondDisabled(const char* aName, ImGuiDir aDir, bool aDisabled)
	{
		if (aDisabled)
		{
			PushItemFlag(ImGuiItemFlags_Disabled, true);
			PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ArrowButton(aName, aDir);
			PopItemFlag();
			PopStyleVar();
			return false;
		}
		
		return ImGui::ArrowButton(aName, aDir);
	}

	bool CrossButton(const char* aName)
	{
		float sz = GetFrameHeight();
		ImVec2 size = ImVec2(sz, sz);

		ImGuiButtonFlags flags = ImGuiButtonFlags_None;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(aName);
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
		const float default_size = GetFrameHeight();
		ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
		if (!ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		const ImU32 text_col = GetColorU32(ImGuiCol_Text);
		RenderNavHighlight(bb, id);
		RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
		
		ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
		ImVec2 center = bb.GetCenter();

		float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
		ImU32 cross_col = GetColorU32(ImGuiCol_Text);
		center -= ImVec2(0.5f, 0.5f);
		window->DrawList->AddLine(center + ImVec2(+cross_extent, +cross_extent), center + ImVec2(-cross_extent, -cross_extent), cross_col, 1.0f);
		window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent), center + ImVec2(-cross_extent, +cross_extent), cross_col, 1.0f);

		return pressed;
	}
}

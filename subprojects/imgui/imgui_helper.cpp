#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_helper.h"
#include "imgui_internal.h"
#include "icons/icons.h"
#include "imgui.h"

#include <time.h>

#include <iostream>

namespace ImGui::Helper
{

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool DragFPropertyXYZ(const char* label,
					  float* value,
					  float step,
					  float min,
					  float max,
					  const char* format,
					  const std::string& help_message,
					  int num)
{
	bool is_value_changed = false;
	ImGui::PushID(label);
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, 90.0f);

	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(num, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});

	const char* button_labels[] = {"X", "Y", "Z", "W"};
	const char* drag_labels[] = {"##x", "##y", "##z", "##w"};

	for (int i = 0; i < num; i++)
	{
		float button_color[4] = {0.0f, 0.0f, 0.0f, 0.5f};
		float framebg_color[4] = {0.4f, 0.4f, 0.4f, 0.1f};
		float framebg_hovered_color[4] = {0.4f, 0.4f, 0.4f, 0.7f};
		float framebg_active_color[4] = {0.5f, 0.5f, 0.5f, 1.0f};

		if (i <= 3)
			button_color[i] = framebg_hovered_color[i] = framebg_active_color[i] = framebg_color[i] = 0.9f;

		ImGui::PushStyleColor(ImGuiCol_Button,
							  ImVec4(button_color[0], button_color[1], button_color[2], button_color[3]));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
							  ImVec4(button_color[0], button_color[1], button_color[2], button_color[3]));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
							  ImVec4(button_color[0], button_color[1], button_color[2], button_color[3]));

		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(framebg_active_color[0], framebg_active_color[1],
															 framebg_active_color[2], framebg_active_color[3]));

		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(framebg_hovered_color[0], framebg_hovered_color[1],
															  framebg_hovered_color[2], framebg_hovered_color[3]));

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
		ImGui::Button(button_labels[i]);
		ImGui::PopStyleColor(1);

		ImGui::SameLine();
		is_value_changed |= ImGui::DragFloat(drag_labels[i], &value[i], step, min, max, format);
		ImGui::PopItemWidth();
		if (i + 1 != num)
		{
			ImGui::SameLine();
		}
		ImGui::PopStyleColor(5);
	}

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();

	return is_value_changed;
}

bool DragPropertyXYZ(const char* label, float* value, float step, float min, float max, const std::string& help_message)
{
	return DragFPropertyXYZ(label, &value[0], step, min, max, "%.3f", help_message);
}
inline void BeginDragProperty(const char* label, const ImVec2& btn_size)
{
	ImGui::PushID(label);

	ImGuiStyle& style = ImGui::GetStyle();

	auto button_color = style.Colors[ImGuiCol_ButtonHovered];
	button_color.w *= 0.7f;
	auto framebg_color = style.Colors[ImGuiCol_Button];
	auto framebg_hovered_color = style.Colors[ImGuiCol_ButtonHovered];
	auto framebg_active_color = style.Colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, framebg_color);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, framebg_active_color);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, framebg_hovered_color);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
	ImGui::Button(label, btn_size);
	ImGui::SameLine();
}
void BeginDragPropertyWithoutColor(const char* label, const ImVec2& btn_size)
{
	ImGui::PushID(label);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
}
inline void EndDragProperty()
{
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(6);
	ImGui::PopID();
}
void EndDragPropertyWithoutColor()
{
	ImGui::PopStyleVar();
	ImGui::PopID();
}
bool DragFloatProperty(const char* label,
					   float& value,
					   float step,
					   float min,
					   float max,
					   const ImVec2& btn_size,
					   const char* format,
					   const std::string& help_message)
{
	bool is_value_changed = false;

	BeginDragProperty(label, btn_size);
	is_value_changed |= ImGui::DragFloat("##drag", &value, step, min, max, format);
	EndDragProperty();

	return is_value_changed;
}
bool DragFloatPropertyWithColor(const char* label,
								float& value,
								const ImVec4& a_btn_color,
								const ImVec4& a_frame_color,
								float step,
								float min,
								float max,
								const ImVec2& btn_size,
								const char* format,
								const std::string& help_message)
{
	bool is_value_changed = false;
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 btn_color = a_btn_color.w == -1 ? style.Colors[ImGuiCol_ButtonHovered] : a_btn_color;

	auto framebg_color = style.Colors[ImGuiCol_Button];
	auto framebg_hovered_color = style.Colors[ImGuiCol_ButtonHovered];
	auto framebg_active_color = style.Colors[ImGuiCol_ButtonActive];
	if (a_frame_color.w != -1)
	{
		framebg_color = a_frame_color;
		framebg_active_color = framebg_color;
		framebg_hovered_color = framebg_color;
		framebg_color.w *= 0.5f;
		framebg_hovered_color.w *= 0.9;
		framebg_active_color.w *= 0.7;
	}

	BeginDragPropertyWithoutColor(label, btn_size);
	{
		auto btn_setter = ButtonColorSetter(btn_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, framebg_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, framebg_active_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, framebg_hovered_color);
		ImGui::Button(label, btn_size);
		ImGui::SameLine();
		is_value_changed |= ImGui::DragFloat("##drag", &value, step, min, max, format);
		ImGui::PopStyleColor(3);
	}
	EndDragPropertyWithoutColor();

	return is_value_changed;
}
bool DragIntProperty(const char* label,
					 int& value,
					 float step,
					 int min,
					 int max,
					 const ImVec2& btn_size,
					 const std::string& help_message)
{
	bool is_value_changed = false;

	BeginDragProperty(label, btn_size);
	is_value_changed |= ImGui::DragInt("##drag", &value, step, min, max);
	EndDragProperty();

	return is_value_changed;
}

bool ToggleButton(const char* label, bool* v, const ImVec2& size)
{
	bool bIsClicked = false;
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGuiContext& g = *GImGui;
	ImVec4 pale_hovered = colors[ImGuiCol_ButtonHovered];
	pale_hovered.w *= 0.5f;
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImGuiStyle& style = g.Style;
	float height =
		(size.y != 0.0f) ? size.y : label_size.y + style.FramePadding.y * 2.0f;				 // ImGui::GetFrameHeight();
	float width = (size.x != 0.0f) ? size.x : label_size.x + style.FramePadding.x * 2.0f;	 // height * 1.55f;
	float rounding = style.FrameRounding;

	ImGui::InvisibleButton(label, ImVec2(width, height));
	if (ImGui::IsItemClicked())
	{
		*v = !*v;
		bIsClicked = true;
	}
	ImGuiContext& gg = *GImGui;
	float ANIM_SPEED = 0.085f;
	if (gg.LastActiveId == gg.CurrentWindow->GetID(label))	  // && g.LastActiveIdTimer < ANIM_SPEED)
		float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
	if (ImGui::IsItemHovered())
	{
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
								 ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonHovered] : pale_hovered), rounding);
	}
	else
	{
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
								 ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]),
								 rounding);
	}

	const ImRect bb(p, {width + p.x, height + p.y});
	if (g.LogEnabled)
		ImGui::LogSetNextTextDecoration("[", "]");
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max + style.FramePadding, label, NULL, &label_size,
							 style.ButtonTextAlign, &bb);

	return bIsClicked;
}
bool ToggleButtonImage(const char* label, bool bIsToggle, const ImVec2& size)
{
	bool bIsClicked = false;
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGuiContext& g = *GImGui;
	ImVec4 pale_hovered = colors[ImGuiCol_ButtonHovered];
	pale_hovered.w *= 0.5f;
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImGuiStyle& style = g.Style;
	float height =
		(size.y != 0.0f) ? size.y : label_size.y + style.FramePadding.y * 2.0f;				 // ImGui::GetFrameHeight();
	float width = (size.x != 0.0f) ? size.x : label_size.x + style.FramePadding.x * 2.0f;	 // height * 1.55f;
	float rounding = style.FrameRounding;

	ImGui::InvisibleButton(label, ImVec2(width, height));
	if (ImGui::IsItemClicked())
	{
		bIsClicked = true;
	}
	ImGuiContext& gg = *GImGui;
	float ANIM_SPEED = 0.085f;
	if (gg.LastActiveId == gg.CurrentWindow->GetID(label))	  // && g.LastActiveIdTimer < ANIM_SPEED)
		float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
	if (ImGui::IsItemHovered())
	{
		draw_list->AddRectFilled(
			p, ImVec2(p.x + width, p.y + height),
			ImGui::GetColorU32(bIsToggle ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_ButtonHovered]), rounding);
	}
	else
	{
		draw_list->AddRectFilled(
			p, ImVec2(p.x + width, p.y + height),
			ImGui::GetColorU32(bIsToggle ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]), rounding);
	}

	const ImRect bb(p, {width + p.x, height + p.y});
	if (g.LogEnabled)
		ImGui::LogSetNextTextDecoration("[", "]");
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max + style.FramePadding, label, NULL, &label_size,
							 style.ButtonTextAlign, &bb);

	return bIsClicked;
}
ButtonColorSetter::ButtonColorSetter(ImVec4 btn_color)
{
	ImGui::PushStyleColor(ImGuiCol_Button, btn_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btn_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, btn_color);
}
ButtonColorSetter::~ButtonColorSetter()
{
	ImGui::PopStyleColor(3);
}
}	 // namespace ImGui::Helper

// ImGui Bezier widget. @r-lyeh, public domain
// v1.03: improve grabbing, confine grabbers to area option, adaptive size, presets, preview.
// v1.02: add BezierValue(); comments; usage
// v1.01: out-of-bounds coord snapping; custom border width; spacing; cosmetics
// v1.00: initial version
//
// [ref] http://robnapier.net/faster-bezier
// [ref] http://easings.net/es#easeInSine
//
// Usage:
// {  static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f };
//    ImGui::Bezier( "easeOutSine", v );       // draw
//    float y = ImGui::BezierValue( 0.5f, v ); // x delta in [0..1] range
// }
namespace ImGui
{

float BezierValue(float dt01, float P[4])
{
	enum
	{
		STEPS = 256
	};
	ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
	ImVec2 results[STEPS + 1];
	bezier_table<STEPS>(Q, results);
	return results[(int) ((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}

int Bezier(const char* label, float P[5])
{
	// visuals
	enum
	{
		SMOOTHNESS = 64
	};	  // curve smoothness: the higher number of segments, the smoother curve
	enum
	{
		CURVE_WIDTH = 4
	};	  // main curved line width
	enum
	{
		LINE_WIDTH = 1
	};	  // handlers: small lines width
	enum
	{
		GRAB_RADIUS = 8
	};	  // handlers: circle radius
	enum
	{
		GRAB_BORDER = 2
	};	  // handlers: circle border width
	enum
	{
		AREA_CONSTRAINED = true
	};	  // should grabbers be constrained to grid area?
	enum
	{
		AREA_WIDTH = 128
	};	  // area width in pixels. 0 for adaptive size (will use max avail width)

	// curve presets
	static struct
	{
		const char* name;
		float points[4];
	} presets[] = {
		{"Linear", 0.000f, 0.000f, 1.000f, 1.000f},

		{"In Sine", 0.470f, 0.000f, 0.745f, 0.715f},	 {"In Quad", 0.550f, 0.085f, 0.680f, 0.530f},
		{"In Cubic", 0.550f, 0.055f, 0.675f, 0.190f},	 {"In Quart", 0.895f, 0.030f, 0.685f, 0.220f},
		{"In Quint", 0.755f, 0.050f, 0.855f, 0.060f},	 {"In Expo", 0.950f, 0.050f, 0.795f, 0.035f},
		{"In Circ", 0.600f, 0.040f, 0.980f, 0.335f},	 {"In Back", 0.600f, -0.28f, 0.735f, 0.045f},

		{"Out Sine", 0.390f, 0.575f, 0.565f, 1.000f},	 {"Out Quad", 0.250f, 0.460f, 0.450f, 0.940f},
		{"Out Cubic", 0.215f, 0.610f, 0.355f, 1.000f},	 {"Out Quart", 0.165f, 0.840f, 0.440f, 1.000f},
		{"Out Quint", 0.230f, 1.000f, 0.320f, 1.000f},	 {"Out Expo", 0.190f, 1.000f, 0.220f, 1.000f},
		{"Out Circ", 0.075f, 0.820f, 0.165f, 1.000f},	 {"Out Back", 0.175f, 0.885f, 0.320f, 1.275f},

		{"InOut Sine", 0.445f, 0.050f, 0.550f, 0.950f},	 {"InOut Quad", 0.455f, 0.030f, 0.515f, 0.955f},
		{"InOut Cubic", 0.645f, 0.045f, 0.355f, 1.000f}, {"InOut Quart", 0.770f, 0.000f, 0.175f, 1.000f},
		{"InOut Quint", 0.860f, 0.000f, 0.070f, 1.000f}, {"InOut Expo", 1.000f, 0.000f, 0.000f, 1.000f},
		{"InOut Circ", 0.785f, 0.135f, 0.150f, 0.860f},	 {"InOut Back", 0.680f, -0.55f, 0.265f, 1.550f},

		// easeInElastic: not a bezier
		// easeOutElastic: not a bezier
		// easeInOutElastic: not a bezier
		// easeInBounce: not a bezier
		// easeOutBounce: not a bezier
		// easeInOutBounce: not a bezier
	};

	// preset selector

	bool reload = 0;
	ImGui::PushID(label);
	if (ImGui::ArrowButton("##lt", ImGuiDir_Left))
	{	 // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##lt"), ImGuiDir_Left, ImVec2(0, 0), 0)
		if (--P[4] >= 0)
			reload = 1;
		else
			++P[4];
	}
	ImGui::SameLine();

	if (ImGui::Button("Presets"))
	{
		ImGui::OpenPopup("!Presets");
	}
	if (ImGui::BeginPopup("!Presets"))
	{
		for (int i = 0; i < IM_ARRAYSIZE(presets); ++i)
		{
			if (i == 1 || i == 9 || i == 17)
				ImGui::Separator();
			if (ImGui::MenuItem(presets[i].name, NULL, P[4] == i))
			{
				P[4] = i;
				reload = 1;
			}
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	if (ImGui::ArrowButton("##rt", ImGuiDir_Right))
	{	 // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##rt"), ImGuiDir_Right, ImVec2(0, 0), 0)
		if (++P[4] < IM_ARRAYSIZE(presets))
			reload = 1;
		else
			--P[4];
	}
	ImGui::SameLine();
	ImGui::PopID();

	if (reload)
	{
		memcpy(P, presets[(int) P[4]].points, sizeof(float) * 4);
	}

	// bezier widget

	const ImGuiStyle& Style = GetStyle();
	const ImGuiIO& IO = GetIO();
	ImDrawList* DrawList = GetWindowDrawList();
	ImGuiWindow* Window = GetCurrentWindow();
	if (Window->SkipItems)
		return false;

	// header and spacing
	int changed = SliderFloat4(label, P, 0, 1, "%.3f", 1.0f);
	int hovered = IsItemActive() || IsItemHovered();	// IsItemDragged() ?
	Dummy(ImVec2(0, 3));

	// prepare canvas
	const float avail = GetContentRegionAvail().x;
	const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
	ImVec2 Canvas(dim, dim);

	ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
		return changed;

	const ImGuiID id = Window->GetID(label);
	hovered |= 0 != ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id,
								  GetCurrentContext()->LastItemData.ItemFlags);

	RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

	// background grid
	for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4))
	{
		DrawList->AddLine(ImVec2(bb.Min.x + i, bb.Min.y), ImVec2(bb.Min.x + i, bb.Max.y),
						  GetColorU32(ImGuiCol_TextDisabled));
	}
	for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4))
	{
		DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i), ImVec2(bb.Max.x, bb.Min.y + i),
						  GetColorU32(ImGuiCol_TextDisabled));
	}

	// eval curve
	ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
	ImVec2 results[SMOOTHNESS + 1];
	bezier_table<SMOOTHNESS>(Q, results);

	// control points: 2 lines and 2 circles
	{
		// handle grabbers
		ImVec2 mouse = GetIO().MousePos, pos[2];
		float distance[2];

		for (int i = 0; i < 2; ++i)
		{
			pos[i] = ImVec2(P[i * 2 + 0], 1 - P[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
			distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
		}

		int selected = distance[0] < distance[1] ? 0 : 1;
		if (distance[selected] < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS))
		{
			SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);

			if (/*hovered &&*/ (IsMouseClicked(0) || IsMouseDragging(0)))
			{
				float& px = (P[selected * 2 + 0] += GetIO().MouseDelta.x / Canvas.x);
				float& py = (P[selected * 2 + 1] -= GetIO().MouseDelta.y / Canvas.y);

				if (AREA_CONSTRAINED)
				{
					px = (px < 0 ? 0 : (px > 1 ? 1 : px));
					py = (py < 0 ? 0 : (py > 1 ? 1 : py));
				}

				changed = true;
			}
		}
	}

	// if (hovered || changed) DrawList->PushClipRectFullScreen();

	// draw curve
	{
		ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
		for (int i = 0; i < SMOOTHNESS; ++i)
		{
			ImVec2 p = {results[i + 0].x, 1 - results[i + 0].y};
			ImVec2 q = {results[i + 1].x, 1 - results[i + 1].y};
			ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			DrawList->AddLine(r, s, color, CURVE_WIDTH);
		}
	}

	// draw preview (cycles every 1s)
	static clock_t epoch = clock();
	ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
	for (int i = 0; i < 3; ++i)
	{
		double now = ((clock() - epoch) / (double) CLOCKS_PER_SEC);
		float delta = ((int) (now * 1000) % 1000) / 1000.f;
		delta += i / 3.f;
		if (delta > 1)
			delta -= 1;
		int idx = (int) (delta * SMOOTHNESS);
		float evalx = results[idx].x;	 //
		float evaly = results[idx].y;	 // ImGui::BezierValue( delta, P );
		ImVec2 p0 = ImVec2(evalx, 1 - 0) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p1 = ImVec2(0, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p2 = ImVec2(evalx, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
		DrawList->AddCircleFilled(p0, GRAB_RADIUS / 2, ImColor(white));
		DrawList->AddCircleFilled(p1, GRAB_RADIUS / 2, ImColor(white));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS / 2, ImColor(white));
	}

	// draw lines and grabbers
	float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
	ImVec4 pink(1.00f, 0.00f, 0.75f, luma), cyan(0.00f, 0.75f, 1.00f, luma);
	ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
	ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
	DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
	DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
	DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
	DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
	DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
	DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));

	// if (hovered || changed) DrawList->PopClipRect();

	return changed;
}

void ShowBezierDemo()
{
	{
		static float v[5] = {0.950f, 0.050f, 0.795f, 0.035f};
		Bezier("easeInExpo", v);
	}
}
}	 // namespace ImGui

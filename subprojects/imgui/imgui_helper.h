#ifndef IMGUI_HELPER_H
#define IMGUI_HELPER_H

#include "imgui.h"
#include <string>

namespace ImGui::Helper
{

class ButtonColorSetter
{
public:
	ButtonColorSetter(ImVec4 btn_color);
	~ButtonColorSetter();
};

void HelpMarker(const char* desc);
inline void BeginDragProperty(const char* label, const ImVec2& btn_size = {0.0f, 0.0f});
inline void BeginDragPropertyWithoutColor(const char* label, const ImVec2& btn_size = {0.0f, 0.0f});
inline void EndDragProperty();
inline void EndDragPropertyWithoutColor();

bool DragFPropertyXYZ(const char* label,
					  float* value,
					  float step = 0.01f,
					  float min = -1000.0f,
					  float max = 1000.0f,
					  const char* format = "%.2f",
					  const std::string& help_message = "",
					  int num = 3);
bool DragPropertyXYZ(const char* label,
					 float* value,
					 float step = 0.01f,
					 float min = -1000.0f,
					 float max = 1000.0f,
					 const std::string& help_message = "");
bool DragFloatProperty(const char* label,
					   float& value,
					   float step = 1.0f,
					   float min = -1000.0f,
					   float max = 1000.0f,
					   const ImVec2& btn_size = {0.0f, 0.0f},
					   const char* format = "%.0f",
					   const std::string& help_message = "");

bool DragFloatPropertyWithColor(const char* label,
								float& value,
								const ImVec4& btn_color,
								const ImVec4& frame_color,
								float step = 1.0f,
								float min = -1000.0f,
								float max = 1000.0f,
								const ImVec2& btn_size = {0.0f, 0.0f},
								const char* format = "%.0f",
								const std::string& help_message = "");

bool DragIntProperty(const char* label,
					 int& value,
					 float step = 1,
					 int min = 0,
					 int max = 1000,
					 const ImVec2& btn_size = {0.0f, 0.0f},
					 const std::string& help_message = "");

bool ToggleButton(const char* label, bool* v, const ImVec2& size = {0.0f, 0.0f});
bool ToggleButtonImage(const char* label, bool bIsToggle, const ImVec2& size = {0.0f, 0.0f});
}	 // namespace ImGui::Helper

namespace ImGui
{
template <int steps>
void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1])
{
	static float C[(steps + 1) * 4], *K = 0;
	if (!K)
	{
		K = C;
		for (unsigned step = 0; step <= steps; ++step)
		{
			float t = (float) step / (float) steps;
			C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t);	  // * P0
			C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t;	  // * P1
			C[step * 4 + 2] = 3 * (1 - t) * t * t;			  // * P2
			C[step * 4 + 3] = t * t * t;					  // * P3
		}
	}
	for (unsigned step = 0; step <= steps; ++step)
	{
		ImVec2 point = {
			K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
			K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y};
		results[step] = point;
	}
}

int Bezier(const char* label, float P[5]);
void ShowBezierDemo();
}	 // namespace ImGui

#endif

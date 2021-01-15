#pragma once

#define USE_IMGUI

#ifdef USE_IMGUI
#include <windows.h>

#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../External/ImGui/imgui_impl_dx11.h"
#include "../External/ImGui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

//ImGui::Image((void*)shader_resource_view.Get(), ImVec2(1280, 720));
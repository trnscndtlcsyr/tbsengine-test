#include "ImguiWrapper.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <wtypes.h>


ImguiWrapper::~ImguiWrapper()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImguiWrapper::Initialize(HWND hwnd)
{
	ImGui::CreateContext();
	if(!ImGui_ImplWin32_Init(hwnd)) return false;
	//if (!ImGui_ImplDX11_Init(gfx.getID3D11Device(), gfx.getID3D11DeviceContext())) return false;
	return true;
}

void ImguiWrapper::Frame()
{
	BeginFrame();

	ImGui::Begin("TEST");
	ImGui::Text("TEXT TEST FPS IMGUI: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
	EndFrame();
}

void ImguiWrapper::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImguiWrapper::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}



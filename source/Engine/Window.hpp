#pragma once

#include<memory>
#include<string>
#include<sstream>

#include "Mouse.hpp"
#include "RenderPipeline.hpp"

class Window
{
public:
	Window(UINT width, UINT height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	int MessageProcessing();
	RenderPipeline& gfx();
	HWND getHandle() const { return hWnd; }
	std::pair<UINT, UINT> GetSize() { return { width, height }; }
private:
	void TextPost(HWND hWnd, const POINTS pt, const char* text);
	void InitWindowClass();
	void OnResize(UINT w, UINT h);
private:
	static LRESULT CALLBACK MsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK MsgProcReplace(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Mouse mouse;
private:
	UINT width;
	UINT height;
	static constexpr const wchar_t* class_name = L"DX2D";
	HWND hWnd;
	HINSTANCE hInstance;
	std::unique_ptr<RenderPipeline> pRenderer;
	bool fullScreenState = false;
	RECT windowRect;
	static const UINT windowStyle = WS_OVERLAPPEDWINDOW;
};


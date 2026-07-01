#pragma once
#include<wtypes.h>

class ImguiWrapper {
public:
	ImguiWrapper() = default;
	~ImguiWrapper();
	bool Initialize(HWND hwnd);
	void Frame();
private:
	void BeginFrame();
	void EndFrame();
};
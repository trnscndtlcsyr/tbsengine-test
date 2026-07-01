#pragma once
#include "DX11RenderDevice.hpp"
#include<memory>
#include "Object3D.hpp"
#include "Camera.hpp"
#include "Light.hpp"

class RenderPipeline {
public:
	RenderPipeline(HWND hwnd);
	void Initialize(float width, float height);
	void CreateResources(Mesh& mesh);
	void Render(Object3D& object);
private:
	Camera cam;
	DirectionalLight dirLight;
	RenderHandle rh;
	TextureHandle th;
	std::unique_ptr<DX11RenderDevice> dxhal;
};
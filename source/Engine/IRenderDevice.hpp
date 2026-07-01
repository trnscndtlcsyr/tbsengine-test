#pragma once
#include "RenderDeviceTypes.hpp"

class IRenderDevice {
public:
	virtual ~IRenderDevice() = default;

	virtual void Initialize(float width, float height) = 0;
	virtual void BeginFrame() noexcept = 0;
	virtual void EndFrame(bool vsync) noexcept = 0;
	virtual void Draw(uint32_t vertexCount) noexcept = 0;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) noexcept = 0;
};
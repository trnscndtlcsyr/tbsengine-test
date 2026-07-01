#pragma once

struct BaseHandle {
	uint32_t id;
	bool isValid() const noexcept { return id != 0; };
};

struct RasterizerStateHandle : BaseHandle {};
struct SamplerStateHandle : BaseHandle {};

struct VertexBufferHandle : BaseHandle {};
struct IndexBufferHandle : BaseHandle {};
struct GeometryBufferHandle {
	VertexBufferHandle vbh;
	IndexBufferHandle ibh;
};
struct ConstantBufferHandle : BaseHandle {};
struct TextureHandle : BaseHandle {};
struct ShaderHandle : BaseHandle {};
struct InputLayoutHandle : BaseHandle {};

struct RenderHandle {
	RasterizerStateHandle rsh;
	SamplerStateHandle ssh;
	GeometryBufferHandle gbh;
	ConstantBufferHandle camerabh;
	ConstantBufferHandle objectbh;
	InputLayoutHandle ilh;
	ShaderHandle vsh;
	ShaderHandle psh;
	ShaderHandle shadowvsh;
	InputLayoutHandle shadowilh;
	ConstantBufferHandle lightcbh;
	TextureHandle shadowmapth;
	RasterizerStateHandle shadowrsh;
	SamplerStateHandle shadowssh;
	ConstantBufferHandle lightpixelcbh;
};

enum class ShaderType { Vertex, Pixel, Compute };
#pragma once
#include<DirectXMath.h>

struct Vertex3D {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT3 normal;
};

struct LightBuffer {
	DirectX::XMFLOAT3 lightDir;
	float padding;
};

static uint32_t size_vertex3d = sizeof(Vertex3D);
static uint32_t size_xmmatrix = sizeof(DirectX::XMMATRIX);
static uint32_t size_lightbuffer = sizeof(LightBuffer);

enum class VertexFormat : uint8_t { float2, float3, float4 };

enum class SamplerFilter : uint8_t { Point, Linear };

enum class RasterizerParam {Default, ShadowMap};
enum class SamplerParam { Default, ShadowMap };

struct VertexElement {
	const char* semanticName;
	uint32_t semanticIndex;
	VertexFormat format;
	uint32_t slot;
	uint32_t alignedByteOffset;
};
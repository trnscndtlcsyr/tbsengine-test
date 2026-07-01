#include "Mesh.hpp"
#include "DX11RenderTypes.hpp"
#include<NoiseGenerator/NoiseGenerator.hpp>
#include<vector>

std::vector<Vertex3D> Plane::GenerateVertices(uint32_t width, uint32_t depth)
{
	std::vector<Vertex3D> vertices;
	for (uint32_t z = 0; z < depth; ++z) {
		for (uint32_t x = 0; x < width; ++x) {
			Vertex3D v;
			v.pos.x = static_cast<float>(x);
			v.pos.z = static_cast<float>(z);
			v.pos.y = 0.0f;
			vertices.push_back(v);
		}
	}
	return vertices;
}

std::vector<uint32_t> Plane::GenerateIndices(uint32_t width, uint32_t depth)
{
	std::vector<uint32_t> indices;
	for (uint32_t z = 0; z < depth - 1; ++z) {
		for (uint32_t x = 0; x < width - 1; ++x) {
			uint32_t topLeft = z * width + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (z + 1) * width + x;
			uint32_t bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);

			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
			indices.push_back(topRight);
		}
	}
	return indices;
}

std::vector<Vertex3D> NoisePlane::GenerateVertices(uint32_t width, uint32_t depth)
{	
	std::shared_ptr<NoiseGenerator> noiseGen = std::make_shared<NoiseGenerator>();
	std::vector<float> noiseMap = noiseGen->generateHeights(static_cast<int>(width), static_cast<int>(depth), 0.0f, 0.0f);
	std::vector<Vertex3D> vertices;
	for (uint32_t z = 0; z < depth; ++z) {
		for (uint32_t x = 0; x < width; ++x) {
			Vertex3D v;
			int index = z * width + x;
			float noise = noiseMap.at(index) / 255.0f;
			noise *= 2.0f;
			v.pos.x = static_cast<float>(x);
			v.pos.z = static_cast<float>(z);
			v.pos.y = noise;
			v.col = { 0.9f, 0.9f, 0.9f, 1.0f };
			float hL = (x > 0) ? noiseMap.at(z * width + (x - 1)) / 255.0f : noise;
			float hR = (x < width - 1) ? noiseMap.at(z * width + (x + 1)) / 255.0f : noise;
			float hD = (z > 0) ? noiseMap.at((z - 1) * width + x) / 255.0f : noise;
			float hU = (z < depth - 1) ? noiseMap.at((z + 1) * width + x) / 255.0f : noise;
			v.normal = CalculateNormal(hL, hR, hD, hU, 1.0f);
			vertices.push_back(v);
		}
	}
	return vertices;
}

std::vector<uint32_t> NoisePlane::GenerateIndices(uint32_t width, uint32_t depth)
{
	std::vector<uint32_t> indices;
	for (uint32_t z = 0; z < depth - 1; ++z) {
		for (uint32_t x = 0; x < width - 1; ++x) {
			uint32_t topLeft = z * width + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (z + 1) * width + x;
			uint32_t bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);

			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(bottomRight);
		}
	}
	return indices;
}

DirectX::XMFLOAT3 NoisePlane::CalculateNormal(float heightLeft, float heightRight, float heightDown, float heightUp, float step)
{
	DirectX::XMVECTOR vNormal = DirectX::XMVectorSet(heightLeft - heightRight, 2.0f * step, heightDown - heightUp, 0.0f);
	vNormal = DirectX::XMVector3Normalize(vNormal);
	DirectX::XMFLOAT3 normal;
	DirectX::XMStoreFloat3(&normal, vNormal);
	return normal;
}

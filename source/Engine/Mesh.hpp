#pragma once
#include "DX11RenderTypes.hpp"
#include <vector>
#include <memory>

class Mesh {
public:
	virtual ~Mesh() = default;
	virtual const Vertex3D* VertexData() const noexcept = 0;
	virtual uint32_t VertexSize() const noexcept = 0;
	virtual const uint32_t* IndexData() const noexcept = 0;
	virtual uint32_t IndexSize() const noexcept = 0;
	virtual uint32_t IndexCount() const noexcept = 0;
	virtual uint32_t VertexCount() const noexcept = 0;
	virtual std::vector<Vertex3D>& GetVertices() noexcept = 0;
	virtual std::vector<uint32_t>& GetIndices() noexcept = 0;
private:
	virtual std::vector<Vertex3D> GenerateVertices(uint32_t width, uint32_t depth) = 0;
	virtual std::vector<uint32_t> GenerateIndices(uint32_t width, uint32_t depth) = 0;
};

class Plane : public Mesh {
public:
	Plane() = default;
	Plane(uint32_t width, uint32_t depth)
	{
		vertices = GenerateVertices(width, depth);
		indices = GenerateIndices(width, depth);
	}
	~Plane() = default;

	const Vertex3D* VertexData() const noexcept override { return vertices.data(); }
	uint32_t VertexSize() const noexcept override { return vertices.size() * sizeof(Vertex3D); }
	const uint32_t* IndexData() const noexcept override { return indices.data(); }
	uint32_t IndexSize() const noexcept override { return indices.size() * sizeof(uint32_t); }
	uint32_t IndexCount() const noexcept override { return indices.size(); }
	uint32_t VertexCount() const noexcept override { return vertices.size(); }
	std::vector<Vertex3D>& GetVertices() noexcept override { return vertices; }
	std::vector<uint32_t>& GetIndices() noexcept override { return indices; }
private:
	std::vector<Vertex3D> GenerateVertices(uint32_t width, uint32_t depth) override;
	std::vector<uint32_t> GenerateIndices(uint32_t width, uint32_t depth) override;
private:
	std::vector<Vertex3D> vertices;
	std::vector<uint32_t> indices;
};

class NoisePlane : public Mesh {
public:
	NoisePlane() = default;
	NoisePlane(uint32_t width, uint32_t depth)
	{
		vertices = GenerateVertices(width, depth);
		indices = GenerateIndices(width, depth);
	}
	~NoisePlane() = default;

	const Vertex3D* VertexData() const noexcept override { return vertices.data(); }
	uint32_t VertexSize() const noexcept override { return vertices.size() * sizeof(Vertex3D); }
	const uint32_t* IndexData() const noexcept override { return indices.data(); }
	uint32_t IndexSize() const noexcept override { return indices.size() * sizeof(uint32_t); }
	uint32_t IndexCount() const noexcept override { return indices.size(); }
	uint32_t VertexCount() const noexcept override { return vertices.size(); }
	std::vector<Vertex3D>& GetVertices() noexcept override { return vertices; }
	std::vector<uint32_t>& GetIndices() noexcept override { return indices; }
private:
	std::vector<Vertex3D> GenerateVertices(uint32_t width, uint32_t depth) override;
	std::vector<uint32_t> GenerateIndices(uint32_t width, uint32_t depth) override;
private:
	std::vector<Vertex3D> vertices;
	std::vector<uint32_t> indices;
};

class MeshFactory {
public:
	static std::shared_ptr<Plane> CreatePlane(uint32_t width, uint32_t depth)
	{
		return std::make_shared<Plane>(width, depth);
	}
};
#pragma once
#include <memory>
#include "Mesh.hpp"
#include "Transform3D.hpp"

class Object3D {
public:
	Object3D() = default;
	explicit Object3D(Transform3D transform, std::shared_ptr<Mesh> mesh)
		: transform{ transform }, mesh{ mesh }
	{}
	~Object3D() = default;
	const DirectX::XMMATRIX GetMatrix();
	void SetTransformRotationX(float angle);
	Mesh& GetMesh() { return *mesh.get(); }
protected:
	Transform3D transform;
	std::shared_ptr<Mesh> mesh;
};
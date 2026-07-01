#pragma once
#include <DirectXMath.h>
#include "Object3D.hpp"

enum class ProjectionType {
	Perspective, Otrhographic
};

class Camera {
public:
	Camera() = default;
	~Camera() = default;
public:
	void SetPosition(float x, float y, float z);
	void SetProjection(ProjectionType type, float width, float height);
	DirectX::XMMATRIX GetRotation() const;
	DirectX::XMMATRIX GetViewProjectionMatrix() const;
private:
	Transform3D transform;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};
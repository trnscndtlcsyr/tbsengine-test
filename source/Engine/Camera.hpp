#pragma once
#include <DirectXMath.h>

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

	DirectX::XMMATRIX GetViewProjectionMatrix() const;
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};
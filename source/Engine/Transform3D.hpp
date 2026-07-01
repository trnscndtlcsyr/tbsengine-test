#pragma once
#include<DirectXMath.h>

class Transform3D {
public:
	Transform3D() 
		:
		position {0.0f, 0.0f, 0.0f},
		rotation {0.0f, 0.0f, 0.0f},
		scale {1.0f, 1.0f, 1.0f}
	{}
	~Transform3D() = default;
	void SetPosition(float x, float y, float z) { position = { x, y, z }; }
	void SetRotationX(float angle) { rotation.x = angle; }
	void SetRotationY(float angle) { rotation.y = angle; }
	void SetRotationZ(float angle) { rotation.z = angle; }
	void SetScale(float x, float y, float z) { scale = { x, y, z }; }
	DirectX::XMMATRIX GetWorldMatrix();
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
};
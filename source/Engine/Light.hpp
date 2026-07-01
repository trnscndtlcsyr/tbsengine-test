#pragma once
#include<DirectXMath.h>

class DirectionalLight {
public:
	DirectionalLight() = default;
	~DirectionalLight() = default;
	void SetPosition(float x, float y, float z);
	void SetDirection(float x, float y, float z);
	DirectX::XMMATRIX GetLightSpaceMatrix() const;
	DirectX::XMVECTOR GetLightDirection() const;
private:
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 target;
};
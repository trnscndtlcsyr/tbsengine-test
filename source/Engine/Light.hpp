#pragma once
#include<DirectXMath.h>

class DirectionalLight {
public:
	DirectionalLight()
		:
		direction{0.5, -1.0f, 0.5f}
	{}
	~DirectionalLight() = default;
	DirectX::XMMATRIX GetLightSpaceMatrix() const;
private:
	DirectX::XMFLOAT3 direction;
};
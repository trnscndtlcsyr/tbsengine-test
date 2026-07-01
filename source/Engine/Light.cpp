#include "Light.hpp"

DirectX::XMMATRIX DirectionalLight::GetLightSpaceMatrix() const
{
	DirectX::XMVECTOR lightPos = DirectX::XMVectorSet(10.0f, 25.0f, 10.0f, 0.0f);
	DirectX::XMVECTOR lightTarget = DirectX::XMVectorSet(25.0f, 0.0f, 25.0f, 0.0f);
	DirectX::XMVECTOR lightUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookAtLH(lightPos, lightTarget, lightUp);
	DirectX::XMMATRIX lightProjection = DirectX::XMMatrixOrthographicLH(100.0f, 100.0f, 1.0f, 200.0f);
	return lightView * lightProjection;
}

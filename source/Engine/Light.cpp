#include "Light.hpp"

void DirectionalLight::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);
}

void DirectionalLight::SetDirection(float x, float y, float z)
{
	direction = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMMATRIX DirectionalLight::GetLightSpaceMatrix() const
{
	DirectX::XMVECTOR lightPos = DirectX::XMVectorSet(position.x, position.y, position.z, 0.0f);
	DirectX::XMVECTOR lightTarget = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
	DirectX::XMVECTOR lightUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookAtLH(lightPos, lightTarget, lightUp);
	DirectX::XMMATRIX lightProjection = DirectX::XMMatrixOrthographicLH(100.0f, 100.0f, 1.0f, 200.0f);
	return DirectX::XMMatrixTranspose(lightView * lightProjection);
}

DirectX::XMVECTOR DirectionalLight::GetLightDirection() const
{
	DirectX::XMVECTOR lightPos = DirectX::XMVectorSet(position.x, position.y, position.z, 0.0f);
	DirectX::XMVECTOR lightTarget = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
	DirectX::XMVECTOR lightDir = DirectX::XMVectorSubtract(lightTarget, lightPos);
	lightDir = DirectX::XMVector3Normalize(lightDir);
	return lightDir;
}

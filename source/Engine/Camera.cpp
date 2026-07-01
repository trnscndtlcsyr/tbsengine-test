#include "Camera.hpp"

void Camera::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(x, y, z, 0.0f);
	DirectX::XMVECTOR focus = DirectX::XMVectorSet(x, y, z + 1.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	viewMatrix = DirectX::XMMatrixLookAtLH(eye, focus, up);
}

void Camera::SetProjection(ProjectionType type, float width, float height)
{
	if (type == ProjectionType::Otrhographic) {
		projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
	}
	else if (type == ProjectionType::Perspective) {
		float fov = DirectX::XMConvertToRadians(45.0f);
		float aspectRatio = width / height;
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.1f, 1000.0f);
	}
}

DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
{
	return DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix));
}

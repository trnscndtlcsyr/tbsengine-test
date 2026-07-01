#include "Transform3D.hpp"
#include<DirectXMath.h>

DirectX::XMMATRIX Transform3D::GetWorldMatrix()
{
	return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

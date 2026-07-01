#include "Object3D.hpp"


const DirectX::XMMATRIX Object3D::GetMatrix() 
{ 
	return DirectX::XMMatrixTranspose(transform.GetWorldMatrix()); 
}

void Object3D::SetTransformRotationX(float angle) 
{ 
	transform.SetRotationY(DirectX::XMConvertToRadians(angle)); 
}
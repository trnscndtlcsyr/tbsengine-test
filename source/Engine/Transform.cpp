#include "Transform.h"

float Transform::GetScaleFactor()
{
	return sqrtf(current._11 * current._11 + current._12 * current._12);
}

void Transform::ChangeMatrix(D2D1::Matrix3x2F& mat)
{
	current = mat * current;
}

D2D1::Matrix3x2F& Transform::GetResultMatrix()
{
	return current;
}

D2D1_POINT_2F Transform::ScreenToWorld(D2D1_POINT_2F& screenPos)
{
	D2D1::Matrix3x2F inverted = current;
	if (!inverted.Invert()) return screenPos;
	return inverted.TransformPoint(screenPos);
}

D2D1_POINT_2F Transform::WorldToScreen(D2D1_POINT_2F& worldPos)
{
	D2D1::Matrix3x2F matrix = current;
	return matrix.TransformPoint(worldPos);
}

void Transform::Translate(const D2D1_POINT_2F& move)
{
	D2D1::Matrix3x2F inverted = current;
	if (!inverted.Invert()) return;
	
	//distance multiplies by matrix scale component
	D2D1_POINT_2F pos = D2D1::Point2F(move.x * inverted._11, move.y * inverted._22);
	D2D1::Matrix3x2F translationMatrix = D2D1::Matrix3x2F::Translation(pos.x, pos.y);
	ChangeMatrix(translationMatrix);
}

void Transform::Scale(const D2D1_POINT_2F& mousePos, float scaleRatio)
{
	float futureScale = GetScaleFactor() * scaleRatio;
	if (futureScale < lowerScaleLimit || futureScale > upperScaleLimit) return;

	D2D1::Matrix3x2F invertedTransform = current;
	if (!invertedTransform.Invert()) return;

	D2D1_POINT_2F worldMousePos = invertedTransform.TransformPoint(mousePos);
	D2D1::Matrix3x2F scaleMatrix = D2D1::Matrix3x2F::Scale(
		D2D1::SizeF(scaleRatio, scaleRatio), 
		worldMousePos
	);
	ChangeMatrix(scaleMatrix);
}




#pragma once
#include<vector>
#include<d2d1_1.h>
#include<d2d1_1helper.h>

//component which contain object position, scale, (rotation??), all object require transform inside
class Transform
{
public:
	Transform()
		: current(D2D1::Matrix3x2F::Identity())
	{}
	~Transform() {}
public:
	float GetScaleFactor();
	D2D1::Matrix3x2F& GetResultMatrix();
	void ChangeMatrix(D2D1::Matrix3x2F& mat);
	D2D1_POINT_2F ScreenToWorld(D2D1_POINT_2F& screenPos);
	D2D1_POINT_2F WorldToScreen(D2D1_POINT_2F& worldPos);
	void Translate(const D2D1_POINT_2F& move);
	void Scale(const D2D1_POINT_2F& v, float scaleFactor);
private:
	float upperScaleLimit = 1.5f;
	float lowerScaleLimit = 0.1f;
	float scaleFactor = 1.0f;
	D2D1::Matrix3x2F current;
};
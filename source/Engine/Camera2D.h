#pragma once
#include "Transform.h"

class Camera2D
{
public:
	Camera2D()
		:
		x(0.0f),
		y(0.0f)
	{}
	void setCameraView(Transform& transform, float w, float h);
	D2D1_RECT_F getView() { return view; }
	bool viewCulling(const D2D1_POINT_2F& pos, const D2D1_POINT_2F& size);
private:
	float x, y;
	float padding = 20.0f;
	D2D1_RECT_F view;
};


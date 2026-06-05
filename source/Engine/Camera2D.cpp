#include "Camera2D.h"

void Camera2D::setCameraView(Transform& transform, float w, float h)
{
	D2D1::Matrix3x2F current = transform.GetResultMatrix();
	if (!current.Invert()) return;

	D2D1_POINT_2F screenTopLeftCorner = D2D1::Point2F(padding + x, padding + y);
	D2D1_POINT_2F screenBottomRightCorner = D2D1::Point2F(w - padding, h - padding);

	D2D1_POINT_2F cameraXY = current.TransformPoint(screenTopLeftCorner);
	D2D1_POINT_2F cameraWH = current.TransformPoint(screenBottomRightCorner);
	view = D2D1::RectF(cameraXY.x, cameraXY.y, cameraWH.x, cameraWH.y);
}

bool Camera2D::viewCulling(const D2D1_POINT_2F& pos, const D2D1_POINT_2F& size)
{
	return (view.left < pos.x + size.x && view.right > pos.x && 
		view.top < pos.y + size.y && view.bottom > pos.y) ? true : false;
}

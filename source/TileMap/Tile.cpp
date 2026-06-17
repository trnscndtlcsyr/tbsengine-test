#include "Tile.hpp"

D2D1_POINT_2F Tile::OrthoToIso(int orthoX, int orthoY, int width, int height)
{
	return D2D1::Point2F(
		(orthoX - orthoY) * (width / 2.0f),
		(orthoX + orthoY) * (height / 2.0f)
	);
}
D2D1_POINT_2F Tile::IsoToOrtho(int isoX, int isoY, int width, int height)
{
	float orthoX = (isoX / (width / 2.0f) + isoX / (width / 2.0f)) / 2.0f;
	float orthoY = (isoY / (height / 2.0f) + isoY / (height / 2.0f)) / 2.0f;
	return D2D1::Point2F(
		static_cast<int>(floor(orthoX)),
		static_cast<int>(floor(orthoY))
	);
}
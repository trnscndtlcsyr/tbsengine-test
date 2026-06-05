#pragma once

#include<d2d1_1helper.h>
#include<d2d1_1.h>
#include<memory>

#include "..\Engine\Texture2D.h"
#include "GameTypes.h"

class Tile
{
public:
	static constexpr float s_TileWidth = 40.0f;
	static constexpr float s_TileHeight = 20.0f;

	static D2D1_POINT_2F OrthoToIso(int orthoX, int orthoY, int width, int height)
	{
		return D2D1::Point2F(
			(orthoX - orthoY) * (width / 2.0f),
			(orthoX + orthoY) * (height / 2.0f)
		);
	}
	static D2D1_POINT_2F IsoToOrtho(int isoX, int isoY, int width, int height)
	{
		float orthoX = (isoX / (width / 2.0f) + isoX / (width / 2.0f)) / 2.0f;
		float orthoY = (isoY / (height / 2.0f) + isoY / (height / 2.0f)) / 2.0f;
		return D2D1::Point2F(
			static_cast<int>(floor(orthoX)),
			static_cast<int>(floor(orthoY))
		);
	}
public:
	Tile() = default;
	Tile(TileType type, int x, int y) 
		: Type(type), xPos(x), yPos(y) 
	{}
	~Tile()
	{}
	TileType GetType() const { return Type; }
	D2D1_POINT_2F GetPosition() const { return D2D1::Point2F(xPos, yPos); }
	D2D1_POINT_2F GetIsoPosition() const { return OrthoToIso(xPos, yPos, s_TileWidth, s_TileHeight); }
private:
	TileType Type;
	int xPos, yPos;
};


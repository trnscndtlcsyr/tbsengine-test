#pragma once

#include<d2d1_1helper.h>
#include<d2d1_1.h>
#include<memory>
#include "GameTypes.hpp"

class Tile
{
public:
	static constexpr float s_TileWidth = 40.0f;
	static constexpr float s_TileHeight = 20.0f;

	static D2D1_POINT_2F OrthoToIso(int orthoX, int orthoY, int width, int height);
	static D2D1_POINT_2F IsoToOrtho(int isoX, int isoY, int width, int height);
public:
	Tile() = default;
	Tile(TileType type, int x, int y)
		: 
		Type{ type },
		xPos{ x },
		yPos{ y }
	{}
	~Tile() = default;
	TileType GetType() const { return Type; }
	D2D1_POINT_2F GetPosition() const { return D2D1::Point2F(xPos, yPos); }
	D2D1_POINT_2F GetIsoPosition() const { return OrthoToIso(xPos, yPos, s_TileWidth, s_TileHeight); }
private:
	TileType Type;
	int xPos, yPos;
};


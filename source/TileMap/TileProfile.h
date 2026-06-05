#pragma once
#include "GameTypes.h"
#include "../Engine/Texture2D.h"

class TileProfile
{
public:
	TileProfile(Texture2D tex, TileType type)
		: texture(tex), type(type)
	{}
	~TileProfile() = default;
	Texture2D& GetTexture() { return texture; }
	TileType GetType() const { return type; }
private:
	Texture2D texture;
	TileType type;
};


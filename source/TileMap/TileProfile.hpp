#pragma once
#include "GameTypes.hpp"
#include "../Engine/Sprite.hpp"
#include "../Engine/AtlasData.hpp"

class TileProfile
{
public:
	TileProfile(AtlasData::SpriteData data, TileType type)
		: 
		spriteData{ data },
		type{ type }
	{}
	~TileProfile() = default;
	AtlasData::SpriteData getData() const { return spriteData; }
	//Sprite& GetSprite() { return sprite; }
	TileType GetType() const { return type; }
private:
	AtlasData::SpriteData spriteData;
	TileType type;
};


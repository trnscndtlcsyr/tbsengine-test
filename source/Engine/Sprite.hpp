#pragma once

#include "Texture2D.hpp"
#include "AtlasData.hpp"

class Sprite {
public:
	Sprite(AtlasData::SpriteData d)
		:
		data{ d }
	{}
private:
	AtlasData::SpriteData data;
};
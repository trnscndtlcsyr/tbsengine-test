#pragma once
#include <vector>
#include "Tile.hpp"
#include "../NoiseGenerator/NoiseGenerator.hpp"

class Chunk
{
public:
	static constexpr int s_TilesPerChunk = 10;
	static constexpr float s_ChunkWidth = s_TilesPerChunk * Tile::s_TileWidth;
	static constexpr float s_ChunkHeight = s_TilesPerChunk * Tile::s_TileHeight;
public:
	Chunk(NoiseGenerator& gen, int startPosX, int startPosY, int tileCountByWidth, int tileCountByHeight);
	Tile& GetTile(int col, int row) { return tiles.at(col * static_cast<int>(s_ChunkWidth) + row); }
	std::vector<Tile>& GetTiles() { return tiles; }
	D2D1_POINT_2F GetPosition() const { return D2D1::Point2F(posX, posY); }
	D2D1_POINT_2F GetIsoPosition();
	D2D1_POINT_2F GetSize() const { return D2D1::Point2F(s_ChunkWidth, s_ChunkHeight); }
	size_t GetTileCount() const { return tiles.size(); }
private:
	int posX, posY;
	std::vector<Tile> tiles;
};


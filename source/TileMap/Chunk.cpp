#include "Chunk.h"
#include "../NoiseGenerator/NoiseGenerator.h"
#include "../NoiseGenerator/Biome.h"

Chunk::Chunk(NoiseGenerator& gen, int chunkPosX, int chunkPosY, int tilePerWidth, int tilePerHeight)
{
	posX = chunkPosX;
	posY = chunkPosY;
	std::vector<Biome> biomes = gen.generateBiomesMap(
		tilePerWidth, tilePerHeight, 
		posX * tilePerWidth, posY * tilePerHeight
	);
	for (size_t y = 0; y < tilePerHeight; ++y)
	{
		for (size_t x = 0; x < tilePerWidth; ++x)
		{
			size_t index = (y * tilePerHeight) + x;
			TileType type = biomes.at(index).Type;
			Tile tile(type, x + chunkPosX * tilePerWidth, y + chunkPosY * tilePerHeight);
			tiles.push_back(tile);
		}
	}
}

D2D1_POINT_2F Chunk::GetIsoPosition()
{
	D2D1_POINT_2F iso = Tile::OrthoToIso(posX, posY, Chunk::s_ChunkWidth, Chunk::s_ChunkHeight);
	float chunk_start_posX = (Tile::s_TileWidth - Chunk::s_ChunkWidth) / 2.0f;
	iso.x += chunk_start_posX;
	return iso;
}


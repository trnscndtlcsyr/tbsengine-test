#include "ChunkMap.hpp"

ChunkMap::ChunkMap()
{
	for (int y = 0; y < ChunkMap::s_map_height; ++y) {
		for (int x = 0; x < ChunkMap::s_map_width; ++x) {
			chunks.push_back(Chunk(noiseGen, x, y, Chunk::s_TilesPerChunk, Chunk::s_TilesPerChunk));
		}
	}
}
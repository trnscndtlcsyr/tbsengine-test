#pragma once
#include "Chunk.hpp"
#include "../NoiseGenerator/NoiseGenerator.hpp"

class ChunkMap
{
public:
	static constexpr int s_map_width = 10;
	static constexpr int s_map_height = 10;
public:
	ChunkMap();
	std::pair<int, int> GetSize() const { return { s_map_width, s_map_height }; }
	Chunk& GetChunk(int col, int row) { return chunks.at(row * ChunkMap::s_map_width + col); }
	std::vector<Chunk>& GetChunks() { return chunks; }
	size_t ChunkCount() const { return chunks.size(); }
private:
	NoiseGenerator noiseGen;
	std::vector<Chunk> chunks;
};

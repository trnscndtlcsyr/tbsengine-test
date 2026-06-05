#pragma once
#include <vector>
#include <random>

class VoronoiDiagram
{
public:
	VoronoiDiagram() = default;
	VoronoiDiagram(unsigned int seed, float scale) 
		: 
		seed(seed), 
		spaceScale(scale),
		rng(seed), 
		distribution(0.0f, 1.0f)
	{}
	~VoronoiDiagram() = default;
	float getDistanceToEdge(float posX, float posY);
	int getRegionID(float posX, float posY);
private:
	void getScaleNodeOffset(int sectorX, int sectorY, float& outX, float& outY);
	void getNodePosition(int sectorX, int sectorY, float& nodeX, float& nodeY);
private:
	unsigned int seed;
	std::mt19937 rng;
	std::uniform_real_distribution<float> distribution;
	float spaceScale;
};


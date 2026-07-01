#include "Terrain.hpp"
#include <NoiseGenerator/NoiseGenerator.hpp>
#include "Object3D.hpp"

Terrain::Terrain(uint32_t width, uint32_t depth)
{
	mesh = std::make_shared<NoisePlane>(width, depth);
}
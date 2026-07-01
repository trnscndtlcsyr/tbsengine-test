#pragma once
#include <NoiseGenerator/NoiseGenerator.hpp>
#include "Object3D.hpp"
#include "Mesh.hpp"

class Terrain : public Object3D {
public:
	Terrain() = default;
	Terrain(uint32_t width, uint32_t depth);
	Transform3D& getTransform() { return transform; }
};
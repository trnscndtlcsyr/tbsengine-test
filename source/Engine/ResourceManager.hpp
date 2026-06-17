#pragma once

#include "Texture2D.hpp"
#include "Graphics2D.hpp"

#include <unordered_map>
#include <string>
#include <memory>

class ResourceManager {
public:
	ResourceManager() = default;
	Graphics2D::comPtr<ID2D1Bitmap> LoadBitmapResource(const WCHAR* filename, const Graphics2D& gfx);
private:
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> texRes;
};
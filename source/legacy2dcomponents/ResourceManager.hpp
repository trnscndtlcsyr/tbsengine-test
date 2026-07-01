#pragma once

#include "Texture2D.hpp"
#include "Graphics2D.hpp"

#include <filesystem>
#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>

class ResourceManager {
public:
	ResourceManager() = default;
	~ResourceManager() = default;
	void AddTexture2DFromFile(const std::filesystem::path& filename, const Graphics2D& gfx, std::wstring_view texName);
	Texture2D* GetTexture2DByName(std::wstring_view texName) const;
private:
	Texture2D CreateTexture2DFromFile(const std::filesystem::path& filename, const Graphics2D& gfx);
	comPtr<ID2D1Bitmap> LoadBitmapResource(const std::filesystem::path& filename, const Graphics2D& gfx);
private:
	std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> texRes;
};
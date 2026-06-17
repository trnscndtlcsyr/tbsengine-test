#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../TileMap/GameTypes.hpp"

namespace AtlasData
{
	struct PointData {
		float x, y;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PointData, x, y)

		struct SizeData {
		float w, h;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SizeData, w, h)

		struct FrameData {
		float x, y, w, h;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FrameData, x, y, w, h)

		struct SpriteData {
		FrameData frame;
		bool rotated;
		bool trimmed;
		FrameData spriteSourceSize;
		SizeData sourceSize;
		PointData pivot;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteData, frame, rotated, trimmed, spriteSourceSize, sourceSize, pivot)

		struct MetaData {
		std::string image;
		std::string format;
		SizeData size;
		float scale;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetaData, image, format, size, scale)

		struct SpriteDataPackage {
		std::unordered_map<std::string, SpriteData> frames;
		MetaData meta;

	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteDataPackage, frames, meta)

		struct MappingTable {
		std::unordered_map<std::string, TileType> table;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MappingTable, table)


		template<typename T>
	concept validAtlasData = requires
	{
		requires std::same_as<T, SpriteDataPackage> || std::same_as<T, MappingTable>;
	};
	template<validAtlasData T>
	static T LoadJsonConfig(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path)) throw std::exception("file doesnt exist");
		std::ifstream file(path);
		if (!file.is_open()) throw std::exception("File already open");
		nlohmann::json j;
		file >> j;
		return j.get<T>();
	};
}
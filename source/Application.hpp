#pragma once

#include "../source/Engine/Window.hpp"
#include "../source/Engine/Timer.hpp"
#include "../source/TileMap/Chunk.hpp"
#include "../source/TileMap/ChunkMap.hpp"
#include "../source/TileMap/TileProfile.hpp"
#include "Engine/Camera2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/AtlasData.hpp"

#include<string>
#include<sstream>

class Application
{
public:
	Application()
		: 
		wnd{ 1000u, 600u, L"DemoTBS" },
		chunkMap{ std::make_unique<ChunkMap>() }
	{}
	int Run();
private:
	void Initialize();
	void FrameRender();
	void SystemUpdate();
private:
	void DrawCollision(const D2D1_RECT_F rect, const Graphics2D::comPtrBrush& brush);
	void DrawTile(const Tile& tile);
	void DrawTextInfo(const WCHAR* text, const D2D1_RECT_F dst);
private:
	ResourceManager resManager;
	Texture2D atlas;
	Window wnd;
	Timer timer;
	Transform transform;
	Camera2D cam;
	std::unique_ptr<ChunkMap> chunkMap;
	std::vector<TileProfile> tileProfiles;
	int error;
	Graphics2D::comPtrBrush pTextBrush = nullptr;
	Graphics2D::comPtrBrush pCollisionBrush = nullptr;
private:
	std::pair<int, int> mapSize;
	std::pair<int, int> currentPos;
	std::pair<int, int> previousPos;
	std::wostringstream wss;
};

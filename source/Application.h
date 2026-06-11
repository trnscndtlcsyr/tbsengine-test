#pragma once

#include "../source/Engine/Window.h"
#include "../source/Engine/Timer.h"
#include "../source/TileMap/Chunk.h"
#include "../source/TileMap/ChunkMap.h"
#include "../source/TileMap/TileProfile.h"
#include "Engine/Camera2D.h"

#include<string>
#include<sstream>

class Application
{
public:
	Application()
		: 
		wnd(1000, 600, L"DemoTBS"), 
		chunkMap(std::make_unique<ChunkMap>())
	{}
	int Run();
private:
	void Initialize();
	void FrameRender();
	void SystemUpdate();
private:
	void DrawCollision(const D2D1_RECT_F rect, const GraphicsComponent::comPtrBrush& brush);
	void DrawTile(const Tile& tile);
	void DrawTextInfo(const WCHAR* text, const D2D1_RECT_F dst);
private:
	Window wnd;
	Timer timer;
	Transform transform;
	Camera2D cam;
	std::unique_ptr<ChunkMap> chunkMap;
	std::vector<TileProfile> tileProfiles;
	int error;
	GraphicsComponent::comPtrBrush pTextBrush = nullptr;
	GraphicsComponent::comPtrBrush pCollisionBrush = nullptr;
private:
	std::pair<int, int> mapSize;
	std::pair<int, int> currentPos;
	std::pair<int, int> previousPos;
	std::wostringstream wss;
};

#pragma once
#include <string>
#include <string_view>

#include <TileMap/Chunk.hpp>
#include <TileMap/ChunkMap.hpp>
#include <TileMap/TileProfile.hpp>
#include <Engine/Window.hpp>
#include <Engine/Timer.hpp>
#include <Engine/AtlasData.hpp>
#include <Engine/ImguiWrapper.hpp>
#include <Engine/Camera.hpp>
#include <Engine/Terrain.hpp>

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
	void DrawTile(const Tile& tile);
	//void DrawCollision(const D2D1_RECT_F rect, const comPtrBrush& brush);
	//void DrawTextInfo(const std::wstring& text, const D2D1_RECT_F dst);
private:
	Window wnd;
	Timer timer;
	std::unique_ptr<ChunkMap> chunkMap;
	std::vector<TileProfile> tileProfiles;
private:
	Terrain terrain;
	//ImguiWrapper wrapper;
private:
	//obsolete
	/*ResourceManager resManager;
	Transform transform;
	Camera2D cam;
	Texture2D* pAtlas;
	comPtrBrush pTextBrush = nullptr;
	comPtrBrush pCollisionBrush = nullptr;*/
private:
	int error;
	std::pair<int, int> mapSize;
	std::pair<int, int> currentPos;
	std::pair<int, int> previousPos;
};

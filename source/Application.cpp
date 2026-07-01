#include "Application.hpp"
#include <Engine/Object3D.hpp>
#include <Engine/Mesh.hpp>

float degree = 0;

int Application::Run()
{
	Initialize();
    while (true)
    {
		timer.Update();
		if ((error = wnd.MessageProcessing()) != 1) return error;
		SystemUpdate();
		FrameRender();
    }
}

void Application::Initialize()
{
	wnd.gfx().Initialize(1000, 600);
	terrain = Terrain(100, 100);
	wnd.gfx().CreateResources(terrain.GetMesh());

	AtlasData::SpriteDataPackage sdp = AtlasData::LoadJsonConfig<AtlasData::SpriteDataPackage>(L"assets/textures/texAtlas.json");
	AtlasData::MappingTable ct = AtlasData::LoadJsonConfig<AtlasData::MappingTable>(L"assets/textures/mappingTable.json");
	//resManager.AddTexture2DFromFile(L"textures/texAtlas.png", wnd.gfx(), L"texAtlas");
	//pAtlas = resManager.GetTexture2DByName(L"texAtlas");

	for (const auto& [spriteName, spriteData] : sdp.frames)
	{
		auto it = ct.table.find(spriteName);
		if(it != ct.table.end()) tileProfiles.push_back(TileProfile(spriteData, it->second));
	}
}

void Application::FrameRender()
{
	terrain.SetTransformRotationX(degree);
	wnd.gfx().Render(terrain);
	/*for (auto& chunk : chunkMap->GetChunks())
	{
		if (cam.viewCulling(chunk.GetIsoPosition(), chunk.GetSize()))
		{
			for (auto& tile : chunk.GetTiles())
			{
				DrawTile(tile);
			}
		}
	}*/
	/*D2D1_SIZE_F rtv_size = wnd.gfx().getContextSize();
	std::pair<UINT, UINT> wnd_size = wnd.GetSize();
	std::wstring log = std::format(
		L"rtv:{}x{}\nwnd:{}x{}\nFPS:{}\nTIME: {}\nSCALE: {}\nCHUNKS: {}\n", 
		rtv_size.width, rtv_size.height, 
		wnd_size.first, wnd_size.second,
		timer.GetFPS(), timer.GetTime(), 
		transform.GetScaleFactor(), chunkMap->ChunkCount());*/

	//DrawTextInfo(log, cam.getView());
	//DrawCollision(cam.getView(), pCollisionBrush);
}

void Application::SystemUpdate()
{
	Mouse::Event mEvent = wnd.mouse.Process();
	switch (mEvent.GetType())
	{
		case mouseEvents_t::WheelUp:
			{
				degree++;
				std::pair<float, float> posPair = mEvent.GetPos();
				D2D1_POINT_2F pos = D2D1::Point2F(posPair.first, posPair.second);
				//wnd.gfx().Scale(transform, pos, 2.0f);
			}
			break;
		case mouseEvents_t::WheelDown:
			{
				degree--;
				std::pair<float, float> posPair = mEvent.GetPos();
				D2D1_POINT_2F pos = D2D1::Point2F(posPair.first, posPair.second);
				//wnd.gfx().Scale(transform, pos, 0.5f);
			}
			break;
		case mouseEvents_t::Move:
			{
				previousPos = mEvent.GetPos();
				if (mEvent.WheelIsPressed()) {
					std::pair<float, float> distance =
					{
						previousPos.first - currentPos.first,
						previousPos.second - currentPos.second
					};
					D2D1_POINT_2F dist = D2D1::Point2F(distance.first, distance.second);
					//wnd.gfx().Translate(transform, dist);
				}
				currentPos = previousPos;
			}
			break;
		case mouseEvents_t::LPress:
			{
				//std::pair<UINT, UINT> size = wnd.GetSize();
				//wnd.gfx().SetFullScreen(TRUE, 1920u, 1080u);
			}
			break;
		case mouseEvents_t::RPress:
			{
				//std::pair<UINT, UINT> size = wnd.GetSize();
				//wnd.gfx().SetFullScreen(FALSE, size.first, size.second);
			}
			break;
	}
	//std::pair<UINT, UINT> size = wnd.GetSize();
	//cam.setCameraView(transform, size.first, size.second);
}

void Application::DrawTile(const Tile& tile)
{
	D2D1_POINT_2F pos = tile.GetIsoPosition();
	D2D1_POINT_2F size = D2D1::Point2F(Tile::s_TileWidth, Tile::s_TileHeight);
	//if (!cam.viewCulling(pos, size)) return;
	
	D2D1_RECT_F dst = D2D1::RectF(pos.x, pos.y, pos.x + Tile::s_TileWidth, pos.y + Tile::s_TileHeight);
	for (auto& profile : tileProfiles)
	{
		if (tile.GetType() == profile.GetType())
		{
			AtlasData::FrameData fData = profile.getData().frame;
			D2D1_RECT_F srcDst = D2D1::RectF(fData.x, fData.y, fData.x + fData.w, fData.y + fData.h);
			//wnd.gfx().DrawSpriteFromAtlas(*pAtlas, dst, srcDst);
		}
	}
}

//void Application::DrawCollision(const D2D1_RECT_F rect, const comPtrBrush& brush)
//{
//	wnd.gfx().DrawRectangle2D(rect, brush.Get());
//}
//
//void Application::DrawTextInfo(const std::wstring& text, const D2D1_RECT_F dst)
//{
//	wnd.gfx().DrawText2D(text, dst, pTextBrush);
//}



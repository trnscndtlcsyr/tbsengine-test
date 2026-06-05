#include "Application.h"

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
	pTextBrush = wnd.gfx().CreateBrush(D2D1::ColorF::White);
	pCollisionBrush = wnd.gfx().CreateBrush(D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f));

	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test.png"), TileType::None));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_abyss.png"), TileType::Abyss));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_ocean.png"), TileType::Ocean));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_iceLand.png"), TileType::IceLand));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_shore.png"), TileType::Shore));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_snowyShore.png"), TileType::SnowyShore));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_plains.png"), TileType::Plains));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_fireLand.png"), TileType::FireLand));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_forest.png"), TileType::Forest));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_tundra.png"), TileType::Tundra));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_desert.png"), TileType::Desert));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_grassyHill.png"), TileType::GrassyHill));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_forestHill.png"), TileType::ForestHill));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_muddyHill.png"), TileType::MuddyHill));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_dryHill.png"), TileType::DryHill));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_snowyHill.png"), TileType::SnowyHill));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_desertDunes.png"), TileType::DesertDunes));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_volcano.png"), TileType::Volcano));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_rockyMountain.png"), TileType::RockyMountain));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_iceMountain.png"), TileType::IceMountain));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_swamp.png"), TileType::Swamp));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_rainForest.png"), TileType::RainForest));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_dryLand.png"), TileType::DryLand));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_savannah.png"), TileType::Savannah));
	tileProfiles.push_back(TileProfile(wnd.gfx().LoadTexture(L"textures/tex_test_geyserLand.png"), TileType::GeyserLand));
}

void Application::FrameRender()
{
	wnd.gfx().RenderStartAndClear(0.5f, 0.5f, 0.7f, 1.0f);
	for (auto& chunk : chunkMap.GetChunks())
	{
		if (cam.viewCulling(chunk.GetIsoPosition(), chunk.GetSize()))
		{
			for (auto& tile : chunk.GetTiles())
			{
				DrawTile(tile);
			}
		}
	}
	D2D1_SIZE_F rtv_size = wnd.gfx().GetRenderContextSize();
	std::pair<UINT, UINT> wnd_size = wnd.GetSize();
	wss << rtv_size.width << "x" << rtv_size.height << std::endl;
	wss << wnd_size.first << "x" << wnd_size.second << std::endl;
	wss << "fps: " << timer.GetFPS() << std::endl;
	wss << "time: " << timer.GetTime() << std::endl;
	wss << "scale: " << transform.GetScaleFactor() << std::endl;
	wss << "chunks: " << chunkMap.ChunkCount() << std::endl;

	DrawTextInfo(wss.str().c_str(), cam.getView());
	DrawCollision(cam.getView(), pCollisionBrush);
	wss.str(std::wstring());

	wnd.gfx().RenderEnd();
}

void Application::SystemUpdate()
{
	Mouse::Event mEvent = wnd.mouse.Process();
	switch (mEvent.GetType())
	{
		case mouseEvents_t::WheelUp:
			{
				std::pair<float, float> posPair = mEvent.GetPos();
				D2D1_POINT_2F pos = D2D1::Point2F(posPair.first, posPair.second);
				wnd.gfx().Scale(transform, pos, 2.0f);
			}
			break;
		case mouseEvents_t::WheelDown:
			{
				std::pair<float, float> posPair = mEvent.GetPos();
				D2D1_POINT_2F pos = D2D1::Point2F(posPair.first, posPair.second);
				wnd.gfx().Scale(transform, pos, 0.5f);
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
					wnd.gfx().Translate(transform, dist);
				}
				currentPos = previousPos;
			}
			break;
		case mouseEvents_t::LPress:
			{
				//std::pair<UINT, UINT> size = wnd.GetSize();
				wnd.gfx().SetFullScreen(TRUE, 1920u, 1080u);
			}
			break;
		case mouseEvents_t::RPress:
			{
				std::pair<UINT, UINT> size = wnd.GetSize();
				wnd.gfx().SetFullScreen(FALSE, size.first, size.second);
			}
			break;
	}
	std::pair<UINT, UINT> size = wnd.GetSize();
	cam.setCameraView(transform, size.first, size.second);
}

void Application::DrawTile(const Tile& tile)
{
	D2D1_POINT_2F pos = tile.GetIsoPosition();
	D2D1_POINT_2F size = D2D1::Point2F(Tile::s_TileWidth, Tile::s_TileHeight);
	if (!cam.viewCulling(pos, size)) return;
	
	D2D1_RECT_F dst = D2D1::RectF(pos.x, pos.y, pos.x + Tile::s_TileWidth, pos.y + Tile::s_TileHeight);
	for (auto& profile : tileProfiles)
	{
		if (tile.GetType() == profile.GetType())
		{
			wnd.gfx().DrawTexture2D(profile.GetTexture(), dst);
			break;
		}
	}
}

void Application::DrawCollision(const D2D1_RECT_F rect, const GraphicsComponent::comPtrBrush& brush)
{
	wnd.gfx().DrawRectangle2D(rect, brush.Get());
}

void Application::DrawTextInfo(const WCHAR* text, const D2D1_RECT_F dst)
{
	wnd.gfx().DrawText2D(text, dst, pTextBrush);
}



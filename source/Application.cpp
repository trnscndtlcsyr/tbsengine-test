#include "Application.hpp"

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
	AtlasData::SpriteDataPackage sdp = 
		AtlasData::LoadJsonConfig<AtlasData::SpriteDataPackage>(L"textures/texAtlas.json");
	AtlasData::MappingTable ct = 
		AtlasData::LoadJsonConfig<AtlasData::MappingTable>(L"textures/mappingTable.json");
	atlas = Texture2D(resManager.LoadBitmapResource(L"textures/texAtlas.png", wnd.gfx()));
	for (const auto& [spriteName, spriteData] : sdp.frames)
	{
		auto it = ct.table.find(spriteName);
		if(it != ct.table.end()) 
			tileProfiles.push_back(TileProfile(spriteData, it->second));
	}
}

void Application::FrameRender()
{
	wnd.gfx().RenderStartAndClear(0.5f, 0.5f, 0.7f, 1.0f);
	for (auto& chunk : chunkMap->GetChunks())
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
	wss << "chunks: " << chunkMap->ChunkCount() << std::endl;

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
			AtlasData::FrameData fData = profile.getData().frame;
			D2D1_RECT_F srcDst = D2D1::RectF(fData.x, fData.y, fData.x + fData.w, fData.y + fData.h);
			wnd.gfx().DrawSpriteFromAtlas(atlas, dst, srcDst);
			//wnd.gfx().DrawTexture2D(profile.GetTexture(), dst);
			break;
		}
	}
}

void Application::DrawCollision(const D2D1_RECT_F rect, const Graphics2D::comPtrBrush& brush)
{
	wnd.gfx().DrawRectangle2D(rect, brush.Get());
}

void Application::DrawTextInfo(const WCHAR* text, const D2D1_RECT_F dst)
{
	wnd.gfx().DrawText2D(text, dst, pTextBrush);
}



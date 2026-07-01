#include "ResourceManager.hpp"
#include "Graphics2D.hpp"

#include <Engine/WinException.hpp>
#include <wincodec.h>
#include <filesystem>


comPtr<ID2D1Bitmap> ResourceManager::LoadBitmapResource(
	const std::filesystem::path& filename,
	const Graphics2D& gfx)
{
	auto* pWICFactory2 = gfx.getIWICImagingFactory2();
	comPtr<IWICBitmapDecoder> pDecoder = nullptr;
	DX_CHECK(pWICFactory2->CreateDecoderFromFilename(
		filename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	));

	comPtr<IWICBitmapFrameDecode> pWICFrame = nullptr;
	DX_CHECK(pDecoder->GetFrame(0, &pWICFrame));

	comPtr<IWICFormatConverter> pWICFConverter = nullptr;
	DX_CHECK(pWICFactory2->CreateFormatConverter(&pWICFConverter));

	DX_CHECK(pWICFConverter->Initialize(
		pWICFrame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	));
	auto* pD2d1Context = gfx.getID2D1DeviceContext();
	comPtr<ID2D1Bitmap> bmp = nullptr;
	DX_CHECK(pD2d1Context->CreateBitmapFromWicBitmap(pWICFConverter.Get(), nullptr, &bmp));
	return bmp;
}

Texture2D ResourceManager::CreateTexture2DFromFile(const std::filesystem::path& filename, const Graphics2D& gfx)
{
	return Texture2D(LoadBitmapResource(filename, gfx));
}

void ResourceManager::AddTexture2DFromFile(
	const std::filesystem::path& filename,
	const Graphics2D& gfx,
	std::wstring_view texName)
{
	std::shared_ptr<Texture2D> ptex 
		= std::make_shared<Texture2D>(std::move(CreateTexture2DFromFile(filename, gfx)));
	texRes.emplace(texName, ptex);
}
Texture2D* ResourceManager::GetTexture2DByName(std::wstring_view name) const
{
	auto it = texRes.find(std::wstring(name));
	if (it == texRes.end()) throw std::exception("cannot find texture by name");
	return it->second.get();
}
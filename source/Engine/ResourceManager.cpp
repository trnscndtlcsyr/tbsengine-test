#include "ResourceManager.hpp"
#include "WinException.hpp"
#include "Graphics2D.hpp"

#include <wincodec.h>


Graphics2D::comPtr<ID2D1Bitmap> ResourceManager::LoadBitmapResource(
	const WCHAR* filename, 
	const Graphics2D& gfx)
{
	//for bitmap creation
	auto* pWICFactory2 = gfx.getIWICImagingFactory2();
	Graphics2D::comPtr<IWICBitmapDecoder> pDecoder = nullptr;
	DX_CHECK(pWICFactory2->CreateDecoderFromFilename(
		filename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	));

	Graphics2D::comPtr<IWICBitmapFrameDecode> pWICFrame = nullptr;
	DX_CHECK(pDecoder->GetFrame(0, &pWICFrame));

	Graphics2D::comPtr<IWICFormatConverter> pWICFConverter = nullptr;
	DX_CHECK(pWICFactory2->CreateFormatConverter(&pWICFConverter));

	DX_CHECK(pWICFConverter->Initialize(
		pWICFrame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	));
	auto* pD2d1Context = gfx.getDeviceContext2D();
	Graphics2D::comPtr<ID2D1Bitmap> bmp = nullptr;
	DX_CHECK(pD2d1Context->CreateBitmapFromWicBitmap(pWICFConverter.Get(), nullptr, &bmp));
	return bmp;
}
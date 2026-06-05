#include "GraphicsComponent.h"
#include "Core.h"
#include "Utils.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "windowscodecs.lib")

//#define FAIL_CHECK(hr) if(FAILED(hr)) throw std::exception();

GraphicsComponent::GraphicsComponent(HWND hWnd) : hr(S_OK), hWnd(hWnd)
{
	//create dxgi and dx3d11 components
	DX3D11_CreateDeviceAndContext(D3D11_CREATE_DEVICE_BGRA_SUPPORT);
	ExtractDXGIDevice();
	ExtractDXGIAdapter();
	CreateDXGIFactoryFromAdapter();

	//create swapchain components
	CreateSwapChain(DXGI_SCALING_NONE, DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
	ExtractBackBuffer();
	ExtractDXGIBackBuffer();

	//create text components
	CreateDWriteFactory();
	CreateTextFormat(L"Impact", L"en-us", 14.0f);
	SetTextAlligments(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	//create dx2d1 device context components (renderTarget)
	DX2D_CreateFactory();
	DX2D_CreateDevice();
	DX2D_CreateDeviceContext();
	DX2D_SetContextTarget();

	//image factory for bitmaps
	CreateWICFactory();

	DisableFullScreen();
}

void GraphicsComponent::DX3D11_CreateDeviceAndContext(D3D11_CREATE_DEVICE_FLAG creationFlags)
{
	D3D_FEATURE_LEVEL m_featureLevel;
	D3D_FEATURE_LEVEL featuresLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	}; 
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		featuresLevels,
		ARRAYSIZE(featuresLevels),
		D3D11_SDK_VERSION,
		&pD3d11Device,
		&m_featureLevel,
		&pD3d11Context
	);
	FAIL_REPORT(hr);
}

void GraphicsComponent::ExtractDXGIDevice()
{
	if (pD3d11Device) 
	{
		hr = pD3d11Device.As(&pDxgiDevice3);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::CreateDWriteFactory()
{
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &pDWriteFactory3);
	FAIL_REPORT(hr);
}

/// <summary>
/// Create text format for rendered text
/// </summary>
/// <param name="font">Font family name</param>
/// <param name="localeName">Font collection (NULL sets it to use the system font collection)</param>
/// <param name="fontSize">font size</param>
void GraphicsComponent::CreateTextFormat(const WCHAR* font, const WCHAR* localeName, FLOAT fontSize)
{
	if (pDWriteFactory3)
	{
		comPtr<IDWriteTextFormat> pBaseTextFormat;
		hr = pDWriteFactory3->CreateTextFormat(
			font,                      
			nullptr,                   
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			localeName,
			&pBaseTextFormat
		);
		FAIL_REPORT(hr);

		hr = pBaseTextFormat.As(&pTextFormat3);
		FAIL_REPORT(hr);
	}
}

/// <summary>
/// set alignment for text and paragraph
/// </summary>
void GraphicsComponent::SetTextAlligments(
	DWRITE_TEXT_ALIGNMENT textAlg = DWRITE_TEXT_ALIGNMENT_LEADING,
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlg = DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
{
	if (pTextFormat3) 
	{
		hr = pTextFormat3->SetTextAlignment(textAlg);
		FAIL_REPORT(hr);

		hr = pTextFormat3->SetParagraphAlignment(paragraphAlg);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::DX2D_CreateFactory()
{
	comPtr<ID2D1Factory> pBaseFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&pBaseFactory));
	FAIL_REPORT(hr);

	hr = pBaseFactory.As(&pD2d1Factory3);
	FAIL_REPORT(hr);
}

void GraphicsComponent::DX2D_CreateDevice()
{
	if (pD2d1Factory3 && pDxgiDevice3)
	{
		hr = pD2d1Factory3->CreateDevice(pDxgiDevice3.Get(), &pD2d1Device);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::DX2D_CreateDeviceContext()
{
	if (pD2d1Device) 
	{
		hr = pD2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2d1Context);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::ExtractDXGIAdapter()
{
	if (pDxgiDevice3) 
	{
		hr = pDxgiDevice3->GetParent(IID_PPV_ARGS(&pDxgiAdapter3));
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::CreateDXGIFactoryFromAdapter()
{
	if (pDxgiAdapter3)
	{
		hr = pDxgiAdapter3->GetParent(IID_PPV_ARGS(&pDxgiFactory5));
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::CreateSwapChain(DXGI_SCALING scaling, DXGI_SWAP_EFFECT swapEffect)
{
	if (pDxgiFactory5 && pD3d11Device)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 0;
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = scaling;
		swapChainDesc.SwapEffect = swapEffect;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		comPtr<IDXGISwapChain1> pBaseSwapChain;
		hr = pDxgiFactory5->CreateSwapChainForHwnd(
			pD3d11Device.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&pBaseSwapChain
		);
		FAIL_REPORT(hr);

		hr = pBaseSwapChain.As(&pSwapChain3);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::ExtractBackBuffer()
{
	if (pSwapChain3) 
	{
		hr = pSwapChain3->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::ExtractDXGIBackBuffer()
{
	if (pSwapChain3) 
	{
		hr = pSwapChain3->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer));
		FAIL_REPORT(hr);
	}
}

/// <summary>
/// Create bitmap from dxgiBackBuffer for getting target bitmap 
/// and set target for DeviceContext (renderTarget)
/// </summary>
void GraphicsComponent::DX2D_SetContextTarget()
{
	if (pD2d1Context && pDxgiBackBuffer)
	{
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
		); 
		UINT nDPI = GetDpiForWindow(hWnd);
		bitmapProperties.dpiX = nDPI;
		bitmapProperties.dpiY = nDPI;
		hr = pD2d1Context->CreateBitmapFromDxgiSurface(
			pDxgiBackBuffer.Get(),
			&bitmapProperties,
			&pD2d1TargetBitmap1
		);
		FAIL_REPORT(hr);

		pD2d1Context->SetTarget(pD2d1TargetBitmap1.Get());
	}
}

/// <summary>
/// Create windows imaging component factory, com init and create com 
/// instance with CLSID_WICImagingFactory param
/// </summary>
void GraphicsComponent::CreateWICFactory()
{
	hr = CoInitialize(nullptr);
	FAIL_REPORT(hr);

	hr = CoCreateInstance(
		CLSID_WICImagingFactory2, 
		nullptr, 
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&pWICFactory2)
	);
	FAIL_REPORT(hr);
}

GraphicsComponent::comPtr<ID2D1SolidColorBrush> GraphicsComponent::CreateBrush(D2D1::ColorF color)
{
	if (pD2d1Context) {
		D2D1_COLOR_F colorf = D2D1::ColorF(color);
		comPtr<ID2D1SolidColorBrush> pBrush;
		hr = pD2d1Context->CreateSolidColorBrush(colorf, &pBrush);
		FAIL_REPORT(hr);
		return pBrush;
	}
}

ID2D1Bitmap* GraphicsComponent::LoadBitmapResource(const WCHAR* filename)
{
	IWICBitmapDecoder* pDecoder = nullptr;
	hr = pWICFactory2->CreateDecoderFromFilename(
		filename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	FAIL_REPORT(hr);

	IWICBitmapFrameDecode* pWICFrame = nullptr;
	hr = pDecoder->GetFrame(0, &pWICFrame);
	FAIL_REPORT(hr);

	IWICFormatConverter* pWICFConverter = nullptr;
	hr = pWICFactory2->CreateFormatConverter(&pWICFConverter);
	FAIL_REPORT(hr);

	hr = pWICFConverter->Initialize(
		pWICFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	);
	FAIL_REPORT(hr);
	ID2D1Bitmap* bmp = nullptr;
	hr = pD2d1Context->CreateBitmapFromWicBitmap(pWICFConverter, nullptr, &bmp);
	FAIL_REPORT(hr);

	if (pDecoder) pDecoder->Release();
	if (pWICFrame) pWICFrame->Release();
	if (pWICFConverter) pWICFConverter->Release();
	return bmp;
}

Texture2D GraphicsComponent::LoadTexture(const WCHAR* filename)
{
	Texture2D tex(LoadBitmapResource(filename));
	return tex;
}

std::vector<Texture2D> GraphicsComponent::LoadTextures(std::vector<const WCHAR*>& fileData)
{
	std::vector<Texture2D> textures;
	for (auto& fdata : fileData)
	{
		Texture2D tex(LoadBitmapResource(fdata));
		textures.push_back(tex);
	}
	return textures;
}

void GraphicsComponent::DrawTexture2D(Texture2D& texture,const D2D1_RECT_F& dst) const
{
	pD2d1Context->DrawBitmap(texture.GetBitmap(), dst, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
}

void GraphicsComponent::DrawRectangle2D(const D2D1_RECT_F& rect, const comPtr<ID2D1SolidColorBrush>& brush) const
{
	pD2d1Context->DrawRectangle(rect, brush.Get());
}

void GraphicsComponent::DrawLine2D(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, const comPtr<ID2D1SolidColorBrush>& brush) const
{
	pD2d1Context->DrawLine(start, end, brush.Get());
}

void GraphicsComponent::DrawText2D(const WCHAR* text, const D2D1_RECT_F& dst, const comPtr<ID2D1SolidColorBrush>& brush)
{
	if (pD2d1Context) {
		cTextLength = (UINT32)wcslen(text);
		pD2d1Context->DrawText(text, cTextLength, pTextFormat3.Get(), dst, brush.Get());
	}
}

void GraphicsComponent::RenderStartAndClear(float r, float g, float b, float a = 1.0f)
{
	D2D1_COLOR_F color = D2D1::ColorF(r, g, b, a);
	pD2d1Context->BeginDraw();
	pD2d1Context->Clear(color);
}

void GraphicsComponent::RenderEnd()
{
	hr = pD2d1Context->EndDraw();
	FAIL_REPORT(hr);
	UINT synchFlag = IsFullScreen ? 1 : 0;
	hr = pSwapChain3->Present(synchFlag, 0);
	FAIL_REPORT(hr);
}

void GraphicsComponent::RenderResize(UINT width, UINT height)
{
	if (IsChangeOnFullScreen) return;

	pD2d1Context->SetTarget(nullptr);
	pD2d1TargetBitmap1.Reset();
	pDxgiBackBuffer.Reset();
	pBackBuffer.Reset();
	
	hr = pSwapChain3->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	FAIL_REPORT(hr);
		
	ExtractBackBuffer();
	ExtractDXGIBackBuffer();
	DX2D_SetContextTarget();

	//OnTransform();
}


/// <summary>
/// Does not allow alt+enter combination
/// </summary>
void GraphicsComponent::DisableFullScreen()
{
	if (pDxgiFactory5) {
		hr = pDxgiFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		FAIL_REPORT(hr);
	}
}

void GraphicsComponent::SetFullScreen(BOOL flag, UINT width, UINT height)
{
	IsChangeOnFullScreen = true;
	if (IsFullScreen == flag) return;

	pD2d1Context->SetTarget(nullptr);
	pD2d1TargetBitmap1.Reset();
	pDxgiBackBuffer.Reset();
	pBackBuffer.Reset();

	hr = pSwapChain3->SetFullscreenState(flag, nullptr);
	FAIL_REPORT(hr);

	hr = pSwapChain3->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	FAIL_REPORT(hr);

	ExtractBackBuffer();
	ExtractDXGIBackBuffer();
	DX2D_SetContextTarget();

	OnFullScreenChanged(flag);

	IsChangeOnFullScreen = false;
}

void GraphicsComponent::OnTransform(Transform& transform)
{
}

void GraphicsComponent::OnScaleChanged(Transform& transform)
{
	CreateTextFormat(L"Impact", L"en-us", 14.0f / transform.GetScaleFactor());
	SetTextAlligments();
}

void GraphicsComponent::OnFullScreenChanged(BOOL flag)
{
	IsFullScreen = flag;
}

void GraphicsComponent::Translate(Transform& transform,const D2D1_POINT_2F& mousePos)
{
	transform.Translate(mousePos);
	ApplyTransform(transform);
}

void GraphicsComponent::Scale(Transform& transform,const D2D1_POINT_2F& view, float scaleFact)
{
	transform.Scale(view, scaleFact);
	OnScaleChanged(transform);
	ApplyTransform(transform);
}

void GraphicsComponent::ApplyTransform(Transform& transform)
{
	if (pD2d1Context) {
		pD2d1Context->SetTransform(transform.GetResultMatrix());
	}
	//OnTransform(transform);
}

GraphicsComponent::~GraphicsComponent()
{}

#include "Graphics2D.hpp"
#include "Core.hpp"
#include "WinException.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "windowscodecs.lib")

Graphics2D::Graphics2D(HWND hWnd) : hr{ S_OK }, hWnd{ hWnd }
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

	pWICFactory2 = CreateWICFactory();

	DisableFullScreen();
}

void Graphics2D::DX3D11_CreateDeviceAndContext(D3D11_CREATE_DEVICE_FLAG creationFlags)
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
	DX_CHECK(D3D11CreateDevice(
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
	));
}

void Graphics2D::ExtractDXGIDevice()
{
	if (pD3d11Device) 
	{
		DX_CHECK(pD3d11Device.As(&pDxgiDevice3));
	}
}

void Graphics2D::CreateDWriteFactory()
{
	DX_CHECK(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, 
		__uuidof(IDWriteFactory3), 
		&pDWriteFactory3
	));
}

/// <summary>
/// Create text format for rendered text
/// </summary>
/// <param name="font">Font family name</param>
/// <param name="localeName">Font collection (nullptr sets it to use the system font collection)</param>
/// <param name="fontSize">font size</param>
void Graphics2D::CreateTextFormat(
	const WCHAR* font, 
	const WCHAR* localeName, 
	FLOAT fontSize
)
{
	if (pDWriteFactory3)
	{
		comPtr<IDWriteTextFormat> pBaseTextFormat;
		DX_CHECK(pDWriteFactory3->CreateTextFormat(
			font,                      
			nullptr,                   
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			localeName,
			&pBaseTextFormat
		));

		DX_CHECK(pBaseTextFormat.As(&pTextFormat3));
	}
}

/// <summary>
/// set alignment for text and paragraph
/// </summary>
void Graphics2D::SetTextAlligments(
	DWRITE_TEXT_ALIGNMENT textAlg = DWRITE_TEXT_ALIGNMENT_LEADING,
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlg = DWRITE_PARAGRAPH_ALIGNMENT_NEAR
)
{
	if (pTextFormat3) 
	{
		DX_CHECK(pTextFormat3->SetTextAlignment(textAlg));
		DX_CHECK(pTextFormat3->SetParagraphAlignment(paragraphAlg));
	}
}

void Graphics2D::DX2D_CreateFactory()
{
	comPtr<ID2D1Factory> pBaseFactory;
	DX_CHECK(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&pBaseFactory)));
	DX_CHECK(pBaseFactory.As(&pD2d1Factory3));
}

void Graphics2D::DX2D_CreateDevice()
{
	if (pD2d1Factory3 && pDxgiDevice3)
	{
		DX_CHECK(pD2d1Factory3->CreateDevice(pDxgiDevice3.Get(), &pD2d1Device));
	}
}

void Graphics2D::DX2D_CreateDeviceContext()
{
	if (pD2d1Device) 
	{
		DX_CHECK(pD2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2d1Context));
	}
}

void Graphics2D::ExtractDXGIAdapter()
{
	if (pDxgiDevice3) 
	{
		DX_CHECK(pDxgiDevice3->GetParent(IID_PPV_ARGS(&pDxgiAdapter3)));
	}
}

void Graphics2D::CreateDXGIFactoryFromAdapter()
{
	if (pDxgiAdapter3)
	{
		DX_CHECK(pDxgiAdapter3->GetParent(IID_PPV_ARGS(&pDxgiFactory5)));
	}
}

void Graphics2D::CreateSwapChain(
	DXGI_SCALING scaling, 
	DXGI_SWAP_EFFECT swapEffect
)
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
		DX_CHECK(pDxgiFactory5->CreateSwapChainForHwnd(
			pD3d11Device.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&pBaseSwapChain
		));

		DX_CHECK(pBaseSwapChain.As(&pSwapChain3));
	}
}

void Graphics2D::ExtractBackBuffer()
{
	if (pSwapChain3) 
	{
		DX_CHECK(pSwapChain3->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	}
}

void Graphics2D::ExtractDXGIBackBuffer()
{
	if (pSwapChain3) 
	{
		DX_CHECK(pSwapChain3->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer)));
	}
}

/// <summary>
/// Create bitmap from dxgiBackBuffer for getting target bitmap 
/// and set target for DeviceContext (renderTarget)
/// </summary>
void Graphics2D::DX2D_SetContextTarget()
{
	if (pD2d1Context && pDxgiBackBuffer)
	{
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE
			)); 
		UINT nDPI = GetDpiForWindow(hWnd);
		bitmapProperties.dpiX = nDPI;
		bitmapProperties.dpiY = nDPI;
		DX_CHECK(pD2d1Context->CreateBitmapFromDxgiSurface(
			pDxgiBackBuffer.Get(),
			&bitmapProperties,
			&pD2d1TargetBitmap1
		));
		pD2d1Context->SetTarget(pD2d1TargetBitmap1.Get());
	}
}

/// <summary>
/// Create windows imaging component factory, com init and create com 
/// instance with CLSID_WICImagingFactory param
/// </summary>
Graphics2D::comPtr<IWICImagingFactory2> Graphics2D::CreateWICFactory()
{
	comPtr<IWICImagingFactory2> pWICFactory2;
	DX_CHECK(CoInitialize(nullptr));
	DX_CHECK(CoCreateInstance(
		CLSID_WICImagingFactory2,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory2)
	));
	return pWICFactory2;
}

Graphics2D::comPtrBrush Graphics2D::CreateBrush(D2D1::ColorF color)
{
	if (pD2d1Context) {
		D2D1_COLOR_F colorf = D2D1::ColorF(color);
		comPtrBrush pBrush;
		DX_CHECK(pD2d1Context->CreateSolidColorBrush(colorf, &pBrush));
		return pBrush;
	}
}

void Graphics2D::DrawTexture2D(Texture2D& texture, const D2D1_RECT_F& dst) const
{
	pD2d1Context->DrawBitmap(
		texture.GetBitmap(),
		dst, 
		1.0f, 
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
}

void Graphics2D::DrawSpriteFromAtlas(
	Texture2D& texAtlas, 
	const D2D1_RECT_F& dst, 
	const D2D1_RECT_F& srcDst
) const
{
	pD2d1Context->DrawBitmap(
		texAtlas.GetBitmap(),
		dst,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		srcDst);
}

void Graphics2D::DrawRectangle2D(const D2D1_RECT_F& rect, const comPtrBrush& brush) const
{
	pD2d1Context->DrawRectangle(rect, brush.Get());
}

void Graphics2D::DrawLine2D(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, const comPtrBrush& brush) const
{
	pD2d1Context->DrawLine(start, end, brush.Get());
}

void Graphics2D::DrawText2D(const WCHAR* text, const D2D1_RECT_F& dst, const comPtrBrush& brush)
{
	if (pD2d1Context) {
		UINT32 cTextLength = static_cast<UINT32>(wcslen(text));
		pD2d1Context->DrawText(
			text, 
			cTextLength, 
			pTextFormat3.Get(), 
			dst, 
			brush.Get()
		);
	}
}

void Graphics2D::RenderStartAndClear(float r, float g, float b, float a = 1.0f)
{
	D2D1_COLOR_F color = D2D1::ColorF(r, g, b, a);
	pD2d1Context->BeginDraw();
	pD2d1Context->Clear(color);
}

void Graphics2D::RenderEnd()
{
	DX_CHECK(pD2d1Context->EndDraw());
	UINT synchFlag = IsFullScreen ? 1 : 0;
	DX_CHECK(pSwapChain3->Present(synchFlag, 0));
}

void Graphics2D::RenderResize(UINT width, UINT height)
{
	if (IsChangeOnFullScreen) return;

	pD2d1Context->SetTarget(nullptr);
	pD2d1TargetBitmap1.Reset();
	pDxgiBackBuffer.Reset();
	pBackBuffer.Reset();
	
	DX_CHECK(pSwapChain3->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));
		
	ExtractBackBuffer();
	ExtractDXGIBackBuffer();
	DX2D_SetContextTarget();

	//OnTransform();
}


/// <summary>
/// Does not allow alt+enter combination
/// </summary>
void Graphics2D::DisableFullScreen()
{
	if (pDxgiFactory5) {
		DX_CHECK(pDxgiFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	}
}

void Graphics2D::SetFullScreen(BOOL flag, UINT width, UINT height)
{
	IsChangeOnFullScreen = true;
	if (IsFullScreen == flag) return;

	pD2d1Context->SetTarget(nullptr);
	pD2d1TargetBitmap1.Reset();
	pDxgiBackBuffer.Reset();
	pBackBuffer.Reset();

	DX_CHECK(pSwapChain3->SetFullscreenState(flag, nullptr));
	DX_CHECK(pSwapChain3->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));

	ExtractBackBuffer();
	ExtractDXGIBackBuffer();
	DX2D_SetContextTarget();

	OnFullScreenChanged(flag);

	IsChangeOnFullScreen = false;
}

void Graphics2D::OnTransform(Transform& transform)
{
}

void Graphics2D::OnScaleChanged(Transform& transform)
{
	CreateTextFormat(L"Impact", L"en-us", 14.0f / transform.GetScaleFactor());
	SetTextAlligments();
}

void Graphics2D::OnFullScreenChanged(BOOL flag)
{
	IsFullScreen = flag;
}

void Graphics2D::Translate(
	Transform& transform,
	const D2D1_POINT_2F& pos
)
{
	transform.Translate(pos);
	ApplyTransform(transform);
}

void Graphics2D::Scale(
	Transform& transform,
	const D2D1_POINT_2F& view, 
	float scaleFact
)
{
	transform.Scale(view, scaleFact);
	OnScaleChanged(transform);
	ApplyTransform(transform);
}

void Graphics2D::ApplyTransform(Transform& transform)
{
	if (pD2d1Context) {
		pD2d1Context->SetTransform(transform.GetResultMatrix());
	}
	//OnTransform(transform);
}

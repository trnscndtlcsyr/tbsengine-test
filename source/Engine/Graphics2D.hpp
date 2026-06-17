#pragma once

#include<d3d11.h>
#include<dwrite_3.h>
#include<wrl/client.h>

#include<dxgi.h>
#include<dxgi1_2.h>
#include<dxgi1_4.h>
#include<dxgi1_6.h>

#include<d2d1_1.h>
#include<d2d1_3.h>
#include<d2d1_1helper.h>
#include<wincodec.h>

#include "Texture2D.hpp"
#include "Transform.hpp"

class Graphics2D
{
public:
	template<typename T> using comPtr = Microsoft::WRL::ComPtr<T>;
	using comPtrBrush = comPtr<ID2D1SolidColorBrush>;
public:
	Graphics2D(HWND hWnd);
	Graphics2D(const Graphics2D&) = delete;
	Graphics2D& operator=(const Graphics2D&) = delete;
	~Graphics2D() = default;
	void RenderStartAndClear(float r, float g, float b, float a);
	void RenderEnd();
	void RenderResize(UINT width, UINT height);
	void DrawSpriteFromAtlas(Texture2D& texAtlas, const D2D1_RECT_F& dst, const D2D1_RECT_F& srcDst) const;
	void DrawTexture2D(Texture2D& texture, const D2D1_RECT_F& dst) const;
	void DrawRectangle2D(const D2D1_RECT_F& rect, const comPtrBrush& brush) const;
	void DrawLine2D(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, const comPtrBrush& brush) const;
	void DrawText2D(const WCHAR* text, const D2D1_RECT_F& dst, const comPtrBrush& brush);
	void Translate(Transform& transform,const D2D1_POINT_2F& move);
	void Scale(Transform& transform,const D2D1_POINT_2F& view, float scaleFactor);
	void ApplyTransform(Transform& transform);
	const D2D1_SIZE_F GetRenderContextSize() const { return pD2d1Context->GetSize(); }
	ID2D1DeviceContext* getDeviceContext2D() const { return pD2d1Context.Get(); }
	IWICImagingFactory2* getIWICImagingFactory2() const { return pWICFactory2.Get(); }
private:
	void DX3D11_CreateDeviceAndContext(D3D11_CREATE_DEVICE_FLAG creationFlags);
	void ExtractDXGIDevice();
	void ExtractDXGIAdapter();
	void CreateDXGIFactoryFromAdapter();
	comPtr<IWICImagingFactory2> CreateWICFactory();
private:
	void CreateSwapChain(DXGI_SCALING scaling, DXGI_SWAP_EFFECT swapEffect);
	void ExtractBackBuffer();
	void ExtractDXGIBackBuffer();
private:
	void CreateDWriteFactory();
	void CreateTextFormat(const WCHAR* font, const WCHAR* localeName, FLOAT fontSize);
	void SetTextAlligments(DWRITE_TEXT_ALIGNMENT textAlg, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlg);
private:
	void DX2D_CreateFactory();
	void DX2D_CreateDevice();
	void DX2D_CreateDeviceContext();
	void DX2D_SetContextTarget();
private:
	//events signalizing when transform or etc been called
	void OnTransform(Transform& transform);
	void OnScaleChanged(Transform& transform);
	void OnFullScreenChanged(BOOL flag);
	/*void OnResizeWindow();
	void OnResizeResolution();*/
private:
	void DisableFullScreen();
public:
	comPtrBrush CreateBrush(D2D1::ColorF color);
	void SetFullScreen(BOOL flag, UINT width, UINT height);
private:
	//Direct3D hardware base for direct2d
	comPtr<ID3D11Device> pD3d11Device = nullptr;
	comPtr<ID3D11DeviceContext> pD3d11Context = nullptr;

	//Direct2D all stuff for context render, shader etc
	comPtr<ID2D1Factory3> pD2d1Factory3 = nullptr;
	comPtr<ID2D1Device> pD2d1Device = nullptr;
	comPtr<ID2D1DeviceContext> pD2d1Context = nullptr;

	//DXGI infrastructure (relation between Windows & hardware)
	comPtr<IDXGIDevice3> pDxgiDevice3 = nullptr;
	comPtr<IDXGIAdapter3> pDxgiAdapter3 = nullptr;
	comPtr<IDXGIFactory5> pDxgiFactory5 = nullptr;
	comPtr<IDXGISurface> pDxgiBackBuffer = nullptr;

	//Frame output stuff 
	comPtr<IDXGISwapChain3> pSwapChain3 = nullptr;
	comPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	comPtr<ID2D1Bitmap1> pD2d1TargetBitmap1 = nullptr; 

	comPtr<IWICImagingFactory2> pWICFactory2 = nullptr;

	//for text creation
	comPtr<IDWriteFactory3> pDWriteFactory3 = nullptr;
	comPtr<IDWriteTextFormat3> pTextFormat3 = nullptr;
private:
	BOOL IsFullScreen = false;
	BOOL IsChangeOnFullScreen = false;
private:
	HRESULT hr;
	HWND hWnd;
};

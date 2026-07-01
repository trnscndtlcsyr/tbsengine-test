#pragma once
#include <d2d1_1.h>
#include <memory>
#include<wrl/client.h>

class Texture2D
{
public:
	Texture2D() = default;
	explicit Texture2D(Microsoft::WRL::ComPtr<ID2D1Bitmap> pbmp)
		: 
		pBitmap{pbmp}
	{}
	Texture2D(Texture2D&&) noexcept = default;
	Texture2D& operator=(Texture2D&&) noexcept = default;
	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	~Texture2D() = default;
	ID2D1Bitmap* GetBitmap() const { return pBitmap.Get(); }
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap;
};


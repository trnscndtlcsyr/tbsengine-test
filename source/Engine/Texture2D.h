#pragma once
#include <d2d1_1.h>
#include <memory>

class Texture2D
{
public:
	Texture2D() = default;
	Texture2D(ID2D1Bitmap* bmp)
	{
		pBitmap = std::make_shared<ID2D1Bitmap*>(bmp);
	}
	~Texture2D() {}
	ID2D1Bitmap* GetBitmap() const { return *pBitmap; }
private:
	std::shared_ptr<ID2D1Bitmap*> pBitmap;
};


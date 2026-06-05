#pragma once
#include <d2d1.h>
//canvas which contains other nodes on UI elements (for example: buttons, text lines, etc)
class UserInterface
{
	UserInterface();
	~UserInterface();
private:
	float padding;
	D2D1_RECT_F mainLayoutRect;
};


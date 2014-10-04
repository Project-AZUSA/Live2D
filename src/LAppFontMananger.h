#pragma once
#include "main.h"

class LAppFontMananger
{
public:
	LAppFontMananger();
	~LAppFontMananger();

	HRESULT InitFont();
	HRESULT DrawMessageText();

	bool ShowMessage(HINSTANCE hinst, int x,int y,int width,int height,wchar_t * msg,
				 int fontHeight,int fontWidth,int fontWeight,bool italic,wchar_t * family,D3DCOLOR color);


private:

};

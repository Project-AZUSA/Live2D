/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* LAppTextureDesc.cpp
*/
#include "LAppTextureDesc.h"


LAppTextureDesc::LAppTextureDesc(LPDIRECT3DTEXTURE9 tex)
{
	data=tex;
}

LAppTextureDesc::~LAppTextureDesc()
{
	data->Release();
}

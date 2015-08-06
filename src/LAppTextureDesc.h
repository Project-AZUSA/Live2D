/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* LAppTextureDesc.h
*/

#pragma once

#include "L2DTextureDesc.h"
#include "Live2DModelD3D.h"

class LAppTextureDesc : public live2d::framework::L2DTextureDesc
{
public:
	LAppTextureDesc(LPDIRECT3DTEXTURE9 tex);
	virtual ~LAppTextureDesc();

private:
	LPDIRECT3DTEXTURE9 data;
};

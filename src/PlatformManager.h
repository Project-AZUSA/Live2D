/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* PlatformManager.h
*/
#pragma once

#include "IPlatformManager.h"

class PlatformManager:public live2d::framework::IPlatformManager
{
public:
	PlatformManager(void);
	~PlatformManager(void);

	unsigned char* loadBytes(const char* path,size_t* size);
	void releaseBytes(void* data);
	live2d::ALive2DModel* loadLive2DModel(const char* path);
	live2d::framework::L2DTextureDesc* loadTexture(live2d::ALive2DModel* model, int no, const char* path);
	void log(const char* txt);
};


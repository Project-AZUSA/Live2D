/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* PlatformManager.cpp
*/

#include "PlatformManager.h"
#include "FileManager.h"
#include "util/UtDebug.h"
#include "Live2DModelD3D.h"
#include "LAppTextureDesc.h"

extern LPDIRECT3DDEVICE9		g_pD3DDevice ;

using namespace live2d;
using namespace live2d::framework;

PlatformManager::PlatformManager(void)
{
}


PlatformManager::~PlatformManager(void)
{
}


unsigned char* PlatformManager::loadBytes(const char* path,size_t* size)
{
	unsigned char* data=FileManager::loadFile(path ,(int *)size);
	return data;
}

void PlatformManager::releaseBytes(void* data)
{
	FileManager::releaseBuffer(data);
}

ALive2DModel* PlatformManager::loadLive2DModel(const char* path)
{
	size_t size;
	unsigned char* buf = loadBytes(path,&size);
	
	//Create Live2D Model Instance
	ALive2DModel* live2DModel = Live2DModelD3D::loadModel(buf,(int)size);
    return live2DModel;
}

L2DTextureDesc* PlatformManager::loadTexture(ALive2DModel* model, int no, const char* path)
{
	LPDIRECT3DTEXTURE9	texture ;

	FileManager::loadTexture( g_pD3DDevice , path ,&texture) ;

	((Live2DModelD3D*)model)->setTexture( no , texture ) ;// テクスチャとモデルを結びつける
	
	LAppTextureDesc* desc=new LAppTextureDesc(texture);

	return desc;
}

void PlatformManager::log(const char* txt)
{
	UtDebug::print( txt );	
}
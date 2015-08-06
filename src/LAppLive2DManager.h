/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* LAppLive2DManager.h
*/
#pragma once

#include "type/LDVector.h"
#include <math.h>
#include "MyLive2DAllocator.h"


class LAppModel;
class L2DViewMatrix;

class LAppLive2DManager{
private :
	
	//  モデルの番号
	int modelIndex;

	MyLive2DAllocator	myAllocator ;
public:
		// モデルデータ
	live2d::LDVector<LAppModel*> models;
    
    LAppLive2DManager() ;    
    ~LAppLive2DManager() ; 
    
	void init();
	void releaseModel();
    LAppModel* getModel(int no){ return models[no]; }
    int getModelNum(){return models.size();}
    bool tapEvent(float x,float y) ;
    void setDrag(float x, float y);
	void changeModel();

	void deviceLost() ;

};


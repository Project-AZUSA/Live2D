/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* LAppModel.h
*/

#pragma once

#include "L2DBaseModel.h"
#include "Live2DModelD3D.h"
#include <vector>
#include <string>

#include "ModelSetting.h"
#include "L2DViewMatrix.h"

class LAppModel : public live2d::framework::L2DBaseModel
{
private:


public:
    ModelSetting*				modelSetting;// モデルの設定
	std::string			modelHomeDir;

	char* ModelPath;
	bool MouseFollow;
	HWND hwnd;

	bool LookAt;
	int LookAtx;
	int LookAty;

	float faceX,faceY,faceZ,bodyX,eyeX,eyeY,mouthY,paraval[10],paraweight[10];
	int num;
	char paraname[10][100];
	bool isSpeaking;

    LAppModel();
    ~LAppModel(void);
    
    void load(int modelIndex);
	void load(const char* path) ;

    void update();
    void draw();
	
    int startMotion(const char name[],int no,int priority);
	int startRandomMotion(const char name[],int priority);
	
	void setExpression(const char name[]);
	void setRandomExpression();
	
	void preloadMotionGroup(const char name[]);
    
	bool hitTest(const char pid[],float testX,float testY);

	live2d::ALive2DModel* getLive2DModel(){return live2DModel;}

	void deviceLost() ;
};







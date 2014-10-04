
#include "LAppLive2DManager.h"

#include "L2DViewMatrix.h"

//Live2DApplication
#include "LAppModel.h"
#include "LAppDefine.h"
#include "LAppModel.h"
#include "L2DMotionManager.h"


using namespace live2d;


LAppLive2DManager::LAppLive2DManager()
	:modelIndex(0)
{
	
	
	
	


	
	live2d::Live2D::init( &myAllocator );
}


LAppLive2DManager::~LAppLive2DManager() 
{
	releaseModel();
	Live2D::dispose();
}


void LAppLive2DManager::releaseModel()
{
	for (unsigned int i=0; i<models.size(); i++)
	{
		delete models[i];
	}
    models.clear();
}


void LAppLive2DManager::setDrag(float x, float y)
{
	for (unsigned int i=0; i<models.size(); i++)
	{
		models[i]->setDrag(x, y);
	}
}



bool LAppLive2DManager::tapEvent(float x,float y)
{
	if(LAppDefine::DEBUG_LOG) UtDebug::print( "tapEvent\n");
	
	for (unsigned int i=0; i<models.size(); i++)
	{
		if(models[i]->hitTest(  HIT_AREA_HEAD,x, y ))
		{
			
			if(LAppDefine::DEBUG_LOG)UtDebug::print( "face\n");
			models[i]->setRandomExpression();
		}
		else if(models[i]->hitTest( HIT_AREA_BODY,x, y))
		{
			if(LAppDefine::DEBUG_LOG)UtDebug::print( "body\n");
			models[i]->startRandomMotion(MOTION_GROUP_TAP_BODY, PRIORITY_NORMAL );
		}
	}
	    
    return true;
}



void LAppLive2DManager::deviceLost(){
	if(LAppDefine::DEBUG_LOG) live2d::UtDebug::print( "DeviceLost @LAppLive2DManager::deviceLost()\n");

	for (unsigned int i=0; i<models.size(); i++)
	{
		models[i]->deviceLost() ;
	}
}

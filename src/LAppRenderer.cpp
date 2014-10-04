
#include "LAppRenderer.h"

#include "LAppLive2DManager.h"
#include "LAppModel.h"
#include "LAppDefine.h"





LAppRenderer::LAppRenderer()
{

	
	viewMatrix.identity() ;
	viewMatrix.setMaxScale( VIEW_MAX_SCALE );
	viewMatrix.setMinScale( VIEW_MIN_SCALE );

	
	viewMatrix.setMaxScreenRect(
			VIEW_LOGICAL_MAX_LEFT,
			VIEW_LOGICAL_MAX_RIGHT,
			VIEW_LOGICAL_MAX_BOTTOM,
			VIEW_LOGICAL_MAX_TOP
			);

	
	setDeviceSize( 800 , 600 ) ;
}


LAppRenderer::~LAppRenderer()
{
	
}


void LAppRenderer::setDeviceSize( int width , int height )
{
	live2d::UtDebug::println(" set Device size : %d , %d" , width , height ) ;

	
	float ratio=(float)height/width;
	float left = VIEW_LOGICAL_LEFT;
	float right = VIEW_LOGICAL_RIGHT;
	float bottom = -ratio;
	float top = ratio;
	viewMatrix.setScreenRect(left,right,bottom,top);

	float screenW=abs(left-right);

	deviceToScreen.identity() ;
	deviceToScreen.multTranslate(-width/2.0f,-height/2.0f );
	deviceToScreen.multScale( screenW/width , -screenW/width );
}



void LAppRenderer::draw()
{
	dragMgr.update();

	live2DMgr->setDrag(dragMgr.getX(), dragMgr.getY());

	int numModels=live2DMgr->getModelNum();
	for (int i=0; i<numModels; i++)
	{
		LAppModel* model = live2DMgr->getModel(i);
		model->update();
		model->draw();
	}
}

void LAppRenderer::setLive2DManager(LAppLive2DManager* mgr)
{
	this->live2DMgr = mgr;
}


void LAppRenderer::scaleView(float cx,float cy,float scale)
{
	viewMatrix.adjustScale(cx,cy,scale);
}


void LAppRenderer::translateView(float shiftX,float shiftY)
{
	viewMatrix.adjustTranslate(shiftX,shiftY);
}



void LAppRenderer::mousePress(int x,int y)
{
	float vx=transformDeviceToViewX( (float)x );
	float vy=transformDeviceToViewY( (float)y );

	if(LAppDefine::DEBUG_TOUCH_LOG) live2d::UtDebug::println( "mouse press / device(%4d,%4d) > logical( %5.3f , %5.3f )  @LAppRenderer#touchMove()" , x , y , vx , vy ) ;
	this->live2DMgr->tapEvent( vx , vy ) ;
}


void LAppRenderer::mouseDrag(int x,int y)
{
	float vx=transformDeviceToViewX( (float)x );
	float vy=transformDeviceToViewY( (float)y );

	if(LAppDefine::DEBUG_TOUCH_LOG) live2d::UtDebug::println( "mouse drag / device(%4d,%4d) > logical( %5.3f , %5.3f )  @LAppRenderer#touchMove()" , x , y , vx , vy ) ;
	dragMgr.set(vx,vy);
}



void LAppRenderer::updateViewMatrix( float dx , float dy , float cx , float cy , float scale )
{
	bool isMaxScale=viewMatrix.isMaxScale();
	bool isMinScale=viewMatrix.isMinScale();
	
	
	viewMatrix.adjustScale(cx, cy, scale);

	
	viewMatrix.adjustTranslate(dx, dy) ;
	
	
	if( ! isMaxScale)
	{
		if(viewMatrix.isMaxScale())
		{
			
			if(LAppDefine::DEBUG_LOG) live2d::UtDebug::println( "max scale" ) ;
		}
	}
	
	if( ! isMinScale)
	{
		if(viewMatrix.isMinScale())
		{
			
			if(LAppDefine::DEBUG_LOG) live2d::UtDebug::println( "min scale" ) ;
		}
	}
	
}


void LAppRenderer::mouseWheel( int delta , int x , int y ){
	float x_onScreen = deviceToScreen.transformX((float)x) ;
	float y_onScreen = deviceToScreen.transformY((float)y) ;

	//float scale = delta < 0 ? 0.5f : 2.0f ; 
	float scale = delta < 0 ? 1.0f/1.4142f : 1.41421f ; 
	

	
	updateViewMatrix( 0 , 0 , x_onScreen , y_onScreen , scale ) ;
}



float LAppRenderer::transformDeviceToViewX(float deviceX)
{
	float screenX = deviceToScreen.transformX( deviceX );	
	return  viewMatrix.invertTransformX(screenX);			
}


float LAppRenderer::transformDeviceToViewY(float deviceY)
{
	float screenY = deviceToScreen.transformY( deviceY );	
	return  viewMatrix.invertTransformY(screenY);			
}


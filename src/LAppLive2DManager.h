
#pragma once

#include "type/LDVector.h"
#include <math.h>
#include "MyLive2DAllocator.h"


class LAppModel;
class L2DViewMatrix;

class LAppLive2DManager{

public:
	live2d::LDVector<LAppModel*> models;
private :

	int modelIndex;

	MyLive2DAllocator	myAllocator ;
public:
    
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


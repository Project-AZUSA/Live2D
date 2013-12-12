/**
 *
 *  このソースはLive2D関連アプリの開発用途に限り
 *  自由に改変してご利用頂けます。
 *
 *  (c) CYBERNOIDS Co.,Ltd. All rights reserved.
 */
#include "L2DMotionManager.h"


L2DMotionManager::L2DMotionManager()
:currentPriority(0),reservePriority(0)
{
    
}


int L2DMotionManager::startMotionPrio( AMotion* motion, bool isDelete, int priority )
{
	if (motion==NULL)
	{
		reservePriority=0;
		return -1;
	}
	if(priority==reservePriority)
	{
		reservePriority=0;//予約を解除
	}
	currentPriority=priority;//再生中モーションの優先度を設定
	return super::startMotion(motion, isDelete);
}


bool L2DMotionManager::updateParam(ALive2DModel* model)
{
	bool updated=super::updateParam(model);
	if(isFinished())
	{
		currentPriority=0;//再生中モーションの優先度を解除
	}
	return updated;
}


bool L2DMotionManager::reserveMotion(int priority)
{
	if ( priority <= reservePriority || priority <= currentPriority )
	{
		return false;
	}
	reservePriority=priority;
	return true;
}

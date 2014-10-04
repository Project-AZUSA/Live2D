#pragma once
#pragma comment(lib,"Dsound.lib")
#pragma comment(lib, "winmm.lib") 

/************************************************************
// 駅勣なライブラリをリンクする
************************************************************/
#pragma comment( lib, "d3d9.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "Imm32.lib" )
#pragma comment(lib,"user32.lib")
/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* PreDef.h 预定义
*/

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define STRICT				//  型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN	//  ヘッダーからあまり使われない関数を省く
#define WINVER        0x501	//  Windows XP以降
#define _WIN32_WINNT  0x501 

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }
#define D3D_DEBUG_INFO		//  Direct3Dデバッグ情報の有効化

#include <windows.h>
#include <Windowsx.h>
#include <mmsystem.h> 
#pragma comment(lib,"Dsound.lib")
#pragma comment(lib, "winmm.lib") 
#include<fstream>
#include<iostream>

#include <crtdbg.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <dsound.h>
#include <tchar.h>

#include<Tlhelp32.h>
#include "resource.h"
#include "uImageDC.h"

// Live2D
#include "Live2D.h"
#include "util/UtSystem.h"
#include "Live2DModelD3D.h"
#include "LAppDefine.h"
#include "LAppDecode.h"

// Live2D Sample
#include "LAppRenderer.h"
#include "LAppLive2DManager.h"
#include "L2DViewMatrix.h"
#include "LAppModel.h"

using namespace live2d;
using namespace live2d::framework;
using namespace std;



/***********************************************************
	必要なライブラリをリンクする
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

/***********************************************************
	グローバル変数(アプリケーション関連)
************************************************************/
#define		USE_LIVE2D						1		//  Live2Dの使用フラグ（デバッグ用）
#define		CHANGE_FULLSCREEN_RESOLUTION	0		//  フルスクリーンの時に解像度を変更する場合 1
bool		g_bWindow		=  true ;				//  起動時の画面モード

HINSTANCE	g_hInstance		= NULL;					//  インスタンス・ハンドル
HWND		g_hWindow		= NULL;					//  ウインドウ・ハンドル
HMENU		g_hMenu			= NULL;					//  メニュー・ハンドル

HANDLE mThread;//消息进程句柄

WCHAR		g_szAppTitle[]	= L"Live2D Azusa";
WCHAR		g_szWndClass[]	= L"L2DFrm";

RECT		g_rectWindow;							//  ウインドウ・モードでの最後の位置

//  起動時の描画領域サイズ
SIZE		g_sizeWindowMode	= {  400 ,300 };	//  ウインドウ・モード

SIZE		g_sizeFullMode	= {  1280 , 720 };		//  フルスクリーン・モード
// SIZE		g_sizeFullMode	= {  1920 , 1080 };		// フルスクリーン・モード
D3DFORMAT	g_formatFull	= D3DFMT_X8R8G8B8;		//  ディスプレイ(バック・バッファ)・フォーマット

POINT pt, pe;
RECT rt, re;

//  アプリケーションの動作フラグ
bool		g_bActive		= false;	//  アクティブ状態
bool isRender=true;

bool Closing=false;
//模型操作同步
bool isRemove=false,isAdd=false;
char modelpath[500];



/***********************************************************
	グローバル変数(DirectX関連)
************************************************************/

//  インターフェイス
LPDIRECT3D9				g_pD3D			= NULL; //  Direct3Dインターフェイス
LPDIRECT3DDEVICE9		g_pD3DDevice	= NULL; //  Direct3DDeviceインターフェイス
D3DPRESENT_PARAMETERS	g_D3DPP;				//  D3DDeviceの設定(現在)

LPDIRECT3DSURFACE9      g_pd3dSurface   = NULL;//透明处理
LPDIRECT3DSURFACE9      g_psysSurface   = NULL;
D3DLOCKED_RECT	lockedRect;

RECT rc, rcSurface ;//rc窗口初始位置
int modelnum=0;//预读模型数
POINT ptWinPos ;
POINT ptSrc = { 0, 0};
SIZE szWin ;
BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
HDC		hdcWnd ;
D3DPRESENT_PARAMETERS	g_D3DPPWindow;			// D3DDeviceの設定(ウインドウ・モード用)
D3DPRESENT_PARAMETERS	g_D3DPPFull;			// D3DDeviceの設定(フルスクリーン・モード用)

bool g_bDeviceLost = false;						// デバイスの消失フラグ

CUImageDC			g_dcSurface;
//文本消息
ID3DXFont* Font		= 0;
ID3DXSprite* Sprite	= 0;
D3DXFONT_DESC lf; // Initialize a LOGFONT structure that describes the font
// we want to create.
LOGFONTW lf1;
RECT tt={0,0,40,20};
D3DCOLOR textcolor;
wchar_t message[500];




/***********************************************************
	Live2D関連
************************************************************/
static LAppRenderer*				s_renderer;
static LAppLive2DManager*			s_live2DMgr;



/***********************************************************
	関数定義
************************************************************/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

// 解像度変更
void ChangeFullscreenResolution() ;

HINSTANCE ThreadID;
//音频文件格式

struct RIFF_HEADER//文件类型
{
	char szRiffID[4];
	DWORD dwRiffSize;
	char szRiffFormat[4];
};

struct WAVE_FORMAT
{
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
};

struct FMT_BLOCK//格式
{
	char szFmtID[4]; // 'f','m','t',' '
	DWORD dwFmtSize;//////////////一般情况下为16，如有附加信息为18
	WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK//可先项 一般可不用
{
	char szFactID[4]; // 'f','a','c','t'
	DWORD dwFactSize;
};

struct DATA_BLOCK//数据头
{
	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;//数据长度
};

LPDIRECTSOUNDBUFFER g_lpdbsBuffer = NULL;
DSBUFFERDESC g_dsbd;
WAVEFORMATEX g_wfmx;
char* g_sndBuffer = NULL;
LPDIRECTSOUND g_lpds = NULL;
BOOL Loadwav(_TCHAR *FileName,UINT Flag);
#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS)
#endif

bool Playing=false;

int AzusaPid=-1;
int SIndex=0;

bool CheckAzusa(int mode);
bool CheckAzusa();

char Sp[1000];
wchar_t Spath[500];

void c2w(wchar_t *pwstr,size_t len,const char *str);
char *w2c(char *pcstr,const wchar_t *pwstr, size_t len);

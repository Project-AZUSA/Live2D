/**
 * Live2D SDK for DirectX / SampleApp1
 *
 * Live2Dのサンプルアプリケーションです
 * 
 * ＜機能＞
 * ・特定の部位をクリックすると表情変更、アクション等を行います
 * ・ドラッグで顔の向きを変更します
 * ・右クリックでキャラクターを変更します。
 * ・'W'キーでウインドウモード、フルスクリーンモードを切り替えます。
 * 
 *  このソースはLive2D関連アプリの開発用途に限り
 *  自由に改変してご利用頂けます。
 *
 *  (c) CYBERNOIDS Co.,Ltd. All rights reserved.
 */
//#pragma comment(linker, "/entry:wWinMain") 


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define STRICT				// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN	// ヘッダーからあまり使われない関数を省く
#define WINVER        0x501	// Windows XP以降
#define _WIN32_WINNT  0x501 

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }
#define D3D_DEBUG_INFO		// Direct3Dデバッグ情報の有効化

#include <windows.h>
#include <Windowsx.h>
#include <mmsystem.h> 
#pragma comment(lib, "winmm.lib") 
#include<fstream>
#include<iostream>
#include <crtdbg.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "resource.h"
#include "uImageDC.h"
// Live2D
#include "Live2D.h"
#include "util/UtSystem.h"
#include "Live2DModelD3D.h"

// Live2D Sample
#include "LAppRenderer.h"
#include "LAppLive2DManager.h"
#include "L2DViewMatrix.h"
#include "LAppModel.h"

/************************************************************
// 必要なライブラリをリンクする
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

/************************************************************
	グローバル変数(アプリケーション関連)
************************************************************/
#define		USE_LIVE2D						1		// Live2Dの使用フラグ（デバッグ用）
#define		CHANGE_FULLSCREEN_RESOLUTION	0		// フルスクリーンの時に解像度を変更する場合 1
using namespace std;
bool		g_bWindow		=  true ;				// 起動時の画面モード

HINSTANCE	g_hInstance		= NULL;					// インスタンス・ハンドル
HWND		g_hWindow		= NULL;					// ウインドウ・ハンドル
HMENU		g_hMenu			= NULL;					// メニュー・ハンドル
HANDLE mThread;//消息进程句柄
WCHAR		g_szAppTitle[]	= L"Azusa";
WCHAR		g_szWndClass[]	= L"L2DFrm";

RECT		g_rectWindow;							// ウインドウ・モードでの最後の位置

// 起動時の描画領域サイズ
SIZE		g_sizeWindowMode	= {  400 , 300 };	// ウインドウ・モード

SIZE		g_sizeFullMode	= {  1280 , 720 };		// フルスクリーン・モード
//SIZE		g_sizeFullMode	= {  1920 , 1080 };		// フルスクリーン・モード
D3DFORMAT	g_formatFull	= D3DFMT_X8R8G8B8;		// ディスプレイ(バック・バッファ)・フォーマット
		 POINT pt, pe;
 RECT rt, re;
// アプリケーションの動作フラグ
bool		g_bActive		= false;	// アクティブ状態
bool Closing=false;
//模型操作同步
bool isRemove=false,isAdd=false;
char modelpath[500];
/************************************************************
	グローバル変数(DirectX関連)
************************************************************/

// インターフェイス
LPDIRECT3D9				g_pD3D			= NULL; // Direct3Dインターフェイス
LPDIRECT3DDEVICE9		g_pD3DDevice	= NULL; // Direct3DDeviceインターフェイス
D3DPRESENT_PARAMETERS	g_D3DPP;				// D3DDeviceの設定(現在)
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
		ID3DXFont* Font   = 0;
		ID3DXSprite* Sprite = 0;
		D3DXFONT_DESC lf; // Initialize a LOGFONT structure that describes the font
                     // we want to create.
		LOGFONTW lf1;
		RECT tt={0,0,40,20};
		D3DCOLOR textcolor;
		wchar_t message[500];
/************************************************************
	Live2D関連
************************************************************/
static LAppRenderer*				s_renderer;
static LAppLive2DManager*			s_live2DMgr;

/************************************************************
	関数定義
************************************************************/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

//解像度変更
void ChangeFullscreenResolution() ;
HINSTANCE ThreadID;
//音频文件格式

struct RIFF_HEADER//文件类型

{char szRiffID[4];
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
/************************************************************
		アプリケーション初期化（最初に一度だけ呼ばれる）
************************************************************/
HRESULT InitApp(HINSTANCE hInst)
{
	// アプリケーションのインスタンス・ハンドルを保存
	g_hInstance = hInst;

	// IMEを禁止する
	//ImmDisableIME(-1);	// このスレッドで禁止(imm32.libをリンクする)

	// ウインドウ・クラスの登録
	WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szWndClass;
	wcex.hIconSm		= NULL;

	if (!RegisterClassEx(&wcex))
		return DXTRACE_ERR(L"InitApp", GetLastError());

	// メイン・ウインドウ作成
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;
	g_rectWindow.right	= g_sizeWindowMode.cx;
	g_rectWindow.bottom	= g_sizeWindowMode.cy;
	AdjustWindowRect(&g_rectWindow, WS_OVERLAPPEDWINDOW, TRUE);
	g_rectWindow.right	= g_rectWindow.right - g_rectWindow.left;
	g_rectWindow.bottom	= g_rectWindow.bottom - g_rectWindow.top;
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;

	RECT rect;
	if (g_bWindow)
	{
		// (ウインドウ・モード用)
		rect.top	= CW_USEDEFAULT;
		rect.left	= CW_USEDEFAULT;
		rect.right	= g_rectWindow.right;
		rect.bottom	= g_rectWindow.bottom;
	}
	else
	{
		// (フルスクリーン・モード用)
		rect.top	= 0;
		rect.left	= 0;
		rect.right	= g_sizeFullMode.cx;
		rect.bottom	= g_sizeFullMode.cy;

		g_hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU1));

		//解像度変更
		ChangeFullscreenResolution() ;
	}

	g_hWindow = CreateWindowEx(WS_EX_NOACTIVATE|WS_EX_TOPMOST,g_szWndClass, g_szAppTitle,
		WS_POPUP,
			rc.left, rc.top, rc.right, rc.bottom,
			NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return DXTRACE_ERR(L"InitApp", GetLastError());

		g_dcSurface.Create(g_sizeWindowMode.cx, -g_sizeWindowMode.cy);
	   SetWindowLong(g_hWindow, GWL_EXSTYLE, (GetWindowLong(g_hWindow, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED);
   // ウインドウ表示
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);
	::GetWindowRect(g_hWindow, &rc);
	hdcWnd = GetWindowDC(g_hWindow);
	return S_OK;
}

/************************************************************
	Setup Live2D
************************************************************/
void SetupLive2D()
{
#if USE_LIVE2D

	//Live2Dモデルのロード
	s_live2DMgr=new LAppLive2DManager();

	//Live2D描画用クラス初期化
	s_renderer = new LAppRenderer();
	s_renderer->setLive2DManager(s_live2DMgr);

	//レンダラのサイズを指定
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;
	fstream f("res\\model.txt",ios::in);
	if(f)
	{
	for (int i = 0; i < modelnum; i++)
	{
		 char path[200];
		f.getline(path,200,';');
		LAppModel* model=new LAppModel();
		s_live2DMgr->models.push_back(model);
		model->load(path);
	}
	f.close();
	}
	//s_live2DMgr->changeModel();
#endif
}

/************************************************************
	Cleanup Live2D
************************************************************/
void CleanupLive2D(void){
#if USE_LIVE2D
	delete s_renderer;
	delete s_live2DMgr;
#endif
}

/************************************************************
	Render Live2D
************************************************************/
VOID RenderLive2D(){
#if USE_LIVE2D
	if( ! s_live2DMgr ) return ;
		
	// サンプルでは、画面のLeft Top (-1,1) , Right Bottom (1,-1) , z = 0 となるViewを前提にLive2Dを描画します。
	// アプリケーションの仕様がことなる場合は、Live2Dの描画の前にワールド座標を変換し、上記の空間に合うように
	// 変換してから呼び出して下さい。
	int numModels=s_live2DMgr->getModelNum();
	for (int i=0; i<numModels; i++)
	{
		LAppModel* model = s_live2DMgr->getModel(i);
		model->live2DModel->setDevice(g_pD3DDevice);
	}
	//-- モデルを描画 --
	s_renderer->draw() ;

#endif
}

/************************************************************
	Device Lost Live2D
************************************************************/
VOID OnLostDeviceLive2D( ){
#if USE_LIVE2D
	if( s_live2DMgr ){
		s_live2DMgr->deviceLost() ;
	}
#endif
}


/************************************************************
	DirectX Graphics初期化
************************************************************/
HRESULT InitDXGraphics(void)
{
	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
		return DXTRACE_ERR(L"InitDXGraphics Direct3DCreate9", E_FAIL);

	// D3DDeviceオブジェクトの設定(ウインドウ・モード用)
	ZeroMemory(&g_D3DPPWindow, sizeof(g_D3DPPWindow));

	g_D3DPPWindow.BackBufferWidth			= 0;
	g_D3DPPWindow.BackBufferHeight			= 0;
	g_D3DPPWindow.BackBufferFormat			= D3DFMT_UNKNOWN;
	g_D3DPPWindow.BackBufferCount			= 1;
	g_D3DPPWindow.MultiSampleType			= D3DMULTISAMPLE_NONE;
	g_D3DPPWindow.MultiSampleQuality		= 0;
	g_D3DPPWindow.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	g_D3DPPWindow.hDeviceWindow				= g_hWindow;
	g_D3DPPWindow.Windowed					= TRUE;
	g_D3DPPWindow.EnableAutoDepthStencil	= FALSE;
	g_D3DPPWindow.AutoDepthStencilFormat	= D3DFMT_UNKNOWN;
	g_D3DPPWindow.Flags						= 0;
	g_D3DPPWindow.FullScreen_RefreshRateInHz= 0;
	//g_D3DPPWindow.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	g_D3DPPWindow.PresentationInterval		= D3DPRESENT_INTERVAL_ONE;

	// D3DDeviceオブジェクトの設定(フルスクリーン・モード)
	ZeroMemory(&g_D3DPPFull, sizeof(g_D3DPPFull));

	g_D3DPPFull.BackBufferWidth				= g_sizeFullMode.cx;
	g_D3DPPFull.BackBufferHeight			= g_sizeFullMode.cy;
	g_D3DPPFull.BackBufferFormat			= g_formatFull;
	g_D3DPPFull.BackBufferCount				= 1;
	g_D3DPPFull.MultiSampleType				= D3DMULTISAMPLE_NONE;
	g_D3DPPFull.MultiSampleQuality			= 0;
	g_D3DPPFull.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	g_D3DPPFull.hDeviceWindow				= g_hWindow;
	g_D3DPPFull.Windowed					= FALSE;
	g_D3DPPFull.EnableAutoDepthStencil		= FALSE;
	g_D3DPPFull.AutoDepthStencilFormat		= D3DFMT_UNKNOWN;
	g_D3DPPFull.Flags						= 0;
	g_D3DPPFull.FullScreen_RefreshRateInHz	= 0;
	g_D3DPPFull.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
//	g_D3DPPFull.PresentationInterval		= D3DPRESENT_INTERVAL_ONE;

	// D3DDeviceオブジェクトの作成
	if (g_bWindow)
		g_D3DPP = g_D3DPPWindow;
	else
		g_D3DPP = g_D3DPPFull;

	HRESULT hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWindow,
						D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
	if (FAILED(hr))
	{
		hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWindow,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
		if (FAILED(hr))
		{
			hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_hWindow,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
			if (FAILED(hr))
				return DXTRACE_ERR(L"InitDXGraphics CreateDevice", hr);
		}
	}

	// ビューポートの設定
	D3DVIEWPORT9 vp;
	vp.X		= 0;
	vp.Y		= 0;
	vp.Width	= g_D3DPP.BackBufferWidth;
	vp.Height	= g_D3DPP.BackBufferHeight;
	vp.MinZ		= 0.0f;
	vp.MaxZ		= 1.0f;
	hr = g_pD3DDevice->SetViewport(&vp);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDXGraphics SetViewport", hr);

	//透明背景处理
		 //create and set the render target surface
	 //it should be lockable on XP and nonlockable on Vista
	if (FAILED(g_pD3DDevice->CreateRenderTarget(g_sizeWindowMode.cx, g_sizeWindowMode.cy, 
		D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, 
		/*!g_is9Ex*/false, // lockable
		&g_pd3dSurface, NULL)))
	{
		return NULL;
	}
	g_pD3DDevice->SetRenderTarget(0, g_pd3dSurface);

	// turn off culling to view both sides of the triangle
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// turn off D3D lighting to use vertex colors
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// Enable alpha blending.
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// Set the source blend state.
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// Set the destination blend state.
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return S_OK;
}


/************************************************************
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
************************************************************/
VOID SetupMatrices()
{
   // Set up world matrix
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );

	D3DXMATRIX Ortho2D;     
	D3DXMATRIX Identity;
    
	int w , h ;
	if( g_bWindow ){
		w = g_sizeWindowMode.cx ;
		h = g_sizeWindowMode.cy ;
	}
	else{
		w = g_sizeFullMode.cx ;
		h = g_sizeFullMode.cy ;
	}

	//--- Live2Dサンプル用に空間を初期化 ---
	//
	// サンプルでは、画面のLeft Top (-1,1) , Right Bottom (1,-1) , z = 0 となるViewを前提にLive2Dを描画します。
	// アプリケーションの仕様がことなる場合は、Live2Dの描画の前にワールド座標を変換し、上記の空間に合うように
	// 変換してから呼び出して下さい。
	L2DViewMatrix*	viewMatrix = s_renderer->getViewMatrix() ;
	D3DXMatrixOrthoOffCenterLH(&Ortho2D
		, viewMatrix->getScreenLeft()  
		, viewMatrix->getScreenRight()  
		, viewMatrix->getScreenBottom()  
		, viewMatrix->getScreenTop() , -1.0f, 1.0f);

	D3DXMatrixIdentity(&Identity);

	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &Ortho2D);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &Identity);
	g_pD3DDevice->SetTransform(D3DTS_VIEW , &Identity);

	//---- Live2D ViewMatrixの座標変換（拡大縮小等が適用される） ----
	float* trGL = viewMatrix->getArray() ;
	g_pD3DDevice->MultiplyTransform(D3DTS_WORLD,(D3DXMATRIXA16*)trGL) ;

	////---- ビューポート ----
	//D3DVIEWPORT9 vp ;	
	//vp.X = 0 ;
	//vp.Y = 0 ;
	//vp.Width  = w ;
	//vp.Height = h ;
	//vp.MinZ = 0.0f ;
	//vp.MaxZ = 1.0f ;
	////ビューポートセット
	//g_pD3DDevice->SetViewport(&vp);

}
bool isRender=true;

/************************************************************
	画面の描画処理
************************************************************/

HRESULT Render(void)
{
	// シーンのクリア
		g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	//同步模型	
   if(isRemove)
   {
	   s_live2DMgr->releaseModel();
	   isRemove=false;
   }
   else if(isAdd)
   {
   		LAppModel* model=new LAppModel();
		s_live2DMgr->models.push_back(model);
		model->load(modelpath);
		isAdd=false;
	
   }

	// シーンの描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{

		SetupMatrices() ;
		
		RenderLive2D() ;

		Font->DrawText(
		
    Sprite,//编译时发现通不过，查阅该函数有6个参数，少了第一个，开头补上，类型为ID3DXSprite* Sprite = 0;
	(LPCWSTR)message, 
    -1, // size of string or -1 indicates null terminating string
	&tt,            // rectangle text is to be formatted to in windows coords
    DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
	textcolor);      // black text
		// シーンの描画終了
		g_pD3DDevice->EndScene();
	}
	// Present the backbuffer contents to the display
	//透明背景处理
	  //GetWindowRect(g_hWindow,&rt); // 获取窗口位置与大小
	g_pD3DDevice->CreateOffscreenPlainSurface(g_sizeWindowMode.cx, g_sizeWindowMode.cy, 
		D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &g_psysSurface, NULL);
	g_pD3DDevice->GetRenderTargetData(g_pd3dSurface, g_psysSurface);
	g_psysSurface->LockRect(&lockedRect, &rcSurface, D3DLOCK_READONLY);
	memcpy(g_dcSurface.GetBits(), lockedRect.pBits, 4 * g_sizeWindowMode.cx * g_sizeWindowMode.cy);
	UpdateLayeredWindow(g_hWindow, hdcWnd, NULL, &szWin, g_dcSurface.GetSafeHdc(), &ptSrc, 0, &stBlend, ULW_ALPHA);
	g_psysSurface->UnlockRect();
	g_psysSurface->Release();
	//GetWindowRect(g_hWindow,&rc);

	// シーンの表示
	return S_OK;
}

/************************************************************
	D3Dに管理されないオブジェクトの終了処理
************************************************************/
HRESULT CleanupD3DObject(void)
{
	OnLostDeviceLive2D() ;

	return S_OK;
}

/************************************************************
	ウインドウ・サイズの変更
************************************************************/
HRESULT ChangeWindowSize(void)
{
	// ウインドウのクライアント領域に合わせる
	CleanupD3DObject();

	HRESULT hr = g_pD3DDevice->Reset(&g_D3DPP);
	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			g_bDeviceLost = true;
		else
			DestroyWindow(g_hWindow);
		return DXTRACE_ERR(L"ChangeWindowSize Reset", hr);
	}

	g_sizeWindowMode.cx = g_D3DPP.BackBufferWidth ;
	g_sizeWindowMode.cy = g_D3DPP.BackBufferHeight ;

	// ビューポートの設定
	D3DVIEWPORT9 vp;
	vp.X		= 0;
	vp.Y		= 0;
	vp.Width	= g_D3DPP.BackBufferWidth;
	vp.Height	= g_D3DPP.BackBufferHeight;
	vp.MinZ		= 0.0f;
	vp.MaxZ		= 1.0f;
	hr = g_pD3DDevice->SetViewport(&vp);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"ChangeWindowSize SetViewport", hr);
		DestroyWindow(g_hWindow);
	}

	//レンダラのサイズを指定
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;

	return hr;
}

/************************************************************
	画面モードの変更
************************************************************/
void ChangeDisplayMode(void)
{
	g_bWindow = !g_bWindow;

	CleanupD3DObject();

	if (g_bWindow)
	{
		g_D3DPP = g_D3DPPWindow;
	}
	else
	{
		g_D3DPP = g_D3DPPFull;
		GetWindowRect(g_hWindow, &g_rectWindow);

		//解像度変更
		ChangeFullscreenResolution() ;
	}

	HRESULT hr = g_pD3DDevice->Reset(&g_D3DPP);
	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			g_bDeviceLost = true;
		else
			DestroyWindow(g_hWindow);
		DXTRACE_ERR(L"ChangeDisplayMode Reset", hr);
		return;
	}

	if (g_bWindow)
	{
		SetWindowLong(g_hWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		if(g_hMenu != NULL)
		{
			SetMenu(g_hWindow, g_hMenu);
			g_hMenu = NULL;
		}
		SetWindowPos(g_hWindow, HWND_NOTOPMOST,
				g_rectWindow.left, g_rectWindow.top,
				g_rectWindow.right - g_rectWindow.left,
				g_rectWindow.bottom - g_rectWindow.top,
				SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowLong(g_hWindow, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		if(g_hMenu == NULL)
		{
			g_hMenu = GetMenu(g_hWindow);
			SetMenu(g_hWindow, NULL);
		}
	}
	//レンダラのサイズを指定
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;

}


/************************************************************
	DirectX Graphicsの終了処理(初期化に失敗したときも呼ばれる)
************************************************************/
bool CleanupDXGraphics(void)
{
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pD3D);

	return true;
}

/************************************************************
	アプリケーションの終了処理（最後に呼ばれる）
************************************************************/
bool CleanupApp(void)
{
	// メニュー・ハンドルの削除
	if (g_hMenu)
		DestroyMenu(g_hMenu);

	// ウインドウ・クラスの登録解除
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/************************************************************
	ウィンドウ処理
************************************************************/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch(msg)
	{
	case WM_ACTIVATE:
		g_bActive = (LOWORD(wParam) != 0);
		break;

	case WM_DESTROY:
		// D3Dに管理されないオブジェクトの終了処理
		CleanupD3DObject();

		// Live2Dの破棄
		CleanupLive2D() ;

		// DirectX Graphicsの終了処理
		CleanupDXGraphics();
		// ウインドウを閉じる
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// ウインドウ・サイズの変更処理
	case WM_SIZE:
		if (g_D3DPP.Windowed != TRUE)
			break;

		if (!g_pD3DDevice || wParam == SIZE_MINIMIZED)
			break;
		g_D3DPP.BackBufferWidth  = LOWORD(lParam);
		g_D3DPP.BackBufferHeight = HIWORD(lParam);

		if(g_bDeviceLost)
			break;
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			ChangeWindowSize();
		break;

	case WM_SETCURSOR:
		if (g_D3DPP.Windowed != TRUE)
		{
			SetCursor(NULL);
			return 1;
		}
		break;

	case WM_KEYDOWN:
		// キー入力の処理
		switch(wParam)
		{
		
		}
		break;

	//case WM_RBUTTONDOWN :
	////	s_live2DMgr->changeModel() ;
	//	break;

case WM_RBUTTONDOWN:
  SetCapture(hWnd); // 设置鼠标捕获(防止光标跑出窗口失去鼠标热点)
  GetCursorPos(&pt); // 获取鼠标光标指针当前位置
  GetWindowRect(hWnd,&rt); // 获取窗口位置与大小
  re.right=rt.right-rt.left; // 保存窗口宽度
  re.bottom=rt.bottom-rt.top; // 保存窗口高度
  
  break;
case WM_RBUTTONUP:
  ReleaseCapture(); // 释放鼠标捕获，恢复正常状态
//MoveWindow(hWnd,re.left,re.top,re.right,re.bottom,true); // 移动窗口

  break;

	case WM_LBUTTONDOWN :
		/*SystemParametersInfo(SPI_SETDRAGFULLWINDOWS,FALSE,NULL,0);
		SendMessage(hWnd, WM_SYSCOMMAND, 0xF012, 0);  
		*/
		if( (wParam & MK_LBUTTON) != 0 ){
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 

			s_renderer->mousePress( xPos , yPos ) ;
		}
	
		break;
	case WM_ERASEBKGND:
		break;
		
	PAINTSTRUCT ps;
	HDC hdc;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_MOUSEMOVE :
		if( (wParam & MK_LBUTTON) != 0 ){
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 
				
			s_renderer->mouseDrag( xPos , yPos ) ;
		}
			GetCursorPos(&pe); // 获取光标指针的新位置
  if(wParam==MK_RBUTTON) // 当鼠标左键按下
  {
   re.left=rt.left+(pe.x - pt.x); // 窗口新的水平位置
   re.top =rt.top+(pe.y - pt.y); // 窗口新的垂直位置
   MoveWindow(hWnd,re.left,re.top,re.right,re.bottom,true); // 移动窗口
   //SetWindowPos(hWnd,HWND_TOPMOST,re.left,re.top,re.right,re.bottom,SWP_NOACTIVATE);
  }

		break;

	case WM_MOUSEWHEEL :
		{
			//ホイール
			int delta = GET_WHEEL_DELTA_WPARAM(wParam); 
			
			//スクリーン座標からローカル座標に変換
			POINT cursor;			
			cursor.x = GET_X_LPARAM(lParam); 
			cursor.y = GET_Y_LPARAM(lParam);
			ScreenToClient( hWnd, &cursor);

			s_renderer->mouseWheel( delta , cursor.x , cursor.y ) ;
		}

		break;

	case WM_COMMAND:
		// 選択されたメニューを実行
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			return 0;
		}
		break;
		default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// デフォルト処理
	return 0;
}
bool Live2DAbort(HINSTANCE hinst)
{
	if(ThreadID==hinst)
	{
		Closing=true;
		//PostMessage(g_hWindow, WM_CLOSE, 0, 0);
	return true;
	}
	else

		return false;
}
//添加模型--同步render
//返回模型index
 int AddModel(HINSTANCE hinst, char* path)
{
		if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
         if(isAdd)
			 return -1;//上一个模型还未添加
		isAdd=true;
        strcpy(modelpath,path);


	}
	catch(...)
	{
		return -1;
	}
	return s_live2DMgr->models.size();
	}
	else

	return -1;
}
//删除所有模型
bool RemoveModels(HINSTANCE hinst)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
		isRemove=true;
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
//返回模型文件
//func为要调用的内容
 bool GetModelPath(HINSTANCE hinst,int index)
{
	
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
        
		cout<<s_live2DMgr->models[index]->ModelPath<<endl;
		return true;
	}
	catch(...)
	{
		return false;
	}
	
	}
	else

		return false;
}
//设置表情
 bool SetExpression(HINSTANCE hinst, const char expid[], int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
		LAppModel* currentModel=s_live2DMgr->models[index];
	currentModel->setExpression(expid);
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
//设置动作
 bool StartMotion(HINSTANCE hinst, const char motiontype[],int motionindex,int priority, int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
		LAppModel* currentModel=s_live2DMgr->models[index];
		currentModel->startMotion(motiontype,motionindex,priority);
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
//设置眼睛朝向
 bool SetEyeBallDirection(HINSTANCE hinst, float x,float y,int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
	 LAppModel* model=	s_live2DMgr->getModel(index);
	 model->eyeX=x;//-1から1の値を加える
		model->eyeY=y;
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
//设置身体朝向
 bool SetBodyDirection(HINSTANCE hinst, float x,int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
	LAppModel* model=	s_live2DMgr->getModel(index);
	model->bodyX=x;//-1から1の値を加える
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}

//设置脸朝向
 bool SetFaceDirection(HINSTANCE hinst, float x,float y,float z,int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
	LAppModel* model=	s_live2DMgr->getModel(index);
	
	model->faceX=x;//-30から30の値を加える
	model->faceY=y;
	model->faceZ=z;
		
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
//设置脸朝向
 bool SetViewDepth(HINSTANCE hinst, float x,float y,float z,int index)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
		::s_renderer->mouseWheel(z,x,y);
		
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
 //设置模型嘴部
 //参数1：张嘴参数值0-1，参数2：模型index
 bool SetMouthOpen(float val,int index)
 {
  try
	 {
		LAppModel* model=	s_live2DMgr->getModel(index);
		
		model->mouthY=val;
		 return true;
	 }
	 catch(...){
		 return false;
	 }
 }

 //设置模型参数
 //参数1：要设置的参数名称，参数2：参数值，参数3：权值，参数4：模型索引
 bool SetModelParameter(char para[],float val,float weight,int index)
 {
	 try
	 {
		LAppModel* model=	s_live2DMgr->getModel(index);
		
		strcpy(model->paraname,para);
		model->paraval=val;
		model->paraweight=weight;
		 return true;
	 }
	 catch(...){
		 return false;
	 }
 }
 //播放音效
 bool PlayModelSound(wchar_t path[],char p[],int index)
 {
 	 try
	 {
		LAppModel* model=	s_live2DMgr->getModel(index);
		RIFF_HEADER riff;
WAVE_FORMAT wform;
FMT_BLOCK fmt;
FACT_BLOCK fact;
DATA_BLOCK data;

FILE* file=fopen(p,"rb");
if(!file)
{
cout<<"文件不存在"<<endl;return false;
}
fread(&riff,sizeof(RIFF_HEADER),1,file);//读RIFF_HEADER 

if(riff.szRiffFormat[0]!='W'||riff.szRiffFormat[1]!='A'&&riff.szRiffFormat[2]!='V'&&riff.szRiffFormat[3]!='E')
{cout<<"音频格式非法"<<endl;return false;}

fread(&fmt,sizeof(FMT_BLOCK),1,file);//读FMT_BLOCK
if(fmt.dwFmtSize==18)//有额外信息需要读掉，否则后面会出错
{
	WORD extra;
	fread(&extra,sizeof(WORD),1,file);
	if(extra>0)
	{
	BYTE *ed=new BYTE[extra];
	fread(ed,sizeof(BYTE),extra,file);
	delete[] ed;
	}
}
while(true)//循环读入文件块，直到读到音频数据
{
	fread(&fact.szFactID,sizeof(char),4,file);//读下一块名称
fread(&fact.dwFactSize,sizeof(DWORD),1,file);//读下一长度
if(fact.szFactID[0]=='d'&&fact.szFactID[1]=='a'&&fact.szFactID[2]=='t'&&fact.szFactID[3]=='a')
{

	data.szDataID[0]='d';data.szDataID[1]='a';data.szDataID[2]='t';data.szDataID[3]='a';
data.dwDataSize=fact.dwFactSize;
break;
}
else
{
	BYTE * fd=new BYTE[fact.dwFactSize];
	fread(fd,sizeof(BYTE),fact.dwFactSize,file);
	delete[] fd;
}
}
WORD bytes = (WORD)((fmt.wavFormat.wBitsPerSample + 7) / 8);
DWORD samplenum=data.dwDataSize/fmt.wavFormat.wChannels/bytes;
BYTE *bd;
short *sd;
if(bytes==1)
{
 bd=new BYTE[samplenum];
fread(bd,sizeof(BYTE),samplenum,file);//写音频数据
}
else
{
 sd=new short[samplenum];
 fread(sd,sizeof(short),samplenum,file);
}
DWORD i;
float *dm=new float[samplenum];//采样频率
float df = 1.0f / (1l << (fmt.wavFormat.wBitsPerSample- 1));
					for (i = 0; i < samplenum; i ++ ) 
						dm[i] = ((bytes == 1) ? bd[i] - 128 : sd[i]) * df;
					if (bytes == 1) delete[] bd; else delete[] sd;
fclose(file);
WORD count=0;
//5采样计算平均值以便同步
int freq=100;//采样频率
DWORD mouthnum=samplenum/(fmt.wavFormat.dwSamplesPerSec/freq)+1;
float *ma=new float[mouthnum];
for(i=0;i<mouthnum;i++)
{
float avg=0;
for(DWORD j=i*fmt.wavFormat.dwSamplesPerSec/freq;j<(i+1)*fmt.wavFormat.dwSamplesPerSec/freq&&j<samplenum;j++)
{
	avg+=fabs(dm[j]);
}
ma[i]=avg/(fmt.wavFormat.dwSamplesPerSec/freq);
}
delete[] dm;
		model->isSpeaking=true;
		bool play=sndPlaySound(path,SND_ASYNC);
		for(i=0;i<mouthnum;i++)
		{
			
			model->mouthY=ma[i]*5;
			Sleep(10);
		}
		delete[] ma;
		model->isSpeaking=false;
		 return play;
	 }
	 catch(...){
		 return false;
	 }
 }
//显示文本
 bool ShowMessage(HINSTANCE hinst, int x,int y,int width,int height,wchar_t * msg,int fontHeight,int fontWidth,int fontWeight,bool italic,wchar_t * family,D3DCOLOR color)
{
	if(ThreadID==hinst)//检测当前调用进程是否合法
	{
	try{
		
		DeleteObject(Font);
		D3DXCreateFont(g_pD3DDevice,	fontHeight,fontWidth,fontWeight,0,italic,DEFAULT_CHARSET,0,0,0,(LPCWSTR)family, &Font);// 编译无法通过，发现第2个参数是结构体D3DXFONT_DESCA类型，重新定义并赋值;
		tt.left=x;tt.top=y;tt.right=width;tt.bottom=height;
		textcolor=color;
		wcscpy(message,msg);
	}
	catch(...)
	{
		return false;
	}
	return true;
	}
	else

	return false;
}
 //将wchar_t* 转成char*的实现函数如下：

char *w2c(char *pcstr,const wchar_t *pwstr, size_t len)

{

int nlength=wcslen(pwstr);

//获取转换后的长度

int nbytes = WideCharToMultiByte( 0, // specify the code page used to perform the conversion

0,         // no special flags to handle unmapped characters

pwstr,     // wide character string to convert

nlength,   // the number of wide characters in that string

NULL,      // no output buffer given, we just want to know how long it needs to be

0,

NULL,      // no replacement character given

NULL );    // we don't want to know if a character didn't make it through the translation

// make sure the buffer is big enough for this, making it larger if necessary

if(nbytes>len)   nbytes=len;

// 通过以上得到的结果，转换unicode 字符为ascii 字符

WideCharToMultiByte( 0, // specify the code page used to perform the conversion

0,         // no special flags to handle unmapped characters

pwstr,   // wide character string to convert

nlength,   // the number of wide characters in that string

pcstr, // put the output ascii characters at the end of the buffer

nbytes,                           // there is at least this much space there

NULL,      // no replacement character given

NULL );

return pcstr ;

}
//将char* 转成wchar_t*的实现函数如下：

//这是把asii字符转换为unicode字符，和上面相同的原理

void c2w(wchar_t *pwstr,size_t len,const char *str)

{

if(str)

    {

      size_t nu = strlen(str);

	  size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,NULL,0);

      if(n>=len)n=len-1;

	  MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,pwstr,(int)n);

   pwstr[n]=0;

    }

}
//读取参数--多个参数时使用
//str为输入字符串，para为接受参数的字符串，index为第几个参数从1开始
void ReadParameter(char* arg,char* para,int index)
{
	int i,count=index,begin=0;

	for(i=0;count>0||i<strlen(arg);i++)
	{
	   if(arg[i]==','||arg[i]=='\0')
	   {
	   count--;
	   if(count>0)
		   begin=i+1;
	   else
		   break;
	   }
	}
	
	strncpy(para,arg+begin,i-begin);
	para[i-begin]='\0';
}
//消息进程--用于处理Azusa发来的消息
DWORD WINAPI MessageThreadProc( LPVOID lpParameter )
{
	while(!Closing)
	   {
 		char str[1000];//读入命令
	
		cin.getline(str,1000,'\n');
	
		//	c2w(message,500,str);
	//	ShowMessage(ThreadID,0,0,400,50,message,30,15,10,false,L"微软雅黑",0xffff0000);
		//初步检测命令的合法性
		if(strlen(str)<=1)
		{
			cout<<"请输入命令"<<endl;
			continue;
		}
			int i=0;
		for( i=0;i<strlen(str);i++)
			if(str[i]=='(')
				break;
		if(i==strlen(str))
		{
		cout<<"非法的命令格式，找不到（）"<<endl;
		continue;
		}
		//命令解析
		char cmd[100],arg[900];
		strncpy(cmd,str,i);
		cmd[i]='\0';
		if(strlen(str)-1-i>1)//有参数
		strncpy(arg,str+i+1,strlen(str)-i-2);
		arg[strlen(str)-i-2]='\0';
		//命令switch
		//终止live2d
		if(strcmp("UI_Live2DAbort",cmd)==0)
		{
		 cout<<"正在关闭"<<endl;
		 Live2DAbort(ThreadID);
		 continue;
		}
		    //添加模型
        //参数1：要添加的模型路径
        //添加模型需保证上一次添加的模型已添加完毕（返回非-1），若返回-1表示上次添加的模型未添加，这是由于模型添加不能在渲染中添加，需等一次渲染结束后添加
		if(strcmp("UI_AddModel",cmd)==0)
		{
		  char path[1000];
		  ReadParameter(arg,path,1);
		       fstream _file;
         _file.open(path,ios::in);
		  if(!_file)
         {
         cout<<path<<"不存在"<<endl;
          }
        else
       {
		       cout<<"正在加载"<<path<<endl;
		  _file.close();
         if(  AddModel(ThreadID,path))
			 cout<<"模型将在下一帧添加"<<endl;
		 else
			 cout<<"添加失败请重试"<<endl;
        }
		 continue;
		}
	    //删除所有模型
        //更换模型请先清除再添加，官方例程也是如此操作，其原因不明
		if(strcmp("UI_RemoveModels",cmd)==0)
		{
		  cout<<"正在清除所有模型"<<endl;
		  if(RemoveModels(ThreadID))
			  cout<<"删除模型成功过"<<endl;
		  else
			cout<<"删除模型失败"<<endl;
		  continue;
		}
		//获得模型的JSON路径
		//参数1：模型索引
		if(strcmp("UI_GetModelPath",cmd)==0)
		{
			char num[10];
			int index;
			ReadParameter(arg,num,1);
			index=atoi(num);
			if(GetModelPath(ThreadID,index))
				cout<<"已返回模型"<<num<<"模型json"<<endl;
			else
				cout<<"获取模型信息失败"<<endl;
			continue;
		}
		//设置表情
		//参数1：表情名称，参数2：模型索引
		if(strcmp("UI_SetExpression",cmd)==0)
		{
			char num[10];
			char expression[100];
			int index;
			ReadParameter(arg,expression,1);
			ReadParameter(arg,num,2);
			index=atoi(num);
			if(SetExpression(ThreadID,expression,index))
			cout<<"模型"<<num<<"已执行表情"<<expression<<endl;
			else
				cout<<"表情设置失败"<<endl;
			continue;
		}
		//设置动作
		//参数1：动作类型名称，参数2：动作索引，参数3：优先级，参数4：模型索引
		if(strcmp("UI_SetMotion",cmd)==0)
		{
			char num[10];
			char mnum[10];
			char pnum[10];
			char motion[100];
			int index,mindex,priority;
			ReadParameter(arg,motion,1);
			ReadParameter(arg,mnum,2);
			ReadParameter(arg,pnum,3);
			ReadParameter(arg,num,4);
			mindex=atoi(mnum);
			priority=atoi(pnum);
			index=atoi(num);
			if(StartMotion(ThreadID,motion,mindex,priority,index))
			cout<<"模型"<<num<<"已执行动作"<<motion<<mnum<<endl;
			else
				cout<<"设置动作失败"<<endl;
			continue;
		}
		//设置模型参数
		//参数1：参数名称，参数2：参数值，参数3：参数权值（作用不明），参数4：模型索引
		if(strcmp("UI_SetParameter",cmd)==0)
		{
			char num[10];
			char vnum[10];
			char wnum[10];
			char para[100];
			int index;
			float value,weight;
			ReadParameter(arg,para,1);
			ReadParameter(arg,vnum,2);
			ReadParameter(arg,wnum,3);
			ReadParameter(arg,num,4);
			value=atof(vnum);
			weight=atof(wnum);
			index=atoi(num);
			if(SetModelParameter(para,value,weight,index))
			cout<<"模型"<<num<<"已设置参数"<<para<<endl;
			else
				cout<<"设置参数失败"<<endl;
			continue;
		}
		//设置嘴部参数
		//参数1：参数值，参数2：模型索引
		if(strcmp("UI_SetMouthOpen",cmd)==0)
		{
			char num[10];
		   char vnum[20];
			int index;
			float val;
			ReadParameter(arg,vnum,1);
			ReadParameter(arg,num,2);
			val=atof(vnum);
			index=atoi(num);
			if(SetMouthOpen(val,index))
			cout<<"模型"<<num<<"Mouth_Y:"<<val<<endl;
			else
				cout<<"嘴部设置失败"<<endl;
			continue;
		}
		//播放音频
		//参数1：wav文件路径，参数2：模型index
		if(strcmp("UI_PlaySound",cmd)==0)
		{
			char path[1000];
		    wchar_t wp[500];
			char num[10];
			int index;
			float val;
			ReadParameter(arg,path,1);
			ReadParameter(arg,num,2);
			index=atoi(num);
			c2w(wp,500,path);

			if(PlayModelSound(wp,path,index))
				cout<<"播放音效"<<path<<endl;
			else
				cout<<"播放失败"<<endl;
			continue;
		}
		//设置眼睛朝向
		//参数1：眼睛X，参数2：眼睛Y，参数3：模型索引
		if(strcmp("UI_SetEyeBalls",cmd)==0)
		{
			char num[10];
		   char xnum[20];
		   char ynum[20];
			int index;
			float x,y;
			ReadParameter(arg,num,3);
			ReadParameter(arg,xnum,1);
			ReadParameter(arg,ynum,2);
			x=atof(xnum);
			y=atof(ynum);
			index=atoi(num);
			if(SetEyeBallDirection(ThreadID,x,y,index))
			cout<<"模型"<<num<<"  EYE_X:"<<xnum<<"  EYE_Y:"<<ynum<<endl;
			else
				cout<<"眼睛设置失败"<<endl;
			continue;
		}
		//设置身体朝向
		//参数1：身体X，参数2：模型索引
		if(strcmp("UI_SetBody",cmd)==0)
		{
			char num[10];
		   char xnum[20];
	
			int index;
			float x;
			ReadParameter(arg,num,2);
			ReadParameter(arg,xnum,1);
			
			x=atof(xnum);
		
			index=atoi(num);
			if(SetBodyDirection(ThreadID,x,index))
			cout<<"模型"<<num<<"  BODY_X:"<<xnum<<endl;
			else
				cout<<"身体设置失败"<<endl;
			continue;
		}
		//设置头部朝向
		//参数1：脸X，参数2：脸Y，参数3：脸Z，参数4：模型索引
		if(strcmp("UI_SetFace",cmd)==0)
		{
			char num[10];
		   char xnum[20];
		   char ynum[20];
		   char znum[20];
			int index;
			float x,y,z;
			ReadParameter(arg,num,4);
			ReadParameter(arg,xnum,1);
			ReadParameter(arg,ynum,2);
			ReadParameter(arg,znum,3);
			x=atof(xnum);
			y=atof(ynum);
			z=atof(znum);
			index=atoi(num);
			if(SetFaceDirection(ThreadID,x,y,z,index))
			cout<<"模型"<<num<<"  FACE_X:"<<xnum<<"  FACE_Y:"<<ynum<<"  FACE_Z："<<znum<<endl;
			else
				cout<<"脸部设置失败"<<endl;
			continue;
		}
		//没有匹配的命令
		cout<<"找不到命令"<<cmd<<endl;
	   }
		return 0;
}
/************************************************************
	アイドル時の処理
************************************************************/
bool AppIdle(void)
{
	if (!g_pD3D || !g_pD3DDevice)
		return false;

	if (!g_bActive)
		return true;

	// 消失したデバイスの復元処理
	HRESULT hr;
	if (g_bDeviceLost)
	{
		Sleep(100);	// 0.1秒待つ

		// デバイス状態のチェック
		hr  = g_pD3DDevice->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (hr == D3DERR_DEVICELOST)
				return true;  // デバイスはまだ失われている

			if (hr != D3DERR_DEVICENOTRESET)
				return false; // 予期せぬエラー

			CleanupD3DObject(); // Direct3Dで管理していないリソースを開放
			hr = g_pD3DDevice->Reset(&g_D3DPP); // 復元を試みる
			if (FAILED(hr))
			{
				if (hr == D3DERR_DEVICELOST)
					return true; // デバイスはまだ失われている

				DXTRACE_ERR(L"AppIdle Reset", hr);
				return false; // デバイスの復元に失敗
			}
		}
		// デバイスが復元した
		g_bDeviceLost = false;
	}
	Sleep(50);



	// 画面の更新
	hr = Render();
	if (hr == D3DERR_DEVICELOST)
		g_bDeviceLost = true;	// デバイスの消失
	else if (FAILED(hr))
		return false;

	return true;
}
extern "C"__declspec(dllexport) int CreateWin()
{
	HINSTANCE hInst=(HINSTANCE)::GetModuleHandle(NULL);

	//ImmDisableIME(-1);	// このスレッドで禁止(imm32.libをリンクする)

	// ウインドウ・クラスの登録
	WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szWndClass;
	wcex.hIconSm		= NULL;
      
	if (!RegisterClassEx(&wcex))
		return 0;

	// メイン・ウインドウ作成
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;
	g_rectWindow.right	= g_sizeWindowMode.cx;
	g_rectWindow.bottom	= g_sizeWindowMode.cy;
	AdjustWindowRect(&g_rectWindow, WS_OVERLAPPEDWINDOW, TRUE);
	g_rectWindow.right	= g_rectWindow.right - g_rectWindow.left;
	g_rectWindow.bottom	= g_rectWindow.bottom - g_rectWindow.top;
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;

	RECT rect;
	if (g_bWindow)
	{
		// (ウインドウ・モード用)
		rect.top	= CW_USEDEFAULT;
		rect.left	= CW_USEDEFAULT;
		rect.right	= g_rectWindow.right;
		rect.bottom	= g_rectWindow.bottom;
	}
	else
	{
		// (フルスクリーン・モード用)
		rect.top	= 0;
		rect.left	= 0;
		rect.right	= g_sizeFullMode.cx;
		rect.bottom	= g_sizeFullMode.cy;

		g_hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU1));

		//解像度変更
		ChangeFullscreenResolution() ;
	}

g_hWindow = CreateWindowEx(WS_EX_NOACTIVATE|WS_EX_TOPMOST,g_szWndClass, g_szAppTitle,
		WS_POPUP,
			rc.left, rc.top, rc.right, rc.bottom,
			NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return 0;

		g_dcSurface.Create(g_sizeWindowMode.cx, -g_sizeWindowMode.cy);
	   SetWindowLong(g_hWindow, GWL_EXSTYLE, (GetWindowLong(g_hWindow, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED);
   // ウインドウ表示
	//ShowWindow(g_hWindow, SW_SHOWNORMAL);
	//UpdateWindow(g_hWindow);
	//::GetWindowRect(g_hWindow, &rc);
	//hdcWnd = GetWindowDC(g_hWindow);
	return 1;
}
/************************************************************
	メイン
************************************************************/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nCmdShow)
{
	// デバッグ ヒープ マネージャによるメモリ割り当ての追跡方法を設定
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
   //::AllocConsole();    // 打开控件台资源
    //freopen("CONOUT$", "w+t", stdout);    // 申请写
	//freopen("CONIN$","r+t",stdin);
	mThread = CreateThread(NULL,0,MessageThreadProc,NULL,0,NULL);
	ThreadID=hInst;
	// アプリケーションに関する初期化
			//根据文本初始化
	fstream f("res\\config.txt",ios::in);
	if(f)
	{
	
	
	f>>rc.left>>rc.top>>rc.right>>rc.bottom>>modelnum;//读入参数
	g_sizeWindowMode.cx=rc.right;g_sizeWindowMode.cy=rc.bottom;
	rcSurface.left= 0;rcSurface.top= 0;rcSurface.right= g_sizeWindowMode.cx;rcSurface.bottom= g_sizeWindowMode.cy;
	ptWinPos.x =  rc.left;ptWinPos.y= rc.top;
	szWin.cx= g_sizeWindowMode.cx;szWin.cy=g_sizeWindowMode.cy ;
	f.close();
	}

	HRESULT hr = InitApp(hInst);

	if (FAILED(hr))
	{
		DXTRACE_ERR(L"WinMain InitApp", hr);
		return 0;
	}

	// DirectX Graphicsの初期化
	hr = InitDXGraphics();
	if (FAILED(hr)){
		DXTRACE_ERR(L"WinMain InitDXGraphics", hr);
	}

	// Live2D初期化
	SetupLive2D() ;
		D3DXCreateFontIndirect(g_pD3DDevice, &lf, &Font);// 编译无法通过，发现第2个参数是结构体D3DXFONT_DESCA类型，重新定义并赋值;
//	MoveWindow(g_hWindow,rc.left,rc.top,rc.right,rc.bottom,true);
		//ShowMessage(ThreadID,0,0,100,40,L"你好",50,20,20,true,L"微软雅黑",0xff00ff00);
	// メッセージ・ループ
	MSG msg;
	cout<<"RegisterAs(Output)"<<endl;
	cout<<"LinkRID(UI_Live2DAbort,false)"<<endl;
	cout<<"LinkRID(UI_AddModel,false)"<<endl;
	cout<<"LinkRID(UI_RemoveModels,false)"<<endl;
	cout<<"LinkRID(UI_GetModelPath,false)"<<endl;
	cout<<"LinkRID(UI_SetExpression,false)"<<endl;
	cout<<"LinkRID(UI_SetMotion,false)"<<endl;
	cout<<"LinkRID(UI_SetParameter,false)"<<endl;
	cout<<"LinkRID(UI_SetMouthOpen,false)"<<endl;
	cout<<"LinkRID(UI_PlaySound,false)"<<endl;
	cout<<"LinkRID(UI_SetEyeBalls,false)"<<endl;
		cout<<"LinkRID(UI_SetBody,false)"<<endl;
			cout<<"LinkRID(UI_SetFace,false)"<<endl;
			cout<<"可用命令参见readme.txt\n请输入命令"<<endl;
	do
	{

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// アイドル処理
			if (!AppIdle())
				// エラーがある場合，アプリケーションを終了する
				DestroyWindow(g_hWindow);
		}
	} while (!Closing);

	// アプリケーションの終了処理
	CleanupApp();
	_CrtDumpMemoryLeaks();
	//FreeConsole();                      // 释放控制台资源
	CloseHandle(mThread);
	DXTRACE_MSG(L"\n-- exit --\n") ;
	exit(0);
	return 0;

//	::ThreadID=(HINSTANCE)CreateThread(NULL,0,CreateWin,NULL,0,NULL);
	//return 0;
}

/************************************************************
//解像度変更
************************************************************/
void ChangeFullscreenResolution(){
#if CHANGE_FULLSCREEN_RESOLUTION

	DEVMODE    devMode;
	
	devMode.dmSize       = sizeof(DEVMODE);
	devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
	devMode.dmPelsWidth  = g_sizeFullMode.cx;
	devMode.dmPelsHeight = g_sizeFullMode.cy;

	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
#endif
}

//DLL调用函数

//启动live2D--Debug

void Live2DStart()
{
	HINSTANCE hInst= (HINSTANCE)GetModuleHandle(NULL);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		::ThreadID=hInst;
	fstream f("res\\config.txt",ios::in);
	if(f)
	{
	f>>rc.left>>rc.top>>rc.right>>rc.bottom>>modelnum;//读入参数
		g_sizeWindowMode.cx=rc.right;g_sizeWindowMode.cy=rc.bottom;
	rcSurface.left= 0;rcSurface.top= 0;rcSurface.right= g_sizeWindowMode.cx;rcSurface.bottom= g_sizeWindowMode.cy;
	ptWinPos.x =  rc.left;ptWinPos.y= rc.top;
	szWin.cx= g_sizeWindowMode.cx;szWin.cy=g_sizeWindowMode.cy ;
	f.close();
	}
  
	// アプリケーションに関する初期化
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		return ;
	}

	// DirectX Graphicsの初期化
	hr = InitDXGraphics();
	if (FAILED(hr)){
	
		return ;
	}

	// Live2D初期化
	SetupLive2D() ;
		D3DXCreateFontIndirect(g_pD3DDevice, &lf, &Font);// 编译无法通过，发现第2个参数是结构体D3DXFONT_DESCA类型，重新定义并赋值;
	// メッセージ・ループ
	MSG msg;
	do
	{
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// アイドル処理
			if (!AppIdle())
				// エラーがある場合，アプリケーションを終了する
				DestroyWindow(g_hWindow);
		}


	} while (!Closing);
	DestroyWindow(g_hWindow);
	// アプリケーションの終了処理
	CleanupApp();
	_CrtDumpMemoryLeaks();

	return ;
}



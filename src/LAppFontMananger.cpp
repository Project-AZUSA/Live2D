#include "LAppFontMananger.h"


extern HINSTANCE				g_ThreadID;
extern LPDIRECT3DDEVICE9  g_pD3DDevice;

//文本消息
ID3DXFont* Font   = 0;
ID3DXSprite* Sprite = 0;
D3DXFONT_DESC lf; // Initialize a LOGFONT structure that describes the font

// we want to create.
LOGFONTW lf1;
RECT tt={0,0,40,20};
D3DCOLOR textcolor;
wchar_t message[500];

LAppFontMananger::LAppFontMananger()
{

}

LAppFontMananger::~LAppFontMananger()
{

}

HRESULT LAppFontMananger::InitFont()
{
	return D3DXCreateFontIndirect(g_pD3DDevice, &lf, &Font);// 第2个参数是结构体D3DXFONT_DESCA类型
}

HRESULT LAppFontMananger::DrawMessageText()
{
	return Font->DrawText(
		Sprite,//编译时发现通不过，查阅该函数有6个参数，少了第一个，开头补上，类型为ID3DXSprite* Sprite = 0;
		(LPCWSTR)message, 
		-1, // size of string or -1 indicates null terminating string
		&tt,            // rectangle text is to be formatted to in windows coords
		DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
		textcolor);      // black text
}


//显示文本
bool LAppFontMananger::ShowMessage(HINSTANCE hinst, int x,int y,int width,int height,wchar_t * msg,
								   int fontHeight,int fontWidth,int fontWeight,bool italic,wchar_t * family,D3DCOLOR color)
{
	if(g_ThreadID==hinst)//检测当前调用进程是否合法
	{
		try{
			DeleteObject(Font);
			D3DXCreateFont(g_pD3DDevice,fontHeight,fontWidth,
				fontWeight,0,italic,DEFAULT_CHARSET,0,0,0,(LPCWSTR)family, &Font);// 编译无法通过，发现第2个参数是结构体D3DXFONT_DESCA类型，重新定义并赋值;
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



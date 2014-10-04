#include "LAppHookMananger.h"
#include <Windows.h>
#include "LAppRenderer.h"

extern HINSTANCE		g_hInstance;
extern HWND				g_hWindow;
extern LAppRenderer*	g_Renderer;

HHOOK  hHook =nullptr;
bool		  isTrack;

LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode >= 0)
	{
		switch (wParam)   
		{  
		case WM_MOUSEMOVE:
			{
				if (isTrack == false)
				{
					break;
				}

				LPMOUSEHOOKSTRUCT lpMouse=(MOUSEHOOKSTRUCT FAR*)lParam; 

				static POINT cursor;
				cursor.x = lpMouse->pt.x;
				cursor.y = lpMouse->pt.y;

				ScreenToClient( g_hWindow, &cursor);

				g_Renderer->mouseDrag(cursor.x,cursor.y);

			}
			break; 
		default:   
			break;   
		}   
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam); 
}


LAppHookMananger::LAppHookMananger()
{
	isTrack = false;

	hHook = SetWindowsHookEx(WH_MOUSE_LL,HookWndProc,g_hInstance,0);
}

LAppHookMananger::~LAppHookMananger()
{
	if (hHook)
	{
		UnhookWindowsHookEx(hHook);
	}
}

void LAppHookMananger::SetHook(bool Flag)
{
	isTrack = Flag;
}





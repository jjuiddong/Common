
#include "stdafx.h"
#include "button.h"


using namespace framework;


cButton::cButton(LPD3DXSPRITE sprite, const int id, const string &name) : //name=button
	cWindow(sprite, id, name)
{

}

cButton::~cButton()
{

}


bool cButton::MessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		{
			Vector2 pos(LOWORD(lParam), HIWORD(lParam));
			if (IsContain(pos))
			{
				return true;
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			Vector2 pos(LOWORD(lParam), HIWORD(lParam));
			if (IsContain(pos))
			{
				DispatchEvent(cEvent(EVENT::BUTTON_CLICK, GetId(), 0));
				dbg::Print( "button left button down id = %d", GetId());
				return true;
			}
		}
		break;

	case WM_LBUTTONUP:
		break;
	}

	return false;
}

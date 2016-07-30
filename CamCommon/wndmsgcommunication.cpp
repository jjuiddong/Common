
#include "stdafx.h"
#include "wndmsgcommunication.h"


cWndMsgCommunication::cWndMsgCommunication() :
m_hWnd(NULL)
{

}

cWndMsgCommunication::~cWndMsgCommunication()
{

}


bool cWndMsgCommunication::Init()
{
	std::cout << "Windows Message Communication" << std::endl;

	// fps_new_001
	HWND hWnd = FindWindowA(NULL, "fps_new_001");
	if (!hWnd)
		return false;

	m_hWnd = hWnd;
	return true;
}


void cWndMsgCommunication::SendData(const sSharedData &data)
{
// 	if (!m_hWnd)
// 		return;
	// fps_new_001
	HWND hWnd = FindWindowA(NULL, "fps_new_001");
	if (!hWnd)
		return;

	const int x = (int)(data.pts[0].x * 10000.f);
	const int y = (int)(data.pts[0].y * 10000.f);
	SendMessage(hWnd, WM_USER + 100, x, y);
}

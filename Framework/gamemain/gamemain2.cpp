
#include "../stdafx.h"
#include "gamemain2.h"
#include <MMSystem.h>
#include <chrono>
#include "../window/utility.h"
#include "../ui/dockmanager.h"
#include "../ui/renderwindow.h"

using namespace framework;

// Window Main Function
int framework::FrameWorkWinMain2(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow,
	const bool dualMonitor)
{
	cGameMain2* gameMain = CreateFrameWork2();
	gameMain->Create(wstr2str(gameMain->m_windowName), 
		gameMain->m_windowRect.Width(), 
		gameMain->m_windowRect.Height());

	srand((int)timeGetTime());

	cDockManager::Get()->InsertMainRenderWindow(gameMain);

	if (!gameMain->Init())
		return 0;

	using namespace std::chrono_literals;
	int oldT = timeGetTime();
	while (gameMain->isOpen())
	{
		const int curT = timeGetTime();
		int elapseT = curT - oldT;
		if (elapseT > 100)
			elapseT = 100;
		const float t = elapseT * 0.001f;
		oldT = curT;

		cDockManager::Get()->UpdateRender(t);
		//std::this_thread::sleep_for(1ms);
	}

	cDockManager::Release();
	graphic::ReleaseRenderer();

	return 0;
}


//LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	cGameMain2::Get()->MessageProc(message, wParam, lParam);
//
//	switch (message)
//	{
//	case WM_KEYDOWN:
//		switch (wParam)
//		{
//		case VK_ESCAPE:
//			DestroyWindow(hWnd);
//			break;
//		case VK_RETURN:
//			break;
//		}
//		break;
//
//	case WM_DESTROY:
//		cGameMain2::Get()->Exit();
//		PostQuitMessage(0);
//		break;
//
//	case WM_ERASEBKGND:
//		return 0;
//
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
//}



/////////////////////////////////////////////////////////////////////////////////////////////
cGameMain2::cGameMain2()
{
}


cGameMain2::~cGameMain2()
{
}


bool cGameMain2::Init()
{
	m_hWnd = getSystemHandle();
	graphic::cResourceManager::Get()->LoadTexture(m_renderer, g_defaultTexture);

	if (!OnInit())
		return false;
	return true;
}


void cGameMain2::Update(const float deltaSeconds)
{
	__super::Update(deltaSeconds);
	graphic::cResourceManager::Get()->Update(deltaSeconds);
}


void cGameMain2::DefaultEventProc(const sf::Event &evt)
{
	__super::DefaultEventProc(evt);
	OnEventProc(evt);
}


void cGameMain2::Exit()
{
	close();
}


void cGameMain2::LostDevice()
{
	__super::LostDevice();
	cDockManager::Get()->LostDevice();
}


void cGameMain2::ResetDevice(graphic::cRenderer *shared // = NULL
)
{
	__super::ResetDevice(shared);
	cDockManager::Get()->ResetDevice(&m_renderer);
}


#include "stdafx.h"
#include "gamemain2.h"
#include <MMSystem.h>
#include <chrono>

using namespace framework;

// Window Main Function
int framework::FrameWorkWinMain2(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow,
	const bool dualMonitor)
{

	// Initialize GDI+
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	cGameMain2* gameMain = CreateFrameWork2();
	gameMain->Create(true, gameMain->m_windowName.str()
		, gameMain->m_windowRect.Width() 
		, gameMain->m_windowRect.Height()
		, NULL
		, false);

	common::initrand();

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

	graphic::ReleaseRenderer();
	cDockManager::Release();
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
cGameMain2::cGameMain2()
	: m_hWnd(NULL)
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


void cGameMain2::Exit()
{
	close();
}


void cGameMain2::LostDevice()
{
	__super::LostDevice();
}


void cGameMain2::ResetDevice()
{
	__super::ResetDevice();
}

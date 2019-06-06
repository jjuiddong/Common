
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
	gameMain->Create(hInstance, true, gameMain->m_windowName.str()
		, gameMain->m_windowRect.Width() 
		, gameMain->m_windowRect.Height()
		, NULL
		, gameMain->m_isTitleBar);

	common::initrand();

	cDockManager::Get()->InsertMainRenderWindow(gameMain);

	if (!gameMain->Init())
		return 0;

	gameMain->m_state = cGameMain2::RUN;

	common::cTimer timer;
	timer.Create();

	using namespace std::chrono_literals;
	double oldT = timer.GetSeconds();
	while (gameMain->isOpen())
	{
		const double curT = timer.GetSeconds();
		const double deltaT = min(curT - oldT, gameMain->m_minDeltaTime);
		const double dt = (cGameMain2::PAUSE == gameMain->m_state) ? 0.f : (deltaT * gameMain->m_slowFactor);
		oldT = curT;

		cDockManager::Get()->UpdateRender((float)dt);

		if (gameMain->m_isLazyMode) // 30 frame
			std::this_thread::sleep_for(10ms);
	}

	gameMain->m_state = cGameMain2::SHUTDOWN;
	gameMain->Shutdown();
	graphic::ReleaseRenderer();
	cDockManager::Release();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	dbg::TerminateLogThread();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
cGameMain2::cGameMain2()
	: m_hWnd(NULL)
	, m_isLazyMode(false)
	, m_slowFactor(1.f)
	, m_minDeltaTime(0.1f)
{
}


cGameMain2::~cGameMain2()
{
}


bool cGameMain2::Init()
{
	m_state = INIT;
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


void cGameMain2::Shutdown()
{
	OnShutdown();
	Clear();
}


void cGameMain2::Pause() {
	if (RUN == m_state)
		m_state = PAUSE;
}

void cGameMain2::Resume()
{
	if (PAUSE == m_state)
		m_state = RUN;
}

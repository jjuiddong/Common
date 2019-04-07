
#include "stdafx.h"
#include "GameMain.h"
#include <MMSystem.h>
#include <chrono>
#include <thread>

//#include "../../wxMemMonitorLib/wxMemMonitor.h"
//MEMORYMONITOR_INNER_PROCESS();
using namespace framework;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//------------------------------------------------------------------------
// WindowMain 함수
//------------------------------------------------------------------------
int framework::FrameWorkWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow,
	const bool dualMonitor)
{
	//if (!memmonitor::Init(memmonitor::INNER_PROCESS,hInstance,"config_memmonitor.json" ))
	//{
	//	MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
	//}

	const HWND hWnd = InitWindow(hInstance, 
		cGameMain::Get()->GetWindowName(), 
		cGameMain::Get()->GetWindowRect(),
		nCmdShow,
		WndProc, 
		dualMonitor);

	if (hWnd == NULL)
		return 0;

	srand((int)timeGetTime());

	ULONG_PTR gdiplusToken;
	using namespace Gdiplus;
	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (!cGameMain::Get()->Init(hInstance, hWnd))
		return 0;

	cGameMain::Get()->Run();
	cGameMain::Get()->ShutDown();
	cGameMain::Release();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	dbg::TerminateLogThread();

	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	cGameMain::Get()->MessageProc(message, wParam, lParam);

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_RETURN:
			break;
		}
		break;

	case WM_DESTROY:
		cGameMain::Get()->Exit();
		PostQuitMessage(0);
		break;

	case WM_ERASEBKGND:
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////
cGameMain *cGameMain::m_pInstance = NULL;

cGameMain::cGameMain()
	: m_slowFactor(1.f)
	, m_hWnd(NULL)
	, m_hInstance(NULL)
{

}


cGameMain::~cGameMain()
{
	//for each (auto &kv in m_scenes)
	//	delete kv.second;
	//m_scenes.clear();
}

// singleton function
cGameMain* cGameMain::Get()
{
	if (!m_pInstance)
		m_pInstance = CreateFrameWork();
	return m_pInstance;
}

void cGameMain::Release()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


bool cGameMain::Init(HINSTANCE hInstance, HWND hWnd)
{
	m_state = INIT;
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	if (!m_renderer.CreateDirectX(false, hWnd, m_windowRect.Width(), m_windowRect.Height()))
	{
		return false;
	}

	//graphic::cResourceManager::Get()->LoadTexture(m_renderer, g_defaultTexture);
	
	if (!OnInit())
		return false;

	return true;
}


void cGameMain::ShutDown()
{
	OnShutdown();
	graphic::ReleaseRenderer();
}


void cGameMain::Run()
{
	using namespace std::chrono_literals;

	m_state = RUN; // Framework 실행

	MSG msg;
	int oldT = timeGetTime();
	while ((RUN == m_state) || (PAUSE == m_state))
	{
		if (PeekMessage( &msg, m_hWnd, 0, 0, PM_NOREMOVE ))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const int curT = timeGetTime();
		int elapseT = curT - oldT;
		if (elapseT > 100) // 너무 간격이 크면, 0.1초를 넘지 않게 한다.
			elapseT = 100;
		const float t = (PAUSE == m_state)? 0.f : (elapseT * 0.001f * m_slowFactor);
		oldT = curT;

		Update(t);
		Render(t);
	}
}


void cGameMain::Update(const float deltaSeconds)
{
	//if (m_currentScene)
	//	m_currentScene->Update(deltaSeconds);

	m_renderer.Update(deltaSeconds);

	graphic::cResourceManager::Get()->Update(deltaSeconds);

	OnUpdate(deltaSeconds);
}


void cGameMain::Render(const float deltaSeconds)
{
	OnRender(deltaSeconds);
}


void	cGameMain::MessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	OnMessageProc(message, wParam, lParam);
}


void cGameMain::Exit()
{
	m_state = SHUTDOWN;
}

const wstring& cGameMain::GetWindowName()
{
	return m_windowName;
}
sRectf cGameMain::GetWindowRect()
{
	return m_windowRect;
}


void cGameMain::Pause() {
	if (RUN == m_state)
		m_state = PAUSE;
}

void cGameMain::Resume()
{
	if (PAUSE == m_state)
		m_state = RUN;
}

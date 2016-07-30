
#include "stdafx.h"
#include "GameMain.h"
#include <MMSystem.h>
#include "../window/utility.h"

//#include "../../wxMemMonitorLib/wxMemMonitor.h"
//MEMORYMONITOR_INNER_PROCESS();


using namespace framework;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//------------------------------------------------------------------------
// WindowMain 함수
//------------------------------------------------------------------------
int framework::FrameWorkWinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
	//if (!memmonitor::Init(memmonitor::INNER_PROCESS,hInstance,"config_memmonitor.json" ))
	//{
	//	MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
	//}

	const HWND hWnd = InitWindow(hInstance, 
		cGameMain::Get()->GetWindowName(), 
		cGameMain::Get()->GetWindowRect(), 
		nCmdShow,
		WndProc);

	if (hWnd == NULL)
		return 0;

	srand((int)timeGetTime());

	if (!cGameMain::Get()->Init(hWnd))
		return 0;

	cGameMain::Get()->Run();
	cGameMain::Get()->ShutDown();
	cGameMain::Release();
	//memmonitor::Cleanup();
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
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////
cGameMain *cGameMain::m_pInstance = NULL;

cGameMain::cGameMain()
	: m_currentScene(NULL)
{

}


cGameMain::~cGameMain()
{
	for each (auto &kv in m_scenes)
		delete kv.second;
	m_scenes.clear();
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


bool cGameMain::Init(HWND hWnd)
{
	m_state = INIT;
	m_hWnd = hWnd;

// 	if (!graphic::InitRenderer(hWnd, 
// 		m_windowRect.right-m_windowRect.left,
// 		m_windowRect.bottom-m_windowRect.top))
// 		return false;
	if (!m_renderer.CreateDirectX(hWnd,
		m_windowRect.right - m_windowRect.left, 
		m_windowRect.bottom - m_windowRect.top))
	{
		return false;
	}

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
	m_state = RUN; // Framework 실행

	MSG msg;
	int oldT = timeGetTime();
	while (RUN == m_state)
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
		const float t = elapseT * 0.001f;
		oldT = curT;

		Update(t);
		Render(t);
		Sleep(0);
	}
}


void cGameMain::Update(const float deltaSeconds)
{
	if (m_currentScene)
		m_currentScene->Update(deltaSeconds);

	m_renderer.Update(deltaSeconds);
	OnUpdate(deltaSeconds);
}


void cGameMain::Render(const float deltaSeconds)
{
	if (m_renderer.ClearScene())
	{
		if (m_currentScene)
			m_currentScene->Render(m_renderer, Matrix44::Identity);

		OnRender(deltaSeconds);
	}
}


void	cGameMain::MessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_currentScene)
		m_currentScene->MessageProc(message, wParam, lParam);
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
const RECT& cGameMain::GetWindowRect()
{
	return m_windowRect;
}


bool cGameMain::InsertScene(cScene *scene)
{
	RETV(!scene, false);

	auto it = m_scenes.find(scene->GetId());
	if (m_scenes.end() != it)
	{
		dbg::ErrLog("Insert Scene Error!! Already Exist scene id = %d \n", scene->GetId());
		return false;
	}

	m_scenes[scene->GetId()] = scene;

	dbg::Log("Insert Scene id=%d, name=%s \n", scene->GetId(), scene->GetName().c_str());
	return true;
}


bool cGameMain::RemoveScene(const int sceneId, const bool removeMemory)
{
	auto it = m_scenes.find(sceneId);
	if (m_scenes.end() == it)
	{
		dbg::ErrLog("RemoveScene Error!! not found scene id = %d \n", sceneId);
		return false;
	}

	if (removeMemory)
	{
		delete m_scenes[sceneId];
	}

	m_scenes.erase(it);
	return true;
}


bool cGameMain::ChangeScene(const int sceneId)
{
	auto it = m_scenes.find(sceneId);
	if (m_scenes.end() == it)
	{
		dbg::ErrLog("ChangeScene Error!! not found scene id = %d\n", sceneId);
		return false;
	}

	if (m_currentScene == it->second)
		return true;

	if (m_currentScene)
		m_currentScene->Hide();

	m_currentScene = it->second;
	it->second->Show();

	return true;
}

//
// 2017-04-24, jjuiddong
// GameMain Class with DockingWindow
//
#pragma once

#include "../ui/renderwindow.h"


namespace framework
{

	class cGameMain2 : public cRenderWindow
	{
	public:
		cGameMain2();
		virtual ~cGameMain2();

		virtual bool Init();
		virtual void Update(const float deltaSeconds) override;
		virtual void DefaultEventProc(const sf::Event &evt) override;
		virtual void Exit();


	protected:
		virtual bool OnInit() { return true; }
		virtual void OnEventProc(const sf::Event &evt) {}
		virtual void OnLostDevice() override;
		virtual void OnResetDevice(graphic::cRenderer *shared) override;


	public:
		HWND m_hWnd;
		wstring m_windowName;
		sRecti m_windowRect;
	};

	// 프레임워크 매인 함수.
	int FrameWorkWinMain2(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow,
		const bool dualMonitor = false);

	// 프레임워크 인스턴스를 생성한다. 반드시 이 함수를 구현해야 한다.
	cGameMain2* CreateFrameWork2();

}

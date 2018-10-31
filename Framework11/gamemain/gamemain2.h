//
// 2017-04-24, jjuiddong
// GameMain Class with DockingWindow
//
#pragma once

#include "../ui/renderwindow.h"


namespace framework
{
	using namespace common;

	class cGameMain2 : public cRenderWindow
	{
	public:
		cGameMain2();
		virtual ~cGameMain2();

		enum STATE
		{
			INIT,
			RUN,
			PAUSE,
			SHUTDOWN,
		};

		virtual bool Init();
		virtual void Update(const float deltaSeconds) override;
		virtual void LostDevice() override;
		virtual void ResetDevice() override;
		virtual void Shutdown() override;
		virtual void Exit();

		void Pause();
		void Resume();


	protected:
		virtual bool OnInit() { return true; }


	public:
		STATE m_state;
		HWND m_hWnd;
		WStrId m_windowName;
		sRectf m_windowRect;
		bool m_isLazyMode; // FPS 30 이하 유지
		float m_slowFactor; // default: 1.f
	};


	// 프레임워크 매인 함수.
	int FrameWorkWinMain2(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow,
		const bool dualMonitor = false);

	// 프레임워크 인스턴스를 생성한다. 반드시 이 함수를 구현해야 한다. (매크로 이용)
	cGameMain2* CreateFrameWork2();
}

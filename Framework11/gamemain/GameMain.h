//
// 2016-05-21, jjuiddong
//		- scene 관리 추가.
//
#pragma once


namespace framework
{

	class cScene;
	class cGameMain
	{
	public:
		cGameMain();
		virtual ~cGameMain();
		
		enum STATE
		{
			INIT,
			RUN,
			PAUSE,
			SHUTDOWN,
		};

		virtual bool Init(HINSTANCE hInstance, HWND hWnd);
		virtual void ShutDown();
		virtual void Run();
		virtual void Update(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual void Exit();

		const wstring& GetWindowName();
		common::sRectf GetWindowRect();		
		void MessageProc( UINT message, WPARAM wParam, LPARAM lParam);
		void Pause();
		void Resume();
		void Slow(const float rate);


	protected:
		virtual bool OnInit() { return true; }
		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnShutdown() {}
		virtual void OnLostDevice() {}
		virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) {}


	public:
		STATE m_state;
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		wstring m_windowName;
		sRectf m_windowRect;
		graphic::cRenderer m_renderer;
		float m_slowFactor; // default: 1.f


	// singleton
	protected:
		static cGameMain* m_pInstance;
	public:
		static cGameMain* Get();
		static void Release();
	};


	// 프레임워크 매인 함수.
	int FrameWorkWinMain(HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		LPSTR lpCmdLine, 
		int nCmdShow,
		const bool dualMonitor=false);

	// 프레임워크 인스턴스를 생성한다. 반드시 이 함수를 구현해야 한다.
	cGameMain* CreateFrameWork();

	//extern cGameMain* g_application; // 전역 어플리케이션 인스턴스
}

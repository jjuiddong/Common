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
			SHUTDOWN,
		};

		virtual bool Init(HWND hWnd);
		virtual void ShutDown();
		virtual void Run();
		virtual void Update(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual void Exit();

		bool InsertScene(cScene *scene);
		bool RemoveScene(const int sceneId, const bool removeMemory=true);
		bool ChangeScene(const int sceneId);

		const wstring& GetWindowName();
		const RECT& GetWindowRect();		
		void MessageProc( UINT message, WPARAM wParam, LPARAM lParam);


	protected:
		virtual bool OnInit() { return true; }
		virtual void OnUpdate(const float elapseT) {}
		virtual void OnRender(const float elapseT) {}
		virtual void OnShutdown() {}
		virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) {}


	protected:
		STATE m_state;
		HWND m_hWnd;
		wstring m_windowName;
		RECT m_windowRect;
		graphic::cRenderer m_renderer;
		cScene *m_currentScene;
		map<int, cScene*> m_scenes;


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
		int nCmdShow);

	// 프레임워크 인스턴스를 생성한다. 반드시 이 함수를 구현해야 한다.
	cGameMain* CreateFrameWork();
}

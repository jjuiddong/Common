//
// 2017-03-14, jjuiddong
// global input manager
//
// upgrade, 2017-10-15
//	singletone -> object
//
#pragma once


namespace framework
{
	class cDockWindow;

	class cInputManager
	{
	public:
		cInputManager();
		virtual ~cInputManager();
		void NewFrame();
		void Update(const float deltaSeconds);
		void MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		void MouseProc(const sf::Event &evt);
		POINT GetDockWindowCursorPos(cDockWindow *dock);
		bool IsClick(const int btn=0);
		bool IsDbClick();


	public:
		bool m_mouseDown[3]; // left, right, middle (Mouse Down -> Down Flag True)
		bool m_mouseClicked[3]; // left, right, middle, (Mouse Down -> Up -> Click Flag True)
		bool m_lBtnDbClick;
		POINT m_mousePt;
		POINT m_mouseClickPt;
		int m_clickTime;
		int m_dbClickTime;
	};

}

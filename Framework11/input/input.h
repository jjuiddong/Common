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
		void Update(const float deltaSeconds);
		void MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		void MouseProc(const sf::Event &evt);
		POINT GetDockWindowCursorPos(cDockWindow *dock);
		bool IsClick();


	public:
		bool m_mouseDown[3]; // left, right, middle
		bool m_lBtnDbClick;
		POINT m_mousePt;
		int m_clickTime;
		int m_dbClickTime;
	};

}

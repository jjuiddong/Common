//
// 2017-03-14, jjuiddong
// gloabl input manager
//
#pragma once


namespace graphic
{

	class cInputManager : public common::cSingleton<cInputManager>
	{
	public:
		cInputManager();
		virtual ~cInputManager();
		void Update(const float deltaSeconds);
		bool IsClick();


	public:
		bool m_lBtnDown;
		bool m_rBtnDown;
		bool m_mBtnDown;
		POINT m_mousePt;
	};

}

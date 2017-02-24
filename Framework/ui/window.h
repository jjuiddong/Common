//
// 2016-05-16, jjuiddong
//		- Init() 함수 추가
//
//
#pragma once


namespace framework
{

	class cWindow : public framework::cEventHandler
							, public graphic::cSprite2
	{
	public:
		cWindow(graphic::cSprite &sprite, const int id, const string &name="window");
		virtual ~cWindow();

		virtual bool Init(graphic::cRenderer &renderer);
		virtual bool Update(const float deltaSeconds) override;
		virtual void Render(graphic::cRenderer &renderer, const Matrix44 &parentTm) override;
		virtual bool MessageProc( UINT message, WPARAM wParam, LPARAM lParam);


	protected:
		bool DispatchEvent( cEvent &event );


	public:
		bool m_isVisible;
	};

}

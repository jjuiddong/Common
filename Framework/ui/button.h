#pragma once


namespace framework
{

	class cButton : public cWindow
	{
	public:
		cButton(graphic::cSprite &sprite, const int id, const string &name="button");
		virtual ~cButton();

		virtual bool MessageProc( UINT message, WPARAM wParam, LPARAM lParam) override;
	};

}

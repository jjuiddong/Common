#pragma once


namespace framework
{

	class cButton : public cWindow
	{
	public:
		cButton(LPD3DXSPRITE sprite, const int id, const string &name="button");
		virtual ~cButton();

		virtual bool MessageProc( UINT message, WPARAM wParam, LPARAM lParam) override;
	};

}

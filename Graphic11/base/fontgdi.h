//
// 2017-03-14, jjuiddong
// GDI Font
//
#pragma once


namespace graphic
{
	class cFontGdi
	{
	public:
		cFontGdi();
		virtual ~cFontGdi();
		bool Create(const int height=36, const int width=20);


	public:
		HFONT m_font;
	};
}

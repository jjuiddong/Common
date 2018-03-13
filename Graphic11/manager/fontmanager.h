//
// 2017-03-14, jjuiddong
// font manager
//
// 2017-08-24
//	- Upgrade DX11
//	- Manager FW1FontWrapper Object
//
#pragma once


namespace graphic
{
	using namespace common;

	class cFontManager
	{
	public:
		cFontManager();
		virtual ~cFontManager();
		std::pair<IFW1FontWrapper*, IDWriteTextFormat*> GetFont(cRenderer &renderer, const char *name = "Arial", const float fontSize = 16.f);
		bool AddFont(const char *name, IFW1FontWrapper *font);
		bool AddTextFormat(const char *name, const float fontSize, IDWriteTextFormat *textFormat);
		void Clear();


	public:
		map<hashcode, IFW1FontWrapper*> m_fonts;  // key= hashcode
		map<std::pair<hashcode, float>, IDWriteTextFormat*> m_textFormats;  // key= hashcode + fontsize
	};

}

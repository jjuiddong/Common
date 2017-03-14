//
// 2017-03-14, jjuiddong
// font manager
//
#pragma once


namespace graphic
{
	class cFontGdi;

	class cFontManager : public common::cSingleton<cFontManager>
	{
	public:
		cFontManager();
		virtual ~cFontManager();
		cFontGdi* GetFontGdi(const string &name);
		bool AddFontGdi(const string &name, cFontGdi *font);
		void Clear();


	public:
		map<string, cFontGdi*> m_fontGdis;
	};

}

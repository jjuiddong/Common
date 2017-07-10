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
		cFontGdi* GetFontGdi(const Str64 &name);
		bool AddFontGdi(const Str64 &name, cFontGdi *font);
		void Clear();


	public:
		map<hashcode, cFontGdi*> m_fontGdis;
	};

}

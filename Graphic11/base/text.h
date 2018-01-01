//
// 2017-08-24, jjuiddong
// DX11 Text Render Class
// using FW1FontWrapper
//
#pragma once


namespace graphic
{
	class cSprite;

	class cText
	{
	public:
		cText();
		cText(cRenderer &renderer
			, const float fontSize=18.f, const bool isBold=true, const char *fontName="±¼¸²"
			, const cColor &color = cColor::WHITE, const wchar_t *text = NULL);
		virtual ~cText();

		bool Create(cRenderer &renderer, const float fontSize = 18.f, const bool isBold = true, const char *fontName = "±¼¸²"
			, const cColor &color=cColor::WHITE, const wchar_t *text=NULL);
		void Render(cRenderer &renderer, const float x, const float y, const wchar_t *text, const bool restore=false);
		void Render(cRenderer &renderer, const float x, const float y, const bool restore = false);

		void SetText(const wchar_t *text);
		void SetColor(const cColor color);
		cColor GetColor() const;
		void Clear();


	public:
		WStr128 m_text;
		DWORD m_color;
		float m_fontSize;
		IDWriteTextLayout *m_textLayout;
		IFW1FontWrapper *m_fontWrapper; // reference
	};


	inline void cText::SetText(const wchar_t *text) { m_text = text; }
	inline void cText::SetColor(const cColor color) { m_color = color.GetAbgr(); }
	inline cColor cText::GetColor() const { return m_color; }
}

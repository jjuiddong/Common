// ¹®ÀÚ¿­ Ãâ·Â °´Ã¼.
#pragma once


namespace graphic
{
	class cSprite;

	class cText
	{
	public:
		cText();
		cText(cRenderer &renderer, const string &text, const int x, const int y,
			const DWORD color=D3DXCOLOR(1,1,1,1),
			const int fontSize=18, const bool isBold=true, const string &fontName="±¼¸²");
		virtual ~cText();

		bool Create(cRenderer &renderer, const int fontSize = 18, const bool isBold = true, const string &fontName = "±¼¸²", 
			cSprite *sprite=NULL);
		void SetText(const string &text);
		void SetText(const int x, const int y, const string &text);
		const string& GetText() const;
		void SetPos(const int x, const int y);
		void SetPos(const float x, const float y);
		const sRect& GetPos() const;
		void SetColor(const DWORD color);
		DWORD GetColor() const;
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Render();
		void Clear();


	public:
		ID3DXFont *m_font;
		cSprite *m_sprite;
		string m_text;
		sRect m_rect;
		DWORD m_color;
	};


	inline void cText::SetText(const string &text) { m_text = text; }
	inline void cText::SetText(const int x, const int y, const string &text) { 
		m_text = text; m_rect.SetX(x); m_rect.SetY(y); }
	inline const string& cText::GetText() const { return m_text; }
	inline void cText::SetColor(const DWORD color) { m_color = color; }
	inline DWORD cText::GetColor() const { return m_color; }
	inline void cText::SetPos(const int x, const int y) { m_rect.SetX(x), m_rect.SetY(y); }
	inline void cText::SetPos(const float x, const float y) { m_rect.SetX((int)x), m_rect.SetY((int)y); }
	inline const sRect& cText::GetPos() const { return m_rect; }
}

//
// 2017-03-14, jjuiddong
// Draw Text On Texture and then Render 3D Space
// 
#pragma once


namespace graphic
{

	class cFontGdi;
	class cText3d
	{
	public:
		cText3d();
		virtual ~cText3d();

		bool Create(cRenderer &renderer, cFontGdi *font, const BILLBOARD_TYPE::TYPE type, 
			const int width, const int height, const int textWidth=128, const int textHeight=64);
		bool SetText(const Matrix44 &tm, const string &text, const DWORD color);
		bool SetTextRect(
			//const Matrix44 &tm
			const Transform &tm
			, const string &text, const DWORD color
			, const BILLBOARD_TYPE::TYPE type, const sRecti &rect);
		void Render(cRenderer &renderer);
		void FillTexture(const DWORD color);


	public:
		string m_text;
		cFontGdi *m_font;
		cTexture m_texture;
		cBillboard m_quad;
		DWORD m_color;
	};

}

//
// 2017-07-17, jjuiddong
// Draw Text On Texture and then Render 3D Space
// cNode2 Derivated
// 
#pragma once


namespace graphic
{

	class cFontGdi;
	class cText3d3 : public cNode2
	{
	public:
		cText3d3();
		virtual ~cText3d3();

		bool Create(cRenderer &renderer, cFontGdi *font, const BILLBOARD_TYPE::TYPE type,
			const int width, const int height, const int textWidth = 128, const int textHeight = 64);
		bool SetText(const Matrix44 &tm, const Str128 &text, const DWORD color);
		bool SetTextRect(
			const Transform &tm
			, const Str128 &text, const DWORD color
			, const BILLBOARD_TYPE::TYPE type, const sRecti &rect);
		virtual bool Render(cRenderer &renderer, const Matrix44 &parentTm = Matrix44::Identity, const int flags = 1) override;
		void FillTexture(const DWORD color);


	public:
		Str128 m_text;
		cFontGdi *m_font;
		cTexture m_texture;
		cBillboard m_quad;
		DWORD m_color;
	};

}

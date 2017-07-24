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
			const int width, const int height, const int textWidth = 256, const int textHeight = 32);
		bool SetText(const Matrix44 &tm, const Str128 &text, const cColor &color);
		bool SetTextRect(
			const Transform &tm
			, const Str128 &text, const cColor &color
			, const BILLBOARD_TYPE::TYPE type, const sRecti &rect);
		bool SetTextRect2(
			cRenderer &renderer
			, const Transform &tm
			, const Str128 &text
			, const cColor &color, const cColor &outlineColor
			, const BILLBOARD_TYPE::TYPE type);
		virtual bool Render(cRenderer &renderer, const Matrix44 &parentTm = Matrix44::Identity, const int flags = 1) override;
		bool RenderOld(cRenderer &renderer, const Matrix44 &parentTm = Matrix44::Identity, const int flags = 1);
		void FillTexture(const cColor &color);


	public:
		Str128 m_text;
		cFontGdi *m_font;
		cTexture m_texture;
		cBillboard m_quad;
		//DWORD m_color;
		cColor m_color;
	};

}

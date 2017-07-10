//
// 2017-06-23, jjuiddong
// Draw Text On Texture and then Render 3D Space
// Using Surface
// 
#pragma once


namespace graphic
{

	class cFontGdi;
	class cText3d2
	{
	public:
		cText3d2();
		virtual ~cText3d2();

		bool Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
			const int width, const int height, const int textWidth = 128, const int textHeight = 64);
		bool SetText(const Matrix44 &tm, const Str128 &text, const DWORD color);
		bool SetTextRect(cRenderer &renderer, const Matrix44 &tm, const Str128 &text, const DWORD color);// , const sRecti &rect);
		void Render(cRenderer &renderer);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);


	public:
		cText m_text;
		cSurface2 m_surface;
		cTexture m_texture;
		cBillboard m_quad;
		DWORD m_color;
	};

}

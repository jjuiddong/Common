//
// 2017-07-17, jjuiddong
// Draw Text On Texture and then Render 3D Space
// cNode Derivated
//
// 2017-08-24
//	- Upgrade DX11
// 
#pragma once


namespace graphic
{
	using namespace common;

	class cText3d3 : public cNode
	{
	public:
		cText3d3();
		virtual ~cText3d3();

		bool Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
			const int width, const int height
			, const int textWidth = 256
			, const int textHeight = 32
			, const float dynScaleMin = 0.5f
			, const float dynScaleMax = 200.5f
			, const float dynScaleAlpha = 1.f
		);

		bool SetTextRect(
			cRenderer &renderer
			, const Transform &tm
			, const wchar_t *text
			, const cColor &color
			, const cColor &outlineColor
			, const BILLBOARD_TYPE::TYPE type
		);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;


	public:
		WStr128 m_text;
		cTexture m_texture;
		cBillboard m_quad;
		cColor m_color;
		Vector3 m_originalScale;
		bool m_isDepthNone;
	};

}

//
// 2017-11-06, jjuiddong
// 2D Rect Render
//
#pragma once


namespace graphic
{

	class cRect2D : public cNode
	{
	public:
		cRect2D();

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1) override;

		void SetRect(const Vector2 &leftTop, const Vector2 &rightBottom, const float width);
		void SetColor(const cColor &color);


	public:
		cColor m_color;
	};

}

//
// 2020-02-25, jjuiddong
// 2D Line List Render
//
#pragma once


namespace graphic
{
	using namespace common;

	class cLine2DList : public cNode
	{
	public:
		cLine2DList(const uint reserve=64);
		virtual ~cLine2DList();

		bool Create(cRenderer &renderer);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1) override;

		void AddLine(const Vector2 &p0, const Vector2 &p1, const float width
			, const cColor &color = cColor::BLACK);
		void ClearLine();


	public:
		struct sLine {
			Matrix44 mat;
			cColor color;
		};
		vector<sLine> m_lines;
		cCubeShape m_shape;
	};

}

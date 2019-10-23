//
// 2017-08-04, jjuiddong
// Upgrade DX9 -> DX11
// Line Cube Class
// Vertex = Point + Diffuse
//
#pragma once


namespace graphic
{
	using namespace common;

	class cLine : public cNode
	{
	public:
		cLine();
		cLine(cRenderer &renderer
			, const Vector3 &p0 = Vector3(0, 0, 0)
			, const Vector3 &p1 = Vector3(1, 1, 1)
			, const float width = 1.f
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor color = cColor::BLACK);

		bool Create(cRenderer &renderer
			, const Vector3 &p0=Vector3(0,0,0)
			, const Vector3 &p1=Vector3(1,1,1)
			, const float width=1.f
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor color = cColor::BLACK);

		void SetLine(const Vector3 &p0, const Vector3 &p1, const float width);
		void SetColor(const cColor &color);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);

		virtual bool RenderInstancing(cRenderer &renderer
			, const int count
			, const Matrix44 *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		) override;


	public:
		cCubeShape m_shape;
		cColor m_color;
	};

}

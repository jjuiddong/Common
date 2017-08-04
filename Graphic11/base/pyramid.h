//
// 2017-06-30, jjuiddong
// Pyramid
// Vertex = Pos + Diffuse
//
//			(head)
//            0
//           /  \
//          /    \
//       1,2 ----- 3,4
//
//       1 ----------- 3
//       | \           |
//       |    \        |
//       |      +      |
//       |       \     |
//       |          \  |
//       2 ----------- 4
//
// 2017-08-04
//	- Upgrade DX9 - DX11
//
#pragma once


namespace graphic
{

	class cPyramid : public cNode2
	{
	public:
		cPyramid();
		virtual ~cPyramid();

		bool Create(cRenderer &renderer
			, const float width=1
			, const float height=1
			, const Vector3 &pos=Vector3(0,0,0)
			, const int vtxType = (eVertexType::POSITION | eVertexType::DIFFUSE)
			, const cColor &color=cColor::BLACK);
		void SetDimension(const float width, const float height);
		void SetPos(const Vector3 &pos);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const float width=1);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const Vector3 &from, const float width = 1);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;


	public:
		cPyramidShape m_shape;
		Transform m_transform;
	};

}

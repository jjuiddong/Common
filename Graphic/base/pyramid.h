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
#pragma once


namespace graphic
{

	class cPyramid
	{
	public:
		cPyramid();
		virtual ~cPyramid();

		bool Create(cRenderer &renderer, const float width=1, const float height=1
			, const Vector3 &pos=Vector3(0,0,0), const DWORD color=0);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void SetDimension(const float width, const float height);
		void SetPos(const Vector3 &pos);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const float width=1);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const Vector3 &from, const float width = 1);


	public:
		Transform m_transform;
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}

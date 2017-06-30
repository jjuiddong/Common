//
// 2017-05-13, jjuiddong
// Debug Arrow Mesh
//
#pragma once


namespace graphic
{

	class cDbgArrow
	{
	public:
		cDbgArrow();
		virtual ~cDbgArrow();

		bool Create(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float size = 1.f);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const float size = 1.f);


	public:
		//cDbgSphere m_head;
		cPyramid m_head;
		cDbgLine m_body;
	};

}

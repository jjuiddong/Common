//
// 2017-05-13, jjuiddong
// Debug Arrow Mesh
//
// 2020-01-29
//	- add arrow type
//		- head: pyramid, cube
//
#pragma once


namespace graphic
{
	using namespace common;

	class cDbgArrow
	{
	public:
		cDbgArrow();
		virtual ~cDbgArrow();

		bool Create(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1
			, const float width = 1.f, const bool isSolid=false
			, const float arrowRatio = 0.75f
			, const int vtxType = eVertexType::POSITION
			, const cColor &color = cColor::WHITE);

		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity
			, const bool showCubeHead = false);

		void SetDirection(const Vector3 &p0, const Vector3 &p1, const float width = 1.f
			, const float arrowRatio = 0.75f);

		void SetDirection(const Vector3 &p0, const Vector3 &p1, const Quaternion &rot
			, const float width = 1.f, const float arrowRatio = 0.75f);

		bool Picking(const Ray &ray, const XMMATRIX &parentTm = XMIdentity
			, const bool isSpherePicking = true
			, OUT float *dist = NULL);

		void SetColor(const cColor &color);
		void SetTechnique(const char *techniqName);


	public:
		bool m_isSolid; // default: false
		cPyramid m_head;
		cCube m_headCube; // cube head
		cLine m_body;
		cColor m_color;
		Transform m_transform;
	};

}

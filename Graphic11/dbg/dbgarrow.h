//
// 2017-05-13, jjuiddong
// Debug Arrow Mesh
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
			, const float size = 1.f, const bool isSolid=false);
		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		void SetDirection(const Vector3 &p0, const Vector3 &p1, const float size = 1.f);
		bool Picking(const Ray &ray, const XMMATRIX &parentTm = XMIdentity
			, const bool isSpherePicking = true
			, OUT float *dist = NULL);
		void SetColor(const cColor &color);
		void SetTechnique(const char *techniqName);


	public:
		bool m_isSolid; // default: false
		cPyramid m_head;
		cLine m_body;
		cColor m_color;
		Transform m_transform;
	};

}

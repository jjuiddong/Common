//
// 2017-05-15, jjuiddong
// 절두체 디버깅 클래스.
//
#pragma once


namespace graphic
{
	using namespace common;
	class cRenderer;

	class cDbgFrustum : public cFrustum
	{
	public:
		cDbgFrustum();
		virtual ~cDbgFrustum();

		bool Create(cRenderer &renderer, const Matrix44 &viewProj, const cColor &color = cColor::BLACK);
		bool SetFrustum(cRenderer &renderer, const Matrix44 &viewProj, const cColor &color = cColor::BLACK);
		bool SetFrustum(cRenderer &renderer, const cFrustum &frustum);
		bool SetFrustum(cRenderer &renderer, const Vector3 vertices[8]);
		void Render(cRenderer &renderer);


	public:
		cDbgBox m_box;
		cDbgLineList m_lineList;
	};

}

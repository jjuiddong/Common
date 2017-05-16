//
// 2017-05-15, jjuiddong
// 절두체 디버깅 클래스.
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cDbgFrustum : public cFrustum
	{
	public:
		cDbgFrustum();
		virtual ~cDbgFrustum();

		bool Create(cRenderer &renderer, const Matrix44 &matViewProj);
		bool Create(cRenderer &renderer, const Vector3 &_min, const Vector3 &_max);
		void SetFrustum(cRenderer &renderer, const Matrix44 &matViewProj);
		void Render(cRenderer &renderer);


	public:
		bool m_fullCheck;	// IsIn, IsInSphere 함수 호출시 체크범위 default : false
		cDbgBox m_box;
		vector<Plane> m_plane;	// frustum의 6개 평면
		Vector3 m_pos; // 절두체 위치
	};

}

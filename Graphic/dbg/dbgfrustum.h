//
// 2017-05-15, jjuiddong
// 절두체 클래스.
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cDbgFrustum : public cDbgBox
	{
	public:
		cDbgFrustum();
		virtual ~cDbgFrustum();

		bool Create(cRenderer &renderer, const Matrix44 &matViewProj);
		bool Create(cRenderer &renderer, const Vector3 &_min, const Vector3 &_max);
		bool IsIn(const Vector3 &point);
		bool IsInSphere(const Vector3 &point, float radius);
		void Render(cRenderer &renderer);


	public:
		bool m_fullCheck;	// IsIn, IsInSphere 함수 호출시 체크범위 default : false
		vector<Plane> m_plane;	// frustum의 6개 평면
		Vector3 m_pos; // 절두체 위치
	};

}

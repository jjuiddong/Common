// 절두체 클래스.
#pragma once


namespace graphic
{
	class cRenderer;

	class cFrustum
	{
	public:
		cFrustum();
		virtual ~cFrustum();

		bool SetFrustum(const Matrix44 &matViewProj);
		bool SetFrustum(const Vector3 &_min, const Vector3 &_max);
		bool IsIn( const Vector3 &point ) const;
		bool IsInSphere( const Vector3 &point, float radius ) const;
		bool IsInBox(const cBoundingBox &bbox) const;
		const Vector3& GetPos() const;
		vector<Plane>& GetPlanes();
		

	public:
		bool m_fullCheck;	// IsIn, IsInSphere 함수 호출시 체크범위 default : false
		vector<Plane> m_plane;	// frustum의 6개 평면
		Vector3 m_pos; // 절두체 위치
	};


	inline const Vector3& cFrustum::GetPos() const { return m_pos; }
	inline vector<Plane>& cFrustum::GetPlanes() { return m_plane; }
}

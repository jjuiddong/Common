//
// 2017-08-03, jjuiddong
// Upgrade DX9 - DX11
// 
#pragma once


namespace graphic
{
	class cRenderer;

	class cFrustum
	{
	public:
		cFrustum();
		virtual ~cFrustum();

		//virtual bool SetFrustum(const Matrix44 &matViewProj);
		virtual bool SetFrustum(const Vector3 &pos, const Vector3 &direction, const Matrix44 &maProj);
		bool IsIn( const Vector3 &point ) const;
		//bool IsInSphere( const Vector3 &point, float radius, const Matrix44 &tm = Matrix44::Identity ) const;
		bool IsInSphere(const cBoundingSphere &sphere) const;
		bool IsInBox(const cBoundingBox &bbox) const;
		float LengthRoughly(const Vector3 &pos) const;
		//const Vector3& GetPos() const;
		//vector<Plane>& GetPlanes();
		
		static void Split2(cCamera &cam, const float f1, const float f2
			, cFrustum &out1, cFrustum &out2);
		static void Split3(cCamera &cam, const float f1, const float f2, const float f3
			, cFrustum &out1, cFrustum &out2, cFrustum &out3);

		void GetGroundPlaneVertices(const Plane &plane, OUT Vector3 outVertices[4]) const;


	public:
		//bool m_fullCheck;	// IsIn, IsInSphere 함수 호출시 체크범위 default : false
		//vector<Plane> m_plane; // frustum의 6개 평면
		//Vector3 m_pos; // Position, near plane center
		//float m_epsilon; // 프러스텀에 정확하게 포함되지 않더라도, 약간의 여분을 주어서 프러스텀에 포함시키기 위한 값, default : 5 

		Matrix44 m_viewProj;
		BoundingFrustum m_frustum;
	};


//	inline const Vector3& cFrustum::GetPos() const { return m_pos; }
	//inline vector<Plane>& cFrustum::GetPlanes() { return m_plane; }
}

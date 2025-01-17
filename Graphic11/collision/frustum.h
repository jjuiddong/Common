//
// 2017-08-03, jjuiddong
// Upgrade DX9 - DX11
// 
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderer;

	class cFrustum
	{
	public:
		cFrustum();
		virtual ~cFrustum();

		bool SetFrustum(const Matrix44 &matViewProj);
		bool SetFrustum(const Vector3 vertices[8]);
		bool SetFrustum(const Vector3 vertices[2], const float h0, const float h1);
		bool IsIn( const Vector3 &point ) const;
		bool IsInSphere(const cBoundingSphere &sphere) const;
		bool IsInBox(const cBoundingBox &bbox) const;
		bool boxInFrustum(const cBoundingBox &bbox) const;		
		int FrustumAABBIntersect(const cBoundingBox &bbox) const;
		float LengthRoughly(const Vector3 &pos) const;
		
		static void Split2(const cCamera &cam, const float f1, const float f2
			, cFrustum &out1, cFrustum &out2);
		static void Split3(const cCamera &cam, const float f1, const float f2, const float f3
			, cFrustum &out1, cFrustum &out2, cFrustum &out3);
		static void Split3_2(const cCamera &cam, const float f1, const float f2, const float f3
			, cFrustum &out1, cFrustum &out2, cFrustum &out3);

		void GetGroundPlaneVertices(const Plane &plane, OUT Vector3 outVertices[4]) const;
		void GetVertices(OUT Vector3 out[8]);

		cFrustum& operator=(const cFrustum &rhs);


	public:
		vector<Plane> m_plane; // frustum�� 6�� ���
		Vector3 m_pos; // Position, near plane center
		Vector3 m_dir; // Direction
		float m_epsilon;

		Matrix44 m_viewProj;
		BoundingFrustum m_frustum;
	};

}

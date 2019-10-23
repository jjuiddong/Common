#pragma once


namespace common
{
	struct Triangle
	{
		Triangle();
		Triangle( const Vector3& v1, const Vector3& v2, const Vector3& v3 );

		void Create( const Vector3& vA, const Vector3& vB, const Vector3& vC );
		
		bool Intersect(	const Vector3& vOrig, const Vector3& vDir,
			float* pfT = NULL, float* pfU = NULL, float* pfV = NULL ) const;

		Vector3 GetClosesPointOnTriangle(const Vector3 &sourcePosition) const;
		
		float Distance( const Vector3& vPos ) const;

		bool Similar(const Triangle &tri, const float limitLen) const;
		
		Vector3 Normal() const;

		Vector3 a, b, c;
	};

}

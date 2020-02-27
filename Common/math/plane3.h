//
// 2020-02-27, jjuiddong
// geometry plane
//		- 4 triangle plane
#pragma once


namespace common
{

	struct Plane3
	{
		Plane3();
		Plane3(const Vector3& axis1, const Vector3& axis2, const Vector3& pos
			, const float size);
		float Distance(const Vector3& vP) const;
		Vector3 Pick(const Vector3& vOrig, const Vector3& vDir) const;

		Triangle tri[4];
		Vector3 N; // Normal
		float D; // D
	};

}

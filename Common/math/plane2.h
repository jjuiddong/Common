//
// 2020-02-12, jjuiddong
// geometry plane
//		- x,y axis normal 
#pragma once


namespace common
{

	struct Plane2
	{
		Plane2();
		Plane2(const Vector3& norm, const Vector3& pos, const Vector3& xAxis);
		float Distance(const Vector3& vP) const;
		Vector3 Pick(const Vector3& vOrig, const Vector3& vDir) const;
		const Plane2 operator * (const Matrix44 &rhs);

		Vector3 N; // Normal
		float D; // D
		Quaternion rot; // rotate to x-axis (Init2 initialize x axis)
	};

}

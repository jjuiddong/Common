//
// refactoring, 2017-07-17
//	irrlicht 엔진 참조
//	- inverse
//	- multiply
//
#pragma once


namespace common
{
	struct Matrix44;
	struct Quaternion
	{
		Quaternion();
		Quaternion( const float fX, const float fY, const float fZ, const float fW );
		Quaternion( const Vector3& vAxis, const float fAngle );
		Quaternion( const Vector3& vDir1, const Vector3& vDir2 );

		Quaternion& operator *= ( const Quaternion& q )
		{
			*this = *this * q;
			return *this;
		} //operator *=

		// https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
		Quaternion operator * ( const Quaternion& q ) const
		{
			Quaternion tmp;
			tmp.w = (q.w * w) - (q.x * x) - (q.y * y) - (q.z * z);
			tmp.x = (q.w * x) + (q.x * w) + (q.y * z) - (q.z * y);
			tmp.y = (q.w * y) + (q.y * w) + (q.z * x) - (q.x * z);
			tmp.z = (q.w * z) + (q.z * w) + (q.x * y) - (q.y * x);
			return tmp;
		} //operator *

		Quaternion operator * ( const Matrix44& m ) const
		{
			return *this * m.GetQuaternion();
		} //operator *
	
		Quaternion Interpolate( const Quaternion& qNext, const float fTime ) const;
		Matrix44 GetMatrix() const;
		Vector3 GetDirection() const;

		void SetRotationX( const float fRadian );
		void SetRotationY( const float fRadian );
		void SetRotationZ( const float fRadian );
		void SetRotationArc( const Vector3& vDir, const Vector3& vDir2 );
		void SetRotationArc(const Vector3& v0, const Vector3& v1, const Vector3 &norm);
		void Euler(const Vector3& euler);
		void Euler2(const Vector3& euler);
		Vector3 Euler() const;
		const Quaternion& Normalize();
		Quaternion Inverse() const;

		float GetRotationAngleXZ() const;


		float x, y, z, w;
	};

}

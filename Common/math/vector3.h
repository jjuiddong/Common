#pragma once

namespace common
{
	struct Vector4;
	struct Matrix44;
	struct Transform;
	struct Quaternion;

	struct Vector3
	{
		float x,y,z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}
		Vector3(const Vector4 &rhs);

		inline bool IsEmpty() const;
		inline bool IsEqual(const Vector3 &rhs, const float epsilon) const;
		float Length() const;
		float Length2() const;
		float LengthRoughly(const Vector3 &rhs) const;
		float Distance(const Vector3 &rhs) const;
		Vector3 Normal() const;
		void Normalize();
		Vector3 MultiplyNormal( const Matrix44& rhs ) const;
		Vector3 MultiplyNormal2(const Matrix44& rhs) const;
		Vector3 Interpolate( const Vector3 &v, const float alpha) const;
		Vector3 Minimum(const Vector3 &rhs) const;
		Vector3 Maximum(const Vector3 &rhs) const;
		Vector3 ToOpenGL() const;

#ifdef USE_D3D11_MATH
		XMVECTOR GetVectorXM() const;
		Vector3 operator * (const XMMATRIX &m) const;
#endif
	
		float DotProduct(const Vector3& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3 CrossProduct(const Vector3& v) const
		{
			return Vector3(
				(y * v.z) - (z * v.y),
				(z * v.x) - (x * v.z),
				(x * v.y) - (y * v.x));
		}


		Vector3 operator + () const;
		Vector3 operator - () const;
		Vector3 operator + ( const Vector3& rhs ) const;
		Vector3 operator - ( const Vector3& rhs ) const;
		Vector3& operator += ( const Vector3& rhs );
		Vector3& operator -= ( const Vector3& rhs );
		Vector3& operator *= ( const Vector3& rhs );
		Vector3& operator /= ( const Vector3& rhs );

		Vector3 operator * ( const Matrix44& rhs ) const;
		Vector3 operator * (const Quaternion& rhs) const;
		Vector3& operator *= ( const Matrix44& rhs );
		Vector3 operator * (const Transform& rhs) const;
		Vector3& operator *= (const Transform& rhs);


		bool operator == (const Vector3 &rhs) const
		{
			return (abs(x - rhs.x) < MATH_EPSILON) &&
				(abs(y - rhs.y) < MATH_EPSILON) &&
				(abs(z - rhs.z) < MATH_EPSILON);
		}

		bool operator != (const Vector3 &rhs) const
		{
			return !(operator==(rhs));
		}

		Vector3 operator * (const Vector3 &v) const {
			return Vector3(x*v.x, y*v.y, z*v.z);
		}

		template <class T>
		Vector3 operator * ( T t ) const {
			return Vector3(x*t, y*t, z*t);
		}

		template <class T>
		Vector3 operator / ( T t ) const {
			return Vector3(x/t, y/t, z/t);
		}

		template <class T>
		Vector3& operator *= ( T t ) {
			*this = Vector3(x*t, y*t, z*t);
			return *this;
		}

		template <class T>
		Vector3& operator /= ( T t ) {
			*this = Vector3(x/t, y/t, z/t);
			return *this;
		}

		
		static const Vector3 Zeroes;
		static const Vector3 Ones;
		static const Vector3 Min;
		static const Vector3 Max;
		static const Vector3 Up;
		static const Vector3 Right;
		static const Vector3 Forward;
	};


	inline bool Vector3::IsEmpty() const
	{
		return (x == 0) && (y == 0) && (z == 0);
	}

	inline bool Vector3::IsEqual(const Vector3 &rhs, const float epsilon) const
	{
		return Distance(rhs) < epsilon;
	}
}

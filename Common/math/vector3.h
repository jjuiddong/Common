#pragma once

namespace common
{
	struct Vector4;
	struct Matrix44;
	struct Vector3
	{
		float x,y,z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}
		Vector3(const Vector4 &rhs);

		bool IsEmpty() const;
		float Length() const;
		float Length2() const;
		float LengthRoughly(const Vector3 &rhs) const;
		Vector3 Normal() const;
		void Normalize();
		float	DotProduct( const Vector3& v ) const;
		Vector3 CrossProduct( const Vector3& v ) const;
		Vector3 MultiplyNormal( const Matrix44& rhs ) const;
		Vector3 MultiplyNormal2(const Matrix44& rhs) const;
		Vector3 Interpolate( const Vector3 &v, const float alpha) const;
		Vector3 Minimum(const Vector3 &rhs);
		Vector3 Maximum(const Vector3 &rhs);

#ifdef USE_D3D11_MATH
		XMVECTOR GetVectorXM() const;
#endif
	
		Vector3 operator + () const;
		Vector3 operator - () const;
		Vector3 operator + ( const Vector3& rhs ) const;
		Vector3 operator - ( const Vector3& rhs ) const;
		Vector3& operator += ( const Vector3& rhs );
		Vector3& operator -= ( const Vector3& rhs );
		Vector3& operator *= ( const Vector3& rhs );
		Vector3& operator /= ( const Vector3& rhs );

		Vector3 operator * ( const Matrix44& rhs ) const;
		Vector3& operator *= ( const Matrix44& rhs );

		bool operator == (const Vector3 &rhs) const
		{
			return (abs(x - rhs.x) < MATH_EPSILON2) &&
				(abs(y - rhs.y) < MATH_EPSILON2) &&
				(abs(z - rhs.z) < MATH_EPSILON2);
		}

		bool operator != (const Vector3 &rhs) const
		{
			return !(operator==(rhs));
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


		static Vector3 Min;
		static Vector3 Max;
		static Vector3 Up;
		static Vector3 Right;
		static Vector3 Forward;
	};

}

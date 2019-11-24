//
// 201xx-xx-xx, jjuiddong
//
//
// 2019-11-23
//		- Vector2, Vector3 copy, assign operator
//
#pragma once


namespace common
{
	struct Vector2;
	struct Vector3;
	struct Matrix44;
	struct Vector4
	{
		float x, y, z, w;

		Vector4() : x(0), y(0), z(0), w(0) {}
		Vector4(float x0, float y0, float z0, float w0) : x(x0), y(y0), z(z0), w(w0) {}
		Vector4(const Vector3 &v, float w0=1.f) : x(v.x), y(v.y), z(v.z), w(w0) {}
		Vector4(const DWORD color);
		Vector4(const Vector2 &v) : x(v.x), y(v.y), z(0.f), w(0.f) {}

		bool IsEmpty() const;
		float DotProduct(const Vector4& v) const;

#ifdef USE_D3D11_MATH
		XMVECTOR GetVectorXM() const;
#endif

		Vector4 operator * ( const float & rhs ) const;
		Vector4 operator * ( const Matrix44& rhs ) const;
		Vector4 operator- (const Vector4 &rhs) const;
		Vector4 operator+ (const Vector4 &rhs) const;
		Vector4& operator *= ( const Matrix44& rhs );
		Vector4& operator=(const Vector3 &rhs);
		Vector4& operator=(const Vector2 &rhs);
	};
}

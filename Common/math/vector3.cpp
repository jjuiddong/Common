
#include "stdafx.h"
#include "math.h"

using namespace common;


Vector3 Vector3::Min(-FLT_MAX, -FLT_MAX, -FLT_MAX);
Vector3 Vector3::Max(FLT_MAX, FLT_MAX, FLT_MAX);
Vector3 Vector3::Up(0, 1, 0);
Vector3 Vector3::Right(1,0,0);
Vector3 Vector3::Forward(0,0,1);


Vector3::Vector3(const Vector4 &rhs) :
	x(rhs.x), y(rhs.y), z(rhs.z)
{
}

//bool Vector3::IsEmpty() const
//{
//	return (x==0) && (y==0) && (z==0);
//}

float Vector3::Length() const
{
	return sqrt(x*x + y*y + z*z);
}

float Vector3::Length2() const
{
	return x*x + y*y + z*z;
}

float Vector3::LengthRoughly(const Vector3 &rhs) const
{
	Vector3 v = *this - rhs;
	return v.x*v.x + v.y*v.y + v.z*v.z;
}


float Vector3::Distance(const Vector3 &rhs) const
{
	Vector3 v = *this - rhs;
	return v.Length();
}


Vector3 Vector3::Normal() const
{
	const float len = Length();
	if (len < 0.001f)
		return Vector3(0,0,0);
	return *this / len;
}


void Vector3::Normalize()
{
	*this = Normal();
}


Vector3 Vector3::operator + () const
{
	return *this;
}


Vector3 Vector3::operator - () const
{
	return Vector3(-x, -y, -z);
}


Vector3 Vector3::operator + ( const Vector3& rhs ) const
{
	return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
}


Vector3 Vector3::operator - ( const Vector3& rhs ) const
{
	return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
}


Vector3& Vector3::operator += ( const Vector3& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}


Vector3& Vector3::operator -= ( const Vector3& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}


Vector3& Vector3::operator *= ( const Vector3& rhs )
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}


Vector3& Vector3::operator /= ( const Vector3& rhs )
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}


Vector3 Vector3::operator * ( const Matrix44& rhs ) const
{
	float	RHW = 1.0f / (x*rhs._14 + y*rhs._24 + z*rhs._34 + rhs._44);
	if (RHW >= FLT_MAX)
		return Vector3(0,0,0);

	Vector3 v;
	v.x = (x*rhs._11 + y*rhs._21 + z*rhs._31 + rhs._41 ) * RHW;
	v.y = (x*rhs._12 + y*rhs._22 + z*rhs._32 + rhs._42 ) * RHW;
	v.z = (x*rhs._13 + y*rhs._23 + z*rhs._33 + rhs._43 ) * RHW;
	return v;
}


Vector3& Vector3::operator *= ( const Matrix44& rhs )
{
	float	RHW = 1.0f / (x * rhs._14 + y * rhs._24 + z * rhs._34 + rhs._44);
	if (RHW >= FLT_MAX)
	{
		*this = Vector3(0,0,0);
		return *this;
	}

	Vector3 v;
	v.x = (x * rhs._11 + y * rhs._21 + z * rhs._31 + rhs._41 ) * RHW;
	v.y = (x * rhs._12 + y * rhs._22 + z * rhs._32 + rhs._42 ) * RHW;
	v.z = (x * rhs._13 + y * rhs._23 + z * rhs._33 + rhs._43 ) * RHW;
	*this = v;
	return *this;
}


Vector3 Vector3::MultiplyNormal( const Matrix44& rhs ) const
{
	Vector3 v;
	v.x = x * rhs._11 + y * rhs._21 + z * rhs._31;
	v.y = x * rhs._12 + y * rhs._22 + z * rhs._32;
	v.z = x * rhs._13 + y * rhs._23 + z * rhs._33;
	v.Normalize();
	return v;
}


// no normalize
Vector3 Vector3::MultiplyNormal2(const Matrix44& rhs) const
{
	Vector3 v;
	v.x = x * rhs._11 + y * rhs._21 + z * rhs._31;
	v.y = x * rhs._12 + y * rhs._22 + z * rhs._32;
	v.z = x * rhs._13 + y * rhs._23 + z * rhs._33;
	return v;
}


//https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
Vector3 Vector3::operator * (const Quaternion& rhs) const
{
	// Extract the vector part of the quaternion
	Vector3 u(rhs.x, rhs.y, rhs.z);

	// Extract the scalar part of the quaternion
	const float s = rhs.w;

	// Do the math
	return u * 2.0f * u.DotProduct(*this)
		+ *this * (s*s - u.DotProduct(u))
		+ u.CrossProduct(*this) * 2.0f * s;
}


Vector3 Vector3::Interpolate( const Vector3 &v, const float alpha) const
{
	return Vector3(x + (alpha * ( v.x - x ) ),
		y + (alpha * ( v.y - y ) ),
		z + (alpha * ( v.z - z ) ) );
}


Vector3 Vector3::Minimum(const Vector3 &rhs) const
{
	Vector3 v;
	v.x = min(x, rhs.x);
	v.y = min(y, rhs.y);
	v.z = min(z, rhs.z);
	return v;
}


Vector3 Vector3::Maximum(const Vector3 &rhs) const
{
	Vector3 v;
	v.x = max(x, rhs.x);
	v.y = max(y, rhs.y);
	v.z = max(z, rhs.z);
	return v;
}


#ifdef USE_D3D11_MATH
XMVECTOR Vector3::GetVectorXM() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

Vector3 Vector3::operator * (const XMMATRIX &m) const 
{
	XMVECTOR v = XMLoadFloat3((XMFLOAT3*)this);
	v = XMVector3Transform(v, m);
	Vector3 ret;
	XMStoreFloat3((XMFLOAT3*)&ret, v);
	return ret;
}
#endif


Vector3 Vector3::operator * (const Transform& rhs) const
{
	return operator*(rhs.GetMatrix());
}


Vector3& Vector3::operator *= (const Transform& rhs)
{
	return operator*=(rhs.GetMatrix());
}

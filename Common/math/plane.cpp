
#include "stdafx.h"
#include "plane.h"

#ifdef USE_D3D9_MATH
	#include <D3dx9math.h>
#endif 

using namespace common;


Vector3 v0;
Vector3 v1;

Plane::Plane()
{
}

Plane::Plane( float a, float b, float c, float d )
{
	v0.x = a;
	v0.y = b;
	v0.z = c;

	N = v0.Normal();
	D = d;
}

Plane::Plane( const Vector3& vN, float d )
{
	N = vN.Normal();
	D = d;
}


Plane::Plane( const Vector3& vN, const Vector3& vPos )
{
	N = vN.Normal();
	D = -N.DotProduct( vPos );
}


Plane::Plane( const Vector3& vA, const Vector3& vB, const Vector3& vC )
{
	v0 = vC - vA;
	v1 = vC - vB;

	N = v0.CrossProduct( v1 );
	N.Normalize();

	D = -N.DotProduct( vA );
}

//
void Plane::Init( const Vector3& vA, const Vector3& vB, const Vector3& vC )
{
	v0 = vC - vA;
	v1 = vC - vB;

	N = v0.CrossProduct( v1 );
	N.Normalize();

	D = -N.DotProduct( vA );
}


float Plane::Distance( const Vector3& vP ) const
{
	float fDot = N.DotProduct( vP );
	return fDot + (D);
}


Vector3 Plane::Pick( const Vector3& vOrig, const Vector3& vDir ) const
{
	return vOrig + vDir * Distance( vOrig ) / vDir.DotProduct( -N );
}


float Plane::GetX( float fY, float fZ ) const
{
	return -( N.y * fY + N.z * fZ + D ) / N.x;
}


float Plane::GetY( float fX, float fZ ) const
{
	return -( N.x * fX + N.z * fZ + D ) / N.y;
}

float Plane::GetZ( float fX, float fY ) const
{
	//Assert( ABS( N.z ) > MATH_EPSILON );
	return -( N.x * fX + N.y * fY + D ) / N.z;
}


float Plane::Collision( const Vector3& vP ) const
{
	return N.x * vP.x + N.y * vP.y + N.z * vP.z + D;
}


bool Plane::CollisionSphere( const Vector3& vP, const float fRadius )
{
	return Collision(vP) <= fRadius;
}


//	LineCross
int Plane::LineCross( const Vector3& v0, const Vector3& v1, Vector3* pvOut ) const
{
	float fSign[2]; 
 
	fSign[0] = N.DotProduct( v0 ) + D;
	fSign[1] = N.DotProduct( v1 ) + D;

	if (fSign[0] * fSign[1] > 0.0F) 
	{
		return 0; 
	} //if

	if (fSign[0] == 0.0F)
	{
		if( pvOut )
		{
			*pvOut = v0;
		} //if

		if (fSign[1] == 0.0F)
		{
			return 3;
		} //if

		return 2;
	} //if

	if (fSign[1] == 0.0F)
	{
		if( pvOut )
		{
			*pvOut = v1;
		} //if
		return 2;
	} //if

	if (pvOut)
	{
		float aDot = v0.x * N.x + v0.y * N.y + v0.z * N.z;
		float bDot = v1.x * N.x + v1.y * N.y + v1.z * N.z;
		float scale = ( -D - aDot) / ( bDot - aDot );
		*pvOut = v0 + ( (v1 - v0)*scale );
	} //if

	return 1;
}


const Plane Plane::operator * (const Matrix44 &rhs)
{
	Plane out;
//#ifdef USE_D3D9_MATH
//	D3DXPlaneTransform((D3DXPLANE*)&out, (D3DXPLANE*)this, (D3DXMATRIX*)&rhs);
//#elif USE_D3D11_MATH
	XMVECTOR p = XMLoadFloat4((XMFLOAT4*)this);
	XMMATRIX m = rhs.GetMatrixXM();	
	XMVECTOR r = XMPlaneTransform(p, m);
	XMStoreFloat4((XMFLOAT4*)&out, r);
//#endif
	out.N.Normalize();
	return out;
}


// 평면에 반사되는 변환 행렬을 리턴한다.
Matrix44 Plane::GetReflectMatrix()
{
	Matrix44 reflect;
//#ifdef USE_D3D9_MATH
//	D3DXMatrixReflect((D3DXMATRIX*)&reflect, (D3DXPLANE*)this);
//#elif USE_D3D11_MATH
	XMVECTOR plane = XMLoadFloat4((XMFLOAT4*)this);
	XMMATRIX m;
	m = XMMatrixReflect(plane);
	XMStoreFloat4x4((XMFLOAT4X4*)&reflect, m);
//#endif
	return reflect;
}


// projection pos to plane
Vector3 Plane::Projection(const Vector3& pos) const
{
	const float d = Distance(pos);
	return pos + N * -d;
}

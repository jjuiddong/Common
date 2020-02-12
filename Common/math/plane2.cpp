
#include "stdafx.h"
#include "plane2.h"

#ifdef USE_D3D9_MATH
	#include <D3dx9math.h>
#endif 

using namespace common;

Plane2::Plane2()
{
}

// initialize normal, x axis
Plane2::Plane2(const Vector3& norm, const Vector3& pos, const Vector3& xAxis)
{
	N = norm.Normal();
	D = -N.DotProduct(pos);
	rot.SetRotationArc(xAxis, Vector3(1, 0, 0));
}


float Plane2::Distance(const Vector3& vP) const
{
	float fDot = N.DotProduct(vP);
	return fDot + (D);
}


Vector3 Plane2::Pick(const Vector3& vOrig, const Vector3& vDir) const
{
	const Vector3 p = vOrig + vDir * Distance(vOrig) / vDir.DotProduct(-N);
	return p * rot;
}


const Plane2 Plane2::operator * (const Matrix44 &rhs)
{
	Plane2 out;
#ifdef USE_D3D9_MATH
	D3DXPlaneTransform((D3DXPLANE*)&out, (D3DXPLANE*)this, (D3DXMATRIX*)&rhs);
#elif USE_D3D11_MATH
	XMVECTOR p = XMLoadFloat4((XMFLOAT4*)this);
	XMMATRIX m = rhs.GetMatrixXM();
	XMVECTOR r = XMPlaneTransform(p, m);
	XMStoreFloat4((XMFLOAT4*)&out, r);
#endif
	return out;
}

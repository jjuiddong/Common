
#include "stdafx.h"
#include "Math.h"

#ifdef USE_D3D9_MATH
	#include <D3dx9math.h>
#endif

using namespace common;


//--------------------------------
//	Constructor
//--------------------------------
Quaternion::Quaternion()
	:x(0), y(0), z(0), w(1)
{

} //Quaternion::Quaternion
//--------------------------------
//
//--------------------------------
Quaternion::Quaternion( const float fX, const float fY, const float fZ, const float fW )
:	x	( fX ),
	y	( fY ),
	z	( fZ ),
	w	( fW )
{	
} //Quaternion

//--------------------------------
//
//--------------------------------
Quaternion::Quaternion( const Vector3& vAxis, const float fAngle )
{
	float s = sinf( fAngle / 2 );
	float c = cosf( fAngle / 2 );
	x = s * vAxis.x;
	y = s * vAxis.y;
	z = s * vAxis.z;
	w = c;
} //Quaternion::Quaternion
//--------------------------------
//
//--------------------------------
Quaternion::Quaternion( const Vector3& vDir1, const Vector3& vDir2 )
{
	SetRotationArc( vDir1, vDir2 );
} //Quaternion::Quaternion

//--------------------------------
//
//--------------------------------
Quaternion Quaternion::Interpolate( const Quaternion& qNext, const float fTime ) const
{
	Quaternion qC;
	const Quaternion&	qA = *this;
	Quaternion			qB = qNext;

	float	fCos =
		qA.x * qB.x +
		qA.y * qB.y +
		qA.z * qB.z +
		qA.w * qB.w;

	if( fCos < 0.0F )
	{	// 쿼터니언 뒤집기.
		qB.x = -qB.x;
		qB.y = -qB.y;
		qB.z = -qB.z;
		qB.w = -qB.w;
		fCos = -fCos;
	} //if

	float	fAlpha	= fTime;
	float	fBeta	= 1.0F - fTime;

	if( ( 1.0F - fCos ) > 0.001F )
	{	// 구 보간 적용.
		float	fTheta = acosf( fCos );

		fBeta	= sinf( fTheta * ( 1.0F - fTime ) ) / sinf( fTheta );
		fAlpha	= sinf( fTheta * fTime            ) / sinf( fTheta );
	} //if

	// 선형 보간.
	qC.x = fBeta * qA.x + fAlpha * qB.x;
	qC.y = fBeta * qA.y + fAlpha * qB.y;
	qC.z = fBeta * qA.z + fAlpha * qB.z;
	qC.w = fBeta * qA.w + fAlpha * qB.w;

	return qC;
} //Quaternion::Interpolate


//https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
Quaternion& Quaternion::lerp(Quaternion q1, Quaternion q2, float time)
{
	const float scale = 1.0f - time;
	return (*this = (q1*scale) + (q2*time));
}


//https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
Quaternion& Quaternion::slerp(Quaternion q1, Quaternion q2, float time
	, float threshold //= 0.05f
)
{
	float angle = q1.dotProduct(q2);

	// make sure we use the short rotation
	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1 - threshold)) // spherical interpolation
	{
		const float theta = acosf(angle);
		//const float invsintheta = reciprocal(sinf(theta));
		const float invsintheta = 1.f / sinf(theta);
		const float scale = sinf(theta * (1.0f - time)) * invsintheta;
		const float invscale = sinf(theta * time) * invsintheta;
		return (*this = (q1*scale) + (q2*invscale));
	}
	else // linear interpolation
		return (*this = lerp(q1, q2, time));
}



//--------------------------------
//
//--------------------------------
Matrix44 Quaternion::GetMatrix() const
{
#ifdef USE_D3D9_MATH
	Matrix44 m;
	D3DXMatrixRotationQuaternion( (D3DXMATRIX*)&m, (D3DXQUATERNION*)this );
	return m;
#elif defined (USE_D3D11_MATH)
	XMVECTOR xq = XMLoadFloat4((XMFLOAT4*)this);
	XMMATRIX xm = XMMatrixRotationQuaternion(xq);
	Matrix44 m;
	XMStoreFloat4x4((XMFLOAT4X4*)&m, xm);
	return m;
#else
	Matrix44 m;

	float X = x;
	float Y = y;
	float Z = z;
	float W = -w;

	float xx = X * X;
	float xy = X * Y;
	float xz = X * Z;
	float xw = X * W;
	float yy = Y * Y;
	float yz = Y * Z;
	float yw = Y * W;
	float zz = Z * Z;
	float zw = Z * W;

	m._11 = 1.0f - 2.0f * (yy + zz);
	m._12 = 2.0f * (xy - zw);
	m._13 = 2.0f * (xz + yw);

	m._21 = 2.0f * (xy + zw);
	m._22 = 1 - 2.0f * (xx + zz);
	m._23 = 2.0f * (yz - xw);

	m._31 = 2.0f * (xz - yw);
	m._32 = 2.0f * (yz + xw);
	m._33 = 1.0f - 2.0f * (xx + yy);

	m._14 = m._24 = m._34 = 0.0f;
	m._41 = m._42 = m._43 = 0.0f;
	m._44 = 1.0f;
	return m;
#endif // USE_D3D9_MATH

} //Quaternion::GetMatrix4

//--------------------------------
//
//--------------------------------
Vector3 Quaternion::GetDirection() const
{
//	Matrix44		matRot = GetMatrix4();
//	return	-Vector3( matRot._31, matRot._32, matRot._33 );

	float xx = x * x;
	float yy = y * y;
//	float zz = z * z;
//	float xy = x * y;
	float yz = y * z;
	float zx = z * x;
	float wx = w * x;
	float wy = w * y;
//	float wz = w * z;

	return -Vector3( 2.0F * ( zx - wy ), 2.0F * ( yz + wx ), 1.0F - 2.0F * ( xx + yy ) ).Normal();
} //Quaternion::GetDirection

//--------------------------------
//
//--------------------------------
void Quaternion::SetRotationX( const float fRadian )
{
	float s = sinf( fRadian / 2 );
	float c = cosf( fRadian / 2 );
	x = s * 1.0F;
	y = s * 0.0F;
	z = s * 0.0F;
	w = c;
} //Quaternion::SetRotationX

//--------------------------------
//
//--------------------------------
void Quaternion::SetRotationY( const float fRadian )
{
	float s = sinf( fRadian / 2 );
	float c = cosf( fRadian / 2 );
	x = s * 0.0F;
	y = s * 1.0F;
	z = s * 0.0F;
	w = c;
} //Quaternion::SetRotationY

//--------------------------------
//
//--------------------------------
void Quaternion::SetRotationZ( const float fRadian )
{
	float s = sinf( fRadian / 2 );
	float c = cosf( fRadian / 2 );
	x = s * 0.0F;
	y = s * 0.0F;
	z = s * 1.0F;
	w = c;
} //Quaternion::SetRotationZ


//--------------------------------
//
//--------------------------------
void Quaternion::SetRotationArc( const Vector3& from, const Vector3& to )
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	Vector3 v0 = from;
	Vector3 v1 = to;
	v0.Normalize();
	v1.Normalize();

	const float d = v0.DotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		x = 0; y = 0; z = 0; w = 1;
		return;
	}
	else if (d <= -1.0f) // exactly opposite
	{
		Vector3 axis(1.0f, 0.f, 0.f);
		axis = axis.CrossProduct(v0);
		if (axis.Length() == 0)
		{
			axis = Vector3(0.f, 1.f, 0.f);
			axis = axis.CrossProduct(v0);
		}
		// same as fromAngleAxis(core::PI, axis).normalize();
		*this = Quaternion(axis.x, axis.y, axis.z, 0).Normalize();
		return;
	}

	const float s = sqrtf((1 + d) * 2); // optimize inv_sqrt
	const float invs = 1.f / s;
	const Vector3 c = v0.CrossProduct(v1)*invs;
	*this = Quaternion(c.x, c.y, c.z, s * 0.5f).Normalize();


	//Vector3 vCross = v0.CrossProduct(v1);
	//const float len = vCross.Length();
	//if (len <= 0.01f)
	//{
	//	x = 0; y = 0; z = 0; w = 1;
	//	return;
	//}

	//float fDot = v0.DotProduct( v1 );
	//float s = (float)sqrtf((1.0f + fDot) * 2.0f);
	////if (0.1f >  s)
	////{
	////	x = 0; y = 1; z = 0; w = 0;
	////	return;
	////}

	//x = vCross.x / s;
	//y = vCross.y / s;
	//z = vCross.z / s;
	//w = s * 0.5f;
} //Quaternion::SetRotationArc


void Quaternion::SetRotationArc(const Vector3& v0, const Vector3& v1, const Vector3 &norm)
{
	Vector3 vCross = v0.CrossProduct(v1);
	const float len = vCross.Length();
	if (len <= 0.01f)
	{
		// v0 - v1 벡터가 정확히 반대 방향이거나, 정확히 같은 방향을 가르킬때,
		// 두 벡터에 직교하는 벡터 norm 에서 180도 회전하거나, 회전하지 않거나
		// 결정한다.
		*this = Quaternion(norm, v0.DotProduct(v1) > 0 ? 0 : MATH_PI);
		return;
	}

	float fDot = v0.DotProduct(v1);
	float s = (float)sqrtf((1.0f + fDot) * 2.0f);

	x = vCross.x / s;
	y = vCross.y / s;
	z = vCross.z / s;
	w = s * 0.5f;
} //Quaternion::SetRotationArc


//--------------------------------
//
//--------------------------------
const Quaternion& Quaternion::Normalize()
{
	//return (*this *= reciprocal_squareroot(X*X + Y*Y + Z*Z + W*W));

	float norm = sqrtf( SQR(x) + SQR(y) + SQR(z) + SQR(w) );

	if( FLOAT_EQ( 0.0F, norm ) )
	{
		x = 0; y = 0; z = 0; w = 1;
		return *this;
	} //if

	norm = 1.0F / norm;

	x = x * norm;
	y = y * norm;
	z = z * norm;
	w = w * norm;

	norm = sqrtf( SQR(x) + SQR(y) + SQR(z) + SQR(w) );

	if( !FLOAT_EQ( 1.0F, norm ) )
	{
		x = 0; y = 0; z = 0; w = 1;
		return *this;
	} //if

	LIMIT_RANGE(-1.0f, w, 1.0f);

	LIMIT_RANGE(-1.0f, x, 1.0f);
	LIMIT_RANGE(-1.0f, y, 1.0f);
	LIMIT_RANGE(-1.0f, z, 1.0f);
	return *this;
} //Quaternion::Normalize


// https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
Quaternion Quaternion::Inverse() const
{
	Quaternion q(-x, -y, -z, w);
	return q;
}


/**
    * @brief Computes the quaternion that is equivalent to a given
    * euler angle rotation.
    * @param euler A 3-vector in order:  roll-pitch-yaw.
	* http://ai.stanford.edu/~acoates/quaternion.h
    */
void Quaternion::Euler(const Vector3& v) 
{
	float euler[ 3];
	euler[ 0] = ANGLE2RAD(v.x);
	euler[ 1] = ANGLE2RAD(v.y);
	euler[ 2] = ANGLE2RAD(v.z);

    float c1 = cos(euler[2] * 0.5f);
    float c2 = cos(euler[1] * 0.5f);
    float c3 = cos(euler[0] * 0.5f);
    float s1 = sin(euler[2] * 0.5f);
    float s2 = sin(euler[1] * 0.5f);
    float s3 = sin(euler[0] * 0.5f);

    x = c1*c2*s3 - s1*s2*c3;
    y = c1*s2*c3 + s1*c2*s3;
    z = s1*c2*c3 - c1*s2*s3;
    w = c1*c2*c3 + s1*s2*s3;
}



/**
* @brief Computes the quaternion that is equivalent to a given
* euler angle rotation.
* @param euler A 3-vector in order:  roll-yaw-pitch
* 인자 v의 각 성분은 radian 값이어야 한다.
* http://ai.stanford.edu/~acoates/quaternion.h
*/
void Quaternion::Euler2(const Vector3& v)
{
	float euler[3];
	euler[0] = v.x;
	euler[1] = v.y;
	euler[2] = v.z;

	float c1 = cos(euler[2] * 0.5f);
	float c2 = cos(euler[1] * 0.5f);
	float c3 = cos(euler[0] * 0.5f);
	float s1 = sin(euler[2] * 0.5f);
	float s2 = sin(euler[1] * 0.5f);
	float s3 = sin(euler[0] * 0.5f);

	x = c1*c2*s3 - s1*s2*c3;
	y = c1*s2*c3 + s1*c2*s3;
	z = s1*c2*c3 - c1*s2*s3;
	w = c1*c2*c3 + s1*s2*s3;
}


/** @brief Returns an equivalent euler angle representation of
	* this quaternion.
	* @return Euler angles in roll-pitch-yaw order.
	* http://ai.stanford.edu/~acoates/quaternion.h
	*/
Vector3 Quaternion::Euler(void) const 
{
	double euler[ 3];
	const static double PI_OVER_2 = MATH_PI * 0.5f;
	const static double EPSILON = MATH_EPSILON2;
	double sqw, sqx, sqy, sqz;

	// quick conversion to Euler angles to give tilt to user
	sqw = w*w;
	sqx = x*x;
	sqy = y*y;
	sqz = z*z;

	euler[1] = asin( clamp(-1, 1, 2.0f * (w*y - x*z)) );
	if (PI_OVER_2 - fabs(euler[1]) > EPSILON) 
	{
		euler[2] = atan2( 2.0f * (x*y + w*z),
			sqx - sqy - sqz + sqw);
		euler[0] = atan2(2.0f * (w*x + y*z),
			sqw - sqx - sqy + sqz);
	} 
	else 
	{
		// compute heading from local 'down' vector
		euler[2] = atan2(2*y*z - 2*x*w,
			2*x*z + 2*y*w);
		euler[0] = 0.0f;

		// If facing down, reverse yaw
		if (euler[1] < 0)
			euler[2] = MATH_PI - euler[2];
	}

	return Vector3((float)euler[0], (float)euler[1], (float)euler[2]);



// 	double roll, pitch, yaw;
// 
// 	double ysqr = y * y;
// 	double t0 = -2.0f * (ysqr + z * z) + 1.0f;
// 	double t1 = +2.0f * (x * y - w * z);
// 	double t2 = -2.0f * (x * z + w * y);
// 	double t3 = +2.0f * (y * z - w * x);
// 	double t4 = -2.0f * (x * x + ysqr) + 1.0f;
// 
// 	t2 = t2 > 1.0f ? 1.0f : t2;
// 	t2 = t2 < -1.0f ? -1.0f : t2;
// 
// 	pitch = std::asin(t2);
// 	roll = std::atan2(t3, t4);
// 	yaw = std::atan2(t1, t0);
// 
// 	return Vector3((float)roll, (float)pitch, (float)yaw);
}


// return x-z plane
float Quaternion::GetRotationAngleXZ() const
{
	const Vector3 v = GetDirection();
	float angle = v.DotProduct(Vector3(0, 0, -1));
	float r = acosf(angle);
	if (v.CrossProduct(Vector3(0, 0, -1)).y < 0)
		r *= -1.f;
	return r;
}

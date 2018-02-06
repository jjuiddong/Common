
#include "stdafx.h"
#include "math.h"

#ifdef USE_D3D9_MATH
	//#include <d3dx9.h>
	#include <D3dx9math.h>
#endif


using namespace common;

const Matrix44 Matrix44::Identity;

Matrix44::Matrix44()
{
	SetIdentity();
}

// ar : float ar[16]
Matrix44::Matrix44(const float *ar)
{
	memcpy((float*)m, ar, sizeof(m));
}

Matrix44::Matrix44(
	float m11, float m12, float m13, float m14
	, float m21, float m22, float m23, float m24
	, float m31, float m32, float m33, float m34
	, float m41, float m42, float m43, float m44
)
{
	_11 = m11; _12 = m12; _13 = m13; _14 = m14;
	_21 = m21; _22 = m22; _23 = m23; _24 = m24;
	_31 = m31; _32 = m32; _33 = m33; _34 = m34;
	_41 = m41; _42 = m42; _43 = m43; _44 = m44;
}


void Matrix44::SetIdentity()
{
	_11 = _22 = _33 = _44 = 1;
	_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
}


void Matrix44::SetRotationX( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_22 = fCos;
	_23 = fSin;
	_32 = -fSin;
	_33 = fCos;
}


void Matrix44::SetRotationY( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_11 = fCos;
	_13 = -fSin;
	_31 = fSin;
	_33 = fCos;
}


void Matrix44::SetRotationZ( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_11 = fCos;
	_12 = fSin;
	_21 = -fSin;
	_22 = fCos;
}


void Matrix44::SetTranslate( const Vector3& pos )
{
	SetIdentity();
	_41 = pos.x;
	_42 = pos.y;
	_43 = pos.z;
}


void Matrix44::SetScale( const Vector3& scale)
{
	SetIdentity();
	_11 = scale.x;
	_22 = scale.y;
	_33 = scale.z;
}


void Matrix44::SetScale(const float scale)
{
	SetIdentity();
	_11 = scale;
	_22 = scale;
	_33 = scale;
}


Matrix44 Matrix44::operator * ( const Matrix44& rhs ) const
{
#ifdef USE_D3D9_MATH
	Matrix44 matrix;
	D3DXMatrixMultiply((D3DXMATRIX*)&matrix, (D3DXMATRIX*)this, (D3DXMATRIX*)&rhs);
	return matrix;
#elif defined(USE_D3D11_MATH)
	XMMATRIX m0 = GetMatrixXM();
	XMMATRIX m1 = rhs.GetMatrixXM();
	XMMATRIX r = m0 * m1;
	Matrix44 matrix;
	XMStoreFloat4x4((XMFLOAT4X4*)&matrix, r);
	return matrix;	
#else
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * rhs.m[k][j];
			}
		}
	}
	return matrix;
#endif // USE_D3D9_MATH
}


Matrix44& Matrix44::operator *= ( const Matrix44& rhs )
{
#ifdef USE_D3D9_MATH
	Matrix44 matrix;
	D3DXMatrixMultiply((D3DXMATRIX*)&matrix, (D3DXMATRIX*)this, (D3DXMATRIX*)&rhs);
	*this = matrix;
	return *this;
#elif defined(USE_D3D11_MATH)
	*this = operator*(rhs);
	return *this;
#else
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * rhs.m[k][j];
			}
		}
	}
	*this = matrix;
	return *this;
#endif // USE_D3D9_MATH
}


void Matrix44::SetView( const Vector3& pos, const Vector3& dir0, const Vector3& up0 )
{
	Vector3 vDir;
	Vector3 vUp;
	Vector3 vCross;

	vDir = dir0.Normal();
	vCross = up0.CrossProduct( vDir );
	vCross.Normalize();
	vUp = vDir.CrossProduct( vCross );

	_11 = vCross.x;
	_12 = vUp.x;
	_13 = vDir.x;
	_14 = 0.0f;
	_21 = vCross.y;
	_22 = vUp.y;
	_23 = vDir.y;
	_24 = 0.0f;
	_31 = vCross.z;
	_32 = vUp.z;
	_33 = vDir.z;
	_34 = 0.0f;
	_41 = -pos.DotProduct( vCross );
	_42 = -pos.DotProduct( vUp );
	_43 = -pos.DotProduct( vDir );
	_44 = 1.0f;
}


void Matrix44::SetView2( const Vector3& pos, const Vector3& lookAt, const Vector3& up0)
{
	Vector3 dir = lookAt - pos;
	dir.Normalize();
	SetView(pos, dir, up0);
}


void Matrix44::SetProjection( const float fov, const float aspect, const float nearPlane, const float farPlane )
{
	float fH = cosf(fov / 2.f) / sinf(fov / 2.f);
	float fW = fH / aspect;
	float fQ = farPlane / (farPlane - nearPlane);

	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = 0.0f;
	_41 = _42 = 0.0f;
	_11 = fW;
	_22 = fH;
	_33 = fQ;
	_34 = 1.0f;
	_43 = -fQ * nearPlane;
	_44 = 1.0f;
	_44 = 0.f;
}


void Matrix44::SetProjectionOrthogonal(const float width, const float height
	, const float nearPlane, const float farPlane)
{
#ifdef USE_D3D9_MATH
	D3DXMatrixOrthoLH((D3DXMATRIX*)this, width, height, nearPlane, farPlane);
#elif defined(USE_D3D11_MATH)
	XMMATRIX view = XMMatrixOrthographicLH(width, height, nearPlane, farPlane);
	XMStoreFloat4x4((XMFLOAT4X4*)this, view);
#else
	assert(0);
#endif
}


void Matrix44::SetProjectionOrthogonal(const float left, const float right, const float top, const float bottom, const float nearPlane, const float farPlane)
{
#ifdef USE_D3D9_MATH
	D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)this, left, right, top, bottom, nearPlane, farPlane);
#else
	XMMATRIX view = XMMatrixOrthographicOffCenterLH(left, right, top, bottom, nearPlane, farPlane);
	XMStoreFloat4x4((XMFLOAT4X4*)this, view);
#endif
}


void Matrix44::SetProjectionScreen(const float width, const float height, const float nearPlane, const float farPlane)
{
	Matrix44 orthoProj;
	orthoProj.SetProjectionOrthogonal(width, height, nearPlane, farPlane);

	// left, top = (0,0), right, bottom = (1,1)

	Matrix44 screenProj(
		  1.f, 0.0f, 0.0f, 0.0f
		, 0.0f, -1.f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, -1.f, 1.f, 0.0f, 1.0f);

	*this = orthoProj * screenProj;
}


Vector3 Matrix44::GetScale() const 
{ 
#ifdef USE_D3D9_MATH
	Vector3 s, t;
	Quaternion q;
	D3DXMatrixDecompose((D3DXVECTOR3*)&s, (D3DXQUATERNION*)&q, (D3DXVECTOR3*)&t, (D3DXMATRIX*)this);
	return s;

#elif defined (USE_D3D11_MATH)
	XMMATRIX xmat = XMLoadFloat4x4((XMFLOAT4X4*)this);
	XMVECTOR scale, xq, tran;
	XMMatrixDecompose(&scale, &xq, &tran, xmat);
	Vector3 s;
	XMStoreFloat3((XMFLOAT3*)&s, scale);
	return s;

#endif

	return Vector3(_11, _22, _33);
}


Quaternion Matrix44::GetQuaternion() const
{
#ifdef USE_D3D9_MATH
	Vector3 s, t;
	Quaternion q;
	D3DXMatrixDecompose((D3DXVECTOR3*)&s, (D3DXQUATERNION*)&q, (D3DXVECTOR3*)&t, (D3DXMATRIX*)this);
	return q;

#elif defined (USE_D3D11_MATH)
	// must call XMMatrixDecompose() function
	// no XMQuaternionRotationMatrix() function 
	// because Matrix is compose if scale, rot, translate Matrix
	// Before Decompose, must normalize matrix rotation axis
	// this work is XMMatrixDecompose() function
	// 2017-10-04, jjuiddong
	XMMATRIX xmat = XMLoadFloat4x4((XMFLOAT4X4*)this);
	XMVECTOR scale, xq, tran;
	XMMatrixDecompose(&scale, &xq, &tran, xmat); 
	Quaternion q;
	XMStoreFloat4((XMFLOAT4*)&q, xq);
	return q;

#else
	Quaternion q;
	float fTr = _11 + _22 + _33 + _44;

	if( fTr >= 1.0F )	// w >= 0.5
	{
		float s = sqrtf( fTr );
		q.x = ( _32 - _23 ) / ( 2.0F * s );
		q.y = ( _13 - _31 ) / ( 2.0F * s );
		q.z = ( _21 - _12 ) / ( 2.0F * s );
		q.w = 0.5F * s;
	}
	else
	{
		float v[3];
		int i, j, k;

		if( _11 > _22 )		i = 0;
		else				i = 1;
		if( _33 > m[i][i] )	i = 2;

		j = ( i + 1 ) % 3;
		k = ( j + 1 ) % 3;

		float s = sqrtf( m[i][i] - m[j][j] - m[k][k] + 1.0F );
		v[i] = 0.5F * s;
		v[j] = ( m[j][i] + m[i][j] ) / ( 2.0F * s );
		v[k] = ( m[k][i] + m[i][k] ) / ( 2.0F * s );

		q.x = v[0];
		q.y = v[1];
		q.z = v[2];
		q.w = ( m[k][j] - m[j][k] ) / ( 2.0F * s );
	}
	return q;
#endif // USE_D3D9_MATH
}



//---------------------------
// Matrix44x4_Inverse
//---------------------------
static void Matrix4x4_Inverse( const float b[][4], float a[][4] )
{
	long	indxc[4], indxr[4], ipiv[4];
	long	i = 0, icol = 0, irow = 0, j = 0, ir = 0, ic = 0;
	float	big, dum, pivinv, temp, bb;

	ipiv[0] = -1;
	ipiv[1] = -1;
	ipiv[2] = -1;
	ipiv[3] = -1;

	a[0][0] = b[0][0];
	a[1][0] = b[1][0];
	a[2][0] = b[2][0];
	a[3][0] = b[3][0];

	a[0][1] = b[0][1];
	a[1][1] = b[1][1];
	a[2][1] = b[2][1];
	a[3][1] = b[3][1];

	a[0][2] = b[0][2];
	a[1][2] = b[1][2];
	a[2][2] = b[2][2];
	a[3][2] = b[3][2];

	a[0][3] = b[0][3];
	a[1][3] = b[1][3];
	a[2][3] = b[2][3];
	a[3][3] = b[3][3];

	for (i = 0; i < 4; i++) {
		big = 0.0f;
		for (j = 0; j < 4; j++) {
			if (ipiv[j] != 0) {
				if (ipiv[0] == -1) {
					if ((bb = ( float) fabs(a[j][0])) > big) {
						big = bb;
						irow = j;
						icol = 0;
					} //if
				} else if (ipiv[0] > 0) {
					return;
				} //if..else if..
				if (ipiv[1] == -1) {
					if ((bb = ( float) fabs(( float) a[j][1])) > big) {
						big = bb;
						irow = j;
						icol = 1;
					} //if
				} else if (ipiv[1] > 0) {
					return;
				} //if..else if..
				if (ipiv[2] == -1) {
					if ((bb = ( float) fabs(( float) a[j][2])) > big) {
						big = bb;
						irow = j;
						icol = 2;
					} //if
				} else if (ipiv[2] > 0) {
					return;
				} //if..else if..
				if (ipiv[3] == -1) {
					if ((bb = ( float) fabs(( float) a[j][3])) > big) {
						big = bb;
						irow = j;
						icol = 3;
					} //if
				} else if (ipiv[3] > 0) {
					return;
				} //if..else if..
			} //if
		} //for

		++(ipiv[icol]);

		if (irow != icol) {

			temp = a[irow][0];
			a[irow][0] = a[icol][0];
			a[icol][0] = temp;

			temp = a[irow][1];
			a[irow][1] = a[icol][1];
			a[icol][1] = temp;

			temp = a[irow][2];
			a[irow][2] = a[icol][2];
			a[icol][2] = temp;

			temp = a[irow][3];
			a[irow][3] = a[icol][3];
			a[icol][3] = temp;
		} //if

		indxr[i] = irow;
		indxc[i] = icol;

		if (a[icol][icol] == 0.0) {
			return;
		} //if

		pivinv = 1.0f / a[icol][icol];
		a[icol][icol] = 1.0f;
		a[icol][0] *= pivinv;
		a[icol][1] *= pivinv;
		a[icol][2] *= pivinv;
		a[icol][3] *= pivinv;

		if (icol != 0) {
			dum = a[0][icol];
			a[0][icol] = 0.0f;
			a[0][0] -= a[icol][0] * dum;
			a[0][1] -= a[icol][1] * dum;
			a[0][2] -= a[icol][2] * dum;
			a[0][3] -= a[icol][3] * dum;
		} //if
		if (icol != 1) {
			dum = a[1][icol];
			a[1][icol] = 0.0f;
			a[1][0] -= a[icol][0] * dum;
			a[1][1] -= a[icol][1] * dum;
			a[1][2] -= a[icol][2] * dum;
			a[1][3] -= a[icol][3] * dum;
		} //if
		if (icol != 2) {
			dum = a[2][icol];
			a[2][icol] = 0.0f;
			a[2][0] -= a[icol][0] * dum;
			a[2][1] -= a[icol][1] * dum;
			a[2][2] -= a[icol][2] * dum;
			a[2][3] -= a[icol][3] * dum;
		} //if
		if (icol != 3) {
			dum = a[3][icol];
			a[3][icol] = 0.0f;
			a[3][0] -= a[icol][0] * dum;
			a[3][1] -= a[icol][1] * dum;
			a[3][2] -= a[icol][2] * dum;
			a[3][3] -= a[icol][3] * dum;
		} //if
	} //for
	if (indxr[3] != indxc[3]) {
		ir = indxr[3];
		ic = indxc[3];

		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;

		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;

		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;

		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[2] != indxc[2]) {
		ir = indxr[2];
		ic = indxc[2];

		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;

		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;

		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;

		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[1] != indxc[1]) {
		ir = indxr[1];
		ic = indxc[1];

		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;

		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;

		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;

		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[0] != indxc[0]) {
		ir = indxr[0];
		ic = indxc[0];

		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;

		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;

		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;

		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
} //Matrix44x4_Invert


void Matrix44::InverseMatrix(Matrix44 &out) const
{
	Matrix44 &matInverse = out;

	if (fabs(_44 - 1.0F) > 0.001F)
	{
		Matrix4x4_Inverse(this->m, matInverse.m);
		return;
	} //if

	if (fabs(_14) > 0.001F || fabs(_24) > 0.001F || fabs(_34) > 0.001F)
	{
		Matrix4x4_Inverse(this->m, matInverse.m);
		return;
	} //if

	float det = _11 * (_22 * _33 - _23 * _32)
		- _12 * (_21 * _33 - _23 * _31)
		+ _13 * (_21 * _32 - _22 * _31);

	if (ABS(det) < MATH_EPSILON)
	{
		Matrix4x4_Inverse(this->m, matInverse.m);
		return;
	} //if

	det = 1.0F / det;

	matInverse._11 = det * (_22 * _33 - _23 * _32);
	matInverse._12 = -det * (_12 * _33 - _13 * _32);
	matInverse._13 = det * (_12 * _23 - _13 * _22);
	matInverse._14 = 0.0F;

	matInverse._21 = -det * (_21 * _33 - _23 * _31);
	matInverse._22 = det * (_11 * _33 - _13 * _31);
	matInverse._23 = -det * (_11 * _23 - _13 * _21);
	matInverse._24 = 0.0F;

	matInverse._31 = det * (_21 * _32 - _22 * _31);
	matInverse._32 = -det * (_11 * _32 - _12 * _31);
	matInverse._33 = det * (_11 * _22 - _12 * _21);
	matInverse._34 = 0.0F;

	matInverse._41 = -(_41 * matInverse._11 + _42 * matInverse._21 + _43 * matInverse._31);
	matInverse._42 = -(_41 * matInverse._12 + _42 * matInverse._22 + _43 * matInverse._32);
	matInverse._43 = -(_41 * matInverse._13 + _42 * matInverse._23 + _43 * matInverse._33);
	matInverse._44 = 1.0F;
}


// 역행렬을 리턴한다.
Matrix44 Matrix44::Inverse() const
{
#ifdef USE_D3D9_MATH
	Matrix44 matInverse;
	D3DXMatrixInverse((D3DXMATRIX*)&matInverse, 0, (D3DXMATRIX*)this);
	return matInverse;

#else
	XMMATRIX matInverse = XMLoadFloat4x4((XMFLOAT4X4*)this);
	matInverse = XMMatrixInverse(NULL, matInverse);
	Matrix44 ret;
	XMStoreFloat4x4((XMFLOAT4X4*)&ret, matInverse);
	return ret;
	//Matrix44 ret;
	//InverseMatrix(ret);
	//return ret;
#endif // USE_D3D9_MATH
}


Matrix44& Matrix44::Inverse2()
{
#ifdef USE_D3D9_MATH
	Matrix44 matInverse;
	D3DXMatrixInverse((D3DXMATRIX*)&matInverse, 0, (D3DXMATRIX*)this);
	*this = matInverse;
#else
	Matrix44 matInverse;
	InverseMatrix(matInverse);
	*this = matInverse;
#endif // USE_D3D9_MATH

	return *this;
}


// 전치행렬을 만든다.
Matrix44& Matrix44::Transpose()
{
#ifdef USE_D3D9_MATH
	Matrix44 m;
	D3DXMatrixTranspose((D3DXMATRIX*)&m, (D3DXMATRIX*)this);
	*this = m;
#else
	Matrix44 m;
	m._11 = _11;
	m._12 = _21;
	m._13 = _31;
	m._14 = _41;
	m._21 = _12;
	m._22 = _22;
	m._23 = _32;
	m._24 = _42;
	m._31 = _13;
	m._32 = _23;
	m._33 = _33;
	m._34 = _43;
	m._41 = _14;
	m._42 = _24;
	m._43 = _34;
	m._44 = _44;
	*this = m;
#endif // USE_D3D9_MATH

	return *this;
}


// 기저벡터 x,y를 이용해 회전행렬을 만든다.
void Matrix44::SetRotationXY(const Vector3 &xAxis, const Vector3 &yAxis)
{
	Vector3 zAxis = xAxis.CrossProduct(yAxis);
	zAxis.Normalize();

	_11 = xAxis.x;
	_12 = xAxis.y;
	_13 = xAxis.z;

	_21 = yAxis.x;
	_22 = yAxis.y;
	_23 = yAxis.z;

	_31 = zAxis.x;
	_32 = zAxis.y;
	_33 = zAxis.z;

	_41 = _42 = _43 = 0;
	_44 = 1;
}


// 기저벡터 x,z를 이용해 회전행렬을 만든다.
void Matrix44::SetRotationXZ(const Vector3 &xAxis, const Vector3 &zAxis)
{
	Vector3 yAxis = zAxis.CrossProduct(xAxis);
	yAxis.Normalize();

	_11 = xAxis.x;
	_12 = xAxis.y;
	_13 = xAxis.z;

	_21 = yAxis.x;
	_22 = yAxis.y;
	_23 = yAxis.z;

	_31 = zAxis.x;
	_32 = zAxis.y;
	_33 = zAxis.z;

	_41 = _42 = _43 = 0;
	_44 = 1;
}


// 기저벡터 y,z를 이용해 회전행렬을 만든다.
void Matrix44::SetRotationYZ(const Vector3 &yAxis, const Vector3 &zAxis)
{
	Vector3 xAxis = yAxis.CrossProduct(zAxis);
	xAxis.Normalize();

	_11 = xAxis.x;
	_12 = xAxis.y;
	_13 = xAxis.z;

	_21 = yAxis.x;
	_22 = yAxis.y;
	_23 = yAxis.z;

	_31 = zAxis.x;
	_32 = zAxis.y;
	_33 = zAxis.z;

	_41 = _42 = _43 = 0;
	_44 = 1;
}


float& Matrix44::operator () (UINT iRow, UINT iCol)
{
	return m[iRow][iCol];
}


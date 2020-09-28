//
// 2017-08-03, jjuiddong
// Upgrade DX9 -> DX11
//
//
#pragma once

namespace common
{
	struct Vector3;
	struct Quaternion;

	struct Matrix44
	{
		union
		{
			// row/col, _23:row2,col3
			struct
			{
				float	_11, _12, _13, _14;
				float	_21, _22, _23, _24;
				float	_31, _32, _33, _34;
				float	_41, _42, _43, _44;
			};
			float	m[4][4];
		};
		static const Matrix44 Identity; // 전역 단위행렬 상수.

		Matrix44();
		Matrix44(const float *ar);
		Matrix44(
			  float m11, float m12, float m13, float m14
			, float m21, float m22, float m23, float m24
			, float m31, float m32, float m33, float m34
			, float m41, float m42, float m43, float m44
		);

		inline void SetIdentity();
		void SetRotationX( const float angle );
		void SetRotationY( const float angle );
		void SetRotationZ( const float angle );
		void SetRotationXY(const Vector3 &xAxis, const Vector3 &yAxis);
		void SetRotationXZ(const Vector3 &xAxis, const Vector3 &zAxis);
		void SetRotationYZ(const Vector3 &yAxis, const Vector3 &zAxis);
		void SetTranslate( const Vector3& pos );
		void SetScale( const Vector3& scale);
		void SetScale(const float scale);
		Vector3 GetScale() const;
		Vector3 GetPosition() const;
		void SetPosition(const Vector3 &pos);
		void SetView( const Vector3& pos, const Vector3& dir0, const Vector3& up0);
		void SetView2( const Vector3& pos, const Vector3& lookAt, const Vector3& up0);
		void SetProjection( const float fov, const float aspect, const float nearPlane, const float farPlane );
		void SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane);
		void SetProjectionOrthogonal(const float left, const float right, const float top, const float bottom, const float nearPlane, const float farPlane);
		void SetProjectionScreen(const float width, const float height, const float nearPlane, const float farPlane);
		Quaternion GetQuaternion() const;
		Matrix44 Inverse() const;
		Matrix44& Inverse2();
		void InverseMatrix(Matrix44 &out) const;
		Matrix44& Transpose();

#ifdef USE_D3D11_MATH
		Matrix44(const XMMATRIX &rhs);
		XMMATRIX GetMatrixXM() const;
		Matrix44& operator = (const XMMATRIX &rhs);
#endif

		Matrix44 operator * ( const Matrix44& rhs ) const;
		Matrix44& operator *= ( const Matrix44& rhs );
		float& operator () (UINT iRow, UINT iCol);
	};


	//inline Vector3 Matrix44::GetScale() const { return Vector3(_11, _22, _33); }
	inline Vector3 Matrix44::GetPosition() const { return Vector3(_41, _42, _43); }
	inline void Matrix44::SetPosition(const Vector3 &pos) { _41=pos.x; _42 = pos.y; _43 = pos.z; }


#ifdef USE_D3D11_MATH
	inline Matrix44::Matrix44(const XMMATRIX &rhs)
	{
		operator=(rhs);
	}

	inline XMMATRIX Matrix44::GetMatrixXM() const
	{
		XMMATRIX m = XMLoadFloat4x4((XMFLOAT4X4*)this);
		return m;
	}

	inline Matrix44& Matrix44::operator = (const XMMATRIX &rhs)
	{
		XMStoreFloat4x4((XMFLOAT4X4*)this, rhs);
		return *this;
	}
#endif

}

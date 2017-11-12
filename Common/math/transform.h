//
// 2017-06-26, jjuiddong
//
//
#pragma once


namespace common
{

	struct Transform
	{
		Transform();

		Matrix44 GetMatrix() const;
#ifdef USE_D3D11_MATH
		Transform(const XMMATRIX &rhs);
		XMMATRIX GetMatrixXM() const;
		const Transform& operator=(const XMMATRIX &rhs);
#endif

		Transform Inverse() const;
		Transform operator*(const Transform &rhs) const;
		const Transform& operator*=(const Transform &rhs);


		Vector3 pos;
		Vector3 scale;
		Quaternion rot;
		const static Transform Identity;
	};



#ifdef USE_D3D11_MATH
	inline Transform::Transform(const XMMATRIX &rhs)
	{
		operator=(rhs);
	}

	inline XMMATRIX Transform::GetMatrixXM() const
	{
		XMVECTOR q = XMLoadFloat4((XMFLOAT4*)&rot);
		XMMATRIX mRot = XMMatrixRotationQuaternion(q);
		return XMMatrixScaling(scale.x, scale.y, scale.z) * mRot * XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

	inline const Transform& Transform::operator=(const XMMATRIX &rhs)
	{
		XMVECTOR _scale, _xq, _tran;
		XMMatrixDecompose(&_scale, &_xq, &_tran, rhs);

		XMStoreFloat3((XMFLOAT3*)&scale, _scale);
		XMStoreFloat3((XMFLOAT3*)&pos, _tran);
		XMStoreFloat4((XMFLOAT4*)&rot, _xq);
		return *this;
	}
#endif
}

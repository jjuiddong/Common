
#include "stdafx.h"
#include "billboard.h"


using namespace graphic;


cBillboard::cBillboard()
{
}

cBillboard::~cBillboard()
{
}


// 빌보드 초기화.
bool cBillboard::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const float width, const float height, 
	const Vector3 &pos, const StrPath &textureFileName, const bool isSizePow2)
{
	if (!__super::Create(renderer, width, height, pos
		, eVertexType::POSITION | eVertexType::TEXTURE, textureFileName))
		return false;

	m_type = type;
	m_scale = m_transform.scale;
	m_transform.pos = pos;
	return true;
}


void cBillboard::Rotate()
{
	Matrix44 mat;

	switch (m_type)
	{
	case BILLBOARD_TYPE::NONE:
		break;

	case BILLBOARD_TYPE::Y_AXIS:
	{
		// Y축 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat._11 = view._11;
		mat._13 = view._13;
		mat._31 = view._31;
		mat._33 = view._33;
		mat.Transpose();
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;

	case BILLBOARD_TYPE::ALL_AXIS_Z:
	{
		// 모든 축에서 빌보드 행렬을 계산한다.
		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), GetMainCamera().GetUpVector());

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
	}
	break;


	case BILLBOARD_TYPE::DYN_SCALE:
	{
		// Fixed Scale Model
		Vector3 pos = m_transform.pos;
		const float len = (pos - GetMainCamera().GetEyePos()).Length();
		const Vector3 scale = m_scale * min(1.5f, max(1.f, len / 50.f));

		Matrix44 S;
		S.SetScale(scale);

		Matrix44 view;
		view.SetView2(m_transform.pos, GetMainCamera().GetEyePos(), Vector3(0, 1, 0));

		mat = view;
		mat._41 = mat._42 = mat._43 = 0;
		mat.Transpose();
		m_transform.scale = scale;
	}
	break;
	}

	Matrix44 R;
	R.SetRotationY(ANGLE2RAD(180)); // treaky code, didn't understand
	//m_transform.rot = (R * mat).GetQuaternion();

	m_normal = (GetMainCamera().GetEyePos() - m_transform.pos).Normal();
	Matrix44 rot = R * mat;
	{
		XMMATRIX xmat = XMLoadFloat4x4((XMFLOAT4X4*)&rot);
		XMVECTOR xq = XMQuaternionRotationMatrix(xmat);
		XMQuaternionNormalize(xq);
		Quaternion q;
		XMStoreFloat4((XMFLOAT4*)&q, xq);
		m_transform.rot = q;
	}
}


// 화면에 출력.
void cBillboard::Render(cRenderer &renderer)
{
	Rotate();
	__super::Render(renderer);
}
